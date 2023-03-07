#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
ifeq ($(CONFIG_ENABLE_PSM_RAM),1)
CPPFLAGS += -DCONF_USER_ENABLE_PSRAM
endif

ifeq ($(CONFIG_ENABLE_CAMERA),1)
CPPFLAGS += -DCONF_USER_ENABLE_CAMERA
endif

ifeq ($(CONFIG_ENABLE_BLSYNC),1)
CPPFLAGS += -DCONF_USER_ENABLE_BLSYNC
endif

ifeq ($(CONFIG_ENABLE_VFS_SPI),1)
CPPFLAGS += -DCONF_USER_ENABLE_VFS_SPI
endif

ifeq ($(CONFIG_ENABLE_VFS_ROMFS),1)
CPPFLAGS += -DCONF_USER_ENABLE_VFS_ROMFS
endif

CFLAGS   += -DBFLB_BLE
ifeq ($(CONFIG_BT_MESH),1)
CFLAGS   += -DCONFIG_BT_MESH
endif

ifeq ($(CONFIG_BLE_TP_SERVER),1)
CFLAGS += -DCONFIG_BLE_TP_SERVER
endif

ifeq ($(CONFIG_BT_STACK_PTS),1)
CFLAGS += -DCONFIG_BT_STACK_PTS
endif

ifeq ($(CONFIG_BT_MESH_MODEL_GEN_SRV),1)
CFLAGS += -DCONFIG_BT_MESH_MODEL_GEN_SRV
endif