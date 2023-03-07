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

$(NAME)_PATH = middleware/third_party/httpd
C_FILES  += $($(NAME)_PATH)/src/os_port.c	\
		$($(NAME)_PATH)/src/axhttpd.c	\
		$($(NAME)_PATH)/src/proc.c	\
		$($(NAME)_PATH)/src/tdate_parse.c \
		$($(NAME)_PATH)/src/base64.c \
		$($(NAME)_PATH)/src/httpd_api.c \
		$($(NAME)_PATH)/src/auth_check.c \
 		$($(NAME)_PATH)/src/cgi_api.c \
 		$($(NAME)_PATH)/src/web_proc.c \
 		$($(NAME)_PATH)/src/cgi.c \
 		$($(NAME)_PATH)/web_c/webpage.c \
 		$($(NAME)_PATH)/web_c/web__AP_html.c \
 		$($(NAME)_PATH)/web_c/web__index_html.c \
 		$($(NAME)_PATH)/web_c/web__Test1_html.c \
 		$($(NAME)_PATH)/web_c/web__Test_html.c 

LwIP_VER ?= lwip-master
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/httpd/inc
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/httpd/Generate_file
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS      += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS  	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  	+= -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  	+= -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  	+= -I$(SOURCE_DIR)/driver/chip/inc

HTML_SRCS  	= $(wildcard web_html/*.html)
HTML_CS 	= $(patsubst web_html/%.html, middleware/third_party/httpd/web_c/web__%_html.c, $(HTML_SRCS))

HTM_SRCS  	= $(wildcard web_html/*.htm)
HTM_CS		= $(patsubst web_html/%.htm, middleware/third_party/httpd/web_c/web__%_htm.c, $(HTM_SRCS))

CSS_SRCS  	= $(wildcard web_html/*.css)
CSS_CS 		= $(patsubst web_html/%.css, middleware/third_party/httpd/web_c/web__%_css.c, $(CSS_SRCS))

JS_SRCS  	= $(wildcard web_html/*.js)
JS_CS 		= $(patsubst web_html/%.js, middleware/third_party/httpd/web_c/web__%_js.c, $(JS_SRCS))

JPEG_SRCS  	= $(wildcard web_html/*.jpeg)
JPEG_CS		= $(patsubst web_html/%.jpeg, middleware/third_party/httpd/web_c/web__%_jpeg.c, $(JPEG_SRCS))

JPG_SRCS  	= $(wildcard web_html/*.jpg)
JPG_CS 		= $(patsubst web_html/%.jpg, middleware/third_party/httpd/web_c/web__%_jpg.c, $(JPG_SRCS))

GIF_SRCS  	= $(wildcard web_html/*.gif)
GIF_CS 		= $(patsubst web_html/%.gif, middleware/third_party/httpd/web_c/web__%_gif.c, $(GIF_SRCS))

WEBPAGE_C	= middleware/third_party/httpd/web_c/webpage.c

###################################################
MID_HTTPD_PATH = $(SOURCE_DIR)/middleware/third_party/httpd
###################################################

###################################################
#MODULE_PATH += $(MID_HTTPD_PATH)
###################################################
