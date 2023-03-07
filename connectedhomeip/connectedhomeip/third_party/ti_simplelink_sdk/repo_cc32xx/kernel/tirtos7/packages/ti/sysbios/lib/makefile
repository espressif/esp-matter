SUBDIRS = $(wildcard */*/.)

.PHONY: $(SUBDIRS)

.all-files: $(SUBDIRS)

$(SUBDIRS):
	@ $(MAKE) -C $@ .all-files

CLEANDIRS = $(addprefix clean-,$(SUBDIRS))

.PHONY: $(CLEANDIRS)

.clean: $(CLEANDIRS)

$(CLEANDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) .clean
