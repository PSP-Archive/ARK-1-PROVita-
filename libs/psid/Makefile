TARGET_LIB=libpsid.a
C_OBJS = \
rc4.o \
psid.o

ifneq ($(RELEASE), 1)
	C_OBJS += psidcheck.o
endif

all: $(TARGET_LIB)

OBJS = $(C_OBJS)
PSPSDK=$(shell psp-config --pspsdk-path)

CC=psp-gcc
INCDIR = $(PROVITA)/common/ansi-c $(PROVITA)/common $(PSPSDK)/include
CFLAGS = -Wall -Os -G0 -fno-pic
CFLAGS += -DFIRMWARE=$(FIRMWARE) -DGAME=$(GAME)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =

include $(PROVITA)/common/global.mak
include $(PROVITA)/common/beauty_lib.mak
