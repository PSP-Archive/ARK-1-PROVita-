#include <stdlib.h>
#include "lib.h"
/*
// Sets a memory region to a specific value
void * memset(void *ptr, int c, size_t size)
{
	byte *p1 = ptr;
	byte *p2 = ptr + size;
	while(p1 != p2) {
		*p1 = c;
		p1++;
	}
    return ptr;
}

// Copies one memory buffer into another
void * memcpy(void *out, const void *in, int size)
{
	byte *pout = out;
	byte *pin = (byte *)in;
	int i;
	for(i = 0; i < size; i++) {
		*pout = *pin;
		pout++;
		pin++;
	}
    return out;
}
*/
//Scan s for the character.  When this loop is finished,
//    s will either point to the end of the string or the
//    character we were looking for
char *(strchr)(const char *s, int c)
{
	while (*s != '\0' && *s != (char)c)
		s++;
	return ( (*s == c) ? (char *) s : NULL );
}

/*
// Returns string length
int strlen(const char *text)
{
	int i = 0;
	while(*text) 
	{
		text++;
		i++;
	}
	return i;
}

// Copies string src into dest
char *strcpy(char *dest, const char *src)
{	
	while(*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	
	*dest = '\0';
	return dest;
}*/

/*
 * Basic Sprintf functions thanks to Fanjita and Noobz
 */ 

void numtohex8(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=0; i<8; i++)
   {
      dst[i]=hex[(n>>((7-i)*4))&15];
   }
}

void numtohex4(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=4; i<8; i++)
   {
      dst[i-4]=hex[(n>>((7-i)*4))&15];
   }
}

void numtohex2(char *dst, int n)
{
   int i;
   static char hex[]="0123456789ABCDEF";
   for (i=6; i<8; i++)
   {
      dst[i-6]=hex[(n>>((7-i)*4))&15];
   }
}

// limited sprintf function - avoids pulling in large library
int writeFormat(char *xibuff, const char *xifmt, u32 xidata)
{
  // check for empty format
  if (xifmt[0] == '\0')
  {
    *xibuff = '?';
    return(1);
  }
  else
  {
    if ((xifmt[0] == '0') &&
        (xifmt[1] == '8') &&
        (xifmt[2] == 'l') &&
        (xifmt[3] == 'X'))
    {
      numtohex8(xibuff, xidata);
      return(8);
    }
    else if ((xifmt[0] == '0') &&
             (xifmt[1] == '4') &&
             (xifmt[2] == 'X'))
    {
      numtohex4(xibuff, xidata);
      return(4);
    }
    else if ((xifmt[0] == '0') &&
             (xifmt[1] == '2') &&
             (xifmt[2] == 'X'))
    {
      numtohex2(xibuff, xidata);
      return(2);
    }
    else if (xifmt[0] == 'c')
    {
      *xibuff = (unsigned char)xidata;
      return(1);
    }
    else if (xifmt[0] == 's')
    {
      const char *lptr   = (const char *)xidata;
      int         lcount = 0;

      /***********************************************************************/
      /* Artificially limit %s format to 150 bytes, as a cheap way to        */
      /* avoid log buffer overflow.                                          */
      /***********************************************************************/
      while ((*lptr != 0) && (lcount < 150))
      {
        *xibuff++ = *lptr++;
        lcount++;
      }
      return(lcount);
    }
    else if (xifmt[0] == 'd')
    {
      char lbuff[15];
      int  lnumbuff = 0;
      int  lcount = 0;
      int  lchar;
      int lnum   = (int)xidata;
      if (lnum < 0)
      {
        *xibuff++ = '-';
        lcount++;
        lnum = 0 - lnum;
      }

      lchar = lnum % 10;
      lbuff[0] = lchar + 48;
      lnumbuff++;
      lnum -= lchar;

      while (lnum > 0)
      {
        lnum  = lnum / 10;
        lchar = lnum % 10;
        lbuff[lnumbuff++] = lchar + 48;
        lnum -= lchar;
      }

      while (lnumbuff > 0)
      {
        *xibuff++ = lbuff[--lnumbuff];
        lcount++;
      }

      return(lcount);
    }
    else if ((xifmt[0] == 'p'))
    {
      numtohex8(xibuff, xidata);
      return(8);
    }

    return(0);
  }
}

void mysprintf11(char *xobuff, const char *xifmt,
   u32 xidata,
   u32 xidata2,
   u32 xidata3,
   u32 xidata4,
   u32 xidata5,
   u32 xidata6,
   u32 xidata7,
   u32 xidata8,
   u32 xidata9,
   u32 xidata10,
   u32 xidata11)
{
  int  lparam = 0;
  char lfmt[10];
  char *lfmtptr;

  while (*xifmt != '\0')
  {
    if (*xifmt != '%')
    {
      *xobuff++ = *xifmt++;
    }
    else
    {
      xifmt++;  // skip the %
      lfmtptr = lfmt;
      while ((*xifmt == '0')
          || (*xifmt == '2')
          || (*xifmt == '4')
          || (*xifmt == '8')
          || (*xifmt == 'l')
          || (*xifmt == 'X')
          || (*xifmt == 'd')
          || (*xifmt == 'p')
          || (*xifmt == 's')
          || (*xifmt == 'c'))
      {
        *lfmtptr ++ = *xifmt++;
      }
      *lfmtptr = '\0';

      switch (lparam)
      {
        case 0:
          xobuff += writeFormat(xobuff, lfmt, xidata);
          break;
        case 1:
          xobuff += writeFormat(xobuff, lfmt, xidata2);
          break;
        case 2:
          xobuff += writeFormat(xobuff, lfmt, xidata3);
          break;
        case 3:
          xobuff += writeFormat(xobuff, lfmt, xidata4);
          break;
        case 4:
          xobuff += writeFormat(xobuff, lfmt, xidata5);
          break;
        case 5:
          xobuff += writeFormat(xobuff, lfmt, xidata6);
          break;
        case 6:
          xobuff += writeFormat(xobuff, lfmt, xidata7);
          break;
        case 7:
          xobuff += writeFormat(xobuff, lfmt, xidata8);
          break;
        case 8:
          xobuff += writeFormat(xobuff, lfmt, xidata9);
          break;
        case 9:
          xobuff += writeFormat(xobuff, lfmt, xidata10);
          break;
        case 10:
          xobuff += writeFormat(xobuff, lfmt, xidata11);
          break;
      }
      lparam++;
    }
  }

  *xobuff = '\0';
}

void mysprintf8(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2, u32 xidata3, u32 xidata4, u32 xidata5, u32 xidata6, u32 xidata7, u32 xidata8)
{
	mysprintf11(xobuff, xifmt, xidata, xidata2, xidata3, xidata4, xidata5, xidata6, xidata7, xidata8, 0, 0, 0);
}

void mysprintf4(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2, u32 xidata3, u32 xidata4)
{
	mysprintf8(xobuff, xifmt, xidata, xidata2, xidata3, xidata4, 0, 0, 0, 0);
}

void mysprintf3(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2, u32 xidata3)
{
	mysprintf11(xobuff, xifmt, xidata, xidata2, xidata3, 0, 0, 0, 0, 0, 0, 0,0);
}

void mysprintf2(char *xobuff, const char *xifmt, u32 xidata, u32 xidata2)
{
	mysprintf3(xobuff, xifmt, xidata, xidata2, 0);
}

void mysprintf1(char *xobuff, const char *xifmt, u32 xidata)
{
	mysprintf3(xobuff, xifmt, xidata, 0, 0);
}

void mysprintf0(char *xobuff, const char *xifmt)
{
	while (*xifmt != '\0')
	{
		*xobuff++ = *xifmt++;
	}
	*xobuff = '\0';
}

// Searches for s1 string on memory
// Returns pointer to string
char* memfindsz(const char* s1, char* start, unsigned int size)
{
	unsigned int i = 0;
	while (i < size && strcmp(start, s1) != 0)
	{
		start++;
		i++;
	}

	if (i < size)
		return start;
	else
		return NULL;
}

// Searches for 32-bit value on memory
// Starting address must be word aligned
// Returns pointer to value
u32* memfindu32(const u32 val, u32* start, unsigned int size)
{
	unsigned int i = 0;
	while (i < size && *start != val)
	{
		start++;
		i++;
	}

	if (i < size)
		return start;
	else
		return NULL;
}

/* * * * * * * * * * * *
 * For functions below:
 * * * * * * * * * * * *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 * * * * * * * * * * * *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * * * * * * * * * * * *
 */

// Compares s1 with s2, returns 0 if both equal
int strcmp(const char *s1, const char *s2)
{
	int val = 0;
	const unsigned char *u1, *u2;

	u1 = (unsigned char *) s1;
	u2 = (unsigned char *) s2;

	while(1)
	{
		if(*u1 != *u2)
		{
			val = (int) *u1 - (int) *u2;
			break;
		}

		if((*u1 == 0) && (*u2 == 0))
		{
			break;
		}

		u1++;
		u2++;
	}

	return val;
}
// Concatenates string s + append
char *strcat(char *s, const char *append)
{
	char *pRet = s;
	while(*s)
	{
		s++;
	}

	while(*append)
	{
		*s++ = *append++;
	}

	*s = 0;

	return pRet;
}

// Compares only "count" chars from strings
int strncmp(const char *s1, const char *s2, size_t count)
{
	int val = 0;
	const unsigned char *u1, *u2;

	u1 = (unsigned char *) s1;
	u2 = (unsigned char *) s2;

	while(count > 0)
	{
		if(*u1 != *u2)
		{
			val = (int) *u1 - (int) *u2;
			break;
		}

		if((*u1 == 0) && (*u2 == 0))
		{
			break;
		}

		u1++;
		u2++;
		count--;
	}

	return val;
}

char * strrchr(const char *cp, int ch)
{
    char *save;
    char c;

    for (save = (char *) NULL; (c = *cp); cp++) {
	if (c == ch)
	    save = (char *) cp;
    }

    return save;
}
