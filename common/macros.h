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

#ifndef _MACROS_H_
#define _MACROS_H_

// Kernelify Address
#define KERNELIFY(f) (0x80000000 | ((unsigned int)(f)))

// j addr
#define JUMP(f) (0x08000000 | (((unsigned int)(f) >> 2) & 0x03ffffff))

// j addr getter (for kernel range, use in combination with KERNELIFY, works with j & jal)
#define JUMP_TARGET(i) (((unsigned int)(i) & 0x03ffffff) << 2)

// jal addr
#define JAL(f) (0x0C000000 | (((unsigned int)(f) >> 2) & 0x03ffffff))

// jal checker
#define IS_JAL(i) ((((unsigned int)i) & 0xFC000000) == 0x0C000000)

// syscall number
#define SYSCALL(n) ((n<<6)|12)

// nop
#define NOP 0

// jr ra
#define JR_RA 0x03E00008

// v0 result setter
#define LI_V0(n) ((0x2402 << 16) | ((n) & 0xFFFF))

// Array Element Counter
#define NELEMS(n) ((sizeof(n)) / sizeof(n[0]))

// Min & Max Macros
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define UNUSED(arg) ((void)(arg))

#endif

