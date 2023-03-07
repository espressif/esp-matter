
###################################################
# Sources
AWS_SRC = middleware/third_party/aws_iot

CORE_FILES = $(AWS_SRC)/src/aws_iot_json_utils.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client_common_internal.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client_connect.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client_publish.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client_subscribe.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client_unsubscribe.c \
             $(AWS_SRC)/src/aws_iot_mqtt_client_yield.c \
             $(AWS_SRC)/src/aws_iot_shadow.c \
             $(AWS_SRC)/src/aws_iot_shadow_actions.c \
             $(AWS_SRC)/src/aws_iot_shadow_json.c \
             $(AWS_SRC)/src/aws_iot_shadow_records.c \
             $(AWS_SRC)/external_libs/jsmn/jsmn.c

PLATFORM_FILES = $(AWS_SRC)/platform/freertos/network_mbedtls_wrapper.c \
                 $(AWS_SRC)/platform/freertos/threads_mutex_wrapper.c \
                 $(AWS_SRC)/platform/freertos/aws_iot_timer.c

SAMPLE_FILES = $(AWS_SRC)/samples/freertos/shadow_sample/shadow_sample.c \
               $(AWS_SRC)/samples/freertos/shadow_sample_console_echo/shadow_console_echo.c \
               $(AWS_SRC)/samples/freertos/subscribe_publish_library_sample/subscribe_publish_library_sample.c \
               $(AWS_SRC)/samples/freertos/subscribe_publish_sample/subscribe_publish_sample.c

TEST_FILES = $(AWS_SRC)/tests/freertos_integration/src/aws_iot_test_auto_reconnect.c \
             $(AWS_SRC)/tests/freertos_integration/src/aws_iot_test_basic_connectivity.c \
             $(AWS_SRC)/tests/freertos_integration/src/aws_iot_test_multiple_clients.c \
             $(AWS_SRC)/tests/freertos_integration/multithreadingTest/aws_iot_test_multithreading_validation.c


C_FILES += $(CORE_FILES)
C_FILES += $(PLATFORM_FILES)
C_FILES += $(SAMPLE_FILES)
C_FILES += $(TEST_FILES)


###################################################
# include path
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/aws_iot/include
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/aws_iot/external_libs/jsmn
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/aws_iot/platform/freertos
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/aws_iot/samples/freertos
CFLAGS      += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
CFLAGS      += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
