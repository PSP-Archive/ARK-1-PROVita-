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
#include <pspiofilemgr.h>
#include <psputility_modules.h>
#include <pspumd.h>
#include <pspctrl.h>
#include <module2.h>
#include <macros.h>
#include <string.h>
#include <pspmoduleinfo.h>
#include <psploadcore.h>
#include "common/offsets.h"
#include "libs/debug/graphics.h"
#include "smartstub.h"

/* A smart stub finder in user mode */

void* _smartStubGetFunctionAddress(SceModuleInfo *info, char *libname, u32 nid);

u32 searchMemory(u32 startAddr, u32 endAddr, u32 value)
{
	u32 i;

	for(i=startAddr; i<endAddr; i+=4)
	{
		if(_lw(i) == value)
		{
			return i;
		}
	}

	return 0;
}

int testModuleInfo(SceModuleInfo *info)
{
	// Bad attribute
	if(info->modattribute & PSP_MODULE_KERNEL)
	{
		return 0;
	}

	// Bad ent table and stub table
	if(info->ent_top >= info->ent_end || info->stub_top >= info->stub_end)
	{
		return 0;
	}

	struct TestFunction {
		char *name;
		u32 nid;
	};

	struct TestFunction funcs[] = {
		{ "IoFileMgrForUser", 0x109F50BC },
		{ "IoFileMgrForUser", 0x6A638D83 },
		{ "IoFileMgrForUser", 0x810C4BC3 },
		{ "sceUtility", 0x2A2B3DE0 },
		{ "sceUtility", 0xE49BFE92 },
	};

	int i; for(i=0; i<NELEMS(funcs); ++i)
	{
		if(NULL == _smartStubGetFunctionAddress(info, funcs[i].name, funcs[i].nid))
		{
			return 0;
		}
	}

	return 1;
}

int smartStubStart(SmartStub *pSmartStub)
{
	u32 baseAddr, startAddr;
	u32 endAddr = USER_BASE + USER_SIZE;
	u32 moduleInfoAddr = 0;
	pSmartStub->moduleInfoAddr = moduleInfoAddr;

	baseAddr = startAddr = GAME_TEXT;

	while ( 1 ) 
	{
		u32 module_start_nid = searchMemory(startAddr, endAddr, 0xD632ACDB);

		if(module_start_nid == 0)
			break;

		// Get syslib address
		// module_start is the first exported function in first exported library
		u32 expAddr = searchMemory(baseAddr, endAddr, module_start_nid);

		if(expAddr == 0)
		{
			goto next;
		}

		expAddr -= 12;

		moduleInfoAddr = searchMemory(baseAddr, endAddr, expAddr);

		if(moduleInfoAddr == 0)
		{
			goto next;
		}

		moduleInfoAddr -= 0x24;

		if(moduleInfoAddr < baseAddr || moduleInfoAddr >= endAddr)
		{
			goto next;
		}

		if(testModuleInfo((void*)moduleInfoAddr))
		{
			break;
		}

next:
		moduleInfoAddr = 0;
		// failed
		startAddr = module_start_nid + 4;
	}

	if (moduleInfoAddr == 0)
	{
		return -1;
	}

	pSmartStub->moduleInfoAddr= moduleInfoAddr;

	return 0;
}

// Find Import Library Pointer
SceLibraryStubTable * findImportLib(SceModuleInfo * pMod, char * library)
{
	// Invalid Arguments
	if(pMod == NULL || library == NULL) return NULL;
	
	// Import Stub Table Start Address
	void * stubTab = pMod->stub_top;
	
	// Iterate Stubs
	int i = 0; while(i < pMod->stub_end - pMod->stub_top)
	{
		// Cast Import Table
		SceLibraryStubTable * pImp = (SceLibraryStubTable *)(stubTab + i);
		
		// Matching Library discovered
		if(pImp->libname != NULL && strcmp(pImp->libname, library) == 0)
		{
			// Return Address
			return pImp;
		}
		
		// Move Pointer
		i += pImp->len * 4;
	}
	
	// Import Library not found
	return NULL;
}

// Find Import Stub Address
unsigned int findImportByNID(SceModuleInfo * pMod, char * library, unsigned int nid)
{
	// Find Import Library
	SceLibraryStubTable * pImp = findImportLib(pMod, library);
	
	// Found Import Library
	if(pImp != NULL)
	{
		// Iterate Imports
		int i = 0; for(; i < pImp->stubcount; i++)
		{
			// Matching Function NID
			if(pImp->nidtable[i] == nid)
			{
				// Return Function Stub Address
				return (unsigned int)(pImp->stubtable + 8 * i);
			}
		}
	}
	
	// Import Stub not found
	return 0;
}

void* _smartStubGetFunctionAddress(SceModuleInfo *info, char *libname, u32 nid)
{
	if(info == NULL)
	{
		return NULL;
	}

	u32 addr = findImportByNID(info, libname, nid);

	return (void*)addr;
}

void* smartStubGetFunctionAddress(SmartStub *stub, char *libname, u32 nid)
{
	return _smartStubGetFunctionAddress((SceModuleInfo *)stub->moduleInfoAddr, libname, nid);
}

void smartStubEnd2(SmartStub *stub)
{
	stub->moduleInfoAddr = 0;
}
