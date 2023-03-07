################################################################################
# General project settings                                                     #
################################################################################

PROJECTNAME = bt_host_empty
SDK_DIR = ../../../..
BGBUILD = $(SDK_DIR)/protocol/bluetooth/bin/gatt/bgbuild.py


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
include $(SDK_DIR)/app/bluetooth/component_host/ncp_host_bt.mk
include $(SDK_DIR)/app/bluetooth/component_host/ncp_gatt.mk


################################################################################
# Include paths                                                                #
################################################################################

override INCLUDEPATHS += . \
$(SDK_DIR)/app/bluetooth/common_host/system \
$(SDK_DIR)/platform/common/inc \
$(SDK_DIR)/protocol/bluetooth/inc \
autogen


################################################################################
# Input files                                                                  #
################################################################################

override C_SRC += \
$(SDK_DIR)/app/bluetooth/common_host/system/system.c \
autogen/gatt_db.c \
app.c \
main.c

PROJ_FILES += $(wildcard $(SDK_DIR)/protocol/bluetooth/bin/gatt/*)
PROJ_FILES += config/btconf/gatt_configuration.btconf

################################################################################
# Target rules                                                                 #
################################################################################

include $(SDK_DIR)/app/bluetooth/component_host/targets.mk

HELP_MESSAGE += "  gattdb  - generate GATT database\n"

.PHONY: gattdb
gattdb:
	@echo Generate GATT database
	@python3 $(BGBUILD) config/btconf -o autogen

autogen/gatt_db.c: config/btconf/gatt_configuration.btconf
	@echo
	@echo The GATT database might be outdated. Please run: make gattdb
	@echo
