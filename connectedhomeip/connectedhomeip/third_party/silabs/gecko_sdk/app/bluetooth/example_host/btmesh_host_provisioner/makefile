################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = btmesh_host_provisioner
SDK_DIR = ../../../..

# Disable threading as handling more than a few mesh devices can cause data loss
HOST_THREADING = 0

# Still add pthread because user input handling uses it
override LDFLAGS += -pthread

################################################################################
# Components                                                                   #
#                                                                              #
# After setting up the toolchain, components can contribute to the project by  #
# appending items to the project variables like INCLUDEPATHS or C_SRC.         #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/toolchain.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_log.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_assert.mk
include $(SDK_DIR)/app/bluetooth/component_host/app_signal.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_btmesh.mk
include $(SDK_DIR)/app/bluetooth/component_host/btmesh_db.mk
include $(SDK_DIR)/app/bluetooth/component_host/btmesh_prov.mk
include $(SDK_DIR)/app/bluetooth/component_host/btmesh_conf.mk
include $(SDK_DIR)/app/bluetooth/component_host/slist.mk
include $(SDK_DIR)/app/bluetooth/component_host/simple_timer.mk


################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += . \
$(SDK_DIR)/app/bluetooth/common/ncp_evt_filter \
$(SDK_DIR)/app/bluetooth/common_host/app_sleep \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc


################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/app_sleep/app_sleep.c \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
$(SDK_DIR)/app/bluetooth/common_host/system/sl_btmesh.c \
app.c \
app_conf.c \
app_ui.c \
main.c


################################################################################
# Target rules                                                                 #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk
