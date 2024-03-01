cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32" ))
    message(FATAL_ERROR "please set esp32 as the IDF_TARGET using 'idf.py set-target esp32'")
endif()

SET(device_type     m5stack)
SET(led_type        vled)
SET(button_type     iot)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver/iot_button"
                                 "$ENV{ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/common/m5stack-tft/repo/components/tft/"
                                 "$ENV{ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/common/m5stack-tft/repo/components/spidriver/")
