/* sha1sum.c - print SHA-1 Message-Digest Algorithm 
 * Copyright (C) 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
 * Copyright (C) 2004 g10 Code GmbH
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* SHA-1 coden take from gnupg 1.3.92. 

   Note, that this is a simple tool to be used for MS Windows.
*/

#ifndef SHA1SUM
#define SHA1SUM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

//#undef BIG_ENDIAN_HOST
//typedef unsigned int u32;

/****************
 * Rotate a 32 bit integer by n bytes
 */
#if defined(__GNUC__) && defined(__i386__)
static inline u32
rol( u32 x, int n)
{
	__asm__("roll %%cl,%0"
		:"=r" (x)
		:"0" (x),"c" (n));
	return x;
}
#else
#define rol(x,n) ( ((x) << (n)) | ((x) >> (32-(n))) )
#endif


typedef struct {
    u32  h0,h1,h2,h3,h4;
    u32  nblocks;
    unsigned char buf[64];
    int  count;
} SHA1_CONTEXT;



void sha1_init( SHA1_CONTEXT *hd );


/****************
 * Transform the message X which consists of 16 32-bit-words
 */
void transform( SHA1_CONTEXT *hd, unsigned char *data );


/* Update the message digest with the contents
 * of INBUF with length INLEN.
 */
void sha1_write( SHA1_CONTEXT *hd, unsigned char *inbuf, size_t inlen);


/* The routine final terminates the computation and
 * returns the digest.
 * The handle is prepared for a new cycle, but adding bytes to the
 * handle will the destroy the returned buffer.
 * Returns: 20 bytes representing the digest.
 */

void sha1_final(SHA1_CONTEXT *hd);

unsigned char *sha1_digest (unsigned char *M, unsigned int len, unsigned char *_digest);


#endif


