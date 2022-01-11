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

#ifndef _OFFSETS_H_
#define _OFFSETS_H_

#include "gameinfo.h"
#include "gameexploitconfig.h"
#include <kermit.h>

// Generic Offsets
#define USER_BASE 0x08800000
#define KERNEL_BASE 0x88000000
#define GAME_TEXT (USER_BASE + 0x4000)
#define SYSMEM_TEXT KERNEL_BASE
#define REBOOT_TEXT (KERNEL_BASE + 0x600000)
#define REBOOTEX_TEXT (KERNEL_BASE + 0xFC0000)
#define FLASH_SONY 0x8B000000

// Memory Partition Size
#define USER_SIZE (24 * 1024 * 1024)
#define KERNEL_SIZE (4 * 1024 * 1024)
#define FLASH_SIZE 0x01000000

// Certificate Loading Exploit
#define CERTLOADER_BUF_OFFSET 0x5C
#define CERTLOADER_STRING_VULNERABILITY 0x490A0D30
#define CERTLOADER_WRITE_ADDR_OFFSET 0x440
#define CERTLOADER_BUF_SIZE 0x938

// 1.61 Firmware Offsets -------------------------------------------------------
#if FIRMWARE == 161

#include "offsets_161.h"

// 1.67 Firmware Offsets -------------------------------------------------------
#elif FIRMWARE == 167

#include "offsets_167.h"

// 1.69 Firmware Offsets -------------------------------------------------------
#elif FIRMWARE == 169

#include "offsets_169.h"

// 1.80 Firmware Offsets -------------------------------------------------------
#elif FIRMWARE == 180

#include "offsets_180.h"

// 2.00 Firmware Offsets -------------------------------------------------------
#elif FIRMWARE == 200

#include "offsets_200.h"

// 2.10 Firmware Offsets -------------------------------------------------------
#elif FIRMWARE == 210

#include "offsets_210.h"

// 2.60 Firmware Offsets -------------------------------------------------------
#elif FIRMWARE == 260

#include "offsets_260.h"

#elif FIRMWARE == 300

#include "offsets_300.h"

#elif FIRMWARE == 310

#include "offsets_310.h"

#else

#error TODO Add Firmware Offsets

#endif

#endif

