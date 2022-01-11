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

#ifndef _CRYPTOGRAPHY_H_
#define _CRYPTOGRAPHY_H_

// Patch Memlmd Cryptography
void patchMemlmd(void);

// Patch MesgLed Cryptography
void patchMesgLed(unsigned int text_addr);

// Get PSID from OpenPSID driver
int getPsid(unsigned char psid[16]);

// Calculate PSID Hash
int psidHash(const unsigned char psid[16], unsigned char out[16]);

#endif

