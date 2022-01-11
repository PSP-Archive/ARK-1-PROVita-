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
#include <string.h>
#include <offsets.h>
#include <systemctrl.h>
#include "sysmem.h"

// Patch System Memory Manager
void patchSystemMemoryManager(void)
{
	// Force Branching
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_1 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_2 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_3 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_4 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_5 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_6 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_7 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_8 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_9 + 2);
	_sh(0x1000, SYSMEM_TEXT + SYSMEM_BRANCH_CHECK_10 + 2);
}

// Patch Game ID Getter
void patchGameInfoGetter(SceModule2 * mod)
{
	// Kernel Module
	if((mod->text_addr & 0x80000000) != 0)
	{
		// Hook Import
		hookImportByNID(mod, "SysMemForKernel", 0xEF29061C, SysMemForKernel_EF29061C_Fixed);
	}
}

// Fixed Game Info Getter Function
void * SysMemForKernel_EF29061C_Fixed(void)
{
	// Find Function
	void * (* SysMemForKernel_EF29061C)(void) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemForKernel", 0xEF29061C);
	
	// Function unavailable (how?!)
	if(SysMemForKernel_EF29061C == NULL) return NULL;
	
	// Get Game Info Structure
	void * gameinfo = SysMemForKernel_EF29061C();
	
	// Structure unavailable
	if(gameinfo == NULL) return NULL;
	
	// Default Game ID
	const char * defaultid = "HOME00000";
	
	// Set Default Game ID
	memcpy(gameinfo + 0x44, defaultid, strlen(defaultid));
	
	// Open Disc Identifier
	int disc = sceIoOpen("disc0:/UMD_DATA.BIN", PSP_O_RDONLY, 0777);
	
	// Opened Disc Identifier
	if(disc >= 0)
	{
		// Read Country Code
		sceIoRead(disc, gameinfo + 0x44, 4);
		
		// Skip Delimiter
		sceIoLseek32(disc, 1, PSP_SEEK_CUR);
		
		// Read Game ID
		sceIoRead(disc, gameinfo + 0x48, 5);
		
		// Close Disc Identifier
		sceIoClose(disc);
	}
	
	// Return Game Info Structure
	return gameinfo;
}

