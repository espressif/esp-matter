################################################################################
# NCP host component with threading for Bluetooth Mesh                         #
################################################################################

# Threading is enabled by default.
# In case of an example handling a lot of data over UART, threaded usage
# might incur data loss or corruption. On the other hand, threaded UART
# handling use less CPU and is adequate for less data rate.
HOST_THREADING ?= 1

ifeq (1, ${HOST_THREADING})
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_bt.mk
else
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_nothread.mk
endif

override C_SRC += \
$(SDK_DIR)/protocol/bluetooth/src/sl_btmesh_ncp_host.c \
$(SDK_DIR)/protocol/bluetooth/src/sl_btmesh_ncp_host_api.c

# Let common code know to build BTMESH specific sources.
override CFLAGS += -DBTMESH
