cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32s2" ))
    message(FATAL_ERROR "please set esp32s2 as the IDF_TARGET using 'idf.py set-target esp32s2'")
endif()

SET(device_type     esp32s2_devkit_c)
SET(led_type        hollow_led)
SET(button_type     gpio)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver")
