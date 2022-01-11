TARGET_LIB=libsmartstub.a
C_OBJS = \
smartstub.o

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
