################################################################################
# Singly-Linked List Platform host component                                   #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/em_assert_mock \
$(SDK_DIR)/platform/common/inc \

override C_SRC += \
$(SDK_DIR)/platform/common/src/sl_slist.c