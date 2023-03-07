################################################################################
# Throughput central component                                                 #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common/throughput_central \
$(SDK_DIR)/app/bluetooth/common/throughput_central/config \
$(SDK_DIR)/app/bluetooth/common/throughput_central/platform \
$(SDK_DIR)/app/bluetooth/common/throughput \
$(SDK_DIR)/app/bluetooth/common/throughput_ui

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common/throughput_central/throughput_central.c

override CFLAGS += -DSL_CATALOG_THROUGHPUT_CENTRAL_PRESENT
