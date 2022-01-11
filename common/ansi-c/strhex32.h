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

#ifndef _ANSI_C_STRHEX32_H_
#define _ANSI_C_STRHEX32_H_

/*
 * The ANSI-C Headers are ment for a ONE-TIME-INCLUSION into a lowlevel
 * executable like the Exploit Payload or the Reboot Buffer!
 *
 * For other, more highlevel, applications like PRX Modules,
 * please use the Kernel Libc or PSPSDK / Newlib Libc.
 */

// Lightweight String Conversion for Debug Screen Output (not threadsafe)
char * hex32(unsigned int v)
{
	// Result String
	static char result[9];
	
	// Iterate Nibbles
	unsigned int i = 0; for(; i < 8; i++)
	{
		// Fetch Nibble
		char nibble = (char)((v >> (i << 2)) & 0xF);
		
		// Number
		if(nibble >= 0 && nibble <= 9) nibble += '0';
		
		// Character
		else nibble += 'A' - 0xA;
		
		// Copy Character
		result[7 - i] = nibble;
	}
	
	// Terminate String
	result[8] = 0;
	
	// Return String
	return result;
}

#endif

