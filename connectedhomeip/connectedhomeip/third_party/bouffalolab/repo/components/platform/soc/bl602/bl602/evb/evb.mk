toolchains := gcc

# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += 
							 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=							 

## This component's src 
COMPONENT_SRCS1 := evb/src/boot/$(toolchains)/entry.S \
				  evb/src/boot/$(toolchains)/start.S \
				  evb/src/debug.c \
				  evb/src/strntoumax.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS1))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))
COMPONENT_SRCS := $(COMPONENT_SRCS1)
COMPONENT_SRCDIRS := evb/src/boot/$(toolchains) evb/src

ifneq ($(CONFIG_LINK_CUSTOMER),1)

ifeq ($(CONFIG_LINK_RAM),1)
LINKER_SCRIPTS := ram.ld
CPPFLAGS += -DRUN_IN_RAM
else

ifeq ($(CONFIG_LINK_ROM),1)
LINKER_SCRIPTS := flash_rom.ld
else
LINKER_SCRIPTS := flash.ld
endif

endif

##
COMPONENT_ADD_LDFLAGS += -L $(COMPONENT_PATH)/evb/ld \
                         $(addprefix -T ,$(LINKER_SCRIPTS))
##                        
COMPONENT_ADD_LINKER_DEPS := $(addprefix evb/ld/,$(LINKER_SCRIPTS))
endif

ifeq ($(CONFIG_DISABLE_PRINT),1)
CPPFLAGS += -DDISABLE_PRINT
endif

ifeq ($(CONFIG_DISABLE_PRINT_FLOAT),1)
CPPFLAGS += -DDISABLE_PRINT_FLOAT
endif

##
#CPPFLAGS += -D
