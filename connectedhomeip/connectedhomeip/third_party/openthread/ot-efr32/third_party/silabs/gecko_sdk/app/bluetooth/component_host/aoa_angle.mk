################################################################################
# AoA angle calculation component                                              #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/aoa_angle \
$(SDK_DIR)/app/bluetooth/common_host/aoa_angle/config

override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/aoa_angle/aoa_angle.c

override CFLAGS += -DAOA_ANGLE
