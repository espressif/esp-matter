# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include
							 
## not be exported to project level

## This component's src 
COMPONENT_SRCS := src/dns_server.c \
                  src/web_server.c
				  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

##
#CPPFLAGS += 
