
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := port/include aws-iot-device-sdk-embedded-C/include aws-iot-device-sdk-embedded-C/external_libs/jsmn

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := aws-iot-device-sdk-embedded-C/src port aws-iot-device-sdk-embedded-C/external_libs/jsmn
