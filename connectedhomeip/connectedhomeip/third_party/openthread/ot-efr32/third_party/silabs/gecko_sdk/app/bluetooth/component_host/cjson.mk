################################################################################
# cJSON component                                                              #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/util/third_party/cjson

override C_SRC += $(SDK_DIR)/util/third_party/cjson/cJSON.c

# 3rd party code, cannot be fixed.
override CFLAGS += -Wno-misleading-indentation
