TARGET_LIB=libcolordebugger.a

C_OBJS = \
colordebugger.o

all: $(TARGET_LIB)

OBJS = $(C_OBJS)
PSPSDK=$(shell psp-config --pspsdk-path)

CC=psp-gcc
INCDIR = $(PROVITA)/common/ansi-c
CFLAGS = -Wall -Os -G0 -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LIBDIR =

include $(PROVITA)/common/global.mak
include $(PROVITA)/common/beauty_lib.mak
