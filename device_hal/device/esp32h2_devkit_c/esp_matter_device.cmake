cmake_minimum_required(VERSION 3.5)
if (NOT ("${IDF_TARGET}" STREQUAL "esp32h2"))
    message(FATAL_ERROR "please set esp32h2 as the IDF_TARGET using 'idf.py -- preview set-target esp32h2'")
endif()

SET(device_hal_path $ENV{ESP_MATTER_DEVICE_PATH}/../../)
SET(device_name esp32h2_devkit_c)
SET(light_type ws2812)
SET(button_type hollow)
SET(used_driver light_driver button_driver)
SET(extra_components_dirs_append "${device_hal_path}/light_driver"
                                 "${device_hal_path}/button_driver"
                                 "$ENV{IDF_PATH}/examples/common_components/led_strip")
