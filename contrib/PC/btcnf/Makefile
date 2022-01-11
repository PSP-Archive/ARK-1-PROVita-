PSPBTCNF_TARGETS = pspbtinf pspbtmnf pspbtnnf
PSPBTCNF_OBJS = $(addsuffix .bin,$(PSPBTCNF_TARGETS))

.PHONY: clean

all: $(PSPBTCNF_OBJS)

clean:
	$(Q)rm -f $(PSPBTCNF_OBJS)

quiet_cmd_btcnf = BTCNF $<
cmd_btcnf = python ./btcnf.py build $<

%.bin:%.txt
	@echo $($(quiet)cmd_btcnf)
	@$(cmd_btcnf)

include ../../../common/quiet.mak
