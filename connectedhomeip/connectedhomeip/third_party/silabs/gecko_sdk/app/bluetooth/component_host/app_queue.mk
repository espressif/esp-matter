################################################################################
# OTS - Object Client component                                                #
################################################################################

# Add to include path
override INCLUDEPATHS += \
$(SDK_DIR)/app/common/util/app_queue


# Attach sources
override C_SRC += \
$(SDK_DIR)/app/common/util/app_queue/app_queue.c