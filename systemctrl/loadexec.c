/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <stdio.h>
#include <string.h>
#include <macros.h>
#include <module2.h>
#include <gameinfo.h>
#include <systemctrl.h>
#include "imports.h"
#include "modulemanager.h"
#include "filesystem.h"
#include "rebootex.h"

// Original Load Reboot Buffer Function
int (* _LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

// sceKernelExitGame Replacement
int returnToLauncher(void)
{
	// Load Execute Parameter
	struct SceKernelLoadExecVSHParam param;
	
	// Clear Memory
	memset(&param, 0, sizeof(param));
	
	// Configure Parameters
	param.size = sizeof(param);
	param.args = strlen(VSH_PATH) + 1;
	param.argp = VSH_PATH;
	param.key = "game";
	
	// Trigger Reboot
	return sctrlKernelLoadExecVSHWithApitype(0x141, VSH_PATH, &param);
}

// Reboot Buffer Loader
int LoadReboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	// Restore Reboot Buffer Configuration
	restoreRebootBuffer();
	
	// Load Sony Reboot Buffer
	return _LoadReboot(arg1, arg2, arg3, arg4);
}

// Patch loadexec_01g.prx
void patchLoadExec(void)
{
	// Find LoadExec Module
	SceModule2 * loadexec = (SceModule2*)sceKernelFindModuleByName("sceLoadExec");
	
	// Find Reboot Loader Function
	_LoadReboot = (void *)loadexec->text_addr + LOADEXEC_LOAD_REBOOT;
	
	// Patch Reboot Buffer Loader
	_sw(JAL(LoadReboot), loadexec->text_addr + LOADEXEC_LOAD_REBOOT_CALL);
	
	// Patch Reboot Buffer Entry Point (for PROCFW Reboot Buffer)
	_sb(0xFC, loadexec->text_addr + LOADEXEC_REBOOT_BUFFER_ADDRESS_SETTER); // lui $at, 0x88FC instead of 0x8860
	
	// Patch K1 Check in LoadExec Call
	_sh(0x1000, loadexec->text_addr + LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_K1_CHECK_1);
	_sh(0x1000, loadexec->text_addr + LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_K1_CHECK_2);
	_sh(0x1000, loadexec->text_addr + LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_K1_CHECK_3);
	
	// Patch sceKernelGetUserLevel Check in LoadExec Call
	_sw(NOP, loadexec->text_addr + LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_USER_LEVEL_CHECK_1);
	_sw(NOP, loadexec->text_addr + LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_USER_LEVEL_CHECK_2);
	_sw(NOP, loadexec->text_addr + LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_USER_LEVEL_CHECK_3);

	_sw(NOP, loadexec->text_addr + 0x2964);
	_sw(NOP, loadexec->text_addr + 0x2968);
	_sw(NOP, loadexec->text_addr + 0x296C);
	_sw(NOP, loadexec->text_addr + 0x2970);
	_sw(NOP, loadexec->text_addr + 0x2974);
	_sw(NOP, loadexec->text_addr + 0x2978);
	_sw(NOP, loadexec->text_addr + 0x297C);
	_sw(NOP, loadexec->text_addr + 0x2980);

	// Patch sceKernelExitGame Syscalls
	sctrlHENPatchSyscall((void *)sctrlHENFindFunction(loadexec->modname, "LoadExecForUser", 0x05572A5F), returnToLauncher);
	sctrlHENPatchSyscall((void *)sctrlHENFindFunction(loadexec->modname, "LoadExecForUser", 0x2AC9954B), returnToLauncher);
}

