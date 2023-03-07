################################################################################
# NCP reset component                                                          #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/ncp_reset

override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/ncp_reset/ncp_reset.c

override CFLAGS += -DSL_CATALOG_NCP_RESET_PRESENT
