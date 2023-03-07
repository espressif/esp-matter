################################################################################
# BT Mesh Configurator Component                                               #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf/config

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf/btmesh_conf.c \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf/btmesh_conf_distributor.c \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf/btmesh_conf_executor.c \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf/btmesh_conf_job.c \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_conf/btmesh_conf_task.c

override CFLAGS += -DSL_CATALOG_BTMESH_CONF_PRESENT