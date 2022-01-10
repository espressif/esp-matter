cmake_minimum_required(VERSION 3.5)
if(NOT ("${IDF_TARGET}" STREQUAL "esp32s2" ))
    message(FATAL_ERROR "please set esp32s2 as the IDF_TARGET using 'idf.py set-target esp32s2'")
endif()

SET(device_hal_path $ENV{ESP_MATTER_DEVICE_PATH}/../../)
SET(device_name  esp32s2_devkit_c)
SET(light_type hollow)
SET(button_type hollow)
SET(used_driver light_driver button_driver)
SET(extra_components_dirs_append "${device_hal_path}/light_driver"
                                 "${device_hal_path}/button_driver")


