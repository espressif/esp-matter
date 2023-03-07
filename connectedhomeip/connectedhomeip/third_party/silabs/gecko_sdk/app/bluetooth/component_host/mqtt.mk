################################################################################
# MQTT component based on the Mosquitto implementation                         #
################################################################################

override INCLUDEPATHS += $(SDK_DIR)/app/bluetooth/common_host/mqtt

override C_SRC += $(SDK_DIR)/app/bluetooth/common_host/mqtt/mqtt.c

ifeq ($(OS),win)
# Mosquitto is outside of GSDK. Therefore, add it directly as a compiler flag
# instead of adding it to INCLUDEPATHS.
override CFLAGS += -I"$(MOSQUITTO_DIR)/devel"
endif

ifeq ($(OS),posix)
override LDFLAGS += -lmosquitto
else
override LDFLAGS += "${MOSQUITTO_DIR}/devel/mosquitto.lib"
endif
