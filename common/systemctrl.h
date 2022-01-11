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

#ifndef _SYSTEMCTRL_H_
#define _SYSTEMCTRL_H_

#include <pspsdk.h>
#include <psploadexec_kernel.h>
#include <pspiofilemgr_kernel.h>
#include <pspthreadman_kernel.h>
#include <psploadcore.h>
#include "module2.h"

#define GAME_ID_MINIMUM_BUFFER_SIZE 10

// Function Name Clones (of old but gold times)
#define sctrlKernelQuerySystemCall sceKernelQuerySystemCall

// Prologue Module Start Handler
typedef void (* STMOD_HANDLER)(SceModule2 *);

// Thread Context
typedef struct SceThreadContext SceThreadContext;

// Find Import Library Stub Table
SceLibraryStubTable * findImportLib(SceModule2 * pMod, char * library);

// Find Import Function Stub Address
unsigned int findImportByNID(SceModule2 * pMod, char * library, unsigned int nid);

// Replace Import Function Stub
int hookImportByNID(SceModule2 * pMod, char * library, unsigned int nid, void * func);

// Set User Level
int sctrlKernelSetUserLevel(int level);

// Set System Firmware Version
int sctrlKernelSetDevkitVersion(int version);

// Get HEN Version
int sctrlHENGetVersion();

// Get HEN Minor Version
int sctrlHENGetMinorVersion();

// Find Filesystem Driver
PspIoDrv * sctrlHENFindDriver(char * drvname);

// Replace Function in Syscall Table
void sctrlHENPatchSyscall(void * addr, void * newaddr);

// Dword Poker (relative to module text_addr)
int sctrlPatchModule(char * modname, unsigned int inst, unsigned int offset);

// Text Address Getter
unsigned int sctrlModuleTextAddr(char * modname);

// Load Execute Module via Kernel Internal Function
int sctrlKernelLoadExecVSHWithApitype(int apitype, const char * file, struct SceKernelLoadExecVSHParam * param);

// Register Prologue Module Start Handler
STMOD_HANDLER sctrlHENSetStartModuleHandler(STMOD_HANDLER new_handler);

// Return Reboot Configuration UMD File
char * sctrlSEGetUmdFile(void);

// Set Reboot Configuration UMD File
void sctrlSESetUmdFile(char * file);

// Find Function Address
unsigned int sctrlHENFindFunction(char * szMod, char * szLib, unsigned int nid);

// Calculate Random Number via KIRK
unsigned int sctrlKernelRand(void);

// Enable or Disable NID Resolver for Library
int sctrlKernelSetNidResolver(char * libname, unsigned int enabled);

// Set Init Apitype
int sctrlKernelSetInitApitype(int apitype);

// Set Init Filename
int sctrlKernelSetInitFileName(char * filename);

// Return Text Address of init.prx
unsigned int sctrlGetInitTextAddr(void);

// Register Custom init.prx sceKernelStartModule Handler
void sctrlSetCustomStartModule(int (* func)(int modid, SceSize argsize, void * argp, int * modstatus, SceKernelSMOption * opt));

// Return Game Product ID of currently running Game
int sctrlKernelGetGameID(char gameid[GAME_ID_MINIMUM_BUFFER_SIZE]);

// Find UID of the specified Thread
int sctrlGetThreadUIDByName(const char * name);

// Return Thread Context of specified Thread (Search by UID)
int sctrlGetThreadContextByUID(int uid, SceKernelThreadKInfo * ctx);

// Return Thread Context of specified Thread (Search by Name)
int sctrlGetThreadContextByName(const char * name, SceKernelThreadKInfo * ctx);

// Flush Instruction and Data Cache
void flushCache(void);

// Missing PSPSDK Functions
int sceKernelQuerySystemCall(void * function);

#endif

