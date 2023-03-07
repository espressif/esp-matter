# Make include file for building an embedded image for the K8.
# This image will run on the internal ARM Cortex M4 processor in the K8.
#
# The build procedure creates an XML file and a HEX file.
# The XML file contains a link to the HEX file as well as additional metadata.
#
# See GP_P008_IS_10803_ProgrammingIF for details.

#------- Toolchain ------------------------

ARCH            ?=
CROSS_COMPILE   ?=arm-none-eabi-

ifeq (,$(TOOLCHAIN))
# Will use the path variable to find the compiler
Q_TOOLCHAIN_PREFIX=
else
# Use an absolute path, add bin, path separator
Q_TOOLCHAIN_PREFIX= $(TOOLCHAIN)/bin/
endif
# Define programs and commands.
CC          = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)gcc
CXX         = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)g++
AR          = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)ar rsc
LINK        = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)ld
OBJCOPY     = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)objcopy
OBJDUMP     = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)objdump
SIZE        = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)size -A
NM          = $(Q_TOOLCHAIN_PREFIX)$(CROSS_COMPILE)nm

#------- Extensions ------------------------

PROGRAM_EXT :=xml
LIB_EXT     :=a

ifneq (,$(QORVO_MAKEFILE_COMPILERFLAGS_OVERRIDE))

ifneq (,$(PREINCLUDE_HEADER))
CFLAGS_COMPILER += -include $(PREINCLUDE_HEADER)
CXXFLAGS_COMPILER += -include $(PREINCLUDE_HEADER)
endif

else

#------- Specific defines ------------------------

MCU?=$(error MCU should be set from hal buildconfig)

ifneq (,$(filter cortex-m0,$(MCU)))
  MCU_CC += -mcpu=cortex-m0 -mthumb
endif

ifneq (,$(filter cortex-m0plus,$(MCU)))
  MCU_CC += -mcpu=cortex-m0plus -mthumb -fdata-sections
endif

ifneq (,$(filter cortex-m3,$(MCU)))
  MCU_CC += -mcpu=cortex-m3 -mthumb
endif

ifneq (,$(filter cortex-m4,$(MCU)))
  MCU_CC += -mcpu=cortex-m4 -mthumb
endif

ifneq (,$(filter cortex-m4f,$(MCU)))
  MCU_CC += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16
endif

#------- Common C/C++ defines ----------------------

FLAGS_COMPILER +=$(MCU_CC)

ifneq (,$(PREINCLUDE_HEADER))
FLAGS_COMPILER += -include $(PREINCLUDE_HEADER)
endif

#------- C compiler options ------------------------

ifneq (1,$(DISABLE_DEFAULT_CFLAGS))
CFLAGS_COMPILER +=-std=gnu99
CFLAGS_COMPILER +=-Os
CFLAGS_COMPILER +=-g
#attention: -fpack-struct will produce non-working code
#i/o registers, which are mapped to structs, are often not byte accessible
CFLAGS_COMPILER +=-funsigned-char -funsigned-bitfields
#CFLAGS_COMPILER +=-ffunction-sections -fstack-usage -Wstack-usage=1024 -Werror='stack-usage=1024' warning for every function, must use -Werror
CFLAGS_COMPILER +=-ffunction-sections -Wstack-usage=1050
CFLAGS_COMPILER +=-Wall -Wformat -Wswitch-default
# Additional warnings, not included by -Wall
CFLAGS_COMPILER +=-fstack-usage
#CFLAGS_COMPILER +=-fshort-wchar # libc complaining if set
endif # DISABLE_DEFAULT_CFLAGS

#Find alignment problems due to casting
# To disable unaligned access: CFLAGS_COMPILER += -mno-unaligned-access
ifeq (,$(FLAGS_COMPILER_DISABLE_ALIGNMENT_WARNING))
CFLAGS_COMPILER += $(if $(filter $<,$(SRC_DISABLE_ALIGNMENT_WARNING)),,-Wcast-align)
endif

# Check on use of undefined diversities in #if statements. Generate warning for this, unless explicitely disabled.
CFLAGS_COMPILER += $(if $(filter $<,$(SRC_DISABLE_UNDEF_WARNING)),,-Wundef)

ifeq (,$(FLAGS_COMPILER_DISABLE_STRICT_ALIASING_WARNING))
CFLAGS_COMPILER += $(if $(filter $<,$(SRC_DISABLE_STRICT_ALIASING_WARNING)),-Wno-strict-aliasing,-Wstrict-aliasing)
endif
CFLAGS_COMPILER +=$(subst $<,-Werror,$(filter $<,$(SRC_NOWARNING)))

#no excuse for these warnings, so flag them as errors. only works for recent versions of gcc
CFLAGS_COMPILER +=-Werror=implicit-function-declaration
CFLAGS_COMPILER +=-Werror=implicit-int
CFLAGS_COMPILER +=-Werror=return-type
CFLAGS_COMPILER += $(FLAGS_COMPILER)

#------- C++ compiler options ------------------------

CXXFLAGS_COMPILER += $(FLAGS_COMPILER)
#CXXFLAGS_COMPILER +=-std=c++11

CXXFLAGS_COMPILER +=-fno-rtti
CXXFLAGS_COMPILER +=-fno-exceptions
CXXFLAGS_COMPILER +=-fno-unwind-tables

#cxxflags from raspberry pi:
CXXFLAGS_COMPILER +=-Wno-reorder
CXXFLAGS_COMPILER +=-Wno-unknown-pragmas

#------- Assembler options ------------------------

ASFLAGS_COMPILER += $(MCU_CC)

#------- Linker options ------------------------
#stdlib not needed
LDFLAGS_COMPILER += $(MCU_CC)
ifneq (1,$(LDFLAGS_NO_NOSTDLIB))
LDFLAGS_COMPILER += -nostdlib
endif
LDFLAGS_COMPILER += -Wl,--gc-sections -fstack-usage
endif # QORVO_MAKEFILE_COMPILERFLAGS_OVERRIDE
LDFLAGS_COMPILER += -Wl,-Map=$*.map,--cref -T $(LINKERSCRIPT)

#------- Library options ------------------------

LIBFLAGS_DIR=$(sort $(subst \,/, $(patsubst %, -L%, $(dir $(LIB)))))
LIBNAMES    =$(sort $(addprefix -l,$(patsubst lib%,%,$(patsubst %.$(LIB_EXT),%,$(notdir $(LIB))))))

LIBFLAGS_COMPILER=-Wl,--start-group $(LIBNAMES) -Wl,--end-group $(LIBFLAGS_DIR)

#------- Linkerscripts ------------------------
#See specific defines
