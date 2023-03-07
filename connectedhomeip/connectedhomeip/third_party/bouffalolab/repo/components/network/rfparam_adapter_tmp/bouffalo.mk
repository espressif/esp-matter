COMPONENT_ADD_INCLUDEDIRS += rftlv

COMPONENT_SRCS := rftlv/phy_rftlv.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := rftlv

