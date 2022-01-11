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
#include <systemctrl_private.h>
#include <offsets.h>
#include "imports.h"
#include "modulemanager.h"
#include "nidresolver.h"
#include "plugin.h"
#include "elf.h"
#include "loadercore.h"
#include "cryptography.h"
#include "rebootconfig.h"

// init.prx Text Address
unsigned int sceInitTextAddr = 0;

// Plugin Loader Status
int pluginLoaded = 0;

// Real Executable Check Function Pointer
int (* ProbeExec1)(unsigned char * buffer, int * check) = NULL;
int (* ProbeExec2)(unsigned char * buffer, int * check) = NULL;

// init.prx Custom sceKernelStartModule Handler
int (* customStartModule)(int modid, SceSize argsize, void * argp, int * modstatus, SceKernelSMOption * opt) = NULL;

// Executable Check #1
int _ProbeExec1(unsigned char * buffer, int * check)
{
	// Check Executable (we patched our files with shifted attributes so this works)
	int result = ProbeExec1(buffer, check);
	
	// Grab Executable Magic
	unsigned int magic = *(unsigned int *)(buffer);
	
	// ELF File
	if(magic == 0x464C457F)
	{
		// Recover Attributes (which we shifted before)
		unsigned short realattr = *(unsigned short *)(buffer + check[19]);
		
		// Mask Attributes
		unsigned short attr = realattr & 0x1E00;
		
		// Kernel Module
		if(attr != 0)
		{
			// Fetch OFW-detected Attributes
			unsigned short attr2 = *(u16*)((void*)(check)+0x58);
			
			// OFW Attributes don't match
			if((attr2 & 0x1E00) != attr)
			{
				// Now they do. :)
				*(u16*)((void*)(check)+0x58) = realattr;
			}
		}
		
		// Flip Switch
		if(check[18] == 0) check[18] = 1;
	}
	
	// Return Result
	return result;
}

// Executable Check #2
int _ProbeExec2(unsigned char * buffer, int * check)
{
	// Check Executable
	int result = ProbeExec2(buffer, check);
	
	// Grab Executable Magic
	unsigned int magic = *(unsigned int *)(buffer);
	
	// Plain Static ELF Executable
	if(magic == 0x464C457F && IsStaticElf(buffer))
	{
		// Fake UMD Apitype (as its the only one that allows Static ELFs... and even that, only as LoadExec Target)
		check[2] = 0x120;
		
		// Invalid Module Info Section
		if(check[19] == 0)
		{
			// Search String Table
			char * strtab = GetStrTab(buffer);
			
			// Found it! :D
			if(strtab != NULL)
			{
				// Cast ELF Header
				Elf32_Ehdr * header = (Elf32_Ehdr *)buffer;
				
				// Section Header Start Pointer
				unsigned char * pData = buffer + header->e_shoff;
				
				// Iterate Section Headers
				int i = 0; for (; i < header->e_shnum; i++)
				{
					// Cast Section Header
					Elf32_Shdr * section = (Elf32_Shdr *)pData;
					
					// Found Module Info Section
					if(strcmp(strtab + section->sh_name, ".rodata.sceModuleInfo") == 0)
					{
						// Fix Section Pointer
						check[19] = section->sh_offset;
						check[22] = 0;
						
						// Stop Search
						break;
					}
					
					// Move to next Section
					pData += header->e_shentsize;
				}
			}
		}
	}
	
	// Return Result
	return result;
}

// Executable File Check
int KernelCheckExecFile(unsigned char * buffer, int * check)
{
	// Patch Executable
	int result = PatchExec1(buffer, check);
	
	// PatchExec1 isn't enough... :(
	if(result != 0)
	{
		// Check Executable
		int checkresult = sceKernelCheckExecFile(buffer, check);
		
		// Grab Executable Magic
		unsigned int magic = *(unsigned int *)(buffer);
		
		// Patch Executable
		result = PatchExec3(buffer, check, magic == 0x464C457F, checkresult);
	}
	
	// Return Result
	return result;
}

int checkPsid(void)
{
	unsigned char key[16];

	getPsid(key);

	if (psidHash(key, key) < 0)
	{
		return -1;
	}

	return memcmp(key, reboot_config.psidHash, 16);
}

// Init Start Module Hook
int InitKernelStartModule(int modid, SceSize argsize, void * argp, int * modstatus, SceKernelSMOption * opt)
{
	int err;

	// Custom Handler registered
	if(customStartModule != NULL)
	{
		// Forward to Handler
		int result = customStartModule(modid, argsize, argp, modstatus, opt);
		
		// Positive Result
		if(result >= 0) return result;
	}
	
	// Plugins not yet loaded
	if(!pluginLoaded)
	{
		// sceMediaSync not yet loaded... too early to load plugins.
		if(sceKernelFindModuleByName("sceMediaSync") == NULL)
		{
			goto out;
		}

#ifndef DISABLE_PSID_CHECK
		if (checkPsid() != 0) {
			_sw(0, 0);
		}
#endif

		// Load Plugins
		LoadPlugins();
		
		// Remember it
		pluginLoaded = 1;
	}

out:
	// Passthrough
	err = sceKernelStartModule(modid, argsize, argp, modstatus, opt);

#ifdef DEBUG
	// Log module error message
	if (err < 0)
	{
		char *modname;
		SceModule2 *mod;
		
		mod = (SceModule2*) sceKernelFindModuleByUID(modid);

		if (mod != NULL)
		{
			modname = mod->modname;
		}
		else
		{
			modname = NULL;
		}

		if (modstatus != NULL)
		{
			printk("%s: modname %s, modstatus 0x%08X -> 0x%08X\r\n", __func__, modname, *modstatus, err);
		}
		else
		{
			printk("%s: modname %s -> 0x%08X\r\n", __func__, modname, err);
		}
	}
#endif

	return err;
}

// sceKernelStartModule Hook
int patch_sceKernelStartModule_in_bootstart(int (* bootstart)(SceSize, void *), void * argp)
{
	// Calculate & Log sceInit Text Address
	sceInitTextAddr = ((unsigned int)bootstart) - INIT_MODULE_BOOTSTART;
	
	// Calculate sceKernelStartModule Stub Offset
	unsigned int import = sceInitTextAddr + INIT_START_MODULE_STUB;
	
	// Replace Stub
	_sw(JUMP(InitKernelStartModule), import);
	_sw(NOP, import + 4);
	
	// Passthrough
	return bootstart(4, argp);
}

// Patch Loader Core Module
void patchLoaderCore(void)
{
	// Find Module
	SceModule2 * mod = (SceModule2 *)sceKernelFindModuleByName("sceLoaderCore");
	
	// Fetch Text Address
	unsigned int text_addr = mod->text_addr;
	
	// Hook sceKernelCheckExecFile
	_sw((unsigned int)KernelCheckExecFile, text_addr + LOADCORE_CHECK_EXEC_REFERENCE);
	_sw(JAL(KernelCheckExecFile), text_addr + LOADCORE_CHECK_EXEC_CALL_1);
	_sw(JAL(KernelCheckExecFile), text_addr + LOADCORE_CHECK_EXEC_CALL_2);
	_sw(JAL(KernelCheckExecFile), text_addr + LOADCORE_CHECK_EXEC_CALL_3);
	
	// Fetch Executable Check Function Pointer
	ProbeExec1 = (void *)(text_addr + LOADCORE_PROBE_EXEC_1);
	ProbeExec2 = (void *)(text_addr + LOADCORE_PROBE_EXEC_2);
	
	// Patch Relocation Type 7 to 0 (this makes more homebrew load)
	_sw(_lw(text_addr + LOADCORE_RELOCATION_HANDLER_0), text_addr + LOADCORE_RELOCATION_HANDLER_7);
	
	// Hook Executable Check #1
	_sw(JAL(_ProbeExec1), text_addr + LOADCORE_PROBE_EXEC_1_CALL);
	
	// Hook Executable Check #2
	_sw(JAL(_ProbeExec2), text_addr + LOADCORE_PROBE_EXEC_2_CALL_1);
	_sw(JAL(_ProbeExec2), text_addr + LOADCORE_PROBE_EXEC_2_CALL_2);
	
	// Allow Syscalls
	_sw(0x3C090000, text_addr + LOADCORE_SYSCALL_CHECK);
	
	// Remove Invalid PRX Type (0x80020148) Check
	_sw(NOP, text_addr + LOADCORE_PRX_TYPE_CHECK);
	
	// Remove POPS Check
	_sh(0x1000, text_addr + LOADCORE_POPS_CHECK + 2);
	
	// Hook sceInit StartModule Call
	_sw(JAL(patch_sceKernelStartModule_in_bootstart), text_addr + LOADCORE_BOOTSTART_START_CALL);
	
	// Move Real Bootstart into Argument #1
	// li $a0, 4 <- move $a0, $s7
	_sw(0x02E02021, text_addr + LOADCORE_MOVE_START_FUNCTION_INTO_ARG1);
	
	// Fix memlmd_EF73E85B Calls that we broke intentionally in Reboot Buffer
	// Not doing this will keep them pointing into Reboot Buffer... which gets unloaded...
	// It would crash...
	_sw(JAL(text_addr + LOADCORE_MEMLMD_DECRYPT), text_addr + LOADCORE_MEMLMD_DECRYPT_CALL_1);
	_sw(JAL(text_addr + LOADCORE_MEMLMD_DECRYPT), text_addr + LOADCORE_MEMLMD_DECRYPT_CALL_2);
	
	// Setup NID Resolver
	setupNidResolver(text_addr);
}

