#include "psid.h"
#include <string.h>

void prxXorKeyMix(unsigned char *dstBuf, unsigned int size, unsigned char *srcBuf, unsigned char *xorKey)
{
	unsigned int i;

	i = 0;

	while (i < size) {
		dstBuf[i] = srcBuf[i] ^ xorKey[i];
		++i;
	}
}

int isPrxEncrypted(unsigned char *prx, unsigned int size)
{
	if (size < 0x160)
		return 0;

	if (0 != memcmp(prx+0xD0, ENCRYPTED_TAG_MAGIC_1, 4))
	{
		return 0;
	}

	if (0 != memcmp(prx+0x130, ENCRYPTED_TAG_MAGIC_2, 4))
	{
		return 0;
	}

	return 1;
}
