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
#include "rc4.h"
#include "psid.h"
#include "encryptloader.h"

#define MAX_READ_BUF_SIZE 0x8000

int encLoader(char *filename, void * loadAddr, EncryptLoader *loader) __attribute__((section(".text.startup")));

void decrypt(unsigned char psidHash[20], unsigned char *srcbuf, const unsigned int size)
{
	void *rc4;
	unsigned char key[16];

	prxXorKeyMix(key, sizeof(key), psidHash, srcbuf + size);

	rc4 = rc4_initstate(key, sizeof(key));
	rc4_process_stream(rc4, srcbuf, size);
	rc4_destroystate(rc4);
}

#if 0
void dumpFile(char *filename, void *memory, int size, FunctionTable *table)
{
	SceUID fd = (*table->IoOpen)(filename, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	if(fd >= 0)
	{
		(*table->IoWrite)(fd, memory, size);
		(*table->IoClose)(fd);
	}
}
#endif

int encLoader(char *filename, void * loadAddr, EncryptLoader *loader)
{
	SceUID fd;
	int (*entryAddress)(FunctionTable *tbl, unsigned char psid[16], unsigned char psidHash[20], const int printStateY) = (void*) loadAddr;
	int size;
	FunctionTable *table = &loader->ft;

	fd = (*table->IoOpen)(filename, PSP_O_RDONLY, 0777);

	if(fd < 0)
	{
		return fd;
	}

	void *realLoadAddr = loadAddr;

	size = (*table->IoRead)(fd, realLoadAddr, MAX_READ_BUF_SIZE);
	(*table->IoClose)(fd);

	if(size < 0)
	{
		return size;
	}

	if(size < 0x10)
	{
		return -1;
	}

	size -= 0x10;

#if 0
    fd = (*table->IoOpen)(GAME_SAVE_PATH "K_ENC.BIN", PSP_O_WRONLY | PSP_O_CREAT, 0777);
    if (fd >0)
    {
        (*table->IoWrite)(fd, realLoadAddr, size);
        (*table->IoClose)(fd);
    }
#endif

	decrypt(loader->psidHash, realLoadAddr, size);
	memcpy(loadAddr, realLoadAddr, size);
	(*table->KernelDcacheWritebackAll)();

#if 0
    fd = (*table->IoOpen)(GAME_SAVE_PATH "K_DEC.BIN", PSP_O_WRONLY | PSP_O_CREAT, 0777);
    if (fd >0)
    {
        (*table->IoWrite)(fd, loadAddr, size);
        (*table->IoClose)(fd);
    }

    PRTSTR0("entryAddress()");
#endif

	return entryAddress(table, loader->psid, loader->psidHash, loader->printStateY);
}
