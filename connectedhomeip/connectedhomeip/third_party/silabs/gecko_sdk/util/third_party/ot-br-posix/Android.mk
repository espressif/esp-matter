#
#  Copyright (c) 2018, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

LOCAL_PATH := $(call my-dir)

ifeq ($(OTBR_ENABLE_ANDROID_MK),1)

ifneq ($(OTBR_PROJECT_ANDROID_MK),)
include $(OTBR_PROJECT_ANDROID_MK)
endif

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE := libotbr-dbus-client
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += -Wall -Wextra -Wno-unused-parameter

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/src \
    external/openthread/include \

LOCAL_SRC_FILES = \
    src/dbus/client/client_error.cpp \
    src/dbus/client/thread_api_dbus.cpp \
    src/dbus/common/dbus_message_helper.cpp \
    src/dbus/common/dbus_message_helper_openthread.cpp \
    src/dbus/common/error.cpp \

LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/src \
    $(LOCAL_PATH)/include \

LOCAL_SHARED_LIBRARIES += libdbus

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE := otbr-agent
LOCAL_MODULE_TAGS := eng
LOCAL_SHARED_LIBRARIES := libdbus

OTBR_GEN_HEADER_DIR := $(local-intermediates-dir)/gen
OTBR_GEN_DBUS_INTROSPECT_HEADER := $(OTBR_GEN_HEADER_DIR)/dbus/server/introspect.hpp

$(OTBR_GEN_DBUS_INTROSPECT_HEADER): $(LOCAL_PATH)/src/dbus/server/introspect.xml
	mkdir -p $(OTBR_GEN_HEADER_DIR)/dbus/server
	echo 'R"INTROSPECT(' > $@
	cat $+ >> $@
	echo ')INTROSPECT"' >> $@

$(LOCAL_PATH)/src/dbus/server/dbus_thread_object.cpp: $(OTBR_GEN_HEADER_DIR)/dbus/server/introspect.hpp

ifneq ($(ANDROID_NDK),1)
LOCAL_SHARED_LIBRARIES += libcutils
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/src \
    external/libchrome \
    external/gtest/include \
    external/openthread/include \
    external/openthread/src \
    external/openthread/src/posix/platform/include \
    $(OTBR_GEN_HEADER_DIR) \
    $(OTBR_PROJECT_INCLUDES)

LOCAL_CFLAGS += -Wall -Wextra -Wno-unused-parameter
LOCAL_CFLAGS += \
    -DOTBR_PACKAGE_VERSION=\"0.2.0\" \
    -DOTBR_ENABLE_DBUS_SERVER=1 \
    -DOTBR_ENABLE_BORDER_AGENT=1 \
    -DOTBR_DBUS_INTROSPECT_FILE=\"\" \
    $(OTBR_PROJECT_CFLAGS) \

LOCAL_CPPFLAGS += -std=c++14

LOCAL_GENERATED_SOURCES = $(OTBR_GEN_DBUS_INTROSPECT_HEADER)

LOCAL_SRC_FILES := \
    src/agent/application.cpp \
    src/agent/main.cpp \
    src/backbone_router/backbone_agent.cpp \
    src/backbone_router/dua_routing_manager.cpp \
    src/backbone_router/nd_proxy.cpp \
    src/border_agent/border_agent.cpp \
    src/common/dns_utils.cpp \
    src/common/logging.cpp \
    src/common/mainloop.cpp \
    src/common/mainloop_manager.cpp \
    src/common/task_runner.cpp \
    src/common/types.cpp \
    src/dbus/common/dbus_message_dump.cpp \
    src/dbus/common/dbus_message_helper.cpp \
    src/dbus/common/dbus_message_helper_openthread.cpp \
    src/dbus/common/error.cpp \
    src/dbus/server/dbus_agent.cpp \
    src/dbus/server/dbus_object.cpp \
    src/dbus/server/dbus_thread_object.cpp \
    src/dbus/server/error_helper.cpp \
    src/mdns/mdns.cpp \
    src/ncp/ncp_openthread.cpp \
    src/sdp_proxy/advertising_proxy.cpp \
    src/sdp_proxy/discovery_proxy.cpp \
    src/utils/dns_utils.cpp \
    src/utils/hex.cpp \
    src/utils/string_utils.cpp \
    src/utils/thread_helper.cpp \

LOCAL_STATIC_LIBRARIES += \
    ot-core \
    libopenthread-cli \
    ot-core \

LOCAL_LDLIBS := \
    -lutil

ifeq ($(OTBR_MDNS),mDNSResponder)
LOCAL_CFLAGS += \
    -DOTBR_ENABLE_MDNS_MDNSSD=1 \

LOCAL_SRC_FILES += \
    src/mdns/mdns_mdnssd.cpp \

LOCAL_SHARED_LIBRARIES += libmdnssd
endif

LOCAL_SRC_FILES += $(OTBR_PROJECT_SRC_FILES)
LOCAL_STATIC_LIBRARIES += $(OTBR_PROJECT_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(OTBR_PROJECT_SHARED_LIBRARIES)

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE := otbr-agent.conf
LOCAL_MODULE_TAGS := eng

OTBR_AGENT_USER ?= root
OTBR_AGENT_GROUP ?= root

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/dbus-1/system.d
OTBR_GEN_DBUS_CONF_DIR := $(local-intermediates-dir)/gen
$(OTBR_GEN_DBUS_CONF_DIR)/otbr-agent.conf: $(LOCAL_PATH)/src/agent/otbr-agent.conf.in
	mkdir -p $(OTBR_GEN_DBUS_CONF_DIR)
	sed -e 's/@OTBR_AGENT_USER@/$(OTBR_AGENT_USER)/g' -e 's/@OTBR_AGENT_GROUP@/$(OTBR_AGENT_GROUP)/g' $< > $@

# Dirty hack for Android.mk to copy config files from the intermediate directory.
LOCAL_PATH := $(local-intermediates-dir)
LOCAL_SRC_FILES := gen/otbr-agent.conf

include $(BUILD_PREBUILT)
endif # ifeq ($(OTBR_ENABLE_ANDROID_MK),1)
