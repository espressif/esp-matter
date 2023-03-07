# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS +=
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := include

## This component's src
COMPONENT_SRCS :=   src/bl_romfs.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

##
#CPPFLAGS +=
