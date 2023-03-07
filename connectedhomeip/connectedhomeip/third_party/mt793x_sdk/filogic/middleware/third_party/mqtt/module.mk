
MQTT_SRC = middleware/third_party/mqtt

C_FILES   +=    $(MQTT_SRC)/MQTTPacket/src/MQTTConnectClient.c    \
                $(MQTT_SRC)/MQTTPacket/src/MQTTConnectServer.c     \
                $(MQTT_SRC)/MQTTPacket/src/MQTTDeserializePublish.c    \
                $(MQTT_SRC)/MQTTPacket/src/MQTTFormat.c   \
                $(MQTT_SRC)/MQTTPacket/src/MQTTPacket.c   \
                $(MQTT_SRC)/MQTTPacket/src/MQTTSerializePublish.c   \
                $(MQTT_SRC)/MQTTPacket/src/MQTTSubscribeClient.c    \
                $(MQTT_SRC)/MQTTPacket/src/MQTTSubscribeServer.c    \
                $(MQTT_SRC)/MQTTPacket/src/MQTTUnsubscribeClient.c    \
                $(MQTT_SRC)/MQTTPacket/src/MQTTUnsubscribeServer.c	  \
                $(MQTT_SRC)/MQTTClient-C/src/MQTTClient.c	\
                $(MQTT_SRC)/MQTTClient-C/src/mediatek/MQTTMediatek.c

#################################################################################
#include path
CFLAGS 	+= -I./include
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTPacket/src
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src
#CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mqtt/MQTTClient-C/src/mediatek
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
