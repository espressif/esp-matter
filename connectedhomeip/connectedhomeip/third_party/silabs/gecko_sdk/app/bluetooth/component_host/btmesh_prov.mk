################################################################################
# BT Mesh Provisioner Component                                                   #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/btmesh_db.mk
include $(SDK_DIR)/app/bluetooth/component_host/slist.mk

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_prov

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_prov/btmesh_prov.c