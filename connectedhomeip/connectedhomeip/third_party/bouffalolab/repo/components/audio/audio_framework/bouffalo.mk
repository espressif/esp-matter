# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += audio_framework/include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := audio_framework/include

## This component's src 
COMPONENT_SRCS := audio_framework/audio_framework.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := audio_framework


##
#CPPFLAGS += 
