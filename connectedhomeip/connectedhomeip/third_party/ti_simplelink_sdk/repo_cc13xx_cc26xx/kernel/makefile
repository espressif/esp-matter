# rules for building
all: release debug

# the following content will be active only if a 'tirtos' subdir exists
ifeq ($(wildcard tirtos), tirtos)

.PHONY: tirtos-release tirtos-debug

release: tirtos-release
debug: tirtos-debug

tirtos-release:
	@ $(MAKE) -C tirtos release

tirtos-debug:
	@ $(MAKE) -C tirtos debug

# rules for cleaning
clean: clean-release clean-debug

clean-release: clean-tirtos-release

clean-debug: clean-tirtos-debug

clean-tirtos-release:
	@ $(MAKE) -C tirtos clean-release

clean-tirtos-debug:
	@ $(MAKE) -C tirtos clean-debug

endif

# the following content will be active only if a 'freertos' subdir exists
ifeq ($(wildcard freertos), freertos)

.PHONY: freertos-release freertos-debug

# rules for building
release: freertos-release
debug: freertos-debug

freertos-release:
	@ $(MAKE) -C freertos release

freertos-debug:

# rules for cleaning
clean-release: clean-freertos-release

clean-freertos-release:
	@ $(MAKE) -C freertos clean-release

endif
