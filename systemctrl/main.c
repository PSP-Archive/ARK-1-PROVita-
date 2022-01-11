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
#include <systemctrl.h>
#include <systemctrl_private.h>
#include "rebootex.h"
#include "modulemanager.h"
#include "loadercore.h"
#include "filesystem.h"
#include "interruptman.h"
#include "cryptography.h"
#include "syspatch.h"
#include "sysmem.h"
#include "exception.h"
#include "../common/ansi-c/functions.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 1, 0);

// Boot Time Entry Point
int module_bootstart(SceSize args, void * argp)
{
#ifdef DEBUG
	colorDebug(0xFF);
	printkInit(GAME_SAVE_PATH "log.txt");
	printk("systemctrl started: compiled at %s %s\r\n", __DATE__, __TIME__);
#endif

	// Apply Module Patches
	patchSystemMemoryManager();
	patchLoaderCore();
	patchModuleManager();
	patchInterruptMan();
	patchMemlmd();
	patchFileManager();

	// Backup Reboot Buffer (including configuration)
	backupRebootBuffer();
	
	// Initialize Module Start Patching
	syspatchInit();
	
	// Initialize Malloc
	oe_mallocinit();
	
	// Flush Cache
	flushCache();
	
	// Register Exception Handler
	// registerExceptionHandler(NULL, NULL);
	
	SceUID fd = sceIoOpen("ms0:/PSP/SAVEDATA/ULUS10528TRIAL01/KDUMP.BIN", PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777);
	sceIoWrite(fd, (const void *)0x88000000, 0x00300000);
	sceIoClose(fd);

	// Return Success
	return 0;
}

// Run Time Entry Point
int module_start(SceSize args, void * argp)
{
	// Simply there to ensure our Entry Point gets called...
	return module_bootstart(args, argp);
}

