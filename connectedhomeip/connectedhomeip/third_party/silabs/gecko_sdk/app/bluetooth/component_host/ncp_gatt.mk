################################################################################
# NCP GATT component                                                           #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common/ncp_gatt

override C_SRC += $(SDK_DIR)/app/bluetooth/common/ncp_gatt/sl_ncp_gatt.c

override CFLAGS += -DSL_CATALOG_NCP_GATT_PRESENT
