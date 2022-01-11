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
#include <systemctrl.h>
#include <offsets.h>
#include "missingfunc.h"
#include "nidresolver.h"

// Original NID Filler
int (* g_origNIDFiller)(void *lib, unsigned int nid, unsigned int unk2, unsigned int unk3) = NULL;

// NID Table
NidResolverLib * nidTable = NULL;
unsigned int nidTableSize = 0;

// Get NID Resolver Library
NidResolverLib * getNidResolverLib(const char * libName)
{
	// Iterate NID Table
	int i = 0; for(; i < nidTableSize; ++i)
	{
		// Found Matching Library
		if(nidTable[i].enabled && 0 == strcmp(libName, nidTable[i].name))
		{
			// Return Library Reference
			return &nidTable[i];
		}
	}
	
	// Library not found or disabled
	return NULL;
}

// Speedy NID Search
static int binarySearch(const NidResolverEntry * nids, unsigned int n, unsigned int oldNid)
{
	// Lowest, Highest and Middle Position
	int low = 0;
	int high = n - 1;
	int mid = 0;
	
	// Search Loop
	while (low <= high)
	{
		// Calculate Middle
		mid = (low + high) / 2;
		
		// Found Matching NID
		if(oldNid == nids[mid].old)
		{
			// Return Index
			return mid;
		}
		
		// Target has to be "before" this one...
		else if(oldNid < nids[mid].old)
		{
			// Set Maximum Index
			high = mid - 1;
		}
		
		// Target has to be "after" this one...
		else
		{
			// Set Minimum Index
			low = mid + 1;
		}
	}
	
	// NID not found
	return -1;
}

// Resolve NID
unsigned int getNidReplacement(const NidResolverLib *lib, unsigned int nid)
{
	// Find NID Index
	int i = binarySearch(lib->nidtable, lib->nidcount, nid);
	
	// Found NID Index
	if(i >= 0) 
	{
		// Get New NID
		unsigned int newNid = lib->nidtable[i].new;
		
		// Valid NID
		if(newNid != UNKNOWNNID) return newNid;
	}
	
	// Return Original NID
	return nid;
}

NidMissingEntry nidMissEntrySysclib[] =
{
	{ 0x89B79CB1, (unsigned int)ownstrcspn,  },
	{ 0x62AE052F, (unsigned int)ownstrspn,   },
	{ 0x87F8D2DA, (unsigned int)ownstrtok,   },
	{ 0x1AB53A58, (unsigned int)ownstrtok_r, },
	{ 0xD3D1A3B9, (unsigned int)strncat,     },
	{ 0x1D83F344, (unsigned int)ownstrtol,   },
	{ 0x909C228B, (unsigned int)&ownsetjmp,  }, // setjmp
	{ 0x18FE80DB, (unsigned int)&ownlongjmp, }, // longjmp
};

NidMissingResolver missing_SysclibForKernel =
{
	"SysclibForKernel",
	nidMissEntrySysclib,
	NELEMS(nidMissEntrySysclib),
};

/////////////////////////////////////////////////////////////////////////

NidMissingEntry missing_LoadCoreForKernel_entries[] =
{
	{ 0xD8779AC6, 0, },
	{ 0x2952F5AC, 0, }, // @sceLoaderCore@ + 0x7298
};

NidMissingResolver missing_LoadCoreForKernel =
{
	"LoadCoreForKernel",
	missing_LoadCoreForKernel_entries,
	NELEMS(missing_LoadCoreForKernel_entries),
};

/////////////////////////////////////////////////////////////////////////

NidMissingEntry missing_sceSyscon_driver_entries[] =
{
	{ 0xC8439C57, 0, }
};

NidMissingResolver missing_sceSyscon_driver =
{
	"sceSyscon_driver",
	missing_sceSyscon_driver_entries,
	NELEMS(missing_sceSyscon_driver_entries),
};

NidMissingEntry missing_scePower_driver_entries[] =
{
	{ 0x737486F2 , 0, } // @scePower_Service@ + 0x3678
};

NidMissingResolver missing_scePower_driver =
{
	"scePower_driver",
	missing_scePower_driver_entries,
	NELEMS(missing_scePower_driver_entries),
};

/////////////////////////////////////////////////////////////////////////

NidMissingResolver *g_missing_resolver[] =
{
	&missing_SysclibForKernel,
	&missing_LoadCoreForKernel,
	&missing_sceSyscon_driver,
	&missing_scePower_driver,
};

/////////////////////////////////////////////////////////////////////////

// Missing NID Resolver
unsigned int resolveMissingNid(const char * libName, unsigned int nid)
{
	// Iterate Missing Library Resolver
	int i = 0; for(; i < NELEMS(g_missing_resolver); ++i)
	{
		// Fetch Resolver
		NidMissingResolver * cur = g_missing_resolver[i];
		
		// Matching Library
		if(0 == strcmp(cur->libname, libName))
		{
			// Iterate NIDs
			int j = 0; for(; j < cur->size; ++j)
			{
				// Matching NID
				if(nid == cur->entry[j].nid)
				{
					// Return Function Pointer
					return cur->entry[j].fp;
				}
			}
			
			// Stop Searching
			break;
		}
	}
	
	// Unimplemented NID
	return 0;
}

// Fill Library Stubs
int fillLibraryStubs(void * lib, unsigned int nid, void * stub, unsigned int nidPos)
{
	// Result
	int result = 0;
	
	// Calculate Stub Destination Address
	unsigned int dest = nidPos * 8 + _lw((unsigned int)stub + 24);
	
	// ???
	if(_lw((unsigned int)stub+52) != 0) goto exit;
	
	// Find Version
	unsigned int * version = (unsigned int *)sctrlHENFindFunction((void *)_lw((unsigned int)stub+36), NULL, 0x11B97506);
	
	// Invalid Version
	if(version != NULL && (*version >> 16) == 0x0606) goto exit;
	
	// Get Module Name
	const char * name = (const char *)_lw((unsigned int)lib + 68);
	
	// Resolve Missing NID
	unsigned int targetAddress = resolveMissingNid(name, nid);
	
	// Missing Function
	if(targetAddress != 0)
	{
		// Write Stub
		_sw(JUMP(targetAddress), dest);
		_sw(NOP, dest + 4);
		
		// Early Exit
		return -1;
	}
	
	// Get Library Resolver
	NidResolverLib * resolver = getNidResolverLib(name);
	
	// Got Library Resolver
	if(resolver != NULL)
	{
		// Resolve NID
		nid = getNidReplacement(resolver, nid);
	}
	
exit:
	// Forward Call
	result = g_origNIDFiller(lib, nid, -1, 0);
	
	// Original NID Filler failed
	if(result < 0)
	{
		// Store Dummy Stub
		_sw(SYSCALL(0x15), dest);
		_sw(NOP, dest + 4);
		
		// Early Exit
		return -1;
	}
	
	// Return Result
	return result;
}

// Find still existing, but no longer exported Functions
void getMissingNidAddress(void)
{
	// sceLoaderCore Functions
	missing_LoadCoreForKernel_entries[0].fp = sctrlModuleTextAddr("sceLoaderCore") + 0x72D8;
}

// Create Sorted NID List
static void NidInsertSort(NidResolverEntry * base, int n, int (* cmp)(const NidResolverEntry *, const NidResolverEntry *))
{
	// Temporary NID Resolver Entry for Swapping
	NidResolverEntry saved;
	
	// Iterate NIDs
	int j = 1; for(; j < n; ++j)
	{
		// Previous Index
		int i = j - 1;
		
		// Fetch NID Resolver Entry
		NidResolverEntry * value = &base[j];
		
		// Compare Data
		while(i >= 0 && cmp(&base[i], value) > 0) i--;
		
		// Alread sorted
		if(++i == j) continue;
		
		// Exchange NID Resolver Entries
		memmove(&saved, value, sizeof(saved));
		memmove(&base[i + 1], &base[i], sizeof(saved) * (j - i));
		memmove(&base[i], &saved, sizeof(saved));
	}
}

// NID Compare Algorithm for Binary Sorting
static int NidCompare(const NidResolverEntry * nid_a, const NidResolverEntry * nid_b)
{
	// Smaller
	if(nid_a->old < nid_b->old) return 0;

	// Bigger or Equal
	return 1;
}

// Sorted Table Setup
static void NidSortTable(NidResolverLib *table, unsigned int size)
{
	// Iterate Resolver Libraries
	unsigned int i = 0; for(; i < size; ++i)
	{
		// Insert into Sorted List
		NidInsertSort(table[i].nidtable, table[i].nidcount, &NidCompare);
	}
}

// Setup NID Resolver in sceLoaderCore
void setupNidResolver(unsigned int text_addr)
{
	// Link 660 NID Resolver Table
	nidTable = nidTable660;
	nidTableSize = nidTableSize660;
	
	// Binary Sort Table
	NidSortTable(nidTable, nidTableSize);
	
	// Fetch missing NIDs
	getMissingNidAddress();
	
	// Backup Original NID Filler Function Pointer
	g_origNIDFiller = (void *)(text_addr + LOADCORE_NID_FILLER);
	
	// li   $a2, -1 <- move $a2, $s1 ; s1 = stub
	_sw(0x02203021, text_addr + LOADCORE_NID_FILLER_CALL - 4);
	
	// jal  sub_00000EA8 <- jal fillLibraryStubs
	_sw(JAL(fillLibraryStubs), text_addr + LOADCORE_NID_FILLER_CALL);
	
	// move $a3, $zr <- move $a3, $s2 ; s2 = nidPos
	_sw(0x02403821, text_addr + LOADCORE_NID_FILLER_CALL + 4);
	
	// Don't write Syscall 0x15 if Resolve failed
	_sw(NOP, text_addr + LOADCORE_NID_SYSCALL15_PART_1);
	_sw(NOP, text_addr + LOADCORE_NID_SYSCALL15_PART_2);
}

