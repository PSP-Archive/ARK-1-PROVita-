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
#include <stdio.h>
#include <string.h>
#include <offsets.h>
#include <macros.h>
#include "module2.h"

// Interrupt Manager Patch
void patchInterruptMan(void)
{
	// Find Module
	SceModule2 * mod = (SceModule2 *)sceKernelFindModuleByName("sceInterruptManager");
	
	// Fetch Text Address
	unsigned int text_addr = mod->text_addr;
	
	// Override Endless Loop of breaking Death with EPC = t7
	_sw(0x408F7000, text_addr + INTERRUPTMAN_BREAKING_LOOP_OF_DEATH);
	_sw(NOP, text_addr + INTERRUPTMAN_BREAKING_LOOP_OF_DEATH + 4);
	
	// Prevent Hardware Register Writing
	_sw(NOP, text_addr + INTERRUPTMAN_HARDWARE_REGISTER_WRITE_BC000000);
	_sw(NOP, text_addr + INTERRUPTMAN_HARDWARE_REGISTER_WRITE_BC000004);
}

