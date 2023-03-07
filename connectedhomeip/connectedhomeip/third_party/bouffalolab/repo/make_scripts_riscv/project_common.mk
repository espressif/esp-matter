#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

ifneq (,$(filter "$(CONFIG_CHIP_NAME)", "BL606P" "BL616"))

ifeq ("$(CPU_ID)", "M0")
CFLAGS += -DBFLB_USE_HAL_DRIVER -DCPU_M0 -DARCH_RISCV -DCFG_CHIP_BL606P
else ifeq ("$(CPU_ID)", "D0")
CFLAGS += -DBFLB_USE_HAL_DRIVER -DCPU_D0 -DARCH_RISCV
endif
include $(BL60X_SDK_PATH)/make_scripts_thead_riscv/project.mk

endif

ifeq ("$(CONFIG_CHIP_NAME)", "BL808")
ifeq ("$(CPU_ID)" ,$(filter "$(CPU_ID)", "M1" "M0"))
CFLAGS += -DBFLB_USE_HAL_DRIVER -DCPU_M1 -DCPU_M0 -DARCH_RISCV -DCFG_CHIP_BL808
else ifeq ("$(CPU_ID)", "D0")
CFLAGS += -DBFLB_USE_HAL_DRIVER -DCPU_D0 -DARCH_RISCV
endif

include $(BL60X_SDK_PATH)/make_scripts_thead_riscv/project.mk

endif
