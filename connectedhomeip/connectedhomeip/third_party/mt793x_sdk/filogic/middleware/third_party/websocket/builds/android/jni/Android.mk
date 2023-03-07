LOCAL_PATH:= $(call my-dir)


ALL_SOURCES := \
	../../../src/rws_common.c \
	../../../src/rws_error.c \
	../../../src/rws_frame.c \
	../../../src/librws.c \
	../../../src/rws_list.c \
	../../../src/rws_memory.c \
	../../../src/rws_socketpriv.c \
	../../../src/rws_socketpub.c \
	../../../src/rws_string.c \
	../../../src/rws_thread.c


ALL_INCLUDES := $(LOCAL_PATH)/../../../

ALL_CFLAGS := -w

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(ALL_SOURCES)
LOCAL_C_INCLUDES += $(ALL_INCLUDES)
LOCAL_CFLAGS += $(ALL_CFLAGS)
LOCAL_MODULE := librws
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)

