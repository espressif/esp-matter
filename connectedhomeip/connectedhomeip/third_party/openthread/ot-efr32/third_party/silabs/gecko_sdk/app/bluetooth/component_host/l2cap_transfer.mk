################################################################################
# L2CAP Transfer component                                                     #
################################################################################

# Add to include path
override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/l2cap_transfer/inc \
$(SDK_DIR)/app/bluetooth/common_host/em_core_mock

# Attach sources
override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/l2cap_transfer/src/sl_bt_l2cap_transfer.c
