TARGET = popcorn

SPU_OBJS = \
sound/dma.o \
sound/out.o \
sound/registers.o \
sound/spu.o \
sound/xa.o \
sound/nullsnd.o

#sound/reverb.o \
#sound/adsr.o \

C_OBJS = main.o prospu.o $(SPU_OBJS)
OBJS = $(C_OBJS) icon.o

all: $(TARGET).prx
INCDIR = $(PROVITA)/common
CFLAGS = -Os -G0 -Wall -fno-pic

ifdef DISABLE_PSID_CHECK
CFLAGS += -DDISABLE_PSID_CHECK=$(DISABLE_PSID_CHECK)
endif

# Firmware Version
CFLAGS += -DFIRMWARE=$(FIRMWARE) -DGAME=$(GAME) -DSAVE=$(SAVE) -DPOPSMAN_VERSION=$(POPSMAN_VERSION) -DPOPS_VERSION=$(POPS_VERSION)

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

PSP_FW_VERSION = 660

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = $(PROVITA)/libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpsid -lpspsystemctrl_kernel -lpspsemaphore -lpsprtc -lpspaudio -ldebug -lcolordebugger

include $(PROVITA)/common/global.mak
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/beauty.mak
