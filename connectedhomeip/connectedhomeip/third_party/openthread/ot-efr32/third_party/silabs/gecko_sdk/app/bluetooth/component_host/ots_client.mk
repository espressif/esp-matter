################################################################################
# OTS - Object Client component                                                #
################################################################################

# Add to include path
override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/ots/inc \
$(SDK_DIR)/app/bluetooth/common_host/ots/config


# Attach sources
override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/ots/src/sl_bt_ots_client.c \
$(SDK_DIR)/app/bluetooth/common/ots/src/sl_bt_ots_common.c