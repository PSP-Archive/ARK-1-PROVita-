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
#include <rebootconfig.h>
#include <systemctrl_se.h>
#include <string.h>
#include "functions.h"
#include "common/ansi-c/functions.h"
#include "libs/debug/graphics.h"
#include "sha1sum.h"
#include "rc4.h"
#include "psid.h"
#include "encLoader/encLoader.h"
#include "encLoader/encryptloader.h"

#ifdef ENABLE_SMARTSTUB
#include "libs/smartstub/smartstub.h"
#endif
 
#ifdef RELEASE
#include "release.h"
#endif

int loaderEntry(void) __attribute__((section(".text.startup")));
void clearBSS(void);

int stubScanner(void);

#ifndef RELEASE
#ifdef PSID_DUMP
void writePsid(unsigned char psid[16]);
#endif
int getPsid(unsigned char psid[16]);
int (* OpenPSIDGetOpenPSID)(unsigned char psid[0x10]) = NULL;
#endif

unsigned char * calcHash(const unsigned char *key, unsigned char *hash);

FunctionTable *g_tbl = NULL;

struct StubList {
	void **addr;
	char *name;
	unsigned int nid;
};

#ifdef ENABLE_SMARTSTUB
void getSmartStub(FunctionTable *tbl, struct StubList *stubList, int stubSize)
{
	SmartStub stub;
	int i;
	
	smartStubStart(&stub);
	smartStubSearch(&stub);
	
	for(i=0; i<stubSize; ++i)
	{
		if(*stubList[i].addr == NULL)
		{
			*stubList[i].addr = smartStubGetFunctionAddress(&stub, stubList[i].name, stubList[i].nid);
		}
	}

	// try sceKernelDcacheWritebackInvalidateAll instead if sceKernelDcacheWritebackAll not presented
	if(tbl->KernelDcacheWritebackAll == NULL)
	{
		tbl->KernelDcacheWritebackAll = smartStubGetFunctionAddress(&stub, "UtilsForUser", 0xB435DEC5);
	}
}

void dumpSmartStub(struct StubList *stubList, int stubSize)
{
	int i, fd;

	fd = g_tbl->IoOpen(GAME_SAVE_PATH "smart.log", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if(fd < 0)
		return;

	for(i=0; i<stubSize; ++i)
	{
		unsigned len;
		
		len = strlen(stubList[i].name);

		g_tbl->IoWrite(fd, &len, sizeof(len));
		g_tbl->IoWrite(fd, stubList[i].name, len);
		g_tbl->IoWrite(fd, &stubList[i].nid, sizeof(stubList[i].nid));

		len = 0;

		if(stubList[i].addr != NULL)
			g_tbl->IoWrite(fd, stubList[i].addr, sizeof(*stubList[i].addr));
		else
			g_tbl->IoWrite(fd, &len, sizeof(len));
	}

	g_tbl->IoClose(fd);
}
#endif

#ifndef ENABLE_SMARTSTUB
void getHardcodedOffsets(FunctionTable *tbl)
{
	// UNO JPN
#if GAME == NPJH00016
	tbl->IoOpen = (void*) 0x088FD46C;
	tbl->IoRead = (void*) 0x088FD444;
	tbl->IoWrite = (void*) 0x088FD43C;
	tbl->IoClose = (void*) 0x088FD454;
	tbl->KernelLibcTime = (void*) 0x088FD634;
	tbl->KernelDcacheWritebackAll = (void*) 0x088FD61C;
	tbl->KernelCreateThread = (void*) 0x088FD5A4;
	tbl->KernelDelayThread = (void*) 0x088FD52C;
	tbl->KernelStartThread = (void*) 0x088FD564;
	tbl->KernelWaitThreadEnd = (void*) 0x088FD574;
	tbl->KernelDeleteVpl = (void*) 0x08BCC364;
	tbl->KernelDeleteFpl = (void*) 0x088FD54C;
	tbl->UtilityLoadModule = (void*) 0x088FDBAC;
	tbl->UtilityUnloadModule = (void*) 0x088FDC3C;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x088FD504;
	tbl->KernelFreePartitionMemory = (void*) 0x088FD514;
	tbl->DisplaySetFrameBuf = (void*) 0x088FD6F4;
#elif GAME == NPHG00038
	tbl->IoOpen = (void*) 0x089944A4;
	tbl->IoRead = (void*) 0x0899446C;
	tbl->IoWrite = (void*) 0x08994464;
	tbl->IoClose = (void*) 0x08994484;
	tbl->KernelLibcTime = (void*) 0x089942CC;
	tbl->KernelDcacheWritebackAll = (void*) 0x089942BC;
	tbl->KernelCreateThread = (void*) 0x0899434C;
	tbl->KernelDelayThread = (void*) 0x089942DC;
	tbl->KernelStartThread = (void*) 0x0899430C;
	tbl->KernelWaitThreadEnd = (void*) 0x0899431C;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x0899416C;
	tbl->UtilityUnloadModule = (void*) 0x00000000;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x00000000;
	tbl->KernelFreePartitionMemory = (void*) 0x089943E4;
	tbl->DisplaySetFrameBuf = (void*) 0x0899426C;
	// UNO USA
#elif GAME == NPUH10027
	tbl->IoOpen = (void*) 0x088FCEA4;
	tbl->IoRead = (void*) 0x088FCE7C;
	tbl->IoWrite = (void*) 0x088FCE74;
	tbl->IoClose = (void*) 0x088FCE8C;
	tbl->KernelLibcTime = (void*) 0x088FD06C;
	tbl->KernelDcacheWritebackAll = (void*) 0x088FD054;
	tbl->KernelCreateThread = (void*) 0x088FCFDC;
	tbl->KernelDelayThread = (void*) 0x088FCF64;
	tbl->KernelStartThread = (void*) 0x088FCF9C;
	tbl->KernelWaitThreadEnd = (void*) 0x088FCFAC;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x088FCF84;
	tbl->UtilityLoadModule = (void*) 0x088FD5E4;
	tbl->UtilityUnloadModule = (void*) 0x088FD674;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x088FCF3C;
	tbl->KernelFreePartitionMemory = (void*) 0x088FCF4C;
	tbl->DisplaySetFrameBuf = (void*) 0x088FD12C;
	// Arcade Darts EUR
#elif GAME == NPEZ00053
	tbl->IoOpen = (void*) 0x089B6204;
	tbl->IoRead = (void*) 0x089B61DC;
	tbl->IoWrite = (void*) 0x089B61D4;
	tbl->IoClose = (void*) 0x089B61EC;
	tbl->KernelLibcTime = (void*) 0x089B608C;
	tbl->KernelDcacheWritebackAll = (void*) 0x089B607C;
	tbl->KernelCreateThread = (void*) 0x089B60EC;
	tbl->KernelDelayThread = (void*) 0x089B6094;
	tbl->KernelStartThread = (void*) 0x089B60BC;
	tbl->KernelWaitThreadEnd = (void*) 0x089B612C;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x089B62C4;
	tbl->UtilityUnloadModule = (void*) 0x089B62EC;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x089B6154;
	tbl->KernelFreePartitionMemory = (void*) 0x089B6164;
	tbl->DisplaySetFrameBuf = (void*) 0x089B6294;
	// Half Minute Hero EUR
#elif GAME == ULES01359
	tbl->IoOpen = (void*) 0x0892B184;
	tbl->IoRead = (void*) 0x0892B154;
	tbl->IoWrite = (void*) 0x0892B14C;
	tbl->IoClose = (void*) 0x0892B164;
	tbl->KernelLibcTime = (void*) 0x0892B37C;
	tbl->KernelDcacheWritebackAll = (void*) 0x0892B36C;
	tbl->KernelCreateThread = (void*) 0x0892B2D4;
	tbl->KernelDelayThread = (void*) 0x0892B244;
	tbl->KernelStartThread = (void*) 0x0892B274;
	tbl->KernelWaitThreadEnd = (void*) 0x0892B284;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x0892B42C;
	tbl->UtilityUnloadModule = (void*) 0x0892B48C;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x0892B224;
	tbl->KernelFreePartitionMemory = (void*) 0x0892B234;
	tbl->DisplaySetFrameBuf = (void*) 0x0892B3CC;
	// UNO EUR
#elif GAME == NPEH00020
	tbl->IoOpen = (void*) 0x088FCEE0;
	tbl->IoRead = (void*) 0x088FCEB8;
	tbl->IoWrite = (void*) 0x088FCEB0;
	tbl->IoClose = (void*) 0x088FCEC8;
	tbl->KernelLibcTime = (void*) 0x088FD0A8;
	tbl->KernelDcacheWritebackAll = (void*) 0x088FD090;
	tbl->KernelCreateThread = (void*) 0x088FD018;
	tbl->KernelDelayThread = (void*) 0x088FCFA0;
	tbl->KernelStartThread = (void*) 0x088FCFD8;
	tbl->KernelWaitThreadEnd = (void*) 0x088FCFE8;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x088FCFC0;
	tbl->UtilityLoadModule = (void*) 0x088FD620;
	tbl->UtilityUnloadModule = (void*) 0x088FD6B0;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x088FCF78;
	tbl->KernelFreePartitionMemory = (void*) 0x088FCF88;
	tbl->DisplaySetFrameBuf = (void*) 0x088FD168;
	// Pool Hall Pro USA
#elif GAME == NPUH10091
	tbl->IoOpen = (void*) 0x08925BA4;
	tbl->IoRead = (void*) 0x08925B7C;
	tbl->IoWrite = (void*) 0x08925B74;
	tbl->IoClose = (void*) 0x08925B8C;
	tbl->KernelLibcTime = (void*) 0x08925A04;
	tbl->KernelDcacheWritebackAll = (void*) 0x089259F4;
	tbl->KernelCreateThread = (void*) 0x08925A5C;
	tbl->KernelDelayThread = (void*) 0x08925A0C;
	tbl->KernelStartThread = (void*) 0x08925A34;
	tbl->KernelWaitThreadEnd = (void*) 0x08925AA4;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x08925C74;
	tbl->UtilityUnloadModule = (void*) 0x08925CA4;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x00000000;
	tbl->KernelFreePartitionMemory = (void*) 0x08925AE4;
	tbl->DisplaySetFrameBuf = (void*) 0x08925C3C;
	// World Of Pool EU
#elif GAME == ULES00821
	tbl->IoOpen = (void*) 0x0891B9C0;
	tbl->IoRead = (void*) 0x0891B980;
	tbl->IoWrite = (void*) 0x0891B978;
	tbl->IoClose = (void*) 0x0891B988;
	tbl->KernelLibcTime = (void*) 0x0891BB48;
	tbl->KernelDcacheWritebackAll = (void*) 0x0891BB30;
	tbl->KernelCreateThread = (void*) 0x0891BA98;
	tbl->KernelDelayThread = (void*) 0x0891BAA8;
	tbl->KernelStartThread = (void*) 0x0891BA90;
	tbl->KernelWaitThreadEnd = (void*) 0x0891BB08;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x0891BE00;
	tbl->UtilityUnloadModule = (void*) 0x0891BE28;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x00000000;
	tbl->KernelFreePartitionMemory = (void*) 0x0891BA88;
	tbl->DisplaySetFrameBuf = (void*) 0x0891BBF8;
	// Urbanix US
#elif GAME == NPUZ00077
	tbl->IoOpen = (void*) 0x08977BB0;
	tbl->IoRead = (void*) 0x08977B88;
	tbl->IoWrite = (void*) 0x08977B78;
	tbl->IoClose = (void*) 0x08977B98;
	tbl->KernelLibcTime = (void*) 0x08977D70;
	tbl->KernelDcacheWritebackAll = (void*) 0x08977D60;
	tbl->KernelCreateThread = (void*) 0x08977CE8;
	tbl->KernelDelayThread = (void*) 0x08977C78;
	tbl->KernelStartThread = (void*) 0x08977CA8;
	tbl->KernelWaitThreadEnd = (void*) 0x08977CB8;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x08977EC8;
	tbl->UtilityUnloadModule = (void*) 0x08977F30;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x00000000;
	tbl->KernelFreePartitionMemory = (void*) 0x08977C70;
	tbl->DisplaySetFrameBuf = (void*) 0x08977E20;
	// Widgets Odyssey 2 EUR
#elif GAME == NPEZ00149
	tbl->IoOpen = (void*) 0x088B7024;
	tbl->IoRead = (void*) 0x088B6FF4;
	tbl->IoWrite = (void*) 0x088B6FE4;
	tbl->IoClose = (void*) 0x088B7004;
	tbl->KernelLibcTime = (void*) 0x088B6E04;
	tbl->KernelDcacheWritebackAll = (void*) 0x088B6DF4;
	tbl->KernelCreateThread = (void*) 0x088B6E8C;
	tbl->KernelDelayThread = (void*) 0x088B6E14;
	tbl->KernelStartThread = (void*) 0x088B6E44;
	tbl->KernelWaitThreadEnd = (void*) 0x088B6E4C;
	tbl->KernelDeleteVpl = (void*) 0x088B6EF4;
	tbl->KernelDeleteFpl = (void*) 0x00000000;
	tbl->UtilityLoadModule = (void*) 0x088B7324;
	tbl->UtilityUnloadModule = (void*) 0x088B737C;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x088B6F3C;
	tbl->KernelFreePartitionMemory = (void*) 0x088B6F4C;
	tbl->DisplaySetFrameBuf = (void*) 0x088B6DBC;
#elif GAME == NPUZ00014
	tbl->IoOpen = (void*) 0x08873FF0;
	tbl->IoRead = (void*) 0x08873FC0;
	tbl->IoWrite = (void*) 0x08873FB8;
	tbl->IoClose = (void*) 0x08873FD0;
	tbl->KernelLibcTime = (void*) 0x08873E18;
	tbl->KernelDcacheWritebackAll = (void*) 0x08873E00;
	tbl->KernelCreateThread = (void*) 0x08873EB0;
	tbl->KernelDelayThread = (void*) 0x08873E28;
	tbl->KernelStartThread = (void*) 0x08873E68;
	tbl->KernelWaitThreadEnd = (void*) 0x08873E70;
	tbl->KernelDeleteVpl = (void*) 0x00000000;
	tbl->KernelDeleteFpl = (void*) 0x08873E58;
	tbl->UtilityLoadModule = (void*) 0x08874060;
	tbl->UtilityUnloadModule = (void*) 0x08874098;
	tbl->UtilityLoadNetModule = (void*) 0x00000000;
	tbl->UtilityUnloadNetModule = (void*) 0x00000000;
	tbl->SysMemUserForUser_91DE343C = (void*) 0x08873F20;
	tbl->KernelFreePartitionMemory = (void*) 0x08873F30;
	tbl->DisplaySetFrameBuf = (void*) 0x08873DC8;
#else
#error Need to define hardcoded offset for your game if without smartstub
#endif
}
#endif

void getImportedStub(FunctionTable *tbl)
{
	memset(tbl, 0, sizeof(*tbl));

#ifdef ENABLE_SMARTSTUB 
#define STUB_ENTRY(FUNCNAME, LIBNAME, NID) { (void**)&tbl->FUNCNAME, #LIBNAME, NID, }
	struct StubList stubList[] = 
	{
		STUB_ENTRY(IoOpen, IoFileMgrForUser, 0x109F50BC),
		STUB_ENTRY(IoRead, IoFileMgrForUser, 0x6A638D83),
		STUB_ENTRY(IoWrite, IoFileMgrForUser, 0x42EC03AC),
		STUB_ENTRY(IoClose, IoFileMgrForUser, 0x810C4BC3),

		STUB_ENTRY(KernelLibcTime, UtilsForUser, 0x27CC57F0),
		STUB_ENTRY(KernelDcacheWritebackAll, UtilsForUser, 0x79D1C3FA),

		STUB_ENTRY(KernelCreateThread, ThreadManForUser, 0x446D8DE6),
		STUB_ENTRY(KernelDelayThread, ThreadManForUser, 0xCEADEB47),
		STUB_ENTRY(KernelStartThread, ThreadManForUser, 0xF475845D),
		STUB_ENTRY(KernelWaitThreadEnd, ThreadManForUser, 0x278C0DF5),
		STUB_ENTRY(KernelDeleteVpl, ThreadManForUser, 0x89B3D48C),
		STUB_ENTRY(KernelDeleteFpl, ThreadManForUser, 0xED1410E0),

		STUB_ENTRY(UtilityLoadModule, sceUtility, 0x2A2B3DE0),
		STUB_ENTRY(UtilityUnloadModule, sceUtility, 0xE49BFE92),
		STUB_ENTRY(UtilityLoadNetModule, sceUtility, 0x1579A159),
		STUB_ENTRY(UtilityUnloadNetModule, sceUtility, 0x64D50C56),

		STUB_ENTRY(SysMemUserForUser_91DE343C, SysMemUserForUser, 0x91DE343C),
		STUB_ENTRY(KernelFreePartitionMemory, SysMemUserForUser, 0xB6D61D02),

		STUB_ENTRY(DisplaySetFrameBuf, sceDisplay, 0x289D82FE),
	};

#undef STUB_ENTRY
	getSmartStub(tbl, stubList, NELEMS(stubList));
#else
	getHardcodedOffsets(tbl);
#endif

	g_tbl = tbl;

#ifdef ENABLE_SMARTSTUB
	dumpSmartStub(stubList, NELEMS(stubList));
#endif
}

// Entry Point
int loaderEntry(void)
{
	EncryptLoader loader;
	EncryptLoader *ploader = &loader;
	FunctionTable *tbl = &ploader->ft;

	// Clear BSS Segment
	clearBSS();

	// Try with smartStub first
	getImportedStub(tbl);

	// Initialize Screen
	initScreen(tbl->DisplaySetFrameBuf);
	
	// Output H.BIN Reach Screen
	PRTSTR0("Reached Loader");

	if ( stubScanner() == 0 )
	{
		#ifdef RELEASE
		unsigned char release_key[16] = RELEASE_KEY;
		memcpy(ploader->psid, release_key, 16);
		#else
		getPsid(ploader->psid);
		#endif
		calcHash(ploader->psid, ploader->psidHash);

		UNUSED(size_encLoaderBuf);
		UNUSED(encLoaderBuf);
		
#ifndef PSID_DUMP
		memcpy((void*)ENCLOADER_LOADADDR, encLoaderBuf, size_encLoaderBuf);

		ploader->printStateY = getPrintStateY();

		int (*encLoader)(char *filename, void * loadAddr, EncryptLoader *ploader);
		encLoader = (void*)ENCLOADER_LOADADDR;
		
		// sync cache
		g_tbl->KernelDcacheWritebackAll();
		return encLoader(GAME_SAVE_PATH "K.BIN", (void*)EXPLOIT_LOADADDR, ploader);
#else
		writePsid(ploader->psid);
		PRTSTR0("PSID dump done.");
#endif
	}
	else
	{
		PRTSTR0("stubScanner failed.");
	}
	
	// Wait for quit using PS button
	while (1)	g_tbl->KernelDelayThread(1000000);

	return 0;
}

unsigned char * calcHash(const unsigned char *key, unsigned char *hash)
{
	// Hashing...
	_sw(PSID_SALT_MAGIC, (unsigned int)hash);
	memcpy(hash+4, key, 16);

	sha1_digest(hash, 20, hash);

	
	return hash;
}

#ifndef RELEASE
#ifdef PSID_DUMP
void writePsid(unsigned char psid[16])
{
	SceUID fd;

	fd = g_tbl->IoOpen(GAME_SAVE_PATH "H.BIN", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if(fd >= 0)
	{
		g_tbl->IoWrite(fd, psid, 16);
		g_tbl->IoClose(fd);
	}
}
#endif

// Find np_matching2.prx OpenPSIDGetOpenPSID Stub
void * FindOpenPSIDGetOpenPSID(void)
{
	// Search Start Position
	unsigned int * start = (unsigned int *)GAME_TEXT;
	
	// Search Force End Position
	unsigned int * end = (unsigned int *)(USER_BASE + USER_SIZE);
	
	// Current Position in RAM
	unsigned int * p = start;
	
	// Scan Memory
	for(; p < end - NP2_SIGNATURE_STRING / 4; p += 64)
	{
		// Found np_matching2.prx Text Address
		if (0 == strcmp(((const char*)(p + NP2_SIGNATURE_STRING / 4)), "sceNpMatching2") && \
				*(p+4) == 0x2409003D && \
				*(p+5) == 0x90AA0000 && \
				*(p+6) == 0x90AB0001 && \
				*(p+7) == 0x01485021 && \
				*(p+8) == 0x90AC0002 && \
				*(p+9) == 0x01685821 && \
				*(p+10) == 0x90AD0003
				)
			break;
	}
	
	// Didn't find it... :(
	if(p >= end - NP2_SIGNATURE_STRING / 4) return NULL;
	
	// Calculate OpenPSIDGetOpenPSID Stub Address
	p += NP2_GET_OPEN_PSID / 4;
	
	// Return it. :D
	return p;
}

int getPsid(unsigned char psid[16])
{
	memset(psid, 0, 16);
	int ret = OpenPSIDGetOpenPSID(psid);
	
	// Unload module
	#ifdef USE_GAME_UTILITY_UNLOAD_MODULE
	g_tbl->UtilityUnloadModule(PSP_MODULE_NP_MATCHING2);
	#endif

	return ret;
}
#endif

// Load required Libraries for OpenPSIDGetOpenPSID (NP_MATCHING2)
int stubScanner(void)
{
	#ifndef RELEASE
	if (OpenPSIDGetOpenPSID != NULL)
		return 0;
	#endif

	// Unload Networking Utilities
	#ifdef USE_GAME_UTILITY_UNLOAD_NET_MODULE
	unloadNetModules();
	#endif

	// Unload Utilities
	#ifdef USE_GAME_UTILITY_UNLOAD_MODULE
	unloadModules();
	#endif
	
	// Game Heap Cleanup
	#ifdef USE_GAME_KERNEL_DELETE_VPL
	freeVPL();
	#endif
	
	// Game Heap Cleanup
	#ifdef USE_GAME_KERNEL_DELETE_FPL
	freeFPL();
	#endif
	
	// Free UserSbrk Heap
	#ifdef USE_GAME_KERNEL_FREE_PARTITION_MEMORY
	freeUserSbrk();
	#endif
	
	#ifndef RELEASE
	#ifdef USE_GAME_UTILITY_LOAD_MODULE
	
	// Load Modules via sceUtilityLoadModule (if available)
	g_tbl->UtilityLoadModule(PSP_MODULE_NP_MATCHING2);
	
	#endif

	// Find OpenPSIDGetOpenPSID in Memory
	OpenPSIDGetOpenPSID = FindOpenPSIDGetOpenPSID();

	if (OpenPSIDGetOpenPSID == NULL)
		return -1;
	#endif

	return 0;
}

// Clear BSS Segment of Payload
void clearBSS(void)
{
	// BSS Start and End Address from Linkfile
	extern char __bss_start, __bss_end;
	
	// Clear Memory
	memset(&__bss_start, 0, &__bss_end - &__bss_start);
}

#ifdef USE_GAME_KERNEL_DELETE_VPL
// Free Game Heap Memory
void freeVPL(void)
{
	// Fetch Game Heap UID
	unsigned int uid = *(unsigned int *)(GAME_TEXT + GAME_VPL_UID);
	
	// Delete Game Heap
	g_tbl->KernelDeleteVpl(uid);
}
#endif

#ifdef USE_GAME_KERNEL_DELETE_FPL
// Free Game Heap Memory
void freeFPL(void)
{
	// Fetch Game Heap UID
	unsigned int uid = *(unsigned int *)(GAME_TEXT + GAME_FPL_UID);
	
	// Delete Game Heap
	g_tbl->KernelDeleteFpl(uid);
}
#endif

#ifdef USE_GAME_KERNEL_FREE_PARTITION_MEMORY
// Free UserSbrk Heap Memory
void freeUserSbrk(void)
{
	// Fetch UID
	unsigned int uid = *(unsigned int *)(GAME_TEXT + GAME_USERSBRK_UID);
	
	// Free Heap
	g_tbl->KernelFreePartitionMemory(uid);
}
#endif

#ifdef USE_GAME_UTILITY_UNLOAD_NET_MODULE
// Unload Networking Modules
void unloadNetModules(void)
{
	// Unload unneeded Networking Modules
	g_tbl->UtilityUnloadNetModule(PSP_NET_MODULE_ADHOC);
}
#endif

#ifdef USE_GAME_UTILITY_UNLOAD_MODULE
// Unload Utilities
void unloadModules(void)
{
	// Unload unneeded Modules
	g_tbl->UtilityUnloadModule(PSP_MODULE_NET_ADHOC);
	g_tbl->UtilityUnloadModule(PSP_MODULE_USB_PSPCM);
	g_tbl->UtilityUnloadModule(PSP_MODULE_USB_MIC);
	g_tbl->UtilityUnloadModule(PSP_MODULE_USB_CAM);
	g_tbl->UtilityUnloadModule(PSP_MODULE_USB_GPS);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_AVCODEC);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_SASCORE);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_ATRAC3PLUS);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_MPEGBASE);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_MP3);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_VAUDIO);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_AAC);
	g_tbl->UtilityUnloadModule(PSP_MODULE_AV_G729);
	g_tbl->UtilityUnloadModule(PSP_MODULE_NP_COMMON);
	g_tbl->UtilityUnloadModule(PSP_MODULE_NP_SERVICE);
	g_tbl->UtilityUnloadModule(PSP_MODULE_NP_MATCHING2);
	g_tbl->UtilityUnloadModule(PSP_MODULE_NP_DRM);
}
#endif
