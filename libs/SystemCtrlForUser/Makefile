TARGET=libpspsystemctrl_user.a

all: $(TARGET)
STUBSRC = SystemCtrlForUser.S
OBJS = SystemCtrlForUser_0000.o SystemCtrlForUser_0001.o SystemCtrlForUser_0002.o SystemCtrlForUser_0003.o SystemCtrlForUser_0004.o SystemCtrlForUser_0005.o SystemCtrlForUser_0006.o SystemCtrlForUser_0007.o SystemCtrlForUser_0008.o SystemCtrlForUser_0009.o SystemCtrlForUser_0010.o SystemCtrlForUser_0011.o SystemCtrlForUser_0012.o SystemCtrlForUser_0013.o SystemCtrlForUser_0014.o SystemCtrlForUser_0015.o SystemCtrlForUser_0016.o SystemCtrlForUser_0017.o SystemCtrlForUser_0018.o SystemCtrlForUser_0019.o SystemCtrlForUser_0020.o SystemCtrlForUser_0021.o SystemCtrlForUser_0022.o SystemCtrlForUser_0023.o

PSPSDK=$(shell psp-config --pspsdk-path)

CC=psp-gcc
INCDIR = 
CFLAGS = -Os -G0 -Wall -fno-builtin-printf -I$(PSPSDK)/include
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LDFLAGS=-nodefaultlibs

LIBDIR =

include $(PROVITA)/common/global.mak
include $(PROVITA)/common/beauty_stub.mak
