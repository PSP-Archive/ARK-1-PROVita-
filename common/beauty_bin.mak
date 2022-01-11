quiet_cmd_compile = CC $<
cmd_compile = $(CC) $(CFLAGS) -c $< -o $@
quiet_cmd_link = LINK $@
cmd_link = $(LD) -T $(LINKFILE) $^ -o $@ $(LIBS)
#quiet_cmd_genpsid = GENPSID $@ $(PSID)
quiet_cmd_genlink = GENLINK $@ $(LOADADDR)
#cmd_genpsid = python $(PROVITA)/contrib/PC/psid/genpsidhash.py $(PSID) $@

%.o: %.c
	@echo $($(quiet)cmd_compile)
	@$(cmd_compile)

%.elf: %.o
	@echo $($(quiet)cmd_link)
	@$(cmd_link)

#psidhash.c:
#	@echo $($(quiet)cmd_genpsid)
#	@$(cmd_genpsid)
