cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32c2" ))
    message(FATAL_ERROR "please set esp32c2 as the IDF_TARGET using 'idf.py set-target esp32c2'")
endif()

SET(device_type     esp32c2_devkit_m)
SET(led_type        gpio)
SET(button_type     iot_button)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver/iot_button")
