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
#include <systemctrl.h>
#include <string.h>
#include <macros.h>
#include <module2.h>
#include <offsets.h>
#include "imports.h"
#include "elf.h"
#include "rc4.h"
#include "rebootconfig.h"
#include "psid.h"

// Memlmd Decrypt Function
int (* memlmdDecrypt)(unsigned char * prx, unsigned int size, unsigned int * newsize, unsigned int use_polling) = NULL;
int (* mesgledDecrypt)(unsigned int * tag, unsigned char * key, unsigned int code, unsigned char * prx, unsigned int size, unsigned int * newsize, unsigned int use_polling, unsigned char * blacklist, unsigned int blacklistsize, unsigned int type, unsigned char * xor_key1, unsigned char * xor_key2);

// Known GZIP Compression Tags
unsigned int compressTag[] = {
	0x28796DAA,
	0x7316308C,
	0x3EAD0AEE,
	0x8555ABF2,
	0xC6BA41D3,
	0x55668D96,
	0xC01DB15D,
};

// PRX Tag Compression Check
int isTagCompressed(unsigned int tag)
{
	// Iterate Supported GZIP Tags
	int i = 0;

	for (; i < NELEMS(compressTag); ++i) {
		// Matching Tag found
		if (compressTag[i] == tag) {
			// GZIP PRX
			return 1;
		}
	}

	// Encrypted PRX
	return 0;
}

// PRX Compression Check
int isPrxCompressed(unsigned char * prx, unsigned int size)
{
	// Minimum PRX Size Check
	if (size < 0x160)
		return 0;
	
	// GZIP Magic detected
	if (*(unsigned short *)(prx + 0x150) == 0x8B1F) {
		// Supported Compression Tag
		if (isTagCompressed(*(unsigned int *)(prx + 0x130))) {
			// GZIP PRX
			return 1;
		}
	}
	
	// Encrypted PRX
	return 0;
}

int psidHash(const unsigned char psid[16], unsigned char out[16])
{
	static unsigned char salt[20] __attribute__((aligned(64)));
	int err;

	_sw(PSID_SALT_MAGIC, (unsigned int)salt);
	memcpy(salt+4, psid, 16);
	err = sceKernelUtilsSha1Digest(salt, sizeof(salt), salt);

	if (err >= 0)
		memcpy(out, salt, 16);

	return err;
}

int getPsid(unsigned char psid[16])
{
	int (*_sceOpenPSIDGetOpenPSID)(unsigned char psid[0x10]), err;

	_sceOpenPSIDGetOpenPSID = (void*)sctrlHENFindFunction("sceOpenPSID_Service", "sceOpenPSID_driver", 0xC69BEBCE);
	memset(psid, 0, 16);
	err = (*_sceOpenPSIDGetOpenPSID)(psid);

	return err;
}

void getPsidHashFromReboot(unsigned char key[16])
{
	memcpy(key, reboot_config.psidHash, 16);
}

// Memlmd Decrypt Function Hook
int _memlmdDecrypt(unsigned char * prx, unsigned int size, unsigned int * newsize, unsigned int use_polling)
{
	// Valid Parameters
	if (prx != NULL && newsize != NULL) {
		// Read GZIP Payload Size
		unsigned int compsize = *(unsigned int*)(prx + 0xB0);

		if (isPrxEncrypted(prx, size)) {
			unsigned char key[16];
			void *rc4;

			getPsidHashFromReboot(key);
			prxXorKeyMix(key, sizeof(key), key, prx+0x80);

			rc4 = rc4_initstate(key, sizeof(key));
			rc4_process_stream(rc4, prx+0x150, compsize);
			rc4_destroystate(rc4);
			goto unzip;
		}
		
		// GZIP Compressed PRX
		if (isPrxCompressed(prx, size)) {
unzip:
			// Remove PRX Header
			memcpy(prx, prx + 0x150, compsize);
			
			// Write GZIP Payload Size
			*newsize = compsize;
			
			// Return Decrypt Success
			return 0;
		}
	}
	
	// Passthrough
	return memlmdDecrypt(prx, size, newsize, use_polling);
}

// Mesgled Decrypt Function Hook
int _mesgledDecrypt(unsigned int * tag, unsigned char * key, unsigned int code, unsigned char * prx, unsigned int size, unsigned int * newsize, unsigned int use_polling, unsigned char * blacklist, unsigned int blacklistsize, unsigned int type, unsigned char * xor_key1, unsigned char * xor_key2)
{
	// Valid Parameters
	if (prx != NULL && newsize != NULL) {
		// Read GZIP Payload Size from PRX Header
		unsigned int compsize = *(unsigned int *)(prx + 0xB0);

		if (isPrxEncrypted(prx, size)) {
			unsigned char key[16];
			void *rc4;

			getPsidHashFromReboot(key);
			prxXorKeyMix(key, sizeof(key), key, prx+0x80);

			rc4 = rc4_initstate(key, sizeof(key));
			rc4_process_stream(rc4, prx+0x150, compsize);
			rc4_destroystate(rc4);

			goto unzip;
		}
		
		// GZIP Compressed PRX
		if (isPrxCompressed(prx, size)) {
unzip:
			// Remove PRX Header
			memcpy(prx, prx + 0x150, compsize);
			
			// Write GZIP Payload Size
			*newsize = compsize;
			
			// Return Decrypt Success
			return 0;
		}
	}
	
	// Passthrough
	return mesgledDecrypt(tag, key, code, prx, size, newsize, use_polling, blacklist, blacklistsize, type, xor_key1, xor_key2);
}

// Patch Memlmd Cryptography
void patchMemlmd(void)
{
	// Find Module
	SceModule2 * mod = (SceModule2*)sceKernelFindModuleByName("sceMemlmd");
	
	// Fetch Text Address
	unsigned int text_addr = mod->text_addr;
	
	// Backup Decrypt Function Pointer
	memlmdDecrypt = (void*)(text_addr + MEMLMD_DECRYPT);
	
	// Hook Decrypt Function Calls
	_sw(JAL(_memlmdDecrypt), text_addr + MEMLMD_DECRYPT_CALL_1);
	_sw(JAL(_memlmdDecrypt), text_addr + MEMLMD_DECRYPT_CALL_2);	
}

// Patch MesgLed Cryptography
void patchMesgLed(unsigned int text_addr)
{
	// Save Original Decrypt Function Pointer
	mesgledDecrypt = (void*)(text_addr + MESGLED_DECRYPT);
	
	// Hook Decrypt Function Calls
	_sw(JAL(_mesgledDecrypt), text_addr + MESGLED_DECRYPT_CALL_1);
	_sw(JAL(_mesgledDecrypt), text_addr + MESGLED_DECRYPT_CALL_2);
	_sw(JAL(_mesgledDecrypt), text_addr + MESGLED_DECRYPT_CALL_3);
	_sw(JAL(_mesgledDecrypt), text_addr + MESGLED_DECRYPT_CALL_4);
	_sw(JAL(_mesgledDecrypt), text_addr + MESGLED_DECRYPT_CALL_5);
	_sw(JAL(_mesgledDecrypt), text_addr + MESGLED_DECRYPT_CALL_6);
}
