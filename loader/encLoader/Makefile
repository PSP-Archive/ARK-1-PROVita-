PSPSDK=$(shell psp-config --pspsdk-path)
INCDIR = $(PSPSDK)/include $(PROVITA)/common $(PROVITA)
LIBS= -ldebug -lpsid -lansic -lcolordebugger -L $(PROVITA)/libs
TARGET = t.bin

all: pre-build $(TARGET)

CFLAGS = -Wall -Os -G0 -fno-pic $(addprefix -I, $(INCDIR))

CFLAGS += -DGAME=$(GAME) -DSAVE=$(SAVE)

C_OBJS = main.o

include $(PROVITA)/common/global.mak
include $(PROVITA)/common/beauty_bin.mak

ENCLOADER_LOADADDR ?= 0x00010000
LINKFILE = enclink.l

# Firmware Version
CFLAGS += -DFIRMWARE=$(FIRMWARE)

ifdef DEBUG
CFLAGS += -DDEBUG=$(DEBUG)
endif

CC = psp-gcc
LD = psp-ld
STRIP = psp-strip
OBJCOPY = psp-objcopy

main.elf: $(C_OBJS)

pre-build: enclink.l

include $(PROVITA)/common/exploit_config.mak

enclink.l:
	$(Q)python $(PROVITA)/contrib/PC/genlink/genlink.py $(ENCLOADER_LOADADDR) encLoader > $@
	@echo GET $@

$(TARGET): main.elf
	$(Q)$(STRIP) -s $<
	$(Q)$(OBJCOPY) -O binary $< $@
	$(Q)bin2c $@ encLoader.h encLoaderBuf
	@echo GET $@

clean:
	$(Q)rm -rf *~ *.s *.o *.elf encLoader.h enclink.l $(TARGET)
