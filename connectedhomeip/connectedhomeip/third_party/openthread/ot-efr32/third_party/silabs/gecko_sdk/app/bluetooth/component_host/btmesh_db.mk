################################################################################
# BT Mesh Database Component                                                   #
################################################################################

override INCLUDEPATHS += \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_db

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/btmesh_db/btmesh_db.c