################################################################################
# AoA Constant Tone Extension (CTE) handler component                          #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/aoa_cte \
$(SDK_DIR)/app/bluetooth/common_host/aoa_cte/config \
$(SDK_DIR)/app/bluetooth/common/ncp_evt_filter

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/aoa_cte/aoa_cte.c \
$(SDK_DIR)/app/bluetooth/common_host/aoa_cte/cte_conn_less.c \
$(SDK_DIR)/app/bluetooth/common_host/aoa_cte/cte_conn.c \
$(SDK_DIR)/app/bluetooth/common_host/aoa_cte/cte_silabs.c