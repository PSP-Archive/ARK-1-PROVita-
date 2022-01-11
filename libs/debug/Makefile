PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)
TARGET_LIB = libdebug.a
C_OBJS = lib.o graphics.o font.o
OBJS = $(C_OBJS)

all: $(TARGET_LIB)
CFLAGS = -O2 -G0 -Wall -s -lstdc++
CXXFLAGS = $(CFLAGS) -fexceptions -fno-rtti
PSP_FW_VERSION=660

include $(PROVITA)/common/global.mak
include $(PSPSDK)/lib/build.mak
include $(PROVITA)/common/beauty_lib.mak
