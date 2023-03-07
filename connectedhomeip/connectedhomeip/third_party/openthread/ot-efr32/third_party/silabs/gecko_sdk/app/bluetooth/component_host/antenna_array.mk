################################################################################
# Antenna Array component                                                      #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/antenna_array

override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/antenna_array/antenna_array.c
