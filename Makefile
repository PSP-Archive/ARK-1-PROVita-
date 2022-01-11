# Number of Compilation Threads
OPT=-j8

PROVITA ?= $(CURDIR)
POPSMAN_VERSION ?= VERSION_180
POPS_VERSION ?= VERSION_PSP
USE_FLASH0_ARK ?= 1
SAVE ?= -1

export DEBUG DISABLE_PSID_CHECK FIRMWARE PSID GAME KXPLOIT PROVITA RELEASE SAVE
export POPSMAN_VERSION POPS_VERSION
export PSID_DUMP KERNEL_DUMP FLASH_DUMP FLASH_RESTORE

SUBDIRS = libs systemctrl galaxy stargate exitgame provsh popcorn inferno rebootbuffer exploit loader/encLoader loader contrib/PC/btcnf
.PHONY: subdirs $(SUBDIRS) cleanobj clean cleanobj distclean copy-bin mkdir-dist encrypt-prx

all: subdirs mkdir-dist copy-bin encrypt-prx

copy-bin: loader/h.bin contrib/PC/btcnf/pspbtinf.bin contrib/PC/btcnf/pspbtmnf.bin contrib/PC/btcnf/pspbtnnf.bin contrib/PSP/fake.cso provsh/EBOOT.PBP
	$(Q)cp loader/h.bin dist/H.BIN
	$(Q)cp exploit/k.enc.bin dist/K.BIN
	$(Q)cp contrib/PC/btcnf/pspbtinf.bin dist/PSPBTINF.BIN
	$(Q)cp contrib/PC/btcnf/pspbtmnf.bin dist/PSPBTMNF.BIN
	$(Q)cp contrib/PC/btcnf/pspbtnnf.bin dist/PSPBTNNF.BIN
	$(Q)cp contrib/PSP/fake.cso dist/FAKECSO0.BIN
	$(Q)cp provsh/EBOOT.PBP dist/VBOOTPBP.BIN

encrypt-prx: \
	dist/MODULE.BIN dist/SYSCTRL0.BIN dist/GALAXY00.BIN dist/INFERNO0.BIN dist/STARGATE.BIN dist/EXITGAME.BIN dist/POPCORN0.BIN \
	dist/MARCH330.BIN dist/POPSMAN0.BIN dist/POPS.BIN dist/PSPVMC00.BIN dist/MEDIASYN.BIN dist/H.BIN
ifeq ($(USE_FLASH0_ARK), 1)
	$(Q)python contrib/PC/pack/pack.py -p dist/FLASH0.ARK contrib/PC/pack/packlist.txt
else
	$(Q)-rm -f dist/FLASH0.ARK
endif
# in the end always destroy tmp release key cache
	$(Q)-rm -f $(tmpReleaseKey)


# Only clean non-library code
cleanobj:
	$(Q)$(MAKE) clean CLEANOBJ=1

distclean clean:
ifndef CLEANOBJ
	$(Q)$(MAKE) $@ -C libs
endif
	$(Q)$(MAKE) $@ -C rebootbuffer
	$(Q)$(MAKE) $@ -C exploit
	$(Q)$(MAKE) $@ -C loader/encLoader
	$(Q)$(MAKE) $@ -C loader
	$(Q)$(MAKE) $@ -C systemctrl
	$(Q)$(MAKE) $@ -C galaxy
	$(Q)$(MAKE) $@ -C stargate
	$(Q)$(MAKE) $@ -C exitgame
	$(Q)$(MAKE) $@ -C provsh
	$(Q)$(MAKE) $@ -C popcorn
	$(Q)$(MAKE) $@ -C inferno
	$(Q)-rm -rf dist *~ | true
	$(Q)-rm -f contrib/PC/btcnf/pspbtinf.bin
	$(Q)-rm -f contrib/PC/btcnf/pspbtmnf.bin
	$(Q)-rm -f contrib/PC/btcnf/pspbtnnf.bin

subdirs: $(SUBDIRS)

$(filter-out libs, $(SUBDIRS)): libs
	$(Q)$(MAKE) $(OPT) -C $@

libs:
	$(Q)$(MAKE) $(OPT) -C $@

exploit: rebootbuffer

loader: exploit

mkdir-dist:
	$(Q)mkdir dist | true

-include $(PROVITA)/.config
include $(PROVITA)/common/check.mak
include $(PROVITA)/common/quiet.mak
include $(PROVITA)/common/mod_enc.mak

ifeq ($(RELEASE), 1)
	tmpReleaseKey=/tmp/psid_key
	PSID = `if [ -f $(tmpReleaseKey) ]; \
			then :; \
			else od -A n -t x -N 16 /dev/urandom | tr -d ' ' | tr '[:lower:]' '[:upper:]' > $(tmpReleaseKey); \
			fi; \
			cat $(tmpReleaseKey);`;
	#PSID=$(od -A n -t x -N 16 /dev/urandom | tr -d ' ' | tr '[:lower:]' '[:upper:]')
	DISABLE_PSID_CHECK = 1
endif
