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
#include <pspiofilemgr.h>
#include <string.h>
#include <macros.h>
#include <offsets.h>
#include "elf.h"
#include "modulemanager.h"

// Module Start Handler
void (* g_module_start_handler)(SceModule2 *) = NULL;

// Partition Check Function
int (* realPartitionCheck)(unsigned int *, unsigned int *) = NULL;

// Internal Module Manager Apitype Field
int * kernel_init_apitype = NULL;

// Internal Module Manager Init Filename Field
char ** kernel_init_filename = NULL;

// Internal Module Manager Init Application Type Field
int * kernel_init_application_type = NULL;

// Prologue Module Function
int (* prologue_module)(void *, SceModule2 *) = NULL;

// Real Executable Check Function
int sceKernelCheckExecFile(unsigned char * buffer, int * check);

// Function Prototypes
int _PartitionCheck(unsigned int * st0, unsigned int * check);
int prologue_module_hook(void * unk0, SceModule2 * mod);
int _sceKernelCheckExecFile(unsigned char * buffer, int * check);
int PatchExec1(unsigned char * buffer, int * check);
int PatchExec2(unsigned char * buffer, int * check);
int PatchExec3(unsigned char * buffer, int * check, int isplain, int checkresult);

int (* ProbeExec3)(unsigned char * buffer, int * check) = NULL;

int _ProbeExec3(unsigned char * buffer, int * check)
{
	//check executable
	int result = ProbeExec3(buffer, check);

	//grab executable magic
	unsigned int magic = *(unsigned int *)(buffer);

	//patch necessary
	if(check[2] >= 0x52 && magic == 0x464C457F && IsStaticElf(buffer))
	{
		//patch check
		check[8] = 3;

		//fake result
		result = 0;
	}

	return result;
}

// sceModuleManager Patch
void patchModuleManager()
{
	// Find Module
	SceModule2 * mod = (SceModule2*)sceKernelFindModuleByName("sceModuleManager");
	
	// Get Functions
	realPartitionCheck = (void *)(mod->text_addr + MODULEMGR_PARTITION_CHECK);
	prologue_module = (void *)(mod->text_addr + MODULEMGR_PROLOGUE_MODULE);
	
	// Get Module Manager Internal Fields (writing to these changes the attribute)
	kernel_init_apitype = (int *)(mod->text_addr + MODULEMGR_INIT_APITYPE_FIELD);
	kernel_init_filename = (char **)(mod->text_addr + MODULEMGR_INIT_FILENAME_FIELD);
	kernel_init_application_type = (int *)(mod->text_addr + MODULEMGR_INIT_APPLICATION_TYPE_FIELD);
	
	// Remove Device Checks
	_sw(0x24020000, mod->text_addr + MODULEMGR_DEVICE_CHECK_1);
	_sw(0x24020000, mod->text_addr + MODULEMGR_DEVICE_CHECK_2);
	_sw(NOP, mod->text_addr + MODULEMGR_DEVICE_CHECK_3);
	_sw(NOP, mod->text_addr + MODULEMGR_DEVICE_CHECK_4);
	_sh(0x1000, mod->text_addr + MODULEMGR_DEVICE_CHECK_5);
	_sw(NOP, mod->text_addr + MODULEMGR_DEVICE_CHECK_6);
	_sw(NOP, mod->text_addr + MODULEMGR_DEVICE_CHECK_7);
	_sh(0x1000, mod->text_addr + MODULEMGR_DEVICE_CHECK_8);
	
	// Replace sceKernelCheckExecFile Import Stub
	_sw(JUMP(_sceKernelCheckExecFile), mod->text_addr + MODULEMGR_CHECK_EXEC_STUB);
	
	// Patch Partition Check Calls
	_sw(JAL(_PartitionCheck), mod->text_addr + MODULEMGR_PARTITION_CHECK_CALL_1);
	_sw(JAL(_PartitionCheck), mod->text_addr + MODULEMGR_PARTITION_CHECK_CALL_2);
	
	// Patch Prologue Module Call
	_sw(JAL(prologue_module_hook), mod->text_addr + MODULEMGR_PROLOGUE_MODULE_CALL);

	ProbeExec3 = (void*)mod->text_addr + MODULEMGR_PROBE_EXEC_3;
	_sw(JAL(_ProbeExec3), mod->text_addr + MODULEMGR_PROBE_EXEC_3_CALL);
}

// Partition Check
int _PartitionCheck(unsigned int * st0, unsigned int * check)
{
	// Get File Descriptor
	SceUID fd = st0[6];
	
	// Allocate Buffer on Stack
	unsigned int p[64 + 64 / sizeof(unsigned int)];
	
	// Align Buffer
	unsigned int * checkBuf = (unsigned int*)((((unsigned int)p) & ~(64-1)) + 64);
	
	// Module Attributes
	unsigned short attributes = 0;
	
	// Invalid File Descriptor
	if(fd < 0) return realPartitionCheck(st0, check);
	
	// Backup File Position
	SceOff pos = sceIoLseek(fd, 0, PSP_SEEK_CUR);
	
	// Invalid File Position
	if(pos < 0) return realPartitionCheck(st0, check);
	
	// Rewind to File Start
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	
	// Insufficient Header Read Size
	if(sceIoRead(fd, checkBuf, 256) < 256)
	{
		// Restore Position
		sceIoLseek(fd, pos, PSP_SEEK_SET);
		
		// Fallback Check
		return realPartitionCheck(st0, check);
	}
	
	// PBP File
	if(checkBuf[0] == 0x50425000)
	{
		// Move to Executable
		sceIoLseek(fd, checkBuf[8], PSP_SEEK_SET);
		
		// Read ELF Header
		sceIoRead(fd, checkBuf, 20);
		
		// Encrypted Module
		if(checkBuf[0] != 0x464C457F)
		{
			// Restore Position
			sceIoLseek(fd, pos, PSP_SEEK_SET);
			
			// Original Check
			return realPartitionCheck(st0, check);
		}
		
		// Move to Module Information
		sceIoLseek(fd, checkBuf[8] + check[19], PSP_SEEK_SET);
		
		// Valid PRX File (it's relocateable)
		if(!IsStaticElf(checkBuf))
		{
			// Allow PSAR Files inside EBOOT.PBP Files
			check[4] = checkBuf[9] - checkBuf[8];
		}
	}
	
	// ELF File
	else if(checkBuf[0] == 0x464C457F)
	{
		// Move to Module Information
		sceIoLseek(fd, check[19], PSP_SEEK_SET);
	}
	
	// We don't know what this is... let's assume it's encrypted.
	else
	{
		// Restore Position
		sceIoLseek(fd, pos, PSP_SEEK_SET);
		
		// Fallback Check
		return realPartitionCheck(st0, check);
	}
	
	// Read Module Attributes
	sceIoRead(fd, &attributes, 2);
	
	// Static ELF File
	if(IsStaticElf(checkBuf)) check[17] = 0;
	
	// PRX File (relocateable)
	else
	{
		// Kernel PRX
		if(attributes & 0x1000) check[0x44/4] = 1;

		// User PRX
		else check[0x44/4] = 0;
	}
	
	// Restore Position
	sceIoLseek(fd, pos, PSP_SEEK_SET);
	
	// Passthrough
	return realPartitionCheck(st0, check);
}

// Prologue Module Hook
int prologue_module_hook(void * unk0, SceModule2 * mod)
{
	// Forward Call
	int result = prologue_module(unk0, mod);
	
	// Load Success
	if(result >= 0)
	{
		// TODO Think about high memory unlocking on Vita lateron...
		
		// Module Start Handler
		if(g_module_start_handler != NULL)
		{
			// Call Module Start Handler
			g_module_start_handler(mod);
		}
	}
	
	// Return Result
	return result;
}

// sceKernelCheckExecFile Hook
int _sceKernelCheckExecFile(unsigned char * buffer, int * check)
{
	// PatchExec1
	int result = PatchExec1(buffer, check);
	
	// PatchExec1 isn't enough... :(
	if(result != 0)
	{
		// Forward to sceKernelCheckExecFile
		int checkresult = sceKernelCheckExecFile(buffer, check);
		
		// Grab Executable Magic
		unsigned int magic = *(unsigned int *)(buffer);
		
		// PatchExec3
		result = PatchExec3(buffer, check, magic == 0x464C457F, checkresult);
	}
	
	// Return Result
	return result;
}

// PatchExec1
int PatchExec1(unsigned char * buffer, int * check)
{
	// Grab Magic
	unsigned int magic = *(unsigned int *)(buffer);
	
	// Invalid Magic
	if(magic != 0x464C457F) return -1;
	
	// Possibly Invalid Apitype
	if(check[2] < 0x120)
	{
		// Custom Apitype
		if(check[2] < 0x52)
		{
			if(check[17] != 0)
			{
				check[18] = 1;
				return 0;
			}
			
			else return -2;
		}
		
		// Invalid Apitype
		else return -1;
	}
	
	// Predefined Apitypes
	else if(check[2] == 0x120 || (check[2] >= 0x140 && check[2] <= 0x143))
	{
		if(check[4] != 0)
		{
			check[17] = 1;
			check[18] = 1;
			
			//PatchExec2
			PatchExec2(buffer, check);
			
			return 0;
		}
		
		else if(check[17] != 0)
		{
			check[18] = 1;
			return 0;
		}
		
		else
		{
		   	return -1;
		}
	}
	
	// Invalid Apitype
	else return -1;
}

// PatchExec2
int PatchExec2(unsigned char * buffer, int * check)
{
	// Patching Result
	int result = 0;
	
	// Find Buffer Index
	int index = (check[19] < 0) ? (check[19] + 3) : (check[19]);
	
	// Exclude Volatile Memory Range from Patching
	unsigned int address = (unsigned int)(buffer + index);
	if(!(address >= 0x88400000 && address <= 0x88800000))
	{
		check[22] = *(unsigned short *)(buffer + index);
		result = *(int *)(buffer + index);
	}
	
	// Return Result
	return result;
}

// PatchExec3
int PatchExec3(unsigned char * buffer, int * check, int isplain, int checkresult)
{
	// ELF Executable
	if(isplain)
	{
		// Custom Apitype
		if(check[2] < 0x52)
		{
			// PatchExec2
			if(PatchExec2(buffer, check) & 0xFF00)
			{
				check[17] = 1;
				checkresult = 0;
			}
		}
		
		// Predefined Apitype
		else
		{
			// Unrelocateable ELF
			if(IsStaticElf(buffer))
			{
				check[8] = 3;
			}
		}
	}
	
	// Return Result
	return checkresult;
}
