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
#include "../common/offsets.h"
#include "../libs/debug/graphics.h"
#include "smartstub.h"

static int isValidUserAddr(u32 addr, u32 size)
{
	u32 baseAddr = GAME_TEXT;
	u32 endAddr = USER_BASE + USER_SIZE;

	if(addr & 3)
	{
		return 0;
	}
	
	if(addr < baseAddr || addr + size >= endAddr)
	{
		return 0;
	}

	return 1;
}

static u32 searchMemory(u32 startAddr, u32 endAddr, u32 value)
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

static int testModuleInfo(SceModuleInfo *info)
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

	if(!isValidUserAddr((u32)info->ent_top, 8))
	{
		return 0;
	}

	// check syslib
	if(_lw((u32)info->ent_top) != 0 || _lw((u32)info->ent_top + 4) != 0x80000000)
	{
		return 0;
	}

	return 1;
}

int smartStubStart(SmartStub *pSmartStub)
{
	pSmartStub->moduleInfoAddr = 0;
	pSmartStub->baseAddr = pSmartStub->startAddr = GAME_TEXT;
	pSmartStub->endAddr = USER_BASE + USER_SIZE;

	return 0;
}

static void smartStubSearch4Level(SmartStub *pSmartStub, u32 syslibExportAddr)
{
	u32 moduleInfoAddr;
	u32 startAddr = pSmartStub->baseAddr;

	while(startAddr < pSmartStub->endAddr)
	{
		moduleInfoAddr = searchMemory(startAddr, pSmartStub->endAddr, syslibExportAddr);

		if(moduleInfoAddr == 0)
			break;

		moduleInfoAddr -= 0x24;

		if(!isValidUserAddr(moduleInfoAddr, sizeof(SceModuleInfo)))
			goto next;

		if(testModuleInfo((SceModuleInfo*)moduleInfoAddr))
		{
			pSmartStub->moduleInfoAddr = moduleInfoAddr;
			break;
		}

next:
		startAddr = moduleInfoAddr + 0x24 + 4;
	}
}

static void smartStubSearch3Level(SmartStub *pSmartStub, u32 nidAddr)
{
	u32 syslibExport; 
	u32 startAddr = pSmartStub->baseAddr;

	while(startAddr < pSmartStub->endAddr)
	{
		syslibExport = searchMemory(startAddr, pSmartStub->endAddr, nidAddr);

		if(syslibExport == 0)
			break;

		syslibExport -= 12;

		if(!isValidUserAddr(syslibExport, 16)) // sizeof(_PspLibraryEntry)
		{
			goto next;
		}

		smartStubSearch4Level(pSmartStub, syslibExport);

		if(pSmartStub->moduleInfoAddr != 0)
			break;
next:
		startAddr = syslibExport + 12 + 4;
	}
}

// for each nid entry try to find syslib export table addr
static void smartStubSearch2Level(SmartStub *pSmartStub, u32 nidAddr)
{
	// try module_info first
	smartStubSearch3Level(pSmartStub, nidAddr);

	if(pSmartStub->moduleInfoAddr != 0)
		return;

	nidAddr -= 4;

	while(isValidUserAddr(nidAddr, 4) && _lw(nidAddr) != 0)
	{
		if(_lw(nidAddr) == 0xD632ACDB) /* module_start */
		{
			smartStubSearch3Level(pSmartStub, nidAddr);
			break;
		}

		nidAddr -= 4;
	}
}

int smartStubSearch(SmartStub *pSmartStub)
{
	u32 startAddr;
	u32 moduleInfoNidAddr = 0;

	pSmartStub->moduleInfoAddr = 0;
	startAddr = pSmartStub->startAddr;

	while(startAddr < pSmartStub->endAddr)
	{
		moduleInfoNidAddr = searchMemory(startAddr, pSmartStub->endAddr, 0xF01D73A7);

		if(moduleInfoNidAddr == 0)
			break;

		// try to find a valid module_info struct
		smartStubSearch2Level(pSmartStub, moduleInfoNidAddr);

		if(pSmartStub->moduleInfoAddr != 0)
			break;

		// failed
		startAddr = moduleInfoNidAddr + 4;
	}

	if(pSmartStub->moduleInfoAddr == 0)
	{
		return -1;
	}

	if(moduleInfoNidAddr != 0)
		pSmartStub->startAddr = moduleInfoNidAddr + 4;

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
static unsigned int findImportByNID(SceModuleInfo * pMod, char * library, unsigned int nid)
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

void* smartStubGetFunctionAddress(SmartStub *stub, char *libname, u32 nid)
{
	SceModuleInfo *info;

	info = (SceModuleInfo *)stub->moduleInfoAddr;

	if(info == NULL)
	{
		return NULL;
	}

	u32 addr = findImportByNID(info, libname, nid);

	return (void*)addr;
}

void *smartStubSearchWholeMemory(char *libname, u32 nid)
{
	SmartStub stub;
	int ret;
	void *addr = NULL;

	smartStubStart(&stub);
	ret = smartStubSearch(&stub);

	while(ret >= 0)
	{
		addr = smartStubGetFunctionAddress(&stub, libname, nid);

		if(addr != NULL)
			break;

		ret = smartStubSearch(&stub);
	}

	return addr;
}

int smartStubSearchLibrary(SmartStub *stub, char *libname)
{
	int ret;
	SceLibraryStubTable * pImp;

	do {
		ret = smartStubSearch(stub);

		if(ret < 0)
			return ret;

		pImp = findImportLib((SceModuleInfo *)stub->moduleInfoAddr, libname);
	} while (pImp == NULL);

	return 0;
}
