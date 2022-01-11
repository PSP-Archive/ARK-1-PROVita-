SUBDIRS = SystemCtrlForKernel SystemCtrlForUser KUBridge ansi-c debug colordebugger psid smartstub prebuilt
.PHONY: subdirs $(SUBDIRS)

all: subdirs
	$(Q)cp $(addsuffix /*.a, $(SUBDIRS)) .

distclean clean:
	$(Q)make $@ -C SystemCtrlForKernel $(OPT)
	$(Q)make $@ -C SystemCtrlForUser $(OPT)
	$(Q)make $@ -C KUBridge $(OPT)
	$(Q)make $@ -C ansi-c $(OPT)
	$(Q)make $@ -C debug $(OPT)
	$(Q)make $@ -C colordebugger $(OPT)
	$(Q)make $@ -C psid $(OPT)
	$(Q)make $@ -C smartstub $(OPT)
	$(Q)rm -rf *.a *~ | true

subdirs: $(SUBDIRS)

$(filter-out prebuilt, $(SUBDIRS)):
	$(MAKE) $(OPT) -C $@

prebuilt: ;
