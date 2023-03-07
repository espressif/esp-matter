# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += ./

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS :=   blog_testc.c \
                    blog_testc1_diable.c \
                    blog_testc2_full.c \
                    blog_testc3_nopri.c \
                    blog_testc4_onlypri.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := ./

##
#CPPFLAGS +=
