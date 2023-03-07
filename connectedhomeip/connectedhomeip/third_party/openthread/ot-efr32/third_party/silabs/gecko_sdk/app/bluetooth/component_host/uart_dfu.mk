################################################################################
# UART Device Firmware Update component                                        #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/uart_dfu

override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/uart_dfu/uart_dfu.c
