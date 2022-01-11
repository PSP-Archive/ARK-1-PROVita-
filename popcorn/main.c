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

#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <systemctrl.h>
#include <systemctrl_private.h>
#include <macros.h>
#include "psid.h"
#include "gameinfo.h"
#include "prospu.h"

extern unsigned char g_icon_png[0x3730];

PSP_MODULE_INFO("PROPopcornManager", 0x1007, 1, 1);

static STMOD_HANDLER g_previous = NULL;

static unsigned int g_pspFwVersion;

static int g_isCustomPBP;
static int g_keysBinFound;
static SceUID g_plain_doc_fd = -1;

#define PGD_ID "XX0000-XXXX00000_00-XXXXXXXXXX000XXX"
#define ACT_DAT "flash2:/act.dat"
#define RIF_MAGIC_FD 0x10000
#define ACT_DAT_FD 0x10001

#define VERSION_PSP 0
#define VERSION_180 1

enum {
	ICON0_OK = 0,
	ICON0_MISSING = 1,
	ICON0_CORRUPTED = 2,
};

static int g_icon0Status;

static unsigned char g_keys[16];

// Get keys.bin path
static int getKeysBinPath(char *keypath, unsigned int size);

// Save keys.bin
static int saveKeysBin(const char *keypath, unsigned char *key, int size);

static void patchPops(SceModule2 *mod);

static void popcornSyspatch(SceModule2 *mod)
{
	printk("%s: %s\r\n", __func__, mod->modname);

	if (0 == strcmp(mod->modname, "pops"))
	{
        #ifndef DISABLE_PSID_CHECK
		psidCheck();
        #endif
		patchPops(mod);
	}

	if(g_previous)
	{
		g_previous(mod);
		return;
	}
}

struct FunctionHook
{
	unsigned int nid;
	void *fp;
};

static const char *getFileBasename(const char *path)
{
	const char *p;

	if(path == NULL)
	{
		return NULL;
	}

	p = strrchr(path, '/');

	if(p == NULL)
	{
		p = path;
	}
	else
	{
		p++;
	}

	return p;
}

static inline int isEbootPBP(const char *path)
{
	const char *p;

	p = getFileBasename(path);

	if(p != NULL && 0 == strcmp(p, "EBOOT.PBP"))
	{
		return 1;
	}

	return 0;
}

static int checkFileDecrypted(const char *filename)
{
	SceUID fd = -1;
	u32 k1;
	int result = 0, ret;
	u8 p[16 + 64], *buf;
	u32 *magic;

	k1 = pspSdkSetK1(0);
	buf = (u8*)((((u32)p) & ~(64-1)) + 64);

	if(!g_isCustomPBP && isEbootPBP(filename))
	{
		goto exit;
	}

	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if(fd < 0)
	{
		goto exit;
	}

	ret = sceIoRead(fd, buf, 16);

	if(ret != 16)
	{
		goto exit;
	}

	magic = (u32*)buf;

	// PGD
	if(*magic == 0x44475000)
	{
		goto exit;
	}

	result = 1;

exit:
	if(fd >= 0)
	{
		sceIoClose(fd);
	}

	pspSdkSetK1(k1);

	return result;
}

static inline int isDocumentPath(const char *path)
{
	const char *p;

	p = getFileBasename(path);
	
	if(p != NULL && 0 == strcmp(p, "DOCUMENT.DAT"))
	{
		return 1;
	}

	return 0;
}

static int sceIoOpenPlain(const char *file, int flag, int mode)
{
	int ret;

	if(flag == 0x40000001 && checkFileDecrypted(file))
	{
		printk("%s: removed PGD open flag\r\n", __func__);
		ret = sceIoOpen(file, flag & ~0x40000000, mode);

		if(ret >= 0 && isDocumentPath(file))
		{
			g_plain_doc_fd = ret;
		}
	}
	else
	{
		ret = sceIoOpen(file, flag, mode);
	}

	return ret;
}

static int myIoOpen(const char *file, int flag, int mode)
{
	int ret;

	if(g_keysBinFound || g_isCustomPBP)
	{
		if(strstr(file, PGD_ID))
		{
			printk("%s: [FAKE]\r\n", __func__);
			ret = RIF_MAGIC_FD;
		} else if (0 == strcmp(file, ACT_DAT))
		{
			printk("%s: [FAKE]\r\n", __func__);
			ret = ACT_DAT_FD;
		} 
		else
		{
			ret = sceIoOpenPlain(file, flag, mode);
		}		
	}
	else
	{
		ret = sceIoOpenPlain(file, flag, mode);
	}

	printk("%s: %s 0x%08X -> 0x%08X\r\n", __func__, file, flag, ret);

	return ret;
}

static int myIoIoctl(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if(cmd == 0x04100001)
	{
		printk("%s: setting PGD key\r\n", __func__);
		// hexdump(indata, inlen);
	}

	if(cmd == 0x04100002)
	{
		printk("%s: setting PGD offset: 0x%08X\r\n", __func__, *(uint*)indata);
	}

	if (g_isCustomPBP || (g_plain_doc_fd >= 0 && g_plain_doc_fd == fd))
	{
		if (cmd == 0x04100001)
		{
			ret = 0;
			printk("%s: [FAKE] 0x%08X 0x%08X -> 0x%08X\r\n", __func__, fd, cmd, ret);
			goto exit;
		}

		if (cmd == 0x04100002)
		{
			ret = sceIoLseek32(fd, *(u32*)indata, PSP_SEEK_SET);

			if(ret < 0)
			{
				printk("%s: sceIoLseek32 -> 0x%08X\r\n", __func__, ret);
			}

			ret = 0;
			printk("%s: [FAKE] 0x%08X 0x%08X -> 0x%08X\r\n", __func__, fd, cmd, ret);
			goto exit;
		}
	}

	ret = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);

exit:
	printk("%s: 0x%08X -> 0x%08X\r\n", __func__, fd, ret);

	return ret;
}

static int myIoGetstat(const char *path, SceIoStat *stat)
{
	int ret;

	if(g_keysBinFound || g_isCustomPBP)
	{
		if(strstr(path, PGD_ID))
		{
			stat->st_mode = 0x21FF;
			stat->st_attr = 0x20;
			stat->st_size = 152;
			ret = 0;
			printk("%s: [FAKE]\r\n", __func__);
		} else if (0 == strcmp(path, ACT_DAT))
		{
			stat->st_mode = 0x21FF;
			stat->st_attr = 0x20;
			stat->st_size = 4152;
			ret = 0;
			printk("%s: [FAKE]\r\n", __func__);
		} 
		else
		{
			ret = sceIoGetstat(path, stat);
		}
	} 
	else
	{
		ret = sceIoGetstat(path, stat);
	}

	printk("%s: %s -> 0x%08X\r\n", __func__, path, ret);

	return ret;
}

static int myIoRead(int fd, unsigned char *buf, int size)
{
	int ret;
	u32 pos;
	u32 k1;

	UNUSED(pos);
	k1 = pspSdkSetK1(0);

	if(fd != RIF_MAGIC_FD && fd != ACT_DAT_FD)
	{
		pos = sceIoLseek32(fd, 0, SEEK_CUR);
	} 
	else
	{
		pos = 0;
	}
	
	if(g_keysBinFound|| g_isCustomPBP)
	{
		if(fd == RIF_MAGIC_FD)
		{
			size = 152;
			printk("%s: fake rif content %d\r\n", __func__, size);
			memset(buf, 0, size);
			strcpy((char*)(buf+0x10), PGD_ID);
			ret = size;
			goto exit;
		} else if (fd == ACT_DAT_FD)
		{
			printk("%s: fake act.dat content %d\r\n", __func__, size);
			memset(buf, 0, size);
			ret = size;
			goto exit;
		}
	}
	
	ret = sceIoRead(fd, buf, size);

	if(ret != size)
	{
		goto exit;
	}

	if (size == 4)
	{
		u32 magic;

		magic = 0x464C457F; // ~ELF

		if(0 == memcmp(buf, &magic, sizeof(magic)))
		{
			magic = 0x5053507E; // ~PSP
			memcpy(buf, &magic, sizeof(magic));
			printk("%s: patch ~ELF -> ~PSP\r\n", __func__);
		}

		ret = size;
		goto exit;
	}
	
	if(size == sizeof(g_icon_png))
	{
		u32 png_signature = 0x474E5089;

		if(g_icon0Status == ICON0_MISSING || ((g_icon0Status == ICON0_CORRUPTED) && 0 == memcmp(buf, &png_signature, 4)))
		{
			printk("%s: fakes a PNG for icon0\r\n", __func__);
			memcpy(buf, g_icon_png, size);

			ret = size;
			goto exit;
		}
	}

	if (g_isCustomPBP && size >= 0x420 && buf[0x41B] == 0x27 &&
			buf[0x41C] == 0x19 &&
			buf[0x41D] == 0x22 &&
			buf[0x41E] == 0x41 &&
			buf[0x41A] == buf[0x41F])
	{
		buf[0x41B] = 0x55;
		printk("%s: unknown patch loc_6c\r\n", __func__);
	}

exit:
	pspSdkSetK1(k1);
	printk("%s: fd=0x%08X pos=0x%08X size=%d -> 0x%08X\r\n", __func__, (uint)fd, (uint)pos, (int)size, ret);

	return ret;
}

static int myIoReadAsync(int fd, unsigned char *buf, int size)
{
	int ret;
	unsigned int pos;
	unsigned int k1;

	UNUSED(pos);
	k1 = pspSdkSetK1(0);
	pos = sceIoLseek32(fd, 0, SEEK_CUR);
	pspSdkSetK1(k1);
	ret = sceIoReadAsync(fd, buf, size);
	printk("%s: 0x%08X 0x%08X 0x%08X -> 0x%08X\r\n", __func__, (uint)fd, (uint)pos, size, ret);

	return ret;
}

static SceOff myIoLseek(SceUID fd, SceOff offset, int whence)
{
	SceOff ret;
	u32 k1;

	k1 = pspSdkSetK1(0);

	if(g_keysBinFound || g_isCustomPBP)
	{
		if (fd == RIF_MAGIC_FD)
		{
			printk("%s: [FAKE]\r\n", __func__);
			ret = 0;
		} else if (fd == ACT_DAT_FD)
		{
			printk("%s: [FAKE]\r\n", __func__);
			ret = 0;
		} 
		else
		{
			ret = sceIoLseek(fd, offset, whence);
		}
	} 
	else
	{
		ret = sceIoLseek(fd, offset, whence);
	}

	pspSdkSetK1(k1);
	printk("%s: 0x%08X 0x%08X 0x%08X -> 0x%08X\r\n", __func__, (uint)fd, (uint)offset, (uint)whence, (int)ret);

	return ret;
}

static int myIoClose(SceUID fd)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);

	if(g_keysBinFound || g_isCustomPBP)
	{
		if (fd == RIF_MAGIC_FD)
		{
			printk("%s: [FAKE]\r\n", __func__);
			ret = 0;
		} else if (fd == ACT_DAT_FD)
		{
			printk("%s: [FAKE]\r\n", __func__);
			ret = 0;
		} 
		else
		{
			ret = sceIoClose(fd);
		}
	} 
	else
	{
		ret = sceIoClose(fd);
	}

	if(g_plain_doc_fd == fd && ret == 0)
	{
		g_plain_doc_fd = -1;
	}

	pspSdkSetK1(k1);
	printk("%s: 0x%08X -> 0x%08X\r\n", __func__, fd, ret);

	return ret;
}

static int _sceDrmBBCipherUpdate(void *ckey, unsigned char *data, int size)
{
	return 0;
}

static int _sceDrmBBCipherInit(void *ckey, int type, int mode, unsigned char *header_key, unsigned char *version_key, unsigned int seed)
{
	return 0;
}

static int _sceDrmBBMacInit(void *mkey, int type)
{
	return 0;
}

static int _sceDrmBBMacUpdate(void *mkey, unsigned char *buf, int size)
{
	return 0;
}

static int _sceDrmBBCipherFinal(void *ckey)
{
	return 0;
}

static int _sceDrmBBMacFinal(void *mkey, unsigned char *buf, unsigned char *vkey)
{
	return 0;
}

static int _sceDrmBBMacFinal2(void *mkey, unsigned char *out, unsigned char *vkey)
{
	return 0;
}

static struct FunctionHook g_ioHooks[] = {
	{ 0x109F50BC, &myIoOpen, },
	{ 0x27EB27B8, &myIoLseek, },
	{ 0x63632449, &myIoIoctl, },
	{ 0x6A638D83, &myIoRead, },
	{ 0xA0B5A7C2, &myIoReadAsync, },
	{ 0xACE946E8, &myIoGetstat, },
	{ 0x810C4BC3, &myIoClose, },
};

static struct FunctionHook g_amctrlHooks[] = {
	{ 0x1CCB66D2, &_sceDrmBBCipherInit, },
	{ 0x0785C974, &_sceDrmBBCipherUpdate, },
	{ 0x9951C50F, &_sceDrmBBCipherFinal, },
	{ 0x525B8218, &_sceDrmBBMacInit, },
	{ 0x58163FBE, &_sceDrmBBMacUpdate, },
	{ 0xEF95A213, &_sceDrmBBMacFinal, },
	{ 0xF5186D8E, &_sceDrmBBMacFinal2, },
};

static int (*sceNpDrmGetVersionKey)(unsigned char * key, unsigned char * act, unsigned char * rif, unsigned int flags);

static int _sceNpDrmGetVersionKey(unsigned char * key, unsigned char * act, unsigned char * rif, unsigned int flags)
{
	char keypath[128];
	int result;
   
	result = (*sceNpDrmGetVersionKey)(key, act, rif, flags);

	if (g_isCustomPBP)
	{
		printk("%s: -> 0x%08X\r\n", __func__, result);
		result = 0;

		if (g_keysBinFound)
		{
			memcpy(key, g_keys, sizeof(g_keys));
		}
		
		printk("%s:[FAKE] -> 0x%08X\r\n", __func__, result);
	}
	else
	{
		getKeysBinPath(keypath, sizeof(keypath));

		if (result == 0)
		{
			int ret;

			UNUSED(ret);
			memcpy(g_keys, key, sizeof(g_keys));
			ret = saveKeysBin(keypath, g_keys, sizeof(g_keys));
			printk("%s: saveKeysBin -> %d\r\n", __func__, ret);
		}
		else
		{
			if (g_keysBinFound)
			{
				memcpy(key, g_keys, sizeof(g_keys));
				result = 0;
			}
		}
	}
	
	return result;
}

static int (*scePspNpDrm_driver_9A34AC9F)(unsigned char *rif);

static int _scePspNpDrm_driver_9A34AC9F(unsigned char *rif)
{
	int result;

	result = (*scePspNpDrm_driver_9A34AC9F)(rif);
	printk("%s: 0x%08X -> 0x%08X\r\n", __func__, (uint)rif, result);

	if (result != 0)
	{
		if (g_keysBinFound || g_isCustomPBP)
		{
			result = 0;
			printk("%s:[FAKE] -> 0x%08X\r\n", __func__, result);
		}
	}

	return result;
}

static int (*_getRifPatch)(const char *name, char *path) = NULL;

static int getRifPatch(char *name, char *path)
{
	int ret;

	if(g_keysBinFound || g_isCustomPBP) {
		strcpy(name, PGD_ID);
	}

	ret = (*_getRifPatch)(name, path);
	printk("%s: %s %s -> 0x%08X\r\n", __func__, name, path, ret);

	return ret;
}

static int myKernelLoadModule(char * fname, int flag, void * opt)
{
	int result = sceKernelLoadModule(GAME_SAVE_PATH "pops.prx", flag, opt);
	printk("%s: fname %s flag 0x%08X -> 0x%08X\r\n", __func__, fname, flag, result);

	return result;
}

void patchPopsMgr(void)
{
	SceModule2 *mod;
	unsigned int text_addr;
	int i;

	mod = (SceModule2*) sceKernelFindModuleByName("scePops_Manager");
	text_addr = mod->text_addr;

	for(i=0; i<NELEMS(g_ioHooks); ++i)
	{
		hookImportByNID(mod, "IoFileMgrForKernel", g_ioHooks[i].nid, g_ioHooks[i].fp);
	}

	_getRifPatch = (void*)(text_addr + 0x00000190);

#if POPSMAN_VERSION == VERSION_180
	_sw(JAL(&getRifPatch), text_addr + 0x2764);
	_sw(JAL(&getRifPatch), text_addr + 0x2C24);
#else
	_sw(JAL(&getRifPatch), text_addr + 0x00002798);
	_sw(JAL(&getRifPatch), text_addr + 0x00002C58);
#endif

	if (g_isCustomPBP)
	{
		for(i=0; i<NELEMS(g_amctrlHooks); ++i)
		{
			hookImportByNID(mod, "sceAmctrl_driver", g_amctrlHooks[i].nid, g_amctrlHooks[i].fp);
		}
	}

	// remove the check in scePopsManLoadModule that only allows loading module below the FW 3.XX
#if POPSMAN_VERSION == VERSION_180
	_sw(NOP, text_addr + 0x1E58);
#else
	_sw(NOP, text_addr + 0x00001E80);
#endif

	sceNpDrmGetVersionKey = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x0F9547E6);
	scePspNpDrm_driver_9A34AC9F = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x9A34AC9F);

	// hook sceNpDrmGetVersionKey call
#if POPSMAN_VERSION == VERSION_180
	_sw(JAL(_sceNpDrmGetVersionKey), text_addr + 0x2990);
#else
	_sw(JAL(_sceNpDrmGetVersionKey), text_addr + 0x000029C4);
#endif

	// hook scePspNpDrm_driver_9A34AC9F call
#if POPSMAN_VERSION == VERSION_180
	_sw(JAL(_scePspNpDrm_driver_9A34AC9F), text_addr + 0x2D74);
#else
	_sw(JAL(_scePspNpDrm_driver_9A34AC9F), text_addr + 0x00002DA8);

	// TN hacks
	_sw(JR_RA, text_addr + 0x2F88);
	_sw(LI_V0(0), text_addr + 0x2F88 + 4);
	_sw(JR_RA, text_addr + 0x35D8);
	_sw(LI_V0(0), text_addr + 0x35D8 + 4);
	_sw(JR_RA, text_addr + 0x3514);
	_sw(LI_V0(0), text_addr + 0x3514 + 4);
	_sw(JR_RA, text_addr + 0x3590);
	_sw(LI_V0(0), text_addr + 0x3590 + 4);
	_sw(JR_RA, text_addr + 0x35AC);
	_sw(LI_V0(0), text_addr + 0x35AC + 4);
	_sw(JR_RA, text_addr + 0x31EC);
	_sw(LI_V0(0), text_addr + 0x31EC + 4);

	// my hacks
	_sw(JR_RA, text_addr + 0x0000342C);
	_sw(LI_V0(0), text_addr + 0x0000342C + 4);

	_sw(JR_RA, text_addr + 0x00003490);
	_sw(LI_V0(0), text_addr + 0x00003490 + 4);
#endif

	// patch loadmodule to load our own pops.prx
#if POPSMAN_VERSION == VERSION_180
	_sw(JAL(myKernelLoadModule), text_addr + 0x1EB8);
#else
	_sw(JAL(myKernelLoadModule), text_addr + 0x00001EE0);
#endif
}

static unsigned int isCustomPBP(void)
{
	SceUID fd = -1;
	const char *filename;
	int result, ret;
	unsigned int psar_offset, pgd_offset, *magic;
	unsigned char p[40 + 64], *header;

	header = (unsigned char*)((((unsigned int)p) & ~(64-1)) + 64);
	filename = sceKernelInitFileName();
	result = 0;

	if(filename == NULL)
	{
		result = 0;
		goto exit;
	}

	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if(fd < 0)
	{
		printk("%s: sceIoOpen %s -> 0x%08X\r\n", __func__, filename, fd);
		result = 0;
		goto exit;
	}

	ret = sceIoRead(fd, header, 40);

	if(ret != 40)
	{
		printk("%s: sceIoRead -> 0x%08X\r\n", __func__, ret);
		result = 0;
		goto exit;
	}

	psar_offset = *(unsigned int*)(header+0x24);
	sceIoLseek32(fd, psar_offset, PSP_SEEK_SET);
	ret = sceIoRead(fd, header, 40);

	if(ret != 40)
	{
		printk("%s: sceIoRead -> 0x%08X\r\n", __func__, ret);
		result = 0;
		goto exit;
	}

	pgd_offset = psar_offset;

	if(0 == memcmp(header, "PSTITLE", sizeof("PSTITLE")-1))
	{
		pgd_offset += 0x200;
	}
	else
	{
		pgd_offset += 0x400;
	}

	sceIoLseek32(fd, pgd_offset, PSP_SEEK_SET);
	ret = sceIoRead(fd, header, 4);

	if(ret != 4)
	{
		printk("%s: sceIoRead -> 0x%08X\r\n", __func__, ret);
		result = 0;
		goto exit;
	}

	magic = (unsigned int*)header;

	// PGD offset
	if(*magic != 0x44475000)
	{
		printk("%s: custom pops found\r\n", __func__);
		result = 1;
	}

exit:
	if(fd >= 0)
	{
		sceIoClose(fd);
	}

	return result;
}

static int (*sceMeAudio_67CD7972)(void *buf, int size);

int _sceMeAudio_67CD7972(void *buf, int size)
{
	int ret;
	unsigned int k1;

	k1 = pspSdkSetK1(0);
	ret = (*sceMeAudio_67CD7972)(buf, size);
	pspSdkSetK1(k1);

	printk("%s: 0x%08X -> 0x%08X\r\n", __func__, size, ret);

	return ret;
}

static void setupPsxFwVersion(unsigned int fw_version)
{
	int (*_SysMemUserForUser_315AD3A0)(unsigned int fw_version);

	_SysMemUserForUser_315AD3A0 = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x315AD3A0);

	if (_SysMemUserForUser_315AD3A0 != NULL)
	{
		_SysMemUserForUser_315AD3A0(fw_version);
	}
}

static int getIcon0Status(void)
{
	unsigned int icon0_offset = 0;
	int result = ICON0_MISSING;
	SceUID fd = -1;;
	const char *filename;
	unsigned char p[40 + 64], *header;
	
	header = (unsigned char*)((((unsigned int)p) & ~(64-1)) + 64);
	filename = sceKernelInitFileName();

	if(filename == NULL)
	{
		goto exit;
	}
	
	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if(fd < 0)
	{
		printk("%s: sceIoOpen %s -> 0x%08X\r\n", __func__, filename, fd);
		goto exit;
	}
	
	sceIoRead(fd, header, 40);
	icon0_offset = *(unsigned int*)(header+0x0c);
	sceIoLseek32(fd, icon0_offset, PSP_SEEK_SET);
	sceIoRead(fd, header, 40);

	if(*(unsigned int*)(header+4) == 0xA1A0A0D)
	{
		if ( *(unsigned int*)(header+0xc) == 0x52444849 && // IHDR
				*(unsigned int*)(header+0x10) == 0x50000000 && // 
				*(unsigned int*)(header+0x14) == *(unsigned int*)(header+0x10)
		   )
		{
			result = ICON0_OK;
		}
		else
		{
			result = ICON0_CORRUPTED;
		}
	}
	else
	{
		result = ICON0_MISSING;
	}

	printk("%s: PNG file status -> %d\r\n", __func__, result);

exit:
	if(fd >= 0)
	{
		sceIoClose(fd);
	}

	return result;
}

static int getKeysBinPath(char *keypath, unsigned int size)
{
	char *p;

	strncpy(keypath, sceKernelInitFileName(), size);
	keypath[size-1] = '\0';
	p = strrchr(keypath, '/');

	if(p == NULL)
	{
		return -1;
	}

	p[1] = '\0';

	if(strlen(keypath) > size - (sizeof("KEYS.BIN") - 1) - 1)
	{
		printk("popcorn: %s too long\r\n", keypath);
		_sw(0, 0);
		return -1;
	}

	strcat(keypath, "KEYS.BIN");

	return 0;
}

static int loadKeysBin(const char *keypath, unsigned char *key, int size)
{
	SceUID keys; 
	int ret;

	keys = sceIoOpen(keypath, PSP_O_RDONLY, 0777);

	if (keys < 0)
	{
		printk("%s: sceIoOpen %s -> 0x%08X\r\n", __func__, keypath, keys);

		return -1;
	}

	ret = sceIoRead(keys, key, size); 

	if (ret == size)
	{
		ret = 0;
	} 
	else
	{
		ret = -2;
	}

	sceIoClose(keys);

	return ret;
}

static int saveKeysBin(const char *keypath, unsigned char *key, int size)
{
	SceUID keys;
	int ret;

	keys = sceIoOpen(keypath, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (keys < 0)
	{
		return -1;
	}

	ret = sceIoWrite(keys, key, size);

	if(ret == size)
	{
		ret = 0;
	} 
	else
	{
		ret = -2;
	}

	sceIoClose(keys);

	return ret;
}

void getKeys(void)
{
	char keypath[512];
	int ret;
	SceIoStat stat;

	getKeysBinPath(keypath, sizeof(keypath));
	ret = sceIoGetstat(keypath, &stat);
	g_keysBinFound = 0;

	if(ret == 0)
	{
		if(loadKeysBin(keypath, g_keys, sizeof(g_keys)) == 0)
		{
			g_keysBinFound = 1;
			printk("popcorn: keys.bin found\r\n");
		}
	}
}

static int patchSyscallStub(void* func, void *addr)
{
	unsigned int syscall_num;

	syscall_num = sceKernelQuerySystemCall(func);

	if(syscall_num == (unsigned int)-1)
	{
		return -1;
	}

	_sw(0x03E00008, (unsigned int)addr);
	_sw(((syscall_num<<6)|12), (unsigned int)(addr+4));

	return 0;
}

int decompressData(unsigned int destSize, const unsigned char *src, unsigned char *dest)
{
	unsigned int k1;
	int ret;

	k1 = pspSdkSetK1(0);

	ret = sceKernelDeflateDecompress(dest, destSize, src, 0);
	printk("%s: 0x%08X 0x%08X 0x%08X -> 0x%08X\r\n", __func__, (uint)destSize, (uint)src, (uint)dest, ret);

	if (ret >= 0)
	{
		/*
		// Log Last Decompress
		int fd = sceIoOpen("ms0:/PSP/SAVEDATA/FIELDFIRMWARE/log.bin", PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);
		if(fd >= 0)
		{
			sceIoWrite(fd, dest, ret);
			sceIoClose(fd);
		}
		*/

		ret = 0x92FF;
		printk("%s: [FAKE] -> 0x%08X\r\n", __func__, ret);
	}

	pspSdkSetK1(k1);

	return ret;
}

static int patchDecompressData(unsigned int text_addr)
{
	int ret;
	void *stub_addr, *patch_addr;

#if POPS_VERSION == VERSION_180
	stub_addr = (void*)(text_addr +0xB1FCC);
	patch_addr = (void*)(text_addr + 0xC2A4);
#else
	stub_addr = (void*)(text_addr +0x000D5424);
	patch_addr = (void*)(text_addr + 0x0000DB78);
#endif

	ret = patchSyscallStub(decompressData, stub_addr);

	if (ret != 0) 
	{
		printk("%s: patchSyscallStub -> 0x%08X\r\n", __func__, ret);

		return -1;
	}

	_sw(JAL(stub_addr), (unsigned int)patch_addr);

	return 0;
}

static void patchIcon0Size(unsigned int text_addr)
{
	unsigned int patch_addr;
	
#if POPS_VERSION == VERSION_180
	patch_addr = text_addr + 0x00017458;
#else
	patch_addr = text_addr + 0x00036D50;
#endif

	_sw(0x24050000 | (sizeof(g_icon_png) & 0xFFFF), patch_addr);
}

#if POPS_VERSION == VERSION_180
unsigned int pops_text_addr = 0;

static int _ctxPoller_Thread(SceSize args, void * argp)
{
	// Endless Loop
	while(1)
	{
		// Threads of Interest
		char * toi[3] = { "mcworker", "cdworker", "popsmain" };
		
		// Iterate Threads of Interest
		int i = 0; for(; i < NELEMS(toi); i++)
		{
			// Thread Info Structure
			SceKernelThreadKInfo info;
			
			// Get Thread Context
			if(sctrlGetThreadContextByName(toi[i], &info) == 0)
			{
				// Output Thread Name
				printk("Thread Name: %s\n", toi[i]);
				
				/*
				// PSPLINK Style Output
				printk("kstack 0x%08X kstacksize 0x%08X\n", (unsigned int)info.kstack, info.kstackSize);
				printk("stack  0x%08X stacksize  0x%08X\n", (unsigned int)info.stack, info.stackSize);
				printk("context 0x%08X, vfpu 0x%08X\n", (unsigned int)info.thContext, (unsigned int)info.vfpuContext);
				printk("Context EPC 0x%08X, Real EPC 0x%08X\n", info.thContext->EPC, info.retAddr);
				*/
				
				// Minimalistic Output with right relative Offset :D
				printk("EPC: 0x%08X (0x%08X)\n", info.retAddr - pops_text_addr, info.retAddr);
			}
		}
		
		// Once every second is fine
		sceKernelDelayThread(1000000);
	}
	
	// Shut up GCC
	return 0;
}
#endif

static void replacePSXSPU(SceModule2 * mod)
{
	// Fetch Text Address
	unsigned int text_addr = mod->text_addr;

	UNUSED(text_addr);
	// Replace Media Engine SPU Background Thread Starter
	hookImportByNID(mod, "sceMeAudio", 0xDE630CD2, _sceMeAudio_DE630CD2);
	
	#ifdef WITH_PEOPS_SPU
	// Replace SPU Register Read Callback
	void * read_stub_addr = (void *)(text_addr + 0x000D5424 + 8);
	patchSyscallStub(spuReadCallback, read_stub_addr);
	unsigned int spuRCB = text_addr + 0x85F4;
	_sw(0x27BDFFFC, spuRCB); // addiu $sp, $sp, -4
	_sw(0xAFBF0000, spuRCB + 4); // sw $ra, 0($sp)
	_sw(JAL(read_stub_addr), spuRCB + 8); // jal callback stub
	_sw(NOP, spuRCB + 12); // nop
	_sw(0x8FBF0000, spuRCB + 16); // lw $ra, 0($sp)
	_sw(0x03E00008, spuRCB + 20); // jr $ra
	_sw(0x27BD0004, spuRCB + 24); // addiu $sp, $sp, 4
	
	// Replace SPU Register Write Callback
	void * write_stub_addr = read_stub_addr + 8;
	patchSyscallStub(spuWriteCallback, write_stub_addr);
	unsigned int spuWCB = text_addr + 0x7F00;
	_sw(0x27BDFFFC, spuWCB); // addiu $sp, $sp, -4
	_sw(0xAFBF0000, spuWCB + 4); // sw $ra, 0($sp)
	_sw(JAL(write_stub_addr), spuWCB + 8); // jal callback stub
	_sw(NOP, spuWCB + 12); // nop
	_sw(0x8FBF0000, spuWCB + 16); // lw $ra, 0($sp)
	_sw(0x03E00008, spuWCB + 20); // jr $ra
	_sw(0x27BD0004, spuWCB + 24); // addiu $sp, $sp, 4
	#endif
}

static void patchPops(SceModule2 *mod)
{
	unsigned int text_addr;

	text_addr = mod->text_addr;
	printk("%s: patching pops\r\n", __func__);

	if(g_isCustomPBP)
	{
		patchDecompressData(text_addr);
	}

	if(g_icon0Status != ICON0_OK)
	{
		patchIcon0Size(text_addr);
	}

	#if POPS_VERSION == VERSION_180
	
	// TODO More Vita Research
	
	// Save Text Address
	pops_text_addr = text_addr;
	
	// Create Context Poller Thread
	int ctxPoller = sceKernelCreateThread("CTXPoller", _ctxPoller_Thread, 0x10, 10 * 1024, 0, NULL);
	
	// Log Creation
	printk("Context Poller Thread Creation: %08X\n", ctxPoller);

	
	// Start Context Poller Thread
	int start = sceKernelStartThread(ctxPoller, 0, NULL);
	
	// Log Start
	printk("Context Poller Thread Start: %08X\n", start);
	
	// Kill Loop #1
	_sw(NOP, text_addr + 0x00006908);
	
	// Kill Loop #2
	_sw(NOP, text_addr + 0x00006D28);
	
	// Kill Loop #3
	_sw(NOP, text_addr + 0x00006778);
	
	// Kill Loop #4
	_sw(NOP, text_addr + 0x000065EC);
	
	// Kill Loop #5
	_sw(NOP, text_addr + 0x000070AC);
	
	// Kill Loop #6
	_sw(NOP, text_addr + 0x00006BD0);
	
	#else
	
	// Prevent Permission Problems
	sceMeAudio_67CD7972 = (void*)sctrlHENFindFunction("scePops_Manager", "sceMeAudio", 0x2AB4FE43);
	hookImportByNID(mod, "sceMeAudio", 0x2AB4FE43, _sceMeAudio_67CD7972);
	
	// Replace PSX Sound Processing Unit Emulator
	replacePSXSPU(mod);
	
	// Patch Manual Name Check
	_sw(0x24020001, text_addr + 0x00025248);
	
	#endif

	flushCache();
}

int module_start(SceSize args, void* argp)
{
	printk("popcorn: init_file = %s\r\n", sceKernelInitFileName());
	g_pspFwVersion = sceKernelDevkitVersion();
	
	getKeys();
	g_isCustomPBP = isCustomPBP();
	g_icon0Status = getIcon0Status();

	if(g_isCustomPBP)
	{
		setupPsxFwVersion(g_pspFwVersion);
	}
	
	g_previous = sctrlHENSetStartModuleHandler(&popcornSyspatch);
	patchPopsMgr();
	
	flushCache();

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	// Shutdown SPU to prevent freeze
	spuShutdown();
	
	return 0;
}
