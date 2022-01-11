TARGET = exitgame
C_OBJS = main.o
OBJS = $(C_OBJS)
all: $(TARGET).prx
INCDIR = $(PROVITA)/common
CFLAGS = -Os -G0 -Wall -fno-pic

CFLAGS += -DGAME=$(GAME) -DSAVE=$(SAVE)

# Firmware Version
CFLAGS += -DFIRMWARE=$(FIRMWARE)

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
LIBS = -lpspsystemctrl_kernel -lcolordebugger

include $(PROVITA)/common/global.mak
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/beauty.mak
