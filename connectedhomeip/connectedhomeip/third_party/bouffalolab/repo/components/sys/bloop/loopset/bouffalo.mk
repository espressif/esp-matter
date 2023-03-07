# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS +=

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := src/loopset_led.c \
                  src/loopset_led_cli.c \
                  src/loopset_ir.c \
                  src/loopset_pwm.c \
                  #src/loopset_i2c.c \



COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src


##
##CPPFLAGS += -DCFG_COMPONENT_BLOG_ENABLE=1
