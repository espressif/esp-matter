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
.NOTPARALLEL:

SOURCE_DIR = ../../..
BINPATH = ~/gcc-arm-none-eabi/bin
PROJ_PATH = ../../../project/mt7687_hdk/apps/iot_sdk/GCC

FEATURE    ?= feature.mk

include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

# suppress all building warning in this module
CFLAGS += -w

CFLAGS += -I$(PROJ_PATH)/../inc
# Project name
TARGET_LIB=libhttpd

BUILD_DIR = Build
OUTPATH = Build

ifeq ($(TGN_VERIFI_ENABLE), 1)
CFLAGS         += -DTGN_VERIFI_ENABLE=1
endif

# Sources
include module.mk

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
HTML_OBJS = $(HTML_CS:%.c=$(BUILD_DIR)/%.o)
HTM_OBJS = $(HTM_CS:%.c=$(BUILD_DIR)/%.o)
CSS_OBJS = $(CSS_CS:%.c=$(BUILD_DIR)/%.o)
JS_OBJS = $(JS_CS:%.c=$(BUILD_DIR)/%.o)
JPEG_OBJS = $(JPEG_CS:%.c=$(BUILD_DIR)/%.o)
JPG_OBJS = $(JPG_CS:%.c=$(BUILD_DIR)/%.o)
GIF_OBJS = $(GIF_CS:%.c=$(BUILD_DIR)/%.o)
WEBPAGE_OBJS = $(WEBPAGE_C:%.c=$(BUILD_DIR)/%.o)

.PHONY: $(TARGET_LIB).a

all: web_c/web_c.o $(TARGET_LIB).a
	-rm -f ./web_c/.objfiles
	-rm -f ./web_c/*.bin
	-rm -f ./web_c/*.dbg
	-rm -f ./web_c/web_list
	-rm -f ./web_c/webcomp_sin
	@echo Build $< Done

$(TARGET_LIB).a: web_c/web_c.o $(HTML_OBJS) $(HTM_OBJS) $(JPEG_OBJS) $(JPG_OBJS) $(GIF_OBJS) $(CSS_OBJS) $(JS_OBJS) $(WEBPAGE_OBJS) $(C_OBJS)
	@echo Gen $(TARGET_LIB).a
	@echo Gen $(TARGET_LIB).a >>$(BUILD_LOG)
	@echo $(AR) -r $(OUTPATH)/$@ $^
	#@$(AR) -r $(OUTPATH)/$@ $^ >>$(BUILD_LOG)
	@$(AR) -r $(OUTPATH)/$@ $(HTML_OBJS) $(HTM_OBJS) $(JPEG_OBJS) $(JPG_OBJS) $(GIF_OBJS) $(CSS_OBJS) $(JS_OBJS) $(WEBPAGE_OBJS) $(C_OBJS) >>$(BUILD_LOG)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@echo $(CC) $(CFLAGS) $@ >> $(BUILD_LOG)
	@$(CC) $(CFLAGS) -c $< -o $@ 2>>$(ERR_LOG)

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -rf $(OUTPATH)/$(TARGET_LIB).a
	rm -rf $(BUILD_DIR)

#################################################################################
DOS2UNIX = dos2unix
WEB_DIR	= web_html

web_c/web_c.o: webcomp_sin $(WEB_DIR)/*
	echo "make web pages library"
	rm -rf $(wildcard $(WEB_DIR)/*.bak)
	$(DOS2UNIX) $(WEB_DIR)/*.* ;
ifeq ($(CROSS_COMPILE),win)
	find $(WEB_DIR) -name "*.*" | sort > web_list
else
	find $(WEB_DIR) -name "*.*" > web_list
endif
	if [ ! -e web_c ] ; then mkdir web_c ; else rm -f web_c/* ; fi
	sh ./webcomp.sh $(WEB_DIR)
	-mv -f webpage.h ./inc/webpage.h
	-mv -f webdata.h ./inc/webdata.h
	-mv -f webpage.c ./web_c/webpage.c
	-mv -f webcomp_sin ./web_c/webcomp_sin
	-mv -f web_list ./web_c/web_list
	-mv -f webdata.bin ./web_c/webdata.bin
ifeq ($(CROSS_COMPILE),win)
	dos2unix ./web_c/webpage.c ;
endif

webcomp_sin: webcomp_sin.c
	@echo "clean webpages.."
	-rm -rf web_c
	-rm -f ./inc/webpage.h
	-rm -f ./inc/webdata.h
	find . \( -name '*.[oasd]' -o -name '.*.flags' \) -type f -print | xargs rm -f
	gcc -o webcomp_sin webcomp_sin.c;

#################################################################################

