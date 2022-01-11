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

#include <offsets.h>

// sceReboot Main Function
int (* sceReboot)(int, int, int, int) = (void *)(REBOOT_TEXT + REBOOT_MAIN);

// Instruction Cache Invalidator
void (* _sceRebootIcacheInvalidateAll)(void) = (void *)(REBOOT_TEXT + REBOOT_ICACHE_FLUSH_ALL);

// Data Cache Invalidator
void (* _sceRebootDacheWritebackInvalidateAll)(void) = (void *)(REBOOT_TEXT + REBOOT_DCACHE_FLUSH_ALL);

// Sony PRX Decrypter Function Pointer
int (* SonyPRXDecrypt)(void *, unsigned int, unsigned int *) = NULL;

int (*pspemuLfatOpen)(char **filename, int unk) = (void *)(REBOOT_TEXT + REBOOT_LFAT_OPEN);
