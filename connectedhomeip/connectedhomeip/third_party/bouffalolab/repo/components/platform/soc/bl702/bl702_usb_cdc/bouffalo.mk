# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 

## This component's src 
COMPONENT_SRCS := Src/usb_device.c \
                  Src/usbd_core.c \
                  Src/usbd_ctlreq.c \
                  Src/usbd_ioreq.c \
                  Src/usbd_cdc.c \
                  Src/usbd_cdc_if.c \
                  Src/usbd_conf.c \
                  Src/usbd_desc.c \
                  Src/bl702_hal_pcd.c \
                  Src/bl702_usb_cdc.c \


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src

##
ifndef CONFIG_USE_STD_DRIVER
CPPFLAGS += -DBFLB_USE_HAL_DRIVER
endif
