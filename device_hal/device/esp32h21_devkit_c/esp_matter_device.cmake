cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32h21"))
    message(FATAL_ERROR "please set esp32h21 as the IDF_TARGET using 'idf.py --preview set-target esp32h21'")
endif()

SET(device_type     esp32h21_devkit_c)
SET(led_type        ws2812)
SET(button_type     iot)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver/iot_button")
