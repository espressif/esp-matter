
###### Definitions #######
PREREQ_HEADER_GENERATION_TARGETS ?=
COMMA=,
SPACE=$(EMPTY) $(EMPTY)
list2lines = $(subst $(SPACE),\n,$(1))

RM    =rm -rf
MKDIR =mkdir -p
MV    =mv

#Quick Non-Linux shell check
ifeq (,$(PWD))
OS_USED?=WIN
ECHO =echo -e
Color_Off=\e[0m
# Bold
BBlack=\e[1;30m
BRed=\e[1;31m
BGreen=\e[1;32m
BYellow=\e[1;33m
BBlue=\e[1;34m
BPurple=\e[1;35m
BCyan=\e[1;36m
BWhite=\e[1;37m
else
OS_USED?=LINUX
ECHO =echo
Color_Off=
# Bold
BBlack=
BRed=
BGreen=
BYellow=
BBlue=
BPurple=
BCyan=
BWhite=
endif

###### Function to convert Linux-style paths to Windows-style #####
# Roots
LINUX_WSL_ROOT := /home/
WINDOWS_WSL_ROOT := wsl$$/Ubuntu-18.04/home/
LINUX_C_ROOT := /mnt/c/
WINDOWS_C_ROOT := C:/
LINUX_D_ROOT := /mnt/d/
WINDOWS_D_ROOT := D:/
LINUX_E_ROOT := /mnt/e/
WINDOWS_E_ROOT := E:/
LINUX_R_ROOT := /mnt/r/
WINDOWS_R_ROOT := R:/
LINUX_Q_ROOT := /mnt/q/
WINDOWS_Q_ROOT := Q:/
# GPHUB_ROOT will be set by m.py normally, but need to strip trailing slashes as IAR cannot handle double slashes in paths
GPHUB_ROOT ?= $(ROOTDIR)/gpHub
override GPHUB_ROOT := $(patsubst %\,%,$(GPHUB_ROOT))
override GPHUB_ROOT := $(patsubst %/,%,$(GPHUB_ROOT))
# Replace windows roots "function"
replace_win_roots = $(patsubst $(LINUX_Q_ROOT)%,$(WINDOWS_Q_ROOT)%,$(patsubst $(LINUX_R_ROOT)%,$(WINDOWS_R_ROOT)%,$(patsubst $(LINUX_C_ROOT)%,$(WINDOWS_C_ROOT)%,$(patsubst $(LINUX_D_ROOT)%,$(WINDOWS_D_ROOT)%,$(patsubst $(LINUX_E_ROOT)%,$(WINDOWS_E_ROOT)%,$(1))))))
replace_win_roots_includes = $(patsubst -I$(LINUX_Q_ROOT)%,-I$(WINDOWS_Q_ROOT)%,$(patsubst -I$(LINUX_R_ROOT)%,-I$(WINDOWS_R_ROOT)%,$(patsubst -I$(LINUX_C_ROOT)%,-I$(WINDOWS_C_ROOT)%,$(patsubst -I$(LINUX_D_ROOT)%,-I$(WINDOWS_D_ROOT)%,$(patsubst -I$(LINUX_E_ROOT)%,-I$(WINDOWS_E_ROOT)%,$(1))))))


###### Speed optimizations #######

#Disable Parallel build with PAR=
PAR?=j4
ifeq (s,$(findstring s, $(MAKEFLAGS)))
MAKEFLAGS=sr$(PAR)
else
MAKEFLAGS=r$(PAR)
endif

###### Build rules ######

# Deleting the default suffixes
# Slight casing variations on Windows could give compilation using default rules iso our own
.SUFFIXES:

#Rules only active if a configuration was included
ifneq (,$(CURRENT_CONF))

# Used flags by compiler:
CFLAGS_COMPILER  ?=
ASFLAGS_COMPILER ?=
LDFLAGS_COMPILER ?=
LIBFLAGS_COMPILER?=

##### Compiler-specific definitions #######

ifneq (WIN, $(OS_USED))
    NR_OF_PROCESSORS=$(shell nproc)
    ifeq ($(shell test $(NR_OF_PROCESSORS) -le 4; echo $$?),0)
        # For regression machines, use all cores
        NR_OF_PROCESSORS_FOR_COMPILATION?=$(NR_OF_PROCESSORS)
    else
        # For local machines, free up one core
        NR_OF_PROCESSORS_FOR_COMPILATION?=$$(( $(NR_OF_PROCESSORS) - 1 ))
    endif
endif

COMPILER_DEFINES?=$(ENV_PATH)/make/compilers/$(COMPILER)/compiler_defines.mk
-include $(COMPILER_DEFINES)

# Define all object files.
ALL_SOURCES := $(SRC) $(ASRC)
# Change wanted output directory to workdirectory
ALL_SOURCES := $(subst $(GPHUB_ROOT),$(WORKDIR),$(subst $(ROOTDIR),$(WORKDIR),$(subst \,/, $(ALL_SOURCES))))
# Available file extensions that lead to objects
OBJS:= $(ALL_SOURCES)
OBJS:= $(OBJS:.cpp=.o)
OBJS:= $(OBJS:.c=.o)
OBJS:= $(OBJS:.S=.o)
OBJS:= $(OBJS:.s=.o)

# Define all listing files.
LSTS:= $(OBJS:.o=.lst)
# Define all dependency files.
DEPS:= $(OBJS:.o=.d)
# Define all library files
LIB := $(subst \,/, $(LIB))
LIB_DEP := $(filter %.$(LIB_EXT), $(LIB))

OBJ_DIRS:= $(foreach file,$(OBJS),$(dir $(file)))
EXPORT_OBJ_DIRS:= $(foreach file,$(subst $(GPHUB_ROOT),$(WORKDIR),$(subst $(ROOTDIR),$(WORKDIR),$(subst \,/, $(patsubst %, %.o, $(basename $(foreach comp,$(EXPORT_LIBCOMPS),$(SRC_$(comp)) $(ASRC_$(comp)))))))),$(dir $(file)))
LST_DIRS:= $(foreach file,$(LSTS),$(dir $(file)))
DEP_DIRS:= $(foreach file,$(DEPS),$(dir $(file)))
TARGET_DIRS:= $(sort $(OBJ_DIRS) $(EXPORT_OBJ_DIRS) $(LST_DIRS) $(DEP_DIRS) $(dir $(APPFILE))) # sort removes duplicates :-)

#Adjust paths
SRC_NOWARNING := $(subst \,/,$(SRC_NOWARNING))
SRC_DISABLE_ALIGNMENT_WARNING := $(subst \,/,$(SRC_DISABLE_ALIGNMENT_WARNING))
SRC_DISABLE_STRICT_ALIASING_WARNING := $(subst \,/,$(SRC_DISABLE_STRICT_ALIASING_WARNING))
SRC_DISABLE_UNDEF_WARNING := $(subst \,/,$(SRC_DISABLE_UNDEF_WARNING))

###### Output files ######

#Primary target - executable
ifneq (,$(APPFILE))
TARGET_FILES = $(APPFILE)
else
ifneq (,$(SOLIB))
TARGET_FILES = $(SOLIB)
else
TARGET_FILES = $(APPLIB)
endif
endif
#Secondary targets - mainly used in debugging
TARGET_FILES_SEC+=$(WORKDIR)/$(APPNAME).elf
TARGET_FILES_SEC+=$(WORKDIR)/$(APPNAME).map
TARGET_FILES_SEC+=$(WORKDIR)/$(APPNAME).lss
TARGET_FILES_SEC+=$(WORKDIR)/$(APPNAME).sym
TARGET_FILES_SEC+=$(WORKDIR)/$(APPNAME).size.sym
TARGET_FILES_SEC+=$(LSTS)

#Digsim targets - mainly used in Digsim debugging
TARGET_FILES_DIGSIM+=$(WORKDIR)/$(APPNAME).elf
TARGET_FILES_DIGSIM+=$(WORKDIR)/$(APPNAME).sym

###### Flags used in build rules ######

INC := $(subst \,/,$(INC))
CFLAGS       =$(CFLAGS_COMPILER) $(FLAGS)
ASFLAGS      =$(ASFLAGS_COMPILER) $(FLAGS)
CXXFLAGS     =$(CXXFLAGS_COMPILER) $(FLAGS)
ALL_CFLAGS   =$(CFLAGS) $(INC)
ALL_CXXFLAGS =$(CXXFLAGS) $(INC)
ALL_ASFLAGS  =$(ASFLAGS) $(AINC)

#Library inclusion
LIBFLAGS    =$(LIBFLAGS_COMPILER)
LDFLAGS     =$(LDFLAGS_COMPILER)

###### Convert Linux-style paths to Windows-style #####
ifneq (, $(CONVERT_TO_WIN_PATHS))
    # Convert paths:
    INC := $(call replace_win_roots_includes,$(INC))
    AINC := $(call replace_win_roots_includes,$(AINC))
    LIB := $(call replace_win_roots,$(LIB))
    OBJS_COMP := $(call replace_win_roots,$(OBJS))
    TARGET = $(call replace_win_roots,$(@))
    SOURCE = $(call replace_win_roots,$(<))
    STEM = $(call replace_win_roots,$(*))
    PRE_REQS = $(call replace_win_roots,$(^))
    BINSHIPPED_COMP = $(call replace_win_roots,$(BINSHIPPED))
    # Dependency files will be generated with Windows paths => convert back to linux
    CORRECT_DEPS := $(PYTHON_BIN) $(ENV_PATH)/make/compilers/windows_only/correctdep.py -r $(WINDOWS_WSL_ROOT) $(LINUX_WSL_ROOT) -r $(WINDOWS_C_ROOT) $(LINUX_C_ROOT) -r $(WINDOWS_R_ROOT) $(LINUX_R_ROOT) -r $(WINDOWS_D_ROOT) $(LINUX_D_ROOT) -r $(WINDOWS_E_ROOT) $(LINUX_E_ROOT)
    # we need a temporary dependency file to not leave a corrupt one
    DEP_FILE = $(@:.o=.dd)
    DEP_FILE_FLAG = -MF $(call replace_win_roots,$(DEP_FILE))
    PREINCLUDE_HEADER := $(call replace_win_roots,$(PREINCLUDE_HEADER))
else
    # INC := $(INC)
    # LIB := $(LIB)
    TARGET = $@
    SOURCE = $<
    STEM = $*
    PRE_REQS = $^
    OBJS_COMP := $(OBJS)
    BINSHIPPED_COMP = $(BINSHIPPED)
    # No need to correct dependencies when not converting paths
    CORRECT_DEPS :=
    DEP_FILE :=
    DEP_FILE_FLAG :=
    CONFIG_HEADER := $(subst \,/,$(CONFIG_HEADER))
    INTERNALS_HEADER := $(subst \,/,$(INTERNALS_HEADER))
    PREINCLUDE_HEADER := $(subst \,/,$(PREINCLUDE_HEADER))
endif

ifneq (,$(LDORDER))
    ORDERED_OBJS_COMP := $(foreach obj,$(LDORDER),$(filter %$(obj),$(OBJS_COMP)))
else
    ORDERED_OBJS_COMP := $(OBJS_COMP)
endif

###### Default build target ######

.PHONY:app all postbuild-app postbuild-all build-app build-all prebuild-script app-target all-target
app: postbuild-app
all: postbuild-all
digsim: $(TARGET_FILES) $(TARGET_FILES_DIGSIM)

postbuild-app: build-app
ifneq (,$(POSTBUILD_SCRIPT))
	cd "$(WORKDIR)" && \
		export ENV_PATH="$(ENV_PATH)" && \
		sh $(POSTBUILD_SCRIPT) $(dir $(firstword $(MAKEFILE_LIST))) $(APPFILE) && \
		cd -
endif

postbuild-all: build-all
ifneq (,$(POSTBUILD_SCRIPT))
	cd "$(WORKDIR)" && \
		export ENV_PATH="$(ENV_PATH)" && \
		sh $(POSTBUILD_SCRIPT) $(dir $(firstword $(MAKEFILE_LIST))) $(APPFILE) && \
		cd -
endif

ifneq (,$(PREBUILD_SCRIPT))
build-app: prebuild-script
	@$(MAKE) -f $(firstword $(MAKEFILE_LIST)) --no-print-directory app-target
else
build-app: app-target
endif

ifneq (,$(PREBUILD_SCRIPT))
build-all: prebuild-script
	@$(MAKE) -f $(firstword $(MAKEFILE_LIST)) --no-print-directory all-target
else
build-all: all-target
endif

prebuild-script:
ifneq (,$(PREBUILD_SCRIPT))
	 sh $(PREBUILD_SCRIPT) $(dir $(firstword $(MAKEFILE_LIST))) $(APPFILE)
endif

app-target: $(TARGET_FILES)
all-target: $(TARGET_FILES) $(TARGET_FILES_SEC)

###### Default build rules ######

# We make all target directories sequentially because we noticed some strange mkdir errors
# when running mkdir in the compile rule which we suspect enabled to some race condition
$(TARGET_DIRS) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)):
	for d in $@; do $(MKDIR) $$d; done

.PRECIOUS : $(WORKDIR)/%.o

#Include compiler specific rules
COMPILER_RULES?=$(ENV_PATH)/make/compilers/$(COMPILER)/compiler_rules.mk
-include $(COMPILER_RULES)

#Use C or CXX compiler to link
ifdef LINK_CXX
LINK_COMMAND=$(CXX)
else
LINK_COMMAND=$(CC)
endif

.SECONDEXPANSION:
%.elf %.map: $(OBJS) $(BINSHIPPED) $(LINKERSCRIPT) $(LIB_DEP) | $$(dir $$@)
	@$(ECHO) "$(BYellow)Linking $(notdir $@)$(Color_Off)"
	$(LINK_COMMAND) $(ORDERED_OBJS_COMP) $(BINSHIPPED) $(LIBFLAGS) --output $(STEM).elf $(LDFLAGS)

%.sym %.size.sym: %.elf
	@$(ECHO) "$(BWhite)Creating symbol table:$(Color_Off)" $@
	$(NM) -S -n -l $< > $(dir $@)/tmp.sym
	$(MV) $(dir $@)tmp.sym $@
	$(NM) -S --size-sort $< > $(dir $@).size.sym

%.lss: %.elf
	@$(ECHO) "$(BWhite)Creating basic listing:$(Color_Off)" $@
	$(OBJDUMP) -h -d -l $(SOURCE) > $(TARGET)

# Define compilation function to avoid repeating recipes
define compile # (compiler, flags)
	@$(ECHO) "$(BYellow)Building $(notdir $@)$(Color_Off)"
	$(1) -c $(2) $(INC) -DGP_FILENAME=$(basename $(notdir $<)). -MMD $(DEP_FILE_FLAG) $(SOURCE) -o $(TARGET)
	$(CORRECT_DEPS) $(DEP_FILE)
endef

# flag_lookup: if the flag is present, return its name
# flag_value: if the flag has a value, return its value
ifdef CONFIG_HEADER
flag_lookup = $(shell grep -Eq -e '\#define[[:space:]]+$(1)$$' -e '\#define[[:space:]]+$(1)[[:space:]]+' $(CONFIG_HEADER) $(INTERNALS_HEADER) && echo $(1))
flag_value = $(shell sed -ne 's/\#define[[:space:]][[:space:]]*$(1)[[:space:]][[:space:]]*(..*)[[:space:]]*/\1/p' $(CONFIG_HEADER) $(INTERNALS_HEADER))
else
flag_lookup = $(patsubst %,$(1),$(filter -D$(1) -D$(1)=%,$(FLAGS)))
flag_value = $(patsubst -D$(1)=%,%,$(filter -D$(1)=%,$(FLAGS)))
endif

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(ROOTDIR)/%.c | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CC), $(CFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(GPHUB_ROOT)/%.c | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CC), $(CFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(ROOTDIR)/%.cpp | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CXX), $(CXXFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(GPHUB_ROOT)/%.cpp | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CXX), $(CXXFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(ROOTDIR)/%.S | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CC), $(ASFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(ROOTDIR)/%.s | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CC), $(ASFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(GPHUB_ROOT)/%.S | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CC), $(ASFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.o: $(GPHUB_ROOT)/%.s | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call compile,$(CC), $(ASFLAGS))

# Define listing function to avoid repeating recipes
define list # (compiler, flags)
	@$(ECHO) "$(BYellow)Creating $(notdir $@)$(Color_Off)"
	$(1) -c $(2) $(INC) -DGP_FILENAME=$(basename $(notdir $<)). -Wa,-adhlns=$(TARGET) -MMD $(SOURCE)
endef

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(ROOTDIR)/%.c | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CC), $(CFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(GPHUB_ROOT)/%.c | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CC), $(CFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(ROOTDIR)/%.cpp | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CXX), $(CXXFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(GPHUB_ROOT)/%.cpp | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CXX), $(CXXFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(ROOTDIR)/%.S | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CC), $(ASFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(GPHUB_ROOT)/%.S | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CC), $(ASFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(ROOTDIR)/%.s | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CC), $(ASFLAGS))

.SECONDEXPANSION:
$(WORKDIR)/%.lst: $(GPHUB_ROOT)/%.s | $$(dir $$@) $(dir $(PREREQ_HEADER_GENERATION_TARGETS)) $(PREREQ_HEADER_GENERATION_TARGETS)
	$(call list,$(CC), $(ASFLAGS))

ifeq (,$(COMPILER_SPECIFIC_LIB_RULE))
.PHONY:
applib: $(APPLIB)

$(APPLIB): $(BINSHIPPED) $(OBJS)
	#create or update archive
	@$(ECHO) "$(BYellow)Archiving  $@$(Color_Off)"
	$(RM) $@ ; \
	$(AR) $(TARGET) $(BINSHIPPED) $(OBJS_COMP)
#Adding libraries flattend through ar scripting
	(echo "OPEN $(TARGET)"; \
	for lib in $(LIBFILES) ; do echo "ADDLIB " $$lib; done; \
	echo "SAVE"; \
	echo "END") | $(firstword $(AR)) -M
endif

ifeq (,$(COMPILER_SPECIFIC_SO_LIB_RULE))
.PHONY:
solib: $(SOLIB)

$(SOLIB): $(BINSHIPPED) $(OBJS)
	#create or update library
	@$(ECHO) "$(BYellow)Creating .so lib  $@$(Color_Off)"
	$(RM) $@ ; \
	$(CXX) -shared -o $(TARGET) $(BINSHIPPED) $(OBJS_COMP) $(LIBFLAGS) $(LDFLAGS)
endif

#Include extra programmer rules
PROGRAMMER_RULES?=$(ENV_PATH)/make/programmers/$(PROGRAMMER)/programmer_rules.mk
-include $(PROGRAMMER_RULES)

###### Dependencies ######

# We generate a file for each .d files with addition rules to avoid the
# "no rule to make target" whenever a .h-file is deleted or renamed.
%.sd: %.d $(ENV_PATH)/make/make_d_rules.py
	$(PYTHON_BIN) $(ENV_PATH)/make/make_d_rules.py <$< >$@

SAFE_DEPS:= $(OBJS:.o=.sd)

-include $(DEPS) $(SAFE_DEPS)
endif

#Include GreenPeak specific rules
-include $(ENV_PATH)/make/gpinternal.mk

.PHONY:clean
clean:
	$(ECHO) "$(BGreen)---- Cleaning $(APPNAME) ----$(Color_Off)"
	$(RM) $(WORKDIR)

help::
	$(ECHO) "$(BWhite)--- Common ---$(Color_Off)\n\
app        - create targets $(BWhite)(default)$(Color_Off)\n$(call list2lines,$(TARGET_FILES))\n\
all        - create targets+secondary\n$(call list2lines,$(filter-out $(LSTS),$(TARGET_FILES_SEC)))\n\
digsim     - make for digsim - create targets\n$(call list2lines,$(filter-out $(LSTS),$(TARGET_FILES_DIGSIM)))\n\
clean      - clean working directory\n$(WORKDIR)\n"
