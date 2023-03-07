################################################################################
# Application log host component                                               #
################################################################################

# Set new line character for Windows host
ifeq ($(OS),win)
override CFLAGS += -DAPP_LOG_NEW_LINE=APP_LOG_NEW_LINE_RN
endif

override CFLAGS += \
-DSL_CATALOG_APP_LOG_PRESENT \
-Wno-format-zero-length 

override INCLUDEPATHS += \
$(SDK_DIR)/app/common/util/app_log \
$(SDK_DIR)/app/bluetooth/common_host/iostream_mock \
$(SDK_DIR)/app/bluetooth/common_host/app_log \
$(SDK_DIR)/app/bluetooth/common_host/app_log/config

override C_SRC += \
$(SDK_DIR)/app/common/util/app_log/app_log.c \
$(SDK_DIR)/app/bluetooth/common_host/app_log/app_log_cli.c \
$(SDK_DIR)/app/bluetooth/common_host/iostream_mock/sl_iostream_handles.c
