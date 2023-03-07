# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS +=

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

ifeq ("$(HAL_CHIP_USE)", "BL616")
	override CONFIG_CHIP_NAME := BL616
endif

## This component's src
ifneq (,$(filter "$(CONFIG_CHIP_NAME)", "BL606P" "BL616"))
COMPONENT_SRCS := src/utils_hex.c \
                  src/utils_crc.c \
                  src/utils_fec.c \
                  src/utils_log.c \
                  src/utils_dns.c \
                  src/utils_list.c \
                  src/utils_ringblk.c \
                  src/utils_rbtree.c \
                  src/utils_hexdump.c \
                  src/utils_time.c \
                  src/utils_notifier.c \
                  src/utils_getopt.c \
                  src/utils_string.c \
                  src/utils_memp.c \
                  src/utils_tlv_bl.c \
                  src/utils_base64.c \
                  src/utils_bitmap_window.c \
                  src/utils_sha256.c \
                  src/test/test_utils_base64.c \
                  src/test/test_utils_ringblk.c \
                  src/test/test_utils_bitmap_window.c \
                  #src/utils_hmac_sha1_fast.c \
                  #src/utils_psk_fast.c \

else
ifeq ("$(CONFIG_CHIP_NAME)", "BL808")
COMPONENT_SRCS := src/utils_hex.c \
                  src/utils_fec.c \
                  src/utils_log.c \
                  src/utils_crc.c \
                  src/utils_dns.c \
                  src/utils_list.c \
                  src/utils_ringblk.c \
                  src/utils_rbtree.c \
                  src/utils_hexdump.c \
                  src/utils_time.c \
                  src/utils_notifier.c \
                  src/utils_getopt.c \
                  src/utils_string.c \
                  src/utils_memp.c \
                  src/utils_tlv_bl.c \
                  src/utils_sha256.c \
                  src/utils_base64.c \

ifeq ("$(CPU_ID)", "M0")
COMPONENT_SRCS += src/utils_bitmap_window.c \
                  src/test/test_utils_ringblk.c \
                  src/test/test_utils_bitmap_window.c \

endif
else
COMPONENT_SRCS := src/utils_hex.c \
                  src/utils_crc.c \
                  src/utils_sha256.c \
                  src/utils_fec.c \
                  src/utils_log.c \
                  src/utils_dns.c \
                  src/utils_list.c \
                  src/utils_ringblk.c \
                  src/utils_rbtree.c \
                  src/utils_hexdump.c \
                  src/utils_time.c \
                  src/utils_notifier.c \
                  src/utils_getopt.c \
                  src/utils_string.c \
                  src/utils_hmac_sha1_fast.c \
                  src/utils_psk_fast.c \
                  src/utils_memp.c \
                  src/utils_tlv_bl.c \
                  src/utils_base64.c \
                  src/utils_bitmap_window.c \
                  src/test/test_utils_base64.c \
                  src/test/test_utils_ringblk.c \
                  src/test/test_utils_bitmap_window.c \

endif
endif


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src src/test


##
#CPPFLAGS +=
ifeq ($(CONFIG_UTIL_BASE64_ENABLE),1)
CPPFLAGS += -DUTILS_BASE64
endif

ifneq ($(CONFIG_CLI_CMD_ENABLE),0)
CPPFLAGS += -DCONFIG_CLI_CMD_ENABLE
endif

CPPFLAGS += -D$(CONFIG_CHIP_NAME)
