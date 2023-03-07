# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += .
							 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=  

## This component's src 
COMPONENT_SRCS := dhcp_server_raw.c
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := .


##
#CPPFLAGS += 