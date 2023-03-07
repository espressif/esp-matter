# Copyright Statement:
#
# (C) 2005-2016  MediaTek Inc. All rights reserved.
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
# Without the prior written permission of MediaTek and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
# if you have agreed to and been bound by the applicable license agreement with
# MediaTek ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of MediaTek Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#

MINISUPP_BASE = $(shell pwd)/..
SOURCE_DIR    = $(MINISUPP_BASE)/../../..

BINPATH = $(SOURCE_DIR)/tools/gcc/linux/gcc-arm-none-eabi/bin
STRIP = $(SOURCE_DIR)/tools/gcc/linux/gcc-arm-none-eabi/bin/arm-none-eabi-strip
PROJ_PATH    ?= $(SOURCE_DIR)/project/mt7933_hdk/apps/bga_evb/GCC
CONFIG_PATH  ?= .

CFLAGS += -I$(PROJ_PATH)/../inc
CFLAGS += -I$(SOURCE_DIR)/$(CONFIG_PATH)
CFLAGS += -mno-unaligned-access
BUILD_BACH  = 1

$(info minisupp ref chip $(IC_CONFIG))
$(info minisupp ref proj $(PROJ_PATH))
DIR      = $(MINISUPP_BASE)/src_protected/wpa_supplicant_2.10
CFLAGS += -I$(DIR)/src           # -DANDROID_P2P  <== Disable P2P for testing ... Michael
CFLAGS += -I$(DIR)/src/utils     # -DANDROID_P2P  <== Disable P2P for testing ... Michael
CFLAGS += -I$(DIR)/src/drivers   # -DANDROID_P2P  <== Disable P2P for testing ... Michael

# Determine use feature.mk , feature_wps.mk or other mk file input in compile command
ifeq ($(FEATURE),)
FEATURE  = feature.mk
endif
include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

ifeq ($(BUILD_WARNING_AS_ERROR),y)
CFLAGS += -Werror=all
endif

# Sources
include module.mk

# Project name
ifeq ($(MTK_WIFI_AP_ENABLE),n)
WPSNAME=_sta
endif

# Project name
ifneq ($(MTK_RELEASE_MODE),)
ifeq ($(MTK_RELEASE_MODE),release)
WLANMODE=_mshrink
else
WLANMODE=_mshrink_dbg
endif
else
WLANMODE=
endif

ifeq ($(MTK_DEBUG_LEVEL),none)
CFLAGS += -DCONFIG_NO_STDOUT_DEBUG
endif

TARGET_LIB=libminisupp${WPSNAME}${LTO_LIB_POST_NAME}${WLANMODE}
BUILD_DIR = $(PWD)/Build
OUTPATH = $(PWD)/Build


export CFLAGS
export INCLUDE
export LDFLAGS
export BUILD_BACH
export DIR


C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)

.PHONY: gen_lib

all: gen_lib
	@echo Build $< Done

include $(SOURCE_DIR)/.rule.mk

gen_lib: $(C_OBJS) $(S_OBJS) wpa_supplicant
	@echo Gen $(TARGET_LIB).a
	@echo Gen $(TARGET_LIB).a >>$(BUILD_LOG)
	$(AR) -r $(BUILD_DIR)/$(TARGET_LIB).a  $(C_OBJS) $(S_OBJS) >>$(BUILD_LOG) 2>/dev/null
	@if [ "$$BUILD_DIR/$(TARGET_LIB).a" != "$$OUTPATH/$(TARGET_LIB).a" ]; then \
		mv $(BUILD_DIR)/$(TARGET_LIB).a $(OUTPATH)/$(TARGET_LIB).a; \
	fi;

wpa_supplicant:
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(DIR)/wpa_supplicant NAME=minisupp$(WPSNAME)${LTO_LIB_POST_NAME}${WLANMODE} PROJ_PATH=$(PROJ_PATH) BUILD_DIR=$(BUILD_DIR)

hostapd:
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(DIR)/hostapd NAME=minihostapd PROJ_PATH=$(PROJ_PATH) BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@echo $(CC) $(CFLAGS) $@ >> $(BUILD_LOG)
	@-$(CC) $(CFLAGS) -c $< -o $@ 2>>$(ERR_LOG)

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
endif

clean:
	rm -rf $(OUTPATH)

prebuilt_install:
	mkdir -p $(SOURCE_DIR)/prebuilt/middleware/MTK/minisupp/lib
	mkdir -p $(SOURCE_DIR)/prebuilt/middleware/MTK/minisupp/inc
	
	$(STRIP) -d $(BUILD_DIR)/$(TARGET_LIB).a
	cp $(BUILD_DIR)/$(TARGET_LIB).a $(SOURCE_DIR)/prebuilt/middleware/MTK/minisupp/lib
	cd ${DIR};\
	find . -name "*.h" -exec bash -c 'cp -p --parents {} $(SOURCE_DIR)/prebuilt/middleware/MTK/minisupp/inc/ ' \;
