#
# Main Project Makefile
# This Makefile is included directly from the user project Makefile in order to call the bouffalo.mk
# makefiles of all components (in a separate make process) to build all the libraries, then links them
# together into the final file. If so, PWD is the project dir (we assume).
#

#
# This makefile requires the environment variable BL60X_SDK_PATH to be set to the top-level bl60x_sdk directory
# where this file is located.
#

.PHONY: all build clean all_binaries list-components

MAKECMDGOALS ?= all
all: all_binaries
# see below for recipe of 'all' target
#
# # other components will add dependencies to 'all_binaries'. The
# reason all_binaries is used instead of 'all' is so that the flash
# target can build everything without triggering the per-component "to
# flash..." output targets.)

help:
	@echo "Welcome to BL60x SDK build system. make targets:"
	@echo ""
	@echo "make all - Build app, components"
	@echo "make clean - Remove all app components output"
	@echo "make list-components - List all components in the project"
	@echo "make [component name]" - build a component as a library

# Non-interactive targets. Mostly, those for which you do not need to build a binary
# NON_INTERACTIVE_TARGET += defconfig clean% %clean help list-components print_flash_cmd

# dependency checks
ifndef MAKE_RESTARTS
ifeq ("$(filter 4.% 3.81 3.82,$(MAKE_VERSION))","")
$(warning bl60x_sdk build system only supports GNU Make versions 3.81 or newer. You may see unexpected results with other Makes.)
endif

endif  # MAKE_RESTARTS

# can't run 'clean' along with any non-clean targets
ifneq ("$(filter clean% %clean,$(MAKECMDGOALS))" ,"")
ifneq ("$(filter-out clean% %clean,$(MAKECMDGOALS))", "")
$(error bl60x_sdk build system doesn't support running 'clean' targets along with any others. Run 'make clean' and then run other targets separately.)
endif
endif

OS ?=

# make BL60X_SDK_PATH a "real" absolute path
# * works around the case where a shell character is embedded in the environment variable value.
# * changes Windows-style C:/blah/ paths to MSYS style /c/blah
ifeq ("$(OS)","Windows_NT")
# On Windows MSYS2, make wildcard function returns empty string for paths of form /xyz
# where /xyz is a directory inside the MSYS root - so we don't use it.
SANITISED_BL60X_SDK_PATH:=$(realpath $(BL60X_SDK_PATH))
else
SANITISED_BL60X_SDK_PATH:=$(realpath $(wildcard $(BL60X_SDK_PATH)))
endif

export BL60X_SDK_PATH := $(SANITISED_BL60X_SDK_PATH)

ifndef BL60X_SDK_PATH
$(error BL60X_SDK_PATH variable is not set to a valid directory.)
endif

ifneq ("$(BL60X_SDK_PATH)","$(SANITISED_BL60X_SDK_PATH)")
$(error If BL60X_SDK_PATH is overriden on command line, it must be an absolute path with no embedded shell special characters)
endif

ifneq ("$(BL60X_SDK_PATH)","$(subst :,,$(BL60X_SDK_PATH))")
$(error BL60X_SDK_PATH cannot contain colons. If overriding BL60X_SDK_PATH on Windows, use MSYS Unix-style /c/dir instead of C:/dir)
endif

# disable built-in make rules, makes debugging saner
MAKEFLAGS_OLD := $(MAKEFLAGS)
MAKEFLAGS +=-rR

# Default path to the project: we assume the Makefile including this file
# is in the project directory
ifndef PROJECT_PATH
PROJECT_PATH := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))
export PROJECT_PATH
endif

# A list of the "common" makefiles, to use as a target dependency
COMMON_MAKEFILES := $(abspath $(BL60X_SDK_PATH)/make_scripts_riscv/project.mk $(BL60X_SDK_PATH)/make_scripts_riscv/common.mk $(BL60X_SDK_PATH)/make_scripts_riscv/component_wrapper.mk $(firstword $(MAKEFILE_LIST)))
export COMMON_MAKEFILES

# The directory where we put all objects/libraries/binaries. The project Makefile can
# configure this if needed.
ifndef BUILD_DIR_BASE
BUILD_DIR_BASE := $(PROJECT_PATH)/build_out
endif
export BUILD_DIR_BASE

# Component directories. These directories are searched for components (either the directory is a component,
# or the directory contains subdirectories which are components.)
# The project Makefile can override these component dirs, or add extras via EXTRA_COMPONENT_DIRS
ifndef COMPONENT_DIRS
EXTRA_COMPONENT_DIRS ?=
COMPONENT_DIRS := $(PROJECT_PATH)/components $(EXTRA_COMPONENT_DIRS) $(BL60X_SDK_PATH)/components $(BL60X_SDK_PATH)/customer_components $(PROJECT_PATH)/$(PROJECT_NAME) $(PROJECT_COMPONENT)
endif
export COMPONENT_DIRS

# The project Makefile can define a list of components, but if it does not do this we just take all available components
# in the component dirs. A component is COMPONENT_DIRS directory, or immediate subdirectory,
# which contains a bouffalo.mk file.
#
# Use the "make list-components" target to debug this step.
#ifndef COMPONENTS
# Find all component names. The component names are the same as the
# directories they're in, so /bla/components/mycomponent/bouffalo.mk -> mycomponent.
# using by https://stackoverflow.com/questions/3774568/makefile-issue-smart-way-to-scan-directory-tree-for-c-files
ifeq ("$(wildcard ${BL60X_SDK_PATH}/components.mk)","")
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
COMPONENTS_RAL_PATH :=  $(dir $(foreach cd,$(COMPONENT_DIRS),                       \
						$(call rwildcard,$(cd)/,bouffalo.mk) 						\
				))
COMPONENTS := $(sort $(foreach comp,$(COMPONENTS_RAL_PATH),$(lastword $(subst /, ,$(comp)))))
COMPONENTS_REAL_PATH := $(patsubst %/,%,$(COMPONENTS_RAL_PATH))
else
COMPONENTS := $(notdir $(PROJECT_PATH))
COMPONENTS_REAL_PATH := $(PROJECT_PATH)/$(notdir $(PROJECT_PATH))
include $(BL60X_SDK_PATH)/components.mk
$(info use existing components.mk file)
endif
#endif
# After a full manifest of component names is determined, subtract the ones explicitly omitted by the project Makefile.
ifdef INCLUDE_COMPONENTS
# match exclude comps with EXCLUDE_COMPONENTS variable
define include_comps_add
include_path += $(filter %/$(1), $(COMPONENTS_REAL_PATH))
endef
$(foreach comp,$(INCLUDE_COMPONENTS),$(eval $(call include_comps_add,$(comp))))
INCLUDE_COMPONENTS_REAL_PATH := $(include_path)
# include components
COMPONENTS := $(filter $(INCLUDE_COMPONENTS), $(COMPONENTS))
COMPONENTS_REAL_PATH := $(filter $(INCLUDE_COMPONENTS_REAL_PATH), $(COMPONENTS_REAL_PATH))
endif
export COMPONENTS

# Resolve all of COMPONENTS into absolute paths in COMPONENT_PATHS.
#
# If a component name exists in multiple COMPONENT_DIRS, we take the first match.
#
# NOTE: These paths must be generated WITHOUT a trailing / so we
# can use $(notdir x) to get the component name.
# TODO remove multiple name in components
#COMPONENT_PATHS := $(foreach comp,$(COMPONENTS),$(firstword $(foreach cd,$(COMPONENT_DIRS),$(wildcard $(dir $(cd))$(comp) $(cd)/$(comp)))))
#COMPONENT_PATHS := $(patsubst %/,%,$(COMPONENTS_RAL_PATH))
COMPONENT_PATHS := $(COMPONENTS_REAL_PATH)
export COMPONENT_PATHS


# Initialise project-wide variables which can be added to by
# each component.
#
# These variables are built up via the component_project_vars.mk
# generated makefiles (one per component).
#
# See docs/build-system.rst for more details.
COMPONENT_INCLUDES :=
COMPONENT_LDFLAGS :=
COMPONENT_SUBMODULES :=
COMPONENT_LIBRARIES :=

# COMPONENT_PROJECT_VARS is the list of component_project_vars.mk generated makefiles
# for each component.
#
# Including $(COMPONENT_PROJECT_VARS) builds the COMPONENT_INCLUDES,
# COMPONENT_LDFLAGS variables and also targets for any inter-component
# dependencies.
#
# See the component_project_vars.mk target in component_wrapper.mk
COMPONENT_PROJECT_VARS := $(addsuffix /component_project_vars.mk,$(notdir $(COMPONENT_PATHS) ))
COMPONENT_PROJECT_VARS := $(sort $(COMPONENT_PROJECT_VARS))
COMPONENT_PROJECT_VARS := $(addprefix $(BUILD_DIR_BASE)/,$(COMPONENT_PROJECT_VARS))
# this line is -include instead of include to prevent a spurious error message on make 3.81
-include $(COMPONENT_PROJECT_VARS)

# Also add top-level project include path, for top-level includes
# COMPONENT_INCLUDES += $(abspath $(BUILD_DIR_BASE)/include/)

export COMPONENT_INCLUDES

# Set variables common to both project & component
include $(BL60X_SDK_PATH)/make_scripts_riscv/common.mk
include $(BL60X_SDK_PATH)/make_scripts_riscv/toolchain.mk

all:
ifeq ("$(OS)","Windows_NT")
else
ifeq ("$(CONFIG_CHIP_NAME)", "BL602")
endif
endif
	@echo "Building Finish. To flash build output."

bins: all
	@cd $(BL60X_SDK_PATH)/image_conf; python3 flash_build.py $(PROJECT_NAME) $(CONFIG_CHIP_NAME)
info: all
	@cd $(BL60X_SDK_PATH)/image_conf; env SDK_APP_BIN=$(APP_BIN) SDK_APP_MAP=$(APP_MAP) python3 codesize.py

# If we have `version.mk` then prefer that for extracting BL60x_SP_SDK version
ifeq ("$(wildcard ${BL60X_SDK_PATH}/version.mk)","")
BL_SDK_VER := $(shell cd ${BL60X_SDK_PATH} && git describe --always --tags --dirty)
EXTRA_CPPFLAGS ?=
EXTRA_CPPFLAGS += -D BL_SDK_VER=\"$(BL_SDK_VER)\"

$(info use git describe to generate Version Define)
else
include $(BL60X_SDK_PATH)/version.mk
$(info use existing version.mk file)
endif
BL_CHIP_NAME := ${CONFIG_CHIP_NAME}

# Set default LDFLAGS
# -nostdlib
# --specs=nosys.specs
EXTRA_LDFLAGS ?= -Wl,--cref -nostartfiles

ifeq ("$(CONFIG_CHIP_NAME)", "VIRTEX7")
LDFLAGS ?=  \
	-march=rv32imc -mabi=ilp32 \
	-g3 -nostartfiles -Wl,--noinhibit-exec,--gc-sections,--relax -Wl,--cref \
	-Wl,--start-group       \
	$(COMPONENT_LDFLAGS) \
	-Wl,--end-group \

else
E21_CPU_LDFLAGS := -march=rv32imfc \
                   -mabi=ilp32f

LDFLAGS ?=  $(E21_CPU_LDFLAGS) \
	$(EXTRA_LDFLAGS) \
	-Wl,--gc-sections	\
	-Wl,-static	\
	-Wl,--start-group	\
	$(COMPONENT_LDFLAGS) \
	-Wl,--end-group \
	-Wl,-EL \
	-lm
endif

# Set default CPPFLAGS, CFLAGS, CXXFLAGS
# These are exported so that components can use them when compiling.
# If you need your component to add CFLAGS/etc for it's own source compilation only, set CFLAGS += in your component's Makefile.
# If you need your component to add CFLAGS/etc globally for all source
#  files, set CFLAGS += in your component's Makefile.projbuild
# If you need to set CFLAGS/CPPFLAGS/CXXFLAGS at project level, set them in application Makefile
#  before including project.mk. Default flags will be added before the ones provided in application Makefile.

# CPPFLAGS used by C preprocessor
# If any flags are defined in application Makefile, add them at the end.
CPPFLAGS ?=
# Enable GNU extensions
CPPFLAGS += -D _GNU_SOURCE
ifeq ($(CONFIG_ENABLE_ACP),1)
CPPFLAGS += -DCONF_USER_ENABLE_ACP
endif
ifeq ($(CONFIG_ENABLE_CAMERA),1)
CPPFLAGS += -DCONF_USER_ENABLE_CAMERA
endif
ifeq ($(CONFIG_ENABLE_PSM_RAM),1)
CPPFLAGS += -DCONF_USER_ENABLE_PSRAM
endif
EXTRA_CPPFLAGS ?=
CPPFLAGS += -D BL_CHIP_NAME=\"$(BL_CHIP_NAME)\" -MMD -MP $(EXTRA_CPPFLAGS)
CPPFLAGS += -DARCH_RISCV

# Warnings-related flags relevant both for C and C++
COMMON_WARNING_FLAGS = -Wall -Werror=all \
	-Wno-error=unused-function \
	-Wno-error=unused-but-set-variable \
	-Wno-error=unused-variable \
	-Wno-error=deprecated-declarations \
	-Wextra \
	-Wno-unused-parameter -Wno-sign-compare

ifdef CONFIG_WARN_WRITE_STRINGS
COMMON_WARNING_FLAGS += -Wwrite-strings
endif #CONFIG_WARN_WRITE_STRINGS

ifdef CONFIG_ENABLE_PSM_RAM
COMMON_WARNING_FLAGS += -DCONF_USER_ENABLE_PSRAM
endif

# Flags which control code generation and dependency generation, both for C and C++
# -nostdlib
# -fno-common
# -funwind-tables
COMMON_FLAGS = \
	-ffunction-sections -fdata-sections \
	-fstrict-volatile-bitfields \
	-fcommon \

# Add -fshort-enums temporarily for bl702
ifeq ("$(CONFIG_CHIP_NAME)", "BL702")
COMMON_FLAGS += -fshort-enums
endif


COMMON_FLAGS_M4_EXT := 	\
	-ffreestanding \
	-fno-strict-aliasing


COMMON_FLAGS += $(COMMON_FLAGS_M4_EXT)

ifdef CONFIG_STACK_CHECK_NORM
COMMON_FLAGS += -fstack-protector
endif
ifdef CONFIG_STACK_CHECK_STRONG
COMMON_FLAGS += -fstack-protector-strong
endif
ifdef CONFIG_STACK_CHECK_ALL
COMMON_FLAGS += -fstack-protector-all
endif
ifeq ($(CONFIG_ENABLE_FP),1)
COMMON_FLAGS += -fno-omit-frame-pointer -DCONF_ENABLE_FRAME_PTR
endif
ifeq ($(CONF_ENABLE_FUNC_BACKTRACE),1)
PROJ_ELF:=$(PROJECT_NAME).elf
COMMON_FLAGS += -fno-omit-frame-pointer -DCONF_ENABLE_FUNC_BACKTRACE_ELF=$(PROJ_ELF) -DCONF_BUILD_PATH=$(BUILD_DIR_BASE)/$(PROJ_ELF)
endif

ifeq ($(CONFIG_ENABLE_STACK_OVERFLOW_CHECK),1)
COMMON_FLAGS += -finstrument-functions -DCONF_ENABLE_STACK_OVERFLOW_CHECK
endif

ifeq ("$(CONFIG_CHIP_NAME)", "VIRTEX7")
OPTIMIZATION_FLAGS = -O2
else
ifdef CONFIG_OPTIMIZATION_LEVEL_RELEASE
OPTIMIZATION_FLAGS = -Os
else
OPTIMIZATION_FLAGS = -Os
endif
endif

ifdef CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED
CPPFLAGS += -DNDEBUG
endif

# Enable generation of debugging symbols
# (we generate even in Release mode, as this has no impact on final binary size.)
DEBUG_FLAGS ?= -gdwarf

# List of flags to pass to C compiler
# If any flags are defined in application Makefile, add them at the end.
EXTRA_CFLAGS ?=

ifeq ("$(CONFIG_CHIP_NAME)", "VIRTEX7")
E21_CPU_CFLAGS := -march=rv32imc -mabi=ilp32

ASMFLAGS := $(E21_CPU_CFLAGS)
CFLAGS := $(strip \
	-march=rv32imc -mabi=ilp32 \
	-O2 -Wall -Wchar-subscripts -Wformat -Wuninitialized -Winit-self -Wignored-qualifiers -Wswitch-default -Wunused -Wundef -Werror -Wno-switch-default -Wno-unused -fdata-sections -ffunction-sections -g3 \
	-fmessage-length=0 -std=gnu99 \
	-save-temps=obj \
	)

CXXFLAGS := $(strip \
	-march=rv32imc -mabi=ilp32 \
	-O2 -Wall -Wchar-subscripts -Wformat -Wuninitialized -Winit-self -Wignored-qualifiers -Wswitch-default -Wunused -Wundef -Werror -Wno-switch-default -Wno-unused -fdata-sections -ffunction-sections -g3 \
	-fmessage-length=0 -std=gnu99 \
	-save-temps=obj \
	)
else
E21_CPU_CFLAGS := -march=rv32imfc \
                   -mabi=ilp32f

ASMFLAGS := $(E21_CPU_CFLAGS)

CFLAGS := $(strip \
	-std=gnu99 \
	$(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) -Wno-old-style-declaration \
	$(CFLAGS) \
	$(E21_CPU_CFLAGS) \
	$(EXTRA_CFLAGS)) \
	-save-temps=obj

CXXFLAGS := $(strip \
	-std=c++11 \
	$(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) \
	$(COMMON_FLAGS) \
	$(COMMON_WARNING_FLAGS) \
	$(CXXFLAGS) \
	$(E21_CPU_CFLAGS) \
	-nostdlib \
	-g3 \
	-fms-extensions \
	-ffunction-sections \
	-fdata-sections \
	-Wall \
	-Wchar-subscripts \
	-Wformat \
	-Winit-self \
	-Wignored-qualifiers \
	-Wswitch-default \
	-Wunused \
	-Wundef \
	-fno-rtti -fno-exceptions \
	-save-temps=obj \
	-fno-use-cxa-atexit\
	)

endif
export CFLAGS CPPFLAGS CXXFLAGS ASMFLAGS

# Set default values that were not previously defined
CC ?= gcc
LD ?= ld
AR ?= ar
OBJCOPY ?= objcopy
SIZE ?= size
STRIP ?= strip

# Set host compiler and binutils
HOSTCC := $(CC)
HOSTLD := $(LD)
HOSTAR := $(AR)
HOSTOBJCOPY := $(OBJCOPY)
HOSTSIZE := $(SIZE)
export HOSTCC HOSTLD HOSTAR HOSTOBJCOPY SIZE

ifndef CONFIG_TOOLPREFIX
$(error Please Configure CONFIG_TOOLPREFIX at toolchain.mk)
endif

# Set target compiler. Defaults to whatever the user has
# configured as prefix + ye olde gcc commands
CC := $(call dequote,$(CONFIG_TOOLPREFIX))gcc
CXX := $(call dequote,$(CONFIG_TOOLPREFIX))g++
LD := $(call dequote,$(CONFIG_TOOLPREFIX))ld
AR := $(call dequote,$(CONFIG_TOOLPREFIX))ar
OBJCOPY := $(call dequote,$(CONFIG_TOOLPREFIX))objcopy
SIZE := $(call dequote,$(CONFIG_TOOLPREFIX))size
STRIP := $(call dequote,$(CONFIG_TOOLPREFIX))strip
export CC CXX LD AR OBJCOPY SIZE

PYTHON=$(call dequote,$(CONFIG_PYTHON))

# the app is the main executable built by the project
APP_ELF:=$(BUILD_DIR_BASE)/$(PROJECT_NAME).elf
APP_MAP:=$(APP_ELF:.elf=.map)
APP_BIN:=$(APP_ELF:.elf=.bin)

# Include any Makefile.projbuild file letting components add
# configuration at the project level
define includeProjBuildMakefile
$(if $(V),$$(info including $(1)/Makefile.projbuild...))
COMPONENT_PATH := $(1)
include $(1)/Makefile.projbuild
endef
$(foreach componentpath,$(COMPONENT_PATHS), \
	$(if $(wildcard $(componentpath)/Makefile.projbuild), \
		$(eval $(call includeProjBuildMakefile,$(componentpath)))))

# ELF depends on the library archive files for COMPONENT_LIBRARIES
# the rules to build these are emitted as part of GenerateComponentTarget below
#
# also depends on additional dependencies (linker scripts & binary libraries)
# stored in COMPONENT_LINKER_DEPS, built via bouffalo.mk files' COMPONENT_ADD_LINKER_DEPS variable
COMPONENT_LINKER_DEPS ?=
$(APP_ELF): $(foreach libcomp,$(COMPONENT_LIBRARIES),$(BUILD_DIR_BASE)/$(libcomp)/lib$(libcomp).a) $(COMPONENT_LINKER_DEPS) $(COMPONENT_PROJECT_VARS)
	$(summary) LD $(patsubst $(PWD)/%,%,$@)
ifeq ($(CONFIG_ZIGBEE), 1)
	$(CXX) -o $@ $(LDFLAGS) -Wl,-Map=$(APP_MAP)
else ifeq ($(CONFIG_CPP_ENABLE), 1)
	$(CXX) -o $@ $(LDFLAGS) -Wl,-Map=$(APP_MAP)
else
	$(CC) $(LDFLAGS) -o $@ -Wl,-Map=$(APP_MAP) $(shell find build_out/ -name bugkiller_*.o)
endif

all_binaries: $(APP_BIN)

## TODO move to bl60x_elftool component
$(APP_BIN): $(APP_ELF)
	@echo "Generating BIN File to $@"
ifeq ("$(CONFIG_CHIP_NAME)", "VIRTEX7")
	$(OBJCOPY) -O ihex $< $(@:.bin=.hex)
else
ifeq ($(CONFIG_ENABLE_ACP),1)
	$(OBJCOPY) -S -O binary -R .rom.cpu1 -R .bugkiller_command -R .bugkiller $< $(@:.bin=.cpu0.bin)
	$(OBJCOPY) -S -O binary -j .rom.cpu1 $< $(@:.bin=.cpu1.bin)
	cp $(@:.bin=.cpu0.bin) $(@:.bin=.acp.bin)
	dd if=$(@:.bin=.cpu1.bin) of=$(@:.bin=.acp.bin) bs=512 seek=2 conv=notrunc
	cp $(@:.bin=.acp.bin) $@
else
ifeq ($(CONFIG_LINK_ROM),1)
	$(OBJCOPY) -S -O binary -R .romdata -R .rom -R .bugkiller_command -R .bugkiller $< $@
	$(OBJCOPY) -S -O binary -j .rom $< $(@:.bin=.rom.bin)
	$(OBJCOPY) -S -O binary -j .romdata $< $(@:.bin=.romdata.bin)
	$(OBJCOPY) -S -O binary -R .romdata -R .rom -R .bugkiller_command -R .bugkiller $< $(@:.bin=.flash.bin)
else
ifeq ($(CONFIG_GEN_ROM),1)
	$(OBJCOPY) -S -O binary -R .bleromro -R .bleromrw -R .rtosromro -R .rtosromrw -R .bugkiller_command -R .bugkiller $< $@
	$(OBJCOPY) -S -O binary -j .bleromro $< $(@:.bin=.bleromro.bin)
	$(OBJCOPY) -S -O binary -j .bleromrw $< $(@:.bin=.bleromrw.bin)
	$(OBJCOPY) -S -O binary -j .rtosromro $< $(@:.bin=.rtosromro.bin)
	$(OBJCOPY) -S -O binary -j .rtosromrw $< $(@:.bin=.rtosromrw.bin)
	$(OBJCOPY) -S -O binary -R .bleromro -R .bleromrw -R .rtosromro -R .rtosromrw -R .bugkiller_command -R .bugkiller $< $(@:.bin=.flash.bin)
else
	$(OBJCOPY) -S -O binary -R .bugkiller_command -R .bugkiller $< $@
endif
endif
endif
endif

$(BUILD_DIR_BASE):
	mkdir -p $(BUILD_DIR_BASE)

# Macro for the recursive sub-make for each component
# $(1) - component directory
# $(2) - component name only
#
# Is recursively expanded by the GenerateComponentTargets macro
define ComponentMake
+$(MAKE) -C $(BUILD_DIR_BASE)/$(2) -f $(BL60X_SDK_PATH)/make_scripts_riscv/component_wrapper.mk COMPONENT_MAKEFILE=$(1)/bouffalo.mk COMPONENT_NAME=$(2)
endef

# Generate top-level component-specific targets for each component
# $(1) - path to component dir
# $(2) - name of component
#
define GenerateComponentTargets
.PHONY: component-$(2)-build component-$(2)-clean

component-$(2)-build: $(call prereq_if_explicit, component-$(2)-clean) | $(BUILD_DIR_BASE)/$(2)
	$(call ComponentMake,$(1),$(2)) build

component-$(2)-clean: | $(BUILD_DIR_BASE)/$(2) $(BUILD_DIR_BASE)/$(2)/component_project_vars.mk
	$(call ComponentMake,$(1),$(2)) clean

$(BUILD_DIR_BASE)/$(2):
	@mkdir -p $(BUILD_DIR_BASE)/$(2)

# tell make it can build any component's library by invoking the -build target
# (this target exists for all components even ones which don't build libraries, but it's
# only invoked for the targets whose libraries appear in COMPONENT_LIBRARIES and hence the
# APP_ELF dependencies.)
$(BUILD_DIR_BASE)/$(2)/lib$(2).a: component-$(2)-build
	$(details) "Target '$$^' responsible for '$$@'" # echo which build target built this file

# add a target to generate the component_project_vars.mk files that
# are used to inject variables into project make pass (see matching
# component_project_vars.mk target in component_wrapper.mk).
#
# If any component_project_vars.mk file is out of date, the make
# process will call this target to rebuild it and then restart.
#
$(BUILD_DIR_BASE)/$(2)/component_project_vars.mk: $(1)/bouffalo.mk $(COMMON_MAKEFILES) $(SDKCONFIG_MAKEFILE) | $(BUILD_DIR_BASE)/$(2)
	$(call ComponentMake,$(1),$(2)) component_project_vars.mk
endef

define GenerateComponentNameTargets
$(1):$(BUILD_DIR_BASE)/$(1)/lib$(1).a
	$(details) "Target '$$^' responsible for '$$@'"
endef

$(foreach component,$(COMPONENT_PATHS),$(eval $(call GenerateComponentTargets,$(component),$(notdir $(component)))))
$(foreach component,$(COMPONENT_PATHS),$(eval $(call GenerateComponentNameTargets,$(notdir $(component)))))

app-clean: $(addprefix component-,$(addsuffix -clean,$(notdir $(COMPONENT_PATHS))))
	$(summary) RM $(APP_ELF)
	rm -f $(APP_ELF) $(APP_BIN) $(APP_MAP)

flash: all
	cd $(BL60X_SDK_PATH)/tools/flash_tool && env SDK_APP_BIN=$(APP_BIN) SDK_BOARD=$(PROJECT_BOARD) SDK_NAME=$(PROJECT_NAME) SDK_MEDIA_BIN=$(APP_MEDIA_BIN) SDK_ROMFS_DIR=$(APP_ROMFS_DIR) SDK_DTS=$(PROJECT_DTS) SDK_XTAL=$(PROJECT_BOARD_XTAL) BL_FLASH_TOOL_INPUT_PATH_cfg2_bin_input=$(APP_BIN) ./bflb_iot_tool --chipname=BL602 --baudrate=2000000 --port=/dev/ttyUSB1 --pt=$(PROJECT_PATH)/img_conf/partition_cfg_2M.toml --dts=$(PROJECT_PATH)/img_conf/bl_factory_params_IoTKitA_40M.dts --firmware=$(APP_BIN) 

#burn code to bl602 board, now only support BL602 IOT-DVK-3s platform, if you want to support older bl602 board you need change parameter port to /dev/ttyUSB0 
flash_only:
	cd $(BL60X_SDK_PATH)/tools/flash_tool && env SDK_APP_BIN=$(APP_BIN) SDK_BOARD=$(PROJECT_BOARD) SDK_NAME=$(PROJECT_NAME) SDK_MEDIA_BIN=$(APP_MEDIA_BIN) SDK_ROMFS_DIR=$(APP_ROMFS_DIR) SDK_DTS=$(PROJECT_DTS) SDK_XTAL=$(PROJECT_BOARD_XTAL) BL_FLASH_TOOL_INPUT_PATH_cfg2_bin_input=$(APP_BIN) ./bflb_iot_tool --chipname=BL602 --baudrate=2000000 --port=/dev/ttyUSB1 --pt=$(PROJECT_PATH)/img_conf/partition_cfg_2M.toml --dts=$(PROJECT_PATH)/img_conf/bl_factory_params_IoTKitA_40M.dts --firmware=$(APP_BIN) 

clean: app-clean

# PHONY target to list components in the build and their paths
list-components:
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENT_DIRS (components searched for here))
	$(foreach cd,$(COMPONENT_DIRS),$(info $(cd)))
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENTS (list of component names))
	$(info $(COMPONENTS))
	$(info $(call dequote,$(SEPARATOR)))
	$(info EXCLUDE_COMPONENTS (list of excluded names))
	$(info $(if $(EXCLUDE_COMPONENTS),$(EXCLUDE_COMPONENTS),(none provided)))
	$(info $(call dequote,$(SEPARATOR)))
	$(info COMPONENT_PATHS (paths to all components):)
	$(foreach cp,$(COMPONENT_PATHS),$(info $(cp)))

local-ci-test:
	../../tools/ci/scripts/local-ci-test.sh
