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
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <string.h>
#include <stdio.h>
#include <systemctrl.h>
#include <systemctrl_private.h>
#include <macros.h>
#include <gameinfo.h>
#include <offsets.h>
#include "loadmodule_patch.h"
#include "savedata_patch.h"
#include "nodrm_patch.h"
#include "psid.h"

PSP_MODULE_INFO("stargate", 0x1007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

// Previous Module Start Handler
STMOD_HANDLER previous;

// Module Start Handler
void stargateSyspatchModuleOnStart(SceModule2 * mod)
{
	// Call Previous Module Start Handler
	if(previous) previous(mod);
	
	// Patch LoadModule Function
	patchLoadModuleFuncs(mod);
	
	// Fix Exploit Game Save
	fixExploitGameModule(mod);
}

// Boot Time Module Start Handler
int stargateStartModuleHandler(int modid, SceSize argsize, void * argp, int * modstatus, SceKernelSMOption * opt)
{
	// Fetch Module
	SceModule2 * mod = (SceModule2 *)sceKernelFindModuleByUID(modid);
	
	// Module not found
	if(mod == NULL) return -1;
	
	// Fix Prometheus Patch #1
	SceLibraryStubTable * import = findImportLib(mod, "Kernel_LibrarZ");
	if(import != NULL)
	{
		strcpy((char *)import->libname, "Kernel_Library");
	}
	
	// Fix Prometheus Patch #2
	import = findImportLib(mod, "Kernel_Librar0");
	if(import != NULL)
	{
		strcpy((char* )import->libname, "Kernel_Library");
	}
	
	// Fix Prometheus Patch #3
	import = findImportLib(mod, "sceUtilitO");
	if(import != NULL)
	{
		strcpy((char*)import->libname, "sceUtility");
	}
	
	// Why do we return an error...?
	return -1;
}

// Idol Master Fix
static void patchLoadExec(void)
{
	// Fix Load Execute CFW Detection
	sctrlPatchModule("sceLoadExec", NOP, LOADEXEC_LOAD_EXEC_FOR_USER_362A956B_FIX);
}

// Entry Point
int module_start(SceSize args, void * argp)
{
	// Hello Message
	printk("stargate started: compiled at %s %s\r\n", __DATE__, __TIME__);
	
	// Fix Idol Master
	patchLoadExec();
	
	// Fetch sceUtility Load Module Functions
	getLoadModuleFuncs();
	
	// Initialize NPDRM
	nodrmInit();
	
	// Register Start Module Handler
	previous = sctrlHENSetStartModuleHandler(stargateSyspatchModuleOnStart);
	
	// Register Boot Start Module Handler
	sctrlSetCustomStartModule(stargateStartModuleHandler);
	
	// Flush Cache
	flushCache();

#ifndef DISABLE_PSID_CHECK
	psidCheck();
#endif

	// Module Start Success
	return 0;
}
