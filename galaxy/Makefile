TARGET = galaxy
C_OBJS = main.o
OBJS = $(C_OBJS) imports.o
all: $(TARGET).prx
INCDIR = $(PSPSDK)/include $(PROVITA)/common $(PROVITA)
CFLAGS = -Os -G0 -Wall
LDFLAGS = -mno-crt0 -nostartfiles -L. -L$(PROVITA)/libs

ifdef DISABLE_PSID_CHECK
CFLAGS += -DDISABLE_PSID_CHECK=$(DISABLE_PSID_CHECK)
endif

# Firmware Version
CFLAGS += -DFIRMWARE=$(FIRMWARE) -DGAME=$(GAME) -DSAVE=$(SAVE)

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

PSP_FW_VERSION = 660

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PRX_EXPORTS = export.exp
LIBS += -lpsid -lpspsystemctrl_kernel

include $(PROVITA)/common/global.mak
PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/beauty.mak
