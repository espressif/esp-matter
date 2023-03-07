################################################################################
# NCP host component without threading                                         #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/ncp_host_nothread \
$(SDK_DIR)/app/bluetooth/common_host/tcp \
$(SDK_DIR)/app/bluetooth/common_host/uart \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/ncp_host_nothread/ncp_host_nothread.c \
$(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/tcp/tcp_$(OS_i).c) \
$(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/uart/uart_$(OS_i).c) \
$(SDK_DIR)/protocol/bluetooth/src/sl_bt_ncp_host.c \
$(SDK_DIR)/protocol/bluetooth/src/sl_bt_ncp_host_api.c

ifeq ($(OS), win)
  # Ws2_32: WinSock library
  override LDFLAGS += -lWs2_32
endif
