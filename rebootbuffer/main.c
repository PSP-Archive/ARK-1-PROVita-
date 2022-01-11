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
#include <macros.h>
#include <rebootconfig.h>
#include <systemctrl_se.h>
#include "common/rc4.h"
#include "common/psid.h"
#include "common/rebootconfig.h"
#include "common/ansi-c/functions.h"
#include "functions.h"

// Function Prototypes
int loadcoreModuleStart(unsigned int args, void * argp, int (* start)(SceSize, void *));
void flushCache(void);
int _sceReboot(int arg1, int arg2, int arg3, int arg4) __attribute__((section(".text.startup")));
int _pspemuLfatOpen(char **filename, int unk);

// Entry Point
int _sceReboot(int arg1, int arg2, int arg3, int arg4)
{
	// TODO Parse Reboot Buffer Configuration (what to configure yet? lol)
	
	// Link Filesystem Buffer to 0x8BA00000
	// before:
	// 0x886020DC: 0x3C028B00 '...<' - lui        $v0, 0x8B00
	// after:
	// 0x886020DC: 0x3C028BA0 '...<' - lui        $v0, 0x8BA0
	_sb(0xA0, REBOOT_TEXT + REBOOT_FILESYSTEM_REPLACE_PATCH_1);
	
	// Link Filesystem Buffer to 0x8BA00000
	// before:
	// 0x886020E8: 0x3C108B00 '...<' - lui        $s0, 0x8B00
	// after:
	// 0x886020E8: 0x3C108BA0 '...<' - lui        $s0, 0x8BA0
	_sb(0xA0, REBOOT_TEXT + REBOOT_FILESYSTEM_REPLACE_PATCH_2);
	
	// Patch ~PSP Check ???
	// before:
	// 0x88602E84: 0xAFA60000 '....' - sw         $a2, 0($sp)
	// after:
	// 0x88602E84: 0xAFA50000 '....' - sw         $a1, 0($sp)
	_sw(0xAFA50000, REBOOT_TEXT + REBOOT_PSP_HEADER_CHECK_PATCH);
	
	// Returns size of the buffer on loading whatever modules
	// before:
	// 0x88602E88: 0x2403FFFF '...$' - li         $v1, -1
	// after:
	// 0x88602E88: 0x20A30000 '... ' - addi       $v1, $a1, 0
	_sw(0x20A30000, REBOOT_TEXT + REBOOT_MODULE_BUFFER_SIZE_PATCH);
	
	// Move LoadCore module_start Address into third Argument
	// before:
	// 0x886024A0: 0x00600008 '..`.' - jr         $v1
	// after:
	// 0x886024A0: 0x00603021 '!0`.' - move       $a2, $v1
	_sw(0x00603021, REBOOT_TEXT + REBOOT_LOADCORE_MOVE_ADDRESS_INTO_ARG3);
	
	// Hook LoadCore module_start Call
	// before:
	// 0x886024A8: 0x03E00008 '....' - jr         $ra
	// after:
	// 0x886024A8: 0x???????? '....' - j          loadcore_module_start
	_sw(JUMP(loadcoreModuleStart), REBOOT_TEXT + REBOOT_LOADCORE_MODULE_START_JUMP);

	// Hook _pspemuLfatOpen to switch pspbtcnf.bin
	_sw(JAL(_pspemuLfatOpen), REBOOT_TEXT + REBOOT_LFAT_OPEN_CALL);
	
	// Flush Cache
	flushCache();
	
	// Forward Call
	return sceReboot(arg1, arg2, arg3, arg4);
}

void getPsidHash(unsigned char key[16])
{
	RebootBufferConfiguration * conf = (RebootBufferConfiguration*)REBOOTEX_CONFIG;

	memcpy(key, conf->psidHash, 16);
}

// PRO GZIP Decrypt Support
int PROPRXDecrypt(void * prx, unsigned int size, unsigned int * newsize)
{
	if(isPrxEncrypted(prx, size))
	{
		unsigned int compsize = *(unsigned int*)(prx + 0xB0);
		unsigned char key[16];
		void *rc4;

		getPsidHash(key);
		prxXorKeyMix(key, sizeof(key), key, prx+0x80);
		rc4 = rc4_initstate(key, sizeof(key));
		rc4_process_stream(rc4, prx+0x150, compsize);
		rc4_destroystate(rc4);
		memcpy(prx, prx+0x150, compsize);
		*newsize = compsize;

		return 0;
	}

	// GZIP Packed PRX File
	if(_lb((unsigned)prx + 0x150) == 0x1F && _lb((unsigned)prx + 0x151) == 0x8B)
	{
		// Read GZIP Size
		unsigned int compsize = *(unsigned int *)(prx + 0xB0);
		
		// Return GZIP Size
		*newsize = compsize;
		
		// Remove PRX Header
		memcpy(prx, prx + 0x150, compsize);
		
		// Fake Decrypt Success
		return 0;
	}
	
	// Decrypt Sony PRX Files
	return SonyPRXDecrypt(prx, size, newsize);
}

// Load Core module_start Hook
int loadcoreModuleStart(unsigned int args, void * argp, int (* start)(SceSize, void *))
{
	// Calculate Text Address
	unsigned int text_addr = ((unsigned int)start) - LOADCORE_MODULE_BOOTSTART;
	
	// Hook Signcheck Function Calls
	_sw(JAL(PROPRXDecrypt), text_addr + LOADCORE_MEMLMD_DECRYPT_CALL_1);
	_sw(JAL(PROPRXDecrypt), text_addr + LOADCORE_MEMLMD_DECRYPT_CALL_2);
	
	// Fetch Original Decrypt Function Stub
	SonyPRXDecrypt = (void *)(text_addr + LOADCORE_MEMLMD_DECRYPT);
	
	// Flush Cache
	flushCache();
	
	// Forward
	return start(args, argp);
}

// Invalidate Instruction and Data Cache
void flushCache(void)
{
	// Invalidate Instruction Cache
	_sceRebootIcacheInvalidateAll();
	
	// Invalidate Data Cache
	_sceRebootDacheWritebackInvalidateAll();
}

int _pspemuLfatOpen(char **filename, int unk)
{
	if(filename != NULL && 0 == strcmp(*filename, "pspbtcnf.bin"))
	{
		RebootBufferConfiguration * conf = (RebootBufferConfiguration*)REBOOTEX_CONFIG;
		char *p = *filename;

		switch(conf->iso_mode)
		{
			case MODE_MARCH33:
				/* March33 ISO Driver */
				// pspbtmnf.bin
				p[5] = 'm';
				break;
			case MODE_UMD:
				/* Homebrew mode, use inferno */
				/* FALL THROUGH */
			case MODE_INFERNO:
				// pspbtinf.bin
				p[5] = 'i';
				break;
			case MODE_NP9660:
				// pspbtnnf.bin
				p[5] = 'n';
				break;
		}
	}

	return pspemuLfatOpen(filename, unk);
}
