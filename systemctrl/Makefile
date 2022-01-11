TARGET = systemctrl

C_OBJS = \
main.o \
filesystem.o \
modulemanager.o \
elf.o \
flushcache.o \
loadercore.o \
cryptography.o \
interruptman.o \
kubridge.o \
sctrl.o \
sctrl_se.o \
oe_malloc.o \
syspatch.o \
loadexec.o \
mediasync.o \
msstor_cache.o \
hooknids.o \
nidresolver.o \
nid_660_data.o \
missingfunc.o \
plugin.o \
rebootex.o \
sysmem.o \
vlffix.o \
exception.o \
printk.o \
jumper.o \
threadctx.o
		
OBJS = \
$(C_OBJS) \
exception_asm.o \
setlongjmp.o
all: $(TARGET).prx
INCDIR = $(PROVITA)/common $(PROVITA)
CFLAGS = -Os -G0 -Wall -fno-pic

ifdef DISABLE_PSID_CHECK
CFLAGS += -DDISABLE_PSID_CHECK=$(DISABLE_PSID_CHECK)
endif

ifdef FLASH_RESTORE
CFLAGS += -DFLASH_RESTORE=$(FLASH_RESTORE)
endif

# Firmware Version
CFLAGS += -DFIRMWARE=$(FIRMWARE) -DGAME=$(GAME) -DSAVE=$(SAVE)

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = $(PROVITA)/libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsemaphore -lpsid -lcolordebugger

PSP_FW_VERSION = 660

include $(PROVITA)/common/global.mak
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/beauty.mak
