TARGET = inferno
C_OBJS = main.o iodrv_funcs.o umd.o isoread.o 
OBJS = $(C_OBJS) imports.o
all: $(TARGET).prx
INCDIR = $(PROVITA)/common $(PROVITA)/common/ansi-c $(PROVITA)
CFLAGS = -Os -G0 -Wall

ifdef DISABLE_PSID_CHECK
CFLAGS += -DDISABLE_PSID_CHECK=$(DISABLE_PSID_CHECK)
endif

# Firmware Version
CFLAGS += -DFIRMWARE=$(FIRMWARE) -DGAME=$(GAME) -DSAVE=$(SAVE)

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

LDFLAGS = -mno-crt0 -nostartfiles -L $(PROVITA)/libs
LIBS = -lpsid -lpspsystemctrl_kernel -lpspsemaphore

PSP_FW_VERSION = 660

PRX_EXPORTS = exports.exp

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

include $(PROVITA)/common/global.mak
PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/beauty.mak
