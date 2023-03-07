# Component wrapper makefile
#
# This makefile gets called recursively from the project make, once for each component.
# COMPONENT_MAKEFILE is set to point at the bouffalo.mk file for the component itself,
# which is included as part of this process (after default variables are defined).
#
# This makefile comprises multiple stages, marked in blocked comments below.
#
# CWD is the build directory of the component.

ifndef PROJECT_PATH
$(error Make was invoked from $(CURDIR). However please do not run make from the sdk or a component directory; invoke make from the project directory. See the README for details.)
endif


################################################################################
# 1) Set default variables for the component build (including configuration
#	loaded from sdkconfig.)
################################################################################

# Find the path to the component
COMPONENT_PATH := $(abspath $(dir $(COMPONENT_MAKEFILE)))
export COMPONENT_PATH

# COMPONENT_BUILD_DIR is otherwise known as CWD for the build
COMPONENT_BUILD_DIR := $(abspath .)

# include elements common to both project & component makefiles
# (includes project configuration set via menuconfig)
include $(BL60X_SDK_PATH)/make_scripts_riscv/common.mk
include $(PROJECT_PATH)/proj_config.mk

# Some of the following defaults may be overriden by the component's bouffalo.mk makefile,
# during the next step:

# Absolute path of the .a file
COMPONENT_LIBRARY = lib$(COMPONENT_NAME).a

# Source dirs a component has. Default to root directory of component.
COMPONENT_SRCDIRS = .

# By default, include only the include/ dir.
COMPONENT_ADD_INCLUDEDIRS = . include
COMPONENT_ADD_LDFLAGS =

# Define optional compiling macros
define compile_exclude
COMPONENT_OBJEXCLUDE += $(1)
endef

define compile_include
COMPONENT_OBJINCLUDE += $(1)
endef

define compile_only_if
$(eval $(if $(1), $(call compile_include, $(2)), $(call compile_exclude, $(2))))
endef

define compile_only_if_not
$(eval $(if $(1), $(call compile_exclude, $(2)), $(call compile_include, $(2))))
endef

COMPONENT_ADD_LINKER_DEPS ?=
COMPONENT_DEPENDS ?=
COMPONENT_EXTRA_CLEAN ?=
COMPONENT_EXTRA_INCLUDES ?=
COMPONENT_OBJEXCLUDE ?=
COMPONENT_OBJINCLUDE ?=
COMPONENT_SUBMODULES ?=

################################################################################
# 2) Include the bouffalo.mk for the specific component (COMPONENT_MAKEFILE) to
#     override variables & optionally define custom targets. Also include global
#     component makefiles.
################################################################################


# Include any Makefile.componentbuild file letting components add
# configuration at the global component level

# Save component_path; we pass it to the called Makefile.componentbuild
# as COMPILING_COMPONENT_PATH, and we use it to restore the current
# COMPONENT_PATH later.
COMPILING_COMPONENT_PATH := $(COMPONENT_PATH)

define includeCompBuildMakefile
$(if $(V),$(info including $(1)/Makefile.componentbuild...))
COMPONENT_PATH := $(1)
include $(1)/Makefile.componentbuild
endef
$(foreach componentpath,$(COMPONENT_PATHS), \
	$(if $(wildcard $(componentpath)/Makefile.componentbuild), \
		$(eval $(call includeCompBuildMakefile,$(componentpath)))))

#Restore COMPONENT_PATH to what it was
COMPONENT_PATH := $(COMPILING_COMPONENT_PATH)

# Include bouffalo.mk for this component.
-include $(BL60X_SDK_PATH)/make_scripts_riscv/project_val.mk
include $(COMPONENT_MAKEFILE)

ifeq ($(filter $(COMPONENT_NAME),$(LOG_ENABLED_COMPONENTS)),$(COMPONENT_NAME))
CPPFLAGS += -DCFG_COMPONENT_BLOG_ENABLE=1
else
CPPFLAGS += -DCFG_COMPONENT_BLOG_ENABLE=0
endif

ifndef COMPONENT_LIB_ONLY
COMPONENT_ADD_LDFLAGS += -l$(COMPONENT_NAME)
endif

ifdef COMPONENT_LIB_ONLY # Skip all below step if component is a lib
.PHONY: build
build:
	$(details) "No build needed for $(COMPONENT_NAME) (COMPONENT_LIB_ONLY)"

.PHONY: clean
clean:
	$(summary) RM component_project_vars.mk
	rm -f component_project_vars.mk

define lib_MakeVariablePath
$(subst $(BL60X_SDK_PATH),$$(BL60X_SDK_PATH),$(subst $(PROJECT_PATH),$$(PROJECT_PATH),$(subst $(BUILD_DIR_BASE),$$(BUILD_DIR_BASE),$(1))))
endef

component_project_vars.mk::
	$(details) "Building component project variables list $(abspath $@)"
	@echo '# Automatically generated build file. Do not edit.' > $@
	@echo 'COMPONENT_INCLUDES += $(call lib_MakeVariablePath,$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS))))' >> $@
	@echo 'COMPONENT_LDFLAGS += $(COMPONENT_ADD_LDFLAGS_HEAD) $(call lib_MakeVariablePath, $(COMPONENT_ADD_LDFLAGS)) $(COMPONENT_ADD_LDFLAGS_TAIL)' >> $@
	@echo 'COMPONENT_LINKER_DEPS += $(call lib_MakeVariablePath,$(call resolvepath,$(COMPONENT_ADD_LINKER_DEPS),$(COMPONENT_PATH)))' >> $@
	@echo 'component-$(COMPONENT_NAME)-build: $(addprefix component-,$(addsuffix -build,$(COMPONENT_DEPENDS)))' >> $@

# COMPONENT_LIB_ONLY end
else
################################################################################
# 3) Set variables that depend on values that may changed by bouffalo.mk
################################################################################

ifndef COMPONENT_CONFIG_ONLY  # Skip steps 3-5 if COMPONENT_CONFIG_ONLY is set

# Object files which need to be linked into the library
# By default we take all .c, .cpp, .cc & .S files in COMPONENT_SRCDIRS.
ifndef COMPONENT_OBJS
# Find all source files in all COMPONENT_SRCDIRS
COMPONENT_OBJS := $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.c,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.c)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.cpp,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.cpp)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.cc,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.cc)))
COMPONENT_OBJS += $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.S,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.S)))
# Make relative by removing COMPONENT_PATH from all found object paths
COMPONENT_OBJS := $(sort $(COMPONENT_OBJS))
COMPONENT_OBJS := $(patsubst $(COMPONENT_PATH)/%,%,$(COMPONENT_OBJS))
else
# Add in components defined by conditional compiling macros
COMPONENT_OBJS += $(COMPONENT_OBJINCLUDE)
endif
# Remove any leading ../ from paths, so everything builds inside build dir
COMPONENT_OBJS := $(call stripLeadingParentDirs,$(COMPONENT_OBJS))

# Do the same for COMPONENT_OBJEXCLUDE (used below)
COMPONENT_OBJEXCLUDE := $(call stripLeadingParentDirs,$(COMPONENT_OBJEXCLUDE))

# COMPONENT_OBJDIRS is COMPONENT_SRCDIRS with the same transform applied
COMPONENT_OBJDIRS := $(call stripLeadingParentDirs,$(COMPONENT_SRCDIRS))

# Remove items disabled by optional compilation
COMPONENT_OBJS := $(foreach obj,$(COMPONENT_OBJS),$(if $(filter $(abspath $(obj)),$(abspath $(COMPONENT_OBJEXCLUDE))), ,$(obj)))

# Remove duplicates
COMPONENT_OBJS := $(call uniq,$(COMPONENT_OBJS))

# If we're called to compile something, we'll get passed the COMPONENT_INCLUDES
# variable with all the include dirs from all the components in random order. This
# means we can accidentally grab a header from another component before grabbing our own.
# To make sure that does not happen, re-order the includes so ours come first.
COMPONENT_PRIV_INCLUDEDIRS ?=
OWN_INCLUDES:=$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_PRIV_INCLUDEDIRS) $(COMPONENT_ADD_INCLUDEDIRS)))
COMPONENT_INCLUDES := $(OWN_INCLUDES) $(filter-out $(OWN_INCLUDES),$(COMPONENT_INCLUDES))


################################################################################
# 4) Define a target to generate component_project_vars.mk Makefile which
# contains common per-component settings which are included directly in the
# top-level project make
#
# (Skipped if COMPONENT_CONFIG_ONLY is set.)
################################################################################

# macro to generate variable-relative paths inside component_project_vars.mk, whenever possible
# ie put literal $(BL60X_SDK_PATH), $(PROJECT_PATH) and $(BUILD_DIR_BASE) into the generated
# makefiles where possible.
#
# This means if directories move (breaking absolute paths), don't need to 'make clean'
define MakeVariablePath
$(subst $(BL60X_SDK_PATH),$$(BL60X_SDK_PATH),$(subst $(PROJECT_PATH),$$(PROJECT_PATH),$(subst $(BUILD_DIR_BASE),$$(BUILD_DIR_BASE),$(1))))
endef

# component_project_vars.mk target for the component. This is used to
# take bouffalo.mk variables COMPONENT_ADD_INCLUDEDIRS,
# COMPONENT_ADD_LDFLAGS, COMPONENT_DEPENDS and COMPONENT_SUBMODULES
# and inject those into the project make pass.
#
# The target here has no dependencies, as the parent target in
# project.mk evaluates dependencies before calling down to here. See
# GenerateComponentTargets macro in project.mk.
#
# If you are thinking of editing the output of this target for a
# component-specific feature, please don't! What you want is a
# Makefile.projbuild for your component (see docs/build-system.rst for
# more.)
#
# Note: The :: target here is not a mistake. This target should always be
# executed, as dependencies are checked by the parent project-level make target.
# See https://www.gnu.org/software/make/manual/make.html#index-_003a_003a-rules-_0028double_002dcolon_0029
component_project_vars.mk::
	$(details) "Building component project variables list $(abspath $@)"
	@echo '# Automatically generated build file. Do not edit.' > $@
	@echo 'COMPONENT_INCLUDES += $(call MakeVariablePath,$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS))))' >> $@
	@echo 'COMPONENT_LDFLAGS += $(COMPONENT_ADD_LDFLAGS_HEAD) $(call MakeVariablePath,-L$(COMPONENT_BUILD_DIR) $(COMPONENT_ADD_LDFLAGS)) $(COMPONENT_ADD_LDFLAGS_TAIL)' >> $@
	@echo 'COMPONENT_LINKER_DEPS += $(call MakeVariablePath,$(call resolvepath,$(COMPONENT_ADD_LINKER_DEPS),$(COMPONENT_PATH)))' >> $@
	@echo 'COMPONENT_SUBMODULES += $(call MakeVariablePath,$(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_SUBMODULES))))' >> $@
	@echo 'COMPONENT_LIBRARIES += $(COMPONENT_NAME)' >> $@
	@echo 'component-$(COMPONENT_NAME)-build: $(addprefix component-,$(addsuffix -build,$(COMPONENT_DEPENDS)))' >> $@

################################################################################
# 5) Where COMPONENT_OWNBUILDTARGET / COMPONENT_OWNCLEANTARGET
# is not set by bouffalo.mk, define default build, clean, etc. targets
#
# (Skipped if COMPONENT_CONFIG_ONLY is set.)
################################################################################

# Default build behaviour: define a phony build target and a COMPONENT_LIBRARY link target.
ifndef COMPONENT_OWNBUILDTARGET
.PHONY: build
build: $(COMPONENT_LIBRARY)

# Build the archive. We remove the archive first, otherwise ar will get confused if we update
# an archive when multiple filenames have the same name (src1/test.o and src2/test.o)
$(COMPONENT_LIBRARY): $(COMPONENT_OBJS)
	$(summary) AR $(patsubst $(PWD)/%,%,$(CURDIR))/$@
	rm -f $@
	$(AR) cru $@ $^
endif

# If COMPONENT_OWNCLEANTARGET is not set, define a phony clean target
ifndef COMPONENT_OWNCLEANTARGET
CLEAN_FILES := $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_OBJEXCLUDE) $(COMPONENT_OBJEXCLUDE:.o=.d) $(COMPONENT_EXTRA_CLEAN) component_project_vars.mk
.PHONY: clean
clean:
	$(summary) RM $(CLEAN_FILES)
	rm -f $(CLEAN_FILES)
endif

DEBUG_FLAGS ?= -gdwarf

# Include all dependency files already generated
-include $(COMPONENT_OBJS:.o=.d)

# This is a fix for situation where the project or IDF dir moves, and instead
# of rebuilding the target the build fails until make clean is run
#
# It adds an empty dependency rule for the (possibly non-existent) source file itself,
# which prevents it not being found from failing the build
#
# $1 == Source File, $2 == .o file used for .d file name
define AppendSourceToDependencies
echo "$1:" >> $$(patsubst %.o,%.d,$2)
endef


# This pattern is generated for each COMPONENT_SRCDIR to compile the files in it.
define GenerateCompileTargets
# $(1) - directory containing source files, relative to $(COMPONENT_PATH) - one of $(COMPONENT_SRCDIRS)
# $(2) - output build directory, which is $(1) with any leading ".."s converted to "."s to ensure output is always under build/
#

$(2)/%.o: $$(COMPONENT_PATH)/$(1)/%.c $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(COMPONENT_OBJDIRS)
	$$(summary) CC $$(patsubst $$(PWD)/%,%,$$(CURDIR))/$$@
	$$(CC) $$(CFLAGS) $$(CPPFLAGS) -D __FILENAME__=\"$$(notdir $$<)\" -D __FILENAME_WO_SUFFIX__=\"$$(subst .c,,$$(notdir $$<))\" -D __FILENAME_WO_SUFFIX_DEQUOTED__=$$(subst .c,,$$(notdir $$<)) -D __COMPONENT_NAME__=\"$$(COMPONENT_NAME)\" -D __COMPONENT_NAME_DEQUOTED__=$$(COMPONENT_NAME) -D __COMPONENT_FILE_NAME__=\"$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<))\" -D__COMPONENT_FILE_NAMED__=$$(COMPONENT_NAME).$$(subst .c,,$$(notdir $$<)) -D__COMPONENT_FILE_NAME_DEQUOTED__=$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<)) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I $(1) -c $$(abspath $$<) -o $$@
	$(call AppendSourceToDependencies,$$<,$$@)

$(2)/%.o: $$(COMPONENT_PATH)/$(1)/%.cpp $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(COMPONENT_OBJDIRS)
	$$(summary) CXX $$(patsubst $$(PWD)/%,%,$$(CURDIR))/$$@
	$$(CXX) $$(CXXFLAGS) $$(CPPFLAGS) -D __FILENAME__=\"$$(notdir $$<)\" -D __FILENAME_WO_SUFFIX__=\"$$(subst .c,,$$(notdir $$<))\" -D __FILENAME_WO_SUFFIX_DEQUOTED__=$$(subst .c,,$$(notdir $$<)) -D __COMPONENT_NAME__=\"$$(COMPONENT_NAME)\" -D __COMPONENT_NAME_DEQUOTED__=$$(COMPONENT_NAME) -D __COMPONENT_FILE_NAME__=\"$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<))\" -D__COMPONENT_FILE_NAMED__=$$(COMPONENT_NAME).$$(subst .c,,$$(notdir $$<)) -D__COMPONENT_FILE_NAME_DEQUOTED__=$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<)) $$(CPPFLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I $(1) -c $$(abspath $$<) -o $$@
	$(call AppendSourceToDependencies,$$<,$$@)

$(2)/%.o: $$(COMPONENT_PATH)/$(1)/%.cc $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(COMPONENT_OBJDIRS)
	$$(summary) CXX $$(patsubst $$(PWD)/%,%,$$(CURDIR))/$$@
	$$(CXX) $$(CXXFLAGS) $$(CPPFLAGS) -D __FILENAME__=\"$$(notdir $$<)\" -D __FILENAME_WO_SUFFIX__=\"$$(subst .c,,$$(notdir $$<))\" -D __FILENAME_WO_SUFFIX_DEQUOTED__=$$(subst .c,,$$(notdir $$<)) -D __COMPONENT_NAME__=\"$$(COMPONENT_NAME)\" -D __COMPONENT_NAME_DEQUOTED__=$$(COMPONENT_NAME) -D __COMPONENT_FILE_NAME__=\"$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<))\" -D__COMPONENT_FILE_NAMED__=$$(COMPONENT_NAME).$$(subst .c,,$$(notdir $$<)) -D__COMPONENT_FILE_NAME_DEQUOTED__=$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<)) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I $(1) -c $$(abspath $$<) -o $$@
	$(call AppendSourceToDependencies,$$<,$$@)

$(2)/%.o: $$(COMPONENT_PATH)/$(1)/%.S $(COMMON_MAKEFILES) $(COMPONENT_MAKEFILE) | $(COMPONENT_OBJDIRS)
	$$(summary) AS $$(patsubst $$(PWD)/%,%,$$(CURDIR))/$$@
	$$(CC) $$(ASMFLAGS) $$(CPPFLAGS) -D __FILENAME__=\"$$(notdir $$<)\" -D __FILENAME_WO_SUFFIX__=\"$$(subst .c,,$$(notdir $$<))\" -D __FILENAME_WO_SUFFIX_DEQUOTED__=$$(subst .c,,$$(notdir $$<)) -D __COMPONENT_NAME__=\"$$(COMPONENT_NAME)\" -D __COMPONENT_NAME_DEQUOTED__=$$(COMPONENT_NAME) -D __COMPONENT_FILE_NAME__=\"$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<))\" -D__COMPONENT_FILE_NAMED__=$$(COMPONENT_NAME).$$(subst .c,,$$(notdir $$<)) -D__COMPONENT_FILE_NAME_DEQUOTED__=$$(COMPONENT_NAME)$$(subst .c,,$$(notdir $$<)) $$(DEBUG_FLAGS) $$(addprefix -I ,$$(COMPONENT_INCLUDES)) $$(addprefix -I ,$$(COMPONENT_EXTRA_INCLUDES)) -I $(1) -c $$(abspath $$<) -o $$@
	$(call AppendSourceToDependencies,$$<,$$@)

# CWD is build dir, create the build subdirectory if it doesn't exist
#
# (NB: Each .o file depends on all relative component build dirs $(COMPONENT_OBJDIRS), including $(2), to work
# around a behaviour make 3.81 where the first pattern (randomly) seems to be matched rather than the best fit. ie if
# you have objects a/y.o and a/b/c.o then c.o can be matched with $(1)=a & %=b/c, meaning that subdir 'a/b' needs to be
# created but wouldn't be created if $(2)=a. Make 4.x doesn't have this problem, it seems to preferentially
# choose the better match ie $(2)=a/b and %=c )
#
# Note: This may cause some issues for out-of-tree source files and make 3.81 :/
#
$(2):
	mkdir -p $(2)
endef

# Generate all the compile target patterns
$(foreach srcdir,$(COMPONENT_SRCDIRS), $(eval $(call GenerateCompileTargets,$(srcdir),$(call stripLeadingParentDirs,$(srcdir)))))

else   # COMPONENT_CONFIG_ONLY is set

build:
	$(details) "No build needed for $(COMPONENT_NAME) (COMPONENT_CONFIG_ONLY)"

clean:
	$(summary) RM component_project_vars.mk
	rm -f component_project_vars.mk

component_project_vars.mk::  # no need to add variables via bouffalo.mk
	@echo '# COMPONENT_CONFIG_ONLY target sets no variables here' > $@

endif  # COMPONENT_CONFIG_ONLY
endif  # COMPONENT_LIB_ONLY

