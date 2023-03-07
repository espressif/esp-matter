# ******************************************************************************
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# ******************************************************************************

# ************
# Project Name
# ************
PROJECT_NAME	= TestAL

# ***********
# Executables
# ***********
RM		= rm -f
ECHO		= echo
MKDIR		= mkdir -p
RMDIR		= rm -rf
CP		= cp -R
MV		= mv

# ****************
# Path Definitions
# ****************
PAL_DIR		= $(CURDIR)/pal
HAL_DIR		= $(CURDIR)/hal
CONFIGS_DIR	= $(CURDIR)/configs

# ****************
# Config Files
# ****************
PROJ_CFG_PATH	= proj.cfg

ifeq ($(wildcard $(PROJ_CFG_PATH)),$(PROJ_CFG_PATH))
include $(PROJ_CFG_PATH)
endif

# ********
# Includes
# ********
INCLUDES_PAL	= $(PAL_DIR)/include
INCLUDES_HAL	= $(HAL_DIR)/include

ifeq ($(OS),linux)
INCLUDES	= $(INCLUDES_PAL) $(if $(findstring target,$(DEVICE)), $(INCLUDES_HAL))

else ifeq ($(OS),freertos)
INCLUDES 	= $(INCLUDES_PAL) $(INCLUDES_HAL)

COMPILER_TYPE_DIR = $(shell echo $(COMPILER_TYPE) | tr a-z A-Z)
INCLUDES_FREERTOS = $(KERNEL_DIR)/OS/FreeRTOS/Source/include
INCLUDES_FREERTOS += $(KERNEL_DIR)/OS/FreeRTOS/Source/portable/$(COMPILER_TYPE_DIR)/ARM_$(CORTEX_SHORT)
INCLUDES_FREERTOS += $(KERNEL_DIR)/OS/FreeRTOS-Plus-CLI
INCLUDES_FREERTOS += $(KERNEL_DIR)/OS/FreeRTOS-Plus-TCP/include
INCLUDES_FREERTOS += $(KERNEL_DIR)/OS/FreeRTOS-Plus-TCP/portable/Compiler/$(COMPILER_TYPE_DIR)
INCLUDES_FREERTOS += $(KERNEL_DIR)/boards/$(BOARD)
INCLUDES_FREERTOS += $(KERNEL_DIR)/boards/$(BOARD)/$(CMSIS_DRIVER)
INCLUDES_EXTRA	= $(INCLUDES_FREERTOS)

else ifeq ($(OS),mbedos)
INCLUDES 	= $(INCLUDES_PAL) $(INCLUDES_HAL)

INCLUDES_MBEDOS	= $(KERNEL_DIR)/rtos/rtx5/TARGET_CORTEX_M
INCLUDES_MBEDOS	+= $(KERNEL_DIR)/platform
INCLUDES_EXTRA	= $(INCLUDES_MBEDOS)

else ifeq ($(OS),no_os)
INCLUDES 	= $(INCLUDES_PAL) $(INCLUDES_HAL)
INCLUDES_EXTRA 	= $(PAL_DIR)/$(OS)

endif

# **************
# Make variables
# **************
ifeq ($(COMPILER_TYPE), armcc)
CC 		= armcc
LD 		= armlink
AR		= armar
AS 		= armasm

else ifeq ($(COMPILER_TYPE), armclang)
CC 		= armclang
LD 		= armlink
AR		= armar
AS 		= armclang

else ifeq ($(COMPILER_TYPE), gcc)
CXX		= $(CROSS_COMPILE)g++
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)ld
AR		= $(CROSS_COMPILE)ar
AS		= $(CROSS_COMPILE)as
endif

# ****************
# Project Definitions
# ****************
SRCS_PAL	= $(shell find $(PAL_DIR)/$(OS) -name '*.c' | grep -v malloc.c)
SRCS_HAL	= $(shell find $(HAL_DIR)/$(BOARD) -name '*.c')

OBJS_PAL	= $(SRCS_PAL:.c=.o)
OBJS_HAL	= $(SRCS_HAL:.c=.o)

BIN_PAL		= libPAL__$(OS)__$(CC)__$(shell echo $(CORTEX_SHORT) | tr A-Z a-z).a
BIN_HAL		= libHAL__$(OS)__$(CC)__$(shell echo $(CORTEX_SHORT) | tr A-Z a-z).a

# *****
# Flags
# *****
ifeq ($(COMPILER_TYPE), armcc)
CFLAGS		+= --cpu=$(CORTEX)
CFLAGS		+= --thumb
CFLAGS		+= -DTEST_DEBUG
CFLAGS		+= -DCMSDK_$(CORTEX_SHORT)
CFLAGS		+= -D__CC_ARM -DFPGA_IMAGE
CFLAGS		+= -DportREMOVE_STATIC_QUALIFIER
CFLAGS		+= -DINCLUDE_xTaskGetIdleTaskHandle=1
CFLAGS		+= -DconfigQUEUE_REGISTRY_SIZE=8
CFLAGS		+= -DDX_PLAT_MPS2_PLUS
CFLAGS		+= -O0 -g
CFLAGS		+= -DCC_TEE -DCC_IOT -DCC_REE
CFLAGS		+= $(foreach incdir, $(INCLUDES), -I$(incdir))
CFLAGS		+= $(foreach incdir, $(INCLUDES_EXTRA), -I$(incdir))

CFLAGS		+= $(if $(findstring freertos,$(OS)), -D$(CMSIS_DRIVER))

ARFLAGS		= -rcs

else ifeq ($(COMPILER_TYPE), armclang)
LDFLAGS		+= --cpu=$(CORTEX)
LDFLAGS		+= --map --verbose
LDFLAGS		+= --list=$(PROJ_NAME).map
LDFLAGS		+= --entry=Reset_Handler
LDFLAGS		+= --dangling_debug_address 0xF0000000 # Moves unneeded debug sections

CFLAGS		+= --target=arm-arm-none-eabi
CFLAGS		+= $(if $(strip $(M_ARCH)),-march=$(M_ARCH),-mcpu=$(CORTEX))
CFLAGS		+= -mlittle-endian
CFLAGS		+= -xc -O0 -g -Wno-pragma-pack
CFLAGS		+= -mthumb
CFLAGS		+= $(if $(findstring 1,$(TZM)), -mcmse -DTZM) # Supports CMSE
CFLAGS		+= -DCORTEX_$(CORTEX_SHORT)
CFLAGS		+= -D$(CMSIS_DRIVER)
CFLAGS		+= -DARM$(CORTEX_SHORT)
CFLAGS		+= -DconfigQUEUE_REGISTRY_SIZE=8U
CFLAGS		+= -DCC_TEE -DDX_PLAT_MPS2_PLUS
CFLAGS		+= $(if $(findstring SSE_200,$(CMSIS_DRIVER)), -mfpu=none)
CFLAGS		+= $(if $(findstring SSE_200,$(CMSIS_DRIVER)), -mcmse)
CFLAGS		+= $(foreach incdir, $(INCLUDES), -I$(incdir))
CFLAGS		+= $(foreach incdir, $(INCLUDES_EXTRA), -I$(incdir))

ARFLAGS		= -rcs

else # aarch64-linux-gnu- , arm-xilinx-linux-gnueabi- , arm-none-eabi-, arm-buildroot-linux-gnueabi-,
# aarch64-buildroot-linux-gnu- and native
CFLAGS		+= $(if $(findstring target,$(DEVICE)), \
		   $(if $(strip $(M_ARCH)),-march=$(M_ARCH),-mcpu=$(CORTEX)),)
CFLAGS		+= -DTEST_DEBUG  -DLITTLE__ENDIAN -DHASLONGLONG
CFLAGS		+= -Wall -Wsign-compare -Wextra
CFLAGS		+= -Wstrict-prototypes -Wwrite-strings
CFLAGS		+= -Wswitch-default -Wunreachable-code
CFLAGS		+= -Winit-self -Wjump-misses-init -Wlogical-op
CFLAGS		+= -o2 -g
CFLAGS		+= $(if $(findstring 1,$(TZM)), -mcmse -DTZM) # Supports CMSE

CFLAGS		+= $(foreach incdir, $(INCLUDES), -I$(incdir))
CFLAGS		+= $(foreach incdir, $(INCLUDES_EXTRA), -I$(incdir))

CFLAGS		+= $(if $(findstring freertos,$(OS)), -D$(CMSIS_DRIVER))

CFLAGS		+= $(if $(findstring mbedos,$(OS)), -Wno-missing-field-initializers, -Wmissing-field-initializers)
CFLAGS		+= $(if $(findstring mbedos,$(OS)), -Wno-unused-parameter)
CFLAGS		+= $(if $(findstring mbedos,$(OS)), -DMBED_DEBUG -DMBED_TRAP_ERRORS_ENABLED=1)
CFLAGS		+= $(if $(findstring mbedos,$(OS)), -std=gnu99 -c)
CFLAGS		+= $(if $(findstring mbedos,$(OS)), -fmessage-length=0 -fno-exceptions -fno-builtin)
CFLAGS		+= $(if $(findstring mbedos,$(OS)), -ffunction-sections -fdata-sections -funsigned-char)
CFLAGS		+= $(if $(findstring mbedos,$(OS)), -fno-delete-null-pointer-checks -fomit-frame-pointer)

ARFLAGS		= -rcs
endif

# ****************
# Build Operations
# ****************
############ Special rules for configuration selection ##############
ifneq ($(wildcard $(PROJ_CFG_PATH)),$(PROJ_CFG_PATH))

all: # default in case there is no proj.cfg and setconfig_ was not used
	$(info Invoke 'make setconfig_<config. name>' to select project configuration )

setconfig_%: $(CONFIGS_DIR)/proj-%.cfg
	$(info [CFG] $< --> $(PROJ_CFG_PATH))
	@ln -s $< $(PROJ_CFG_PATH)

$(CONFIGS_DIR)/proj-%.cfg:
	@$(info Unknown project configuration. $@ does not exist.)
	@exit 1;

clrconfig:
	$(info No active configuration )

############ Special rules for project configuration selection ##############
else

all : check_var print_var pal $(if $(findstring target,$(DEVICE)),hal) passed

check_var: guard-OS guard-CORTEX\
$(if $(findstring target,$(DEVICE)),guard-CROSS_COMPILE guard-BOARD) \
$(if $(findstring freertos,$(OS)),guard-KERNEL_DIR) \
$(if $(findstring mbedos,$(OS)),guard-KERNEL_DIR)

guard-%:
	@ if [ "${${*}}" = "" ]; then \
	echo "$* is undefined"; \
	exit 1; \
	fi

print_var:
	@echo "\033[42m*********************************************\033[0m"
	@echo "\033[42m board[$(BOARD)] device[$(DEVICE)] OS[$(OS)] \033[0m"
	@echo "\033[42m*********************************************\033[0m"

pal: $(OBJS_PAL)
	$(AR) $(ARFLAGS) $(BIN_PAL) $(OBJS_PAL)

hal: $(OBJS_HAL)
	$(AR) $(ARFLAGS) $(BIN_HAL) $(OBJS_HAL) $(PAL_BIN)

passed:
	@echo "\033[42m***************\033[0m"
	@echo "\033[42m* P A S S E D *\033[0m"
	@echo "\033[42m***************\033[0m"

clrconfig:
	$(RM) $(PROJ_CFG_PATH)



endif

clean:
	$(shell find . -name '*.d' -delete)
	$(shell find . -name '*.o' -delete)

distclean: clrconfig clean
	$(shell find . -name '*.a' -delete)
	$(RMDIR) $(shell find $(HAL_DIR) -name 'includes*')
	$(RMDIR) Release

# END PAL Makefile
