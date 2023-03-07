#
# Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
#
# This software, including source code, documentation and related
# materials ("Software") is owned by Cypress Semiconductor Corporation
# or one of its affiliates ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products.  Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
#

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)))
endif

#
# List the supported toolchains
#
CY_SUPPORTED_TOOLCHAINS=GCC_ARM
ifeq ($(filter $(TOOLCHAIN),$(CY_SUPPORTED_TOOLCHAINS)),)
$(error must use supported TOOLCHAIN such as: $(CY_SUPPORTED_TOOLCHAINS))
endif

ifeq ($(OS),Windows_NT)
    CY_OS_DIR=Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CY_OS_DIR=Linux64
    endif
    ifeq ($(UNAME_S),Darwin)
        CY_OS_DIR=OSX
    endif
endif

CY_COMPILER_DIR_BWC:=$(CY_INTERNAL_TOOL_gcc_BASE)
CY_MODUS_SHELL_DIR_BWC:=$(CY_INTERNAL_TOOLS)/$(CY_TOOL_modus-shell_BASE)

################################################################################
# Feature processing
################################################################################
#
# floating point and other device specific compiler flags
#

DIRECT_LOAD?=0

# Bluetooth Device address
ifneq ($(BT_DEVICE_ADDRESS),)
CY_CORE_CGS_ARGS+=-O DLConfigBD_ADDRBase:$(BT_DEVICE_ADDRESS)
endif

# HCI transport
CY_APP_DEFINES+=\
    -DWICED_HCI_TRANSPORT_UART=1 \
    -DWICED_HCI_TRANSPORT_SPI=2
ifeq ($(TRANSPORT),UART)
CY_APP_DEFINES+=-DWICED_HCI_TRANSPORT=1
else
CY_APP_DEFINES+=-DWICED_HCI_TRANSPORT=2
endif

# special handling for chip download
CY_CORE_APP_CHIPLOAD_FLAGS+=-NOHCIRESET

# use btp file to determine flash layout
CY_CORE_LD_DEFS+=BTP=$(CY_CORE_BTP)

#
# Core flags and defines
#
CY_CORE_COMMON_OPTIONS+=\
    -fdata-sections\
    -ffunction-sections\
    -Wall\
    -Wextra\
    -Wshadow\

CY_CORE_CFLAGS+=\
    $(CY_CORE_COMMON_OPTIONS)\

CY_CORE_CXXFLAGS+=\
    $(CY_CORE_COMMON_OPTIONS)\
    -fno-exceptions\

CY_CORE_SFLAGS=

CY_CORE_LDFLAGS=\
    -nostartfiles\
    $(CY_CORE_EXTRA_LD_FLAGS)

CY_CORE_DEFINES+=\
    -DCYW$(CHIP)$(CHIP_REV)=1 \
    -DBCM$(CHIP)$(CHIP_REV)=1 \
    -DBCM$(CHIP)=1 \
    -DCYW$(CHIP)=1 \
    -DCHIP=$(CHIP) \
    -DAPP_CHIP=$(CHIP) \
    -DOTA_CHIP=$(CHIP) \
    -DCHIP_REV_$(BLD)_$(CHIP)$(CHIP_REV)=1 \
    -DCOMPILER_ARM \
    -DSPAR_APP_SETUP=application_setup \
    -DPLATFORM='"$(subst -,_,$(TARGET))"' \
    -D$(subst -,_,$(TARGET))

CY_CORE_EXTRA_DEFINES=\
    -DWICED_SDK_MAJOR_VER=3 \
    -DWICED_SDK_MINOR_VER=2 \
    -DWICED_SDK_REV_NUMBER=0 \
    -DWICED_SDK_BUILD_NUMBER=20467

#
# Set the output file paths
#
ifneq ($(CY_BUILD_LOCATION),)
CY_APP_ELF_FILE?=$(CY_INTERNAL_BUILD_LOCATION)/$(APPNAME)/$(TARGET)/$(CONFIG)/$(APPNAME).elf
else
CY_APP_ELF_FILE?=\$$\{cy_prj_path\}/$(notdir $(CY_INTERNAL_BUILD_LOC))/$(TARGET)/$(CONFIG)/$(APPNAME).elf
endif

#
# Tools specifics
#
# declare configurator support conditionally for all boards
ifneq ($(LIBNAME),BTSDK_TopLevel)
CY_SUPPORTED_TOOL_TYPES+=device-configurator bt-configurator
endif

# hint for bt-configurator
CY_OPEN_bt_configurator_DEVICE=--device 20xxx

# application-specific tools
ifneq ($(CY_OS_DIR),Windows)
  CY_BT_APP_TOOLS_WINDOWS_ONLY=MeshClient WsOtaUpgrade WMBT hello_client
  CY_OPEN_TYPE_LIST+=$(filter-out $(CY_BT_APP_TOOLS_WINDOWS_ONLY),$(CY_BT_APP_TOOLS))
  CY_SUPPORTED_TOOL_TYPES+=$(filter-out $(CY_BT_APP_TOOLS_WINDOWS_ONLY),$(CY_BT_APP_TOOLS))
else
  CY_OPEN_TYPE_LIST+=$(CY_BT_APP_TOOLS)
  CY_SUPPORTED_TOOL_TYPES+=$(CY_BT_APP_TOOLS)
endif

CY_WICED_TOOLS_ROOT=$(SEARCH_btsdk-tools)
CY_WICED_TOOLS_DIR=$(CY_WICED_TOOLS_ROOT)/$(CY_OS_DIR)

CY_BT_APP_TOOLS_UTILS_DIR=$(SEARCH_btsdk-utils)
CY_BT_APP_TOOLS_UTILS_DIR_ABS=$(realpath $(SEARCH_btsdk-utils))
CY_BT_HOST_APPS_BT_BLE_DIR=$(SEARCH_btsdk-host-apps-bt-ble)
CY_BT_HOST_APPS_BT_BLE_DIR_ABS=$(realpath $(SEARCH_btsdk-host-apps-bt-ble))
CY_BT_HOST_PEER_APPS_MESH_DIR=$(SEARCH_btsdk-host-peer-apps-mesh)
CY_BT_HOST_PEER_APPS_MESH_DIR_ABS=$(realpath $(SEARCH_btsdk-host-peer-apps-mesh))
CY_BT_PEER_APPS_OTA_DIR=$(SEARCH_btsdk-peer-apps-ota)
CY_BT_PEER_APPS_BLE_DIR=$(SEARCH_btsdk-peer-apps-ble)

ifneq ($(filter BTSpy,$(CY_BT_APP_TOOLS)),)
ifeq ($(CY_OS_DIR),Windows)
  CY_OPEN_BTSpy_TOOL=$(CY_BT_APP_TOOLS_UTILS_DIR)/BTSpy/$(CY_OS_DIR)/BTSpy.exe
else
  ifeq ($(CY_OS_DIR),Linux64)
    CY_OPEN_BTSpy_TOOL=$(CY_BT_APP_TOOLS_UTILS_DIR)/BTSpy/$(CY_OS_DIR)/RunBtSpy.sh
  else
    CY_OPEN_BTSpy_TOOL=$(CY_BT_APP_TOOLS_UTILS_DIR_ABS)/BTSpy/open.sh
    CY_OPEN_BTSpy_TOOL_ARGS=$(CY_BT_APP_TOOLS_UTILS_DIR_ABS)/BTSpy/$(CY_OS_DIR)/bt_spy.app
  endif
endif
CY_OPEN_BTSpy_tool_EXT=
CY_OPEN_BTSpy_FILE=
CY_OPEN_BTSpy_TOOL_FLAGS=
CY_OPEN_BTSpy_TOOL_NEWCFG_FLAGS=
endif

ifneq ($(filter ClientControl,$(CY_BT_APP_TOOLS)),)
ifeq ($(CY_OS_DIR),Windows)
  CY_OPEN_ClientControl_TOOL=$(CY_BT_HOST_APPS_BT_BLE_DIR)/client_control/$(CY_OS_DIR)/ClientControl.exe
else
  ifeq ($(CY_OS_DIR),Linux64)
    CY_OPEN_ClientControl_TOOL=$(CY_BT_HOST_APPS_BT_BLE_DIR)/client_control/$(CY_OS_DIR)/RunClientControl.sh
  else
    CY_OPEN_ClientControl_TOOL=$(CY_BT_HOST_APPS_BT_BLE_DIR_ABS)/client_control/open.sh
    CY_OPEN_ClientControl_TOOL_ARGS=$(CY_BT_HOST_APPS_BT_BLE_DIR_ABS)/client_control/$(CY_OS_DIR)/ClientControl.app
  endif
endif
endif

ifneq ($(filter MeshClient,$(CY_BT_APP_TOOLS)),)
ifeq ($(CY_OS_DIR),Windows)
CY_OPEN_MeshClient_TOOL=$(CY_BT_HOST_PEER_APPS_MESH_DIR)/peer/$(CY_OS_DIR)/MeshClient/Release/x86/MeshClient.exe
endif
endif

ifneq ($(filter ClientControlMesh,$(CY_BT_APP_TOOLS)),)
ifeq ($(CY_OS_DIR),Windows)
  CY_OPEN_ClientControlMesh_TOOL=$(CY_BT_HOST_PEER_APPS_MESH_DIR)/host/VS_ClientControl/Release/ClientControlMesh
else
  ifeq ($(CY_OS_DIR),Linux64)
    CY_OPEN_ClientControlMesh_TOOL=$(CY_BT_HOST_PEER_APPS_MESH_DIR)/host/Qt_ClientControl/$(CY_OS_DIR)/RunClientControl.sh
  else
    CY_OPEN_ClientControlMesh_TOOL=$(CY_BT_HOST_PEER_APPS_MESH_DIR_ABS)/host/Qt_ClientControl/open.sh
    CY_OPEN_ClientControlMesh_TOOL_ARGS=$(CY_BT_HOST_PEER_APPS_MESH_DIR_ABS)/host/Qt_ClientControl/macos/mesh_client.app
  endif
endif
endif

# need to add ota image file
ifneq ($(filter WsOtaUpgrade,$(CY_BT_APP_TOOLS)),)
ifeq ($(CY_OS_DIR),Windows)
CY_OPEN_WsOtaUpgrade_TOOL=$(CY_BT_PEER_APPS_OTA_DIR)/$(CY_OS_DIR)/WsOtaUpgrade/Release/x64/WsOtaUpgrade.exe
CY_OPEN_WsOtaUpgrade_TOOL_ARGS=$(CY_CONFIG_DIR)/$(APPNAME)_$(TARGET).ota.bin
endif
endif

# need to sign ota image file ecdsa_genkey, ecdsa_sign, ecdsa_verify
ifneq ($(filter ecdsa_genkey,$(CY_BT_APP_TOOLS)),)
CY_OPEN_ecdsa_genkey_TOOL=$(CY_BT_APP_TOOLS_UTILS_DIR)/ecdsa256/bin/$(CY_OS_DIR)/ecdsa_genkey
endif
ifneq ($(filter ecdsa_sign,$(CY_BT_APP_TOOLS)),)
CY_OPEN_ecdsa_sign_TOOL=$(CY_BT_APP_TOOLS_UTILS_DIR)/ecdsa256/bin/$(CY_OS_DIR)/ecdsa_genkey
CY_OPEN_ecdsa_sign_TOOL_ARGS=$(CY_CONFIG_DIR)/$(APPNAME)_$(TARGET).ota.bin
endif
ifneq ($(filter ecdsa_verify,$(CY_BT_APP_TOOLS)),)
CY_OPEN_ecdsa_verify_TOOL=$(CY_BT_APP_TOOLS_UTILS_DIR)/ecdsa256/bin/$(CY_OS_DIR)/ecdsa_verify
CY_OPEN_ecdsa_verify_TOOL_ARGS=$(CY_CONFIG_DIR)/$(APPNAME)_$(TARGET).ota.bin
endif

# manufacturing test, display in file system
ifneq ($(filter WMBT,$(CY_BT_APP_TOOLS)),)
$(info $(subst /,\,$(CY_BT_APP_TOOLS_UTILS_DIR)/wmbt))
CY_OPEN_WMBT_TOOL=explorer.exe
CY_OPEN_WMBT_TOOL_ARGS=/separate /e $(subst /,\,$(CY_INTERNAL_APPLOC)/tools/btsdk-utils/wmbt)
endif

# hello client peer app
ifneq ($(filter hello_client,$(CY_BT_APP_TOOLS)),)
ifeq ($(CY_OS_DIR),Windows)
CY_OPEN_hello_client_TOOL=$(CY_BT_PEER_APPS_BLE_DIR)/hello_sensor/$(CY_OS_DIR)/HelloClient/Release/x64/HelloClient.exe
endif
endif

#
# IDE specifics for program/debug
#

#
# vscode app launchers
#
define tool_launch_json
{\
 \"label\": \"Tool: $(1)\",\
 \"type\": \"shell\",\
 \"windows\" : {\
 \"command\": \"\$${config:modustoolbox.toolsPath}/modus-shell/bin/bash.exe\",\
 \"args\": [\"--norc\",\"-c\",\
 \"export PATH=/bin:/usr/bin:\$$PATH ; \$${config:modustoolbox.toolsPath}/modus-shell/bin/make.exe open CY_OPEN_TYPE=$(1) CY_CONFIG_JOB_CONTROL=\"],\
 },\
 \"linux\" : {\
 \"command\": \"bash\",\
 \"args\": [\"--norc\",\"-c\",\"make open CY_OPEN_TYPE=$(1) CY_CONFIG_JOB_CONTROL=\"],\
 },\
 \"osx\" : {\
 \"command\": \"bash\",\
 \"args\": [\"--norc\",\"-c\",\"make open CY_OPEN_TYPE=$(1) CY_CONFIG_JOB_CONTROL=\"],\
 },\
 \"problemMatcher\": \"\$$gcc\",\
},
endef
CY_BT_APP_TOOLS_JSON=$(foreach tool,$(CY_BT_APP_TOOLS),$(call tool_launch_json,$(tool)))

CY_GCC_BASE_DIR=$(subst $(CY_INTERNAL_TOOLS)/,,$(CY_INTERNAL_TOOL_gcc_BASE))
CY_GCC_VERSION=$(shell $(CY_INTERNAL_TOOL_arm-none-eabi-gcc_EXE) -dumpversion)
CY_OPENOCD_EXE_DIR=$(patsubst $(CY_INTERNAL_TOOLS)/%,%,$(CY_INTERNAL_TOOL_openocd_EXE))
CY_OPENOCD_SCRIPTS_DIR=$(patsubst $(CY_INTERNAL_TOOLS)/%,%,$(CY_INTERNAL_TOOL_openocd_scripts_SCRIPT))

ifneq ($(CY_BUILD_LOCATION),)
CY_ELF_FILE?=$(CY_INTERNAL_BUILD_LOC)/$(TARGET)/$(CONFIG)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET)
else
CY_ELF_FILE?=./$(notdir $(CY_INTERNAL_BUILD_LOC))/$(TARGET)/$(CONFIG)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET)
endif

CY_C_FLAGS=$(subst $(CY_SPACE),\"$(CY_COMMA)$(CY_NEWLINE_MARKER)\",$(strip $(CY_RECIPE_CFLAGS)))

CY_JLINK_DEVICE_CFG=Cortex-M4
CY_OPENOCD_DEVICE_CFG=$(CY_INTERNAL_BASELIB_PATH)/platforms/CYW$(CY_TARGET_DEVICE)_openocd.cfg
CY_OPENOCD_ARG =-s $(CY_INTERNAL_BASELIB_PATH)/platforms
CY_OPENOCD_ARG+=-f CYW$(CY_TARGET_DEVICE)_openocd.cfg

CY_VSCODE_ARGS="s|&&CY_ELF_FILE&&|$(CY_ELF_FILE)|g;"\
                "s|&&CY_MTB_PATH&&|$(CY_TOOLS_DIR)|g;"\
                "s|&&TARGET&&|$(TARGET)|g;"\
                "s|&&CY_OPENOCD_ADDL_SEARCH&&|$(CY_INTERNAL_BASELIB_PATH)/platforms|g;"\
                "s|&&CY_OPENOCD_CFG_FILE&&|$(CY_OPENOCD_DEVICE_CFG)|g;"\
                "s|&&CY_TOOL_CHAIN_DIRECTORY&&|$(subst ",,$(CY_CROSSPATH))|g;"\
                "s|&&CY_GCC_VERSION&&|$(CY_GCC_VERSION)|g;"\
                "s|&&CY_C_FLAGS&&|$(CY_C_FLAGS)|g;"\
                "s|&&CY_OPENOCD_EXE_DIR&&|$(CY_OPENOCD_EXE_DIR)|g;"\
                "s|&&CY_OPENOCD_SCRIPTS_DIR&&|$(CY_OPENOCD_SCRIPTS_DIR)|g;"\
                "s|&&CY_CDB_FILE&&|$(CY_CDB_FILE)|g;"\
                "s|&&CY_JLINK_DEVICE_CFG&&|$(CY_JLINK_DEVICE_CFG)|g;"\
                "s|&&CY_BT_APP_TOOLS&&|$(CY_BT_APP_TOOLS_JSON)|g;"\

ifeq ($(CY_USE_CUSTOM_GCC),true)
CY_VSCODE_ARGS+="s|&&CY_GCC_BIN_DIR&&|$(CY_INTERNAL_TOOL_gcc_BASE)/bin|g;"\
				"s|&&CY_GCC_DIRECTORY&&|$(CY_INTERNAL_TOOL_gcc_BASE)|g;"
else
CY_VSCODE_ARGS+="s|&&CY_GCC_BIN_DIR&&|$$\{config:modustoolbox.toolsPath\}/$(CY_GCC_BASE_DIR)/bin|g;"\
				"s|&&CY_GCC_DIRECTORY&&|$$\{config:modustoolbox.toolsPath\}/$(CY_GCC_BASE_DIR)|g;"
endif

CY_ECLIPSE_ARGS="s|&&CY_OPENOCD_ARG&&|$(CY_OPENOCD_ARG)|g;"\
                "s|&&CY_JLINK_DEVICE&&|$(CY_JLINK_DEVICE_CFG)|g;"\
                "s|&&CY_APPNAME&&|$(CY_IDE_PRJNAME)|g;"\
                "s|&&CY_PROG_FILE&&|$(CY_APP_ELF_FILE)|g;"\
                "s|&&CY_ECLIPSE_GDB&&|$(CY_ECLIPSE_GDB)|g;"

DEVICE_GEN?=$(DEVICE)

# Command for updating the device(s) (Note: this doesn't get expanded and used until "bsp" target)
CY_BSP_DEVICES_CMD=\
	designFile=$$($(CY_FIND) $(CY_TARGET_GEN_DIR) -name *.modus);\
	if [[ $$designFile ]]; then\
		echo "Running device-configurator for $(DEVICE_GEN)...";\
		$(CY_CONFIG_MODUS_EXEC)\
		$(CY_CONFIG_LIBFILE)\
		--build $$designFile\
		--set-device=$(subst $(CY_SPACE),$(CY_COMMA),$(DEVICE_GEN));\
		cfgStatus=$$(echo $$?);\
		if [ $$cfgStatus != 0 ]; then echo "ERROR: Device-configuration failed for $$designFile"; exit $$cfgStatus; fi;\
	else\
		echo "Could not detect .modus file. Skipping update...";\
	fi;
