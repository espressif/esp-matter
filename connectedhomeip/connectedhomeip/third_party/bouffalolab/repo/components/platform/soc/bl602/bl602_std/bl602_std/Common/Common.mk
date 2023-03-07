#clear_vars
LOCAL_SRCS_FILE:=

MODULE_DIR:= BSP/Common
MODULE_OUT_DIR:= $(TARGET_OUT_PATH)/BSP/Common

COMMON_CFLAGS:= 
COMMON_INCLUDE:= 
common_objs_target :=

COM_EXCLUDE_DIR:= BSP/Common/mbedtls 
                  
COM_ALL_DIRS := $(shell find $(MODULE_DIR) -maxdepth 1 -mindepth 1 -type d)
COM_DIRS=$(filter-out $(COM_EXCLUDE_DIR), $(COM_ALL_DIRS)) 
COM_MKS=$(foreach com_dir,$(COM_DIRS),$(wildcard $(com_dir)/*.mk))

#$(info $(COM_MKS))
include $(COM_MKS)

base_objects += $(common_objs_target)

.PHONY: common
common: $(common_objs_target)
