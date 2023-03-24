cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32c6" ))
    message(FATAL_ERROR "please set esp32c6 as the IDF_TARGET using 'idf.py --preview set-target esp32c6'")
endif()

SET(device_type     esp32c2_devkit_m)
SET(led_type        gpio)
SET(button_type     hollow_button)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver/button")
