PSPSDK=$(shell psp-config --pspsdk-path)
INCDIR = $(PSPSDK)/include $(PROVITA)/common $(PROVITA)
LIBS= -ldebug -lpsid -lansic -lcolordebugger -L $(PROVITA)/libs
TARGET = h.bin

CFLAGS = -Wall -Os -G0 -fno-pic $(addprefix -I, $(INCDIR))


LINKFILE = link.l
RELEASE_FILE = release.h

PRE_BUILD = $(LINKFILE)
ifeq ($(RELEASE), 1)
PRE_BUILD += $(RELEASE_FILE)
CFLAGS += -DRELEASE=$(RELEASE)
endif

all: $(PRE_BUILD) $(TARGET)

ifdef PSID_DUMP
CFLAGS += -DPSID_DUMP=$(PSID_DUMP)
endif

CFLAGS += -DGAME=$(GAME) -DSAVE=$(SAVE) -DENCLOADER_LOADADDR=$(ENCLOADER_LOADADDR) -DEXPLOIT_LOADADDR=$(EXPLOIT_LOADADDR)

C_OBJS = main.o sha1sum.o

include $(PROVITA)/common/global.mak
include $(PROVITA)/common/beauty_bin.mak

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

include $(PROVITA)/common/exploit_config.mak

ifeq ($(ENABLE_SMARTSTUB), 1)
	LIBS += -lsmartstub
	CFLAGS += -DENABLE_SMARTSTUB=$(ENABLE_SMARTSTUB)
endif

cmd_genlink = python $(PROVITA)/contrib/PC/genlink/genlink.py $(LOADER_LOADADDR) loaderEntry > $(LINKFILE)
cmd_release = (python $(PROVITA)/contrib/PC/psid/release.py $(PSID)) > $(RELEASE_FILE)

$(LINKFILE):
	$(Q)$(cmd_genlink)

$(RELEASE_FILE):
	$(Q)$(cmd_release)


$(TARGET): main.elf
	$(Q)$(STRIP) -s $<
	$(Q)$(OBJCOPY) -O binary $< $@
	@echo GET $@

clean:
	$(Q)rm -rf *~ *.s *.o *.elf h.bin $(LINKFILE) $(RELEASE_FILE)
