################################################################################
# Signal handler component                                                     #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/app_signal

override C_SRC += $(foreach OS_i, $(OS), $(SDK_DIR)/app/bluetooth/common_host/app_signal/app_signal_$(OS_i).c)
