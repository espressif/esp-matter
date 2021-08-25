cmake_minimum_required(VERSION 3.5)
if(NOT ("${IDF_TARGET}" STREQUAL "esp32" ))
    message(FATAL_ERROR "please set esp32 as the IDF_TARGET using 'idf.py set-target esp32'")
endif()

SET(device_hal_path $ENV{ESP_MATTER_DEVICE_PATH}/../../)
SET(device_name  m5stack)
SET(light_type vled)
SET(button_type hollow)
SET(used_driver light_driver button_driver)
SET(extra_components_dirs_append "${device_hal_path}/light_driver"
                                 "${device_hal_path}/button_driver"
                                 "$ENV{ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/common/m5stack-tft/repo/components/tft/"
                                 "$ENV{ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/common/m5stack-tft/repo/components/spidriver/")

