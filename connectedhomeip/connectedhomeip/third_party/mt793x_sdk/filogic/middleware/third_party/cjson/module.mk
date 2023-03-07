CJSON_SRC = middleware/third_party/cjson

C_FILES  += 	$(CJSON_SRC)/src/cJSON.c \
		$(CJSON_SRC)/src/cJSON_internal.c


CFLAGS     += -I$(SOURCE_DIR)/middleware/third_party/cjson/inc
CFLAGS     += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include

