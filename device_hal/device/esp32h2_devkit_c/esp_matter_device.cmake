cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32h2"))
    message(FATAL_ERROR "please set esp32h2 as the IDF_TARGET using 'idf.py --preview set-target esp32h2'")
endif()

SET(device_type     esp32h2_devkit_c)
SET(led_type        ws2812)
SET(button_type     iot)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver/iot_button")
