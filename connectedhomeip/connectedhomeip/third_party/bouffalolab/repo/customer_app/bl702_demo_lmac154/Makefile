#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := bl702_demo_lmac154
PROJECT_PATH := $(abspath .)
PROJECT_BOARD := evb
export PROJECT_PATH PROJECT_BOARD
#CONFIG_TOOLPREFIX :=

-include ./proj_config.mk

ifeq ($(origin BL60X_SDK_PATH), undefined)
BL60X_SDK_PATH_GUESS ?= $(shell pwd)
BL60X_SDK_PATH ?= $(BL60X_SDK_PATH_GUESS)/../..
$(info ****** Please SET BL70X_SDK_PATH ******)
$(info ****** Trying SDK PATH [$(BL60X_SDK_PATH)])
endif

INCLUDE_COMPONENTS += bl702 bl702_std newlibc bl702_rf hosal vfs yloop utils cli blog
INCLUDE_COMPONENTS += blmtd blfdt
INCLUDE_COMPONENTS += easyflash4
INCLUDE_COMPONENTS += lmac154

ifeq ($(CONFIG_USB_CDC),1)
INCLUDE_COMPONENTS += bl702_usb_cdc
endif

ifeq ($(CONFIG_BT),1)
INCLUDE_COMPONENTS += blecontroller_702_std blestack
endif


ifeq ($(CONFIG_GEN_ROM),1)
	INCLUDE_COMPONENTS += bl702_freertos_rom
else
	ifeq ($(CONFIG_BUILD_ROM_CODE),1)
		INCLUDE_COMPONENTS += bl702_freertos
	else
		INCLUDE_COMPONENTS += bl702_freertos_rom
	endif
endif

INCLUDE_COMPONENTS += $(PROJECT_NAME)

COMPONENT_COMMON := fs libc stage sys utils
COMPONENT_PLATFORM := soc/bl702 hosal
COMPONENT_NETWORK := ble lmac154
COMPONENT_DIRS := $(PROJECT_PATH)/$(PROJECT_NAME)
COMPONENT_DIRS += $(addprefix $(BL60X_SDK_PATH)/components/, $(COMPONENT_COMMON))
COMPONENT_DIRS += $(addprefix $(BL60X_SDK_PATH)/components/platform/, $(COMPONENT_PLATFORM))
COMPONENT_DIRS += $(addprefix $(BL60X_SDK_PATH)/components/network/, $(COMPONENT_NETWORK))

include $(BL60X_SDK_PATH)/make_scripts_riscv/project.mk
