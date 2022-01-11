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
#include <systemctrl.h>
#include <systemctrl_private.h>
#include <module2.h>
#include <stdio.h>
#include <string.h>
#include <macros.h>
#include "imports.h"
#include "modulemanager.h"
#include "filesystem.h"
#include "cryptography.h"
#include "mediasync.h"
#include "msstor_cache.h"
#include "loadexec.h"
#include "vlffix.h"
#include "rebootconfig.h"
#include "sysmem.h"
#include "lflash0.h"

// Previous Module Start Handler
STMOD_HANDLER previous = NULL;

// Return Boot Status
static int isSystemBooted(void)
{
	// Find Function
	int (* sceKernelGetSystemStatus)(void) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemForKernel", 0x452E3696);
	
	// Get System Status
	int result = sceKernelGetSystemStatus();
	
	// System booted
	if(result == 0x20000) return 1;
	
	// Still booting
	return 0;
}

// Module Start Handler
static void PROSyspatchOnModuleStart(SceModule2 * mod)
{
	// System fully booted Status
	static int booted = 0;

#ifdef DEBUG
	int apitype = sceKernelInitApitype();
	
	printk("syspatch: %s(0x%04X)\r\n", mod->modname, apitype);
	hookImportByNID(mod, "KDebugForKernel", 0x84F370BC, printk);
#endif
	
	// Apply Directory IO PSP Emulation
	patchFileManagerImports(mod);
	
	// Apply Game ID Patch
	patchGameInfoGetter(mod);
	
	// By this time we are ready to...
	if(0 == strcmp(mod->modname, "sceDisplay_Service"))
	{
		// Patch loadexec_01g.prx
		patchLoadExec();
		
		// Exit Handler
		goto flush;
	}
	
	// Media Sync about to start...
	if(0 == strcmp(mod->modname, "sceMediaSync"))
	{
		// Patch mediasync.prx
		patchMediaSync(mod->text_addr);
		
		// Exit Handler
		goto flush;
	}
	
	// MesgLed Cryptography about to start...
	if(0 == strcmp(mod->modname, "sceMesgLed"))
	{
		// Patch mesg_led_01g.prx
		patchMesgLed(mod->text_addr);
		
		// Exit Handler
		goto flush;
	}
	
	// VLF Module Patches
	if(0 == strcmp(mod->modname, "VLF_Module"))
	{
		// Patch VLF Module
		patchVLF(mod);
		
		// Exit Handler
		goto flush;
	}
	
#if FIRMWARE >= 210
	// Kermit Peripheral Patches
	if(0 == strcmp(mod->modname, "sceKermitPeripheral_Driver"))
	{
		// Patch Kermit Peripheral Module
		patchKermitPeripheral(mod);
		
		// Exit Handler
		goto flush;
	}
#endif

#ifdef FLASH_RESTORE
	// Avoid flash corruption when loading other eCFWs by fresh loading it
	// Only use on Uno / Half Minute Hero games
	if(0 == strcmp(mod->modname, "UNO")
		|| 0 == strcmp(mod->modname, "saty")
		//|| 0 == strcmp(mod->modname, "Widget")
		)
	{
		//colorDebug(0xFFFFFFFF);
		SceUID fd = sceIoOpen(FLASH_BACKUP, PSP_O_RDONLY, 0777);
		if (fd >= 0)
		{
			int read = sceIoRead(fd, FLASH_SONY, FLASH_SIZE);
			sceIoClose(fd);

			sony_unpatch();
			//colorDebug(0x88888888);
		}
		
		// Exit Handler
		goto flush;
	}
#endif

#if 0
	if(0 == strcmp(mod->modname, "simple") || \
			0 == strcmp(mod->modname, "complex") || \
			0 == strcmp(mod->modname, "scePops_Manager")
			)
	{
		installModuleJALTrace(mod);

		goto flush;
	}
#endif
	
	// Boot Complete Action not done yet
	if(booted == 0)
	{
		// Boot is complete
		if(isSystemBooted())
		{
			// Initialize Memory Stick Speedup Cache
			msstorCacheInit();
			
			patchFileSystemDirSyscall();
			printkSync();

			// Boot Complete Action done
			booted = 1;
		}
	}
	
	// No need to flush
	goto exit;
	
flush:
	// Flush Cache
	flushCache();
	
exit:
	// Forward to previous Handler
	if(previous) previous(mod);
}

// Add Module Start Patcher
void syspatchInit(void)
{
	// Register Module Start Handler
	previous = sctrlHENSetStartModuleHandler(PROSyspatchOnModuleStart);
}

#ifdef FLASH_RESTORE
void sony_unpatch()
{
	SceModule2 *wlan = sceKernelFindModuleByName("sceWlan_Driver");

	// Working
	if (wlan != NULL)
	{
		unsigned int wlanDrv51B0BBB8_kxpPatch = (u32)wlan->text_addr + 0x000052A4;

		_sw(NOP, wlanDrv51B0BBB8_kxpPatch);
	}


	// Non working
	SceModule2 *save = sceKernelFindModuleByName("sceVshSDUtility_Module");

	if (save != NULL)
	{
		unsigned int uno_eu_id = (u32)save->text_addr + 0x00020794;

		_sb('$', uno_eu_id);
		_sb('$', uno_eu_id+1);
	}
}
#endif
