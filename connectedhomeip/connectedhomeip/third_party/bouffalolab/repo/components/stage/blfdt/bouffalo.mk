# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += inc

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := src/fdt.c src/fdt_ro.c src/fdt_wip.c src/fdt_sw.c src/fdt_rw.c src/fdt_strerror.c\
src/fdt_empty_tree.c src/fdt_addresses.c src/fdt_overlay.c\
test/tc_blfdt_dump.c test/tc_blfdt_wifi.c \
test/blfdt_cli_test.c \


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src test

##
#CPPFLAGS +=

ifneq ($(CONFIG_CLI_CMD_ENABLE), 0)
CPPFLAGS += -DCONFIG_CLI_CMD_ENABLE
endif
