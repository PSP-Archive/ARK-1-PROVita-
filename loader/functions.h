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

#ifndef _GAME_FUNCTIONS_H_
#define _GAME_FUNCTIONS_H_

#include "offsets.h"

/*
 * These Mappings are Syscall Mappings! Don't bother trying them inside
 * of a Kernel Permission Context! You will crash!
 */

// EBOOT.BIN Stubs Mapping
extern void (* KernelLibcTime)(int, int, int, int, int);
extern void (* KernelDelayThread)(uint32_t);
extern int (* IoOpen)(char *, int, int);
extern int (* IoRead)(int, void *, int);
extern int (* IoWrite)(int, void *, int);
extern int (* IoClose)(int);
extern void (* KernelDcacheWritebackAll)(void);
extern int (* SysMemUserForUser_91DE343C)( void *unk );
extern int (* DisplaySetFrameBuf)(void *topaddr, int bufferwidth, int pixelformat, int sync);

// Optional Stubs #ifdef GAME_UTILITY_LOAD_MODULE
extern int (* UtilityLoadModule)(int);
extern int (* UtilityLoadNetModule)(int);

extern int (* KernelDeleteVpl)(int);
extern int (* KernelDeleteFpl)(int);

extern int (* UtilityUnloadModule)(int);
extern int (* UtilityUnloadNetModule)(int);

extern int (* KernelFreePartitionMemory)(int);
extern int (* LoadCertFromFlash)(int, int, int, int, int, int);
extern int (* OpenPSIDGetOpenPSID)(unsigned char psid[0x10]);

/*
 * These functions are ment for using when initial kernel access has been
 * granted, for example through the mean of a kernel exploit.
 */

// iofilemgr.prx Functions
extern int (* KernelIOOpen)(char *, int, int);
extern int (* KernelIOWrite)(int, void *, unsigned);
extern int (* KernelIORead)(int, void *, unsigned);
extern int (* KernelIOClose)(int);
extern int (* KernelIODopen)(char *);
extern int (* KernelIODread)(int, SceIoDirent *);
extern int (* KernelIODclose)(int);

// sysmem.prx Functions
extern void (* _sceKernelIcacheInvalidateAll)(void);
extern void (* _sceKernelDcacheWritebackInvalidateAll)(void);
extern int (* _sceKernelGzipDecompress)(unsigned char *dest, unsigned int destSize, const unsigned char *src, void *unknown);
extern void (* _sceKernelDcacheInvalidateRange)(const void *p, unsigned int size);

// loadcore.prx Functions
extern SceModule2 * (* _sceKernelFindModuleByName)(char *);

// threadman.prx Functions
extern SceUID (* _sceKernelCreateThread)(const char *name, SceKernelThreadEntry entry,\
		int initPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam *option);
extern int (* _sceKernelStartThread)(SceUID thid, SceSize arglen, void *argp);

extern int (* WlanGetEtherAddr)(unsigned char *destAddr);

extern int (* _sceKermit_driver_4F75AA05)(KermitPacket *packet, u32 cmd_mode, u32 cmd, u32 argc, u32 allow_callback, u64 *resp);

// Free Game Heap Memory
void freeVPL(void);

// Free Game Heap Memory
void freeFPL(void);

// Free UserSbrk Heap Memory
void freeUserSbrk(void);

// Unload Networking Modules
void unloadNetModules(void);

// Unload Utilities
void unloadModules(void);

// Repair damaged instruction in kxploing
void repairInstruction(void);

void initKernelThread(void);

void flashVitaDump( char *packName );

#endif
