# Program the device.
# Define programs and commands.

# DRIVE?=D:/
ifeq (WSL, $(OS_USED))
USEDRIVE:=$(DRIVE)
else
ifeq (LINUX, $(OS_USED))
USEDRIVE:=$(DRIVE)
else
USEDRIVE:=$(shell $(PYTHON_BIN) $(ENV_PATH)/make/programmers/cmsisdap/selectDrive.py $(DRIVE))
endif
endif


program: $(APPFILE:.$(PROGRAM_EXT)=.hex) postbuild-app
ifeq ($(USEDRIVE), 0)
	@$(ECHO) "$(BYellow)No valid programming path - '$(DRIVE)'$(Color_Off)"
else
ifeq ($(shell test $(USEDRIVE) -gt 1 2>/dev/null; echo $$?),0)
	@$(ECHO) "$(BYellow)More than one DAPLINK drive, use << m.py makefile.py program DRIVE=X:/ >> to select the correct drive$(Color_Off)"
else
ifneq ($(USEDRIVE),)
	@$(ECHO) "$(BPurple)Programming with Drag and Drop: to $(USEDRIVE)\n$< $(Color_Off)"
	@cp $< $(USEDRIVE)
else

	@$(ECHO) "$(BYellow)No valid programming path \(no path, invalid path or more than one path\)$(Color_Off)"
endif
endif
endif

help::
	$(ECHO) "$(BWhite)--- Programmer --- $(Color_Off)\n\
program           - Program with \n$(APPFILE)\n"
