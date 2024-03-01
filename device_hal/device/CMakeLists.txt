include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(requires led_driver)

if ("${button_type}" STREQUAL "iot")
    list(APPEND requires iot_button)
else()
    list(APPEND requires button)
endif()

idf_component_register(SRC_DIRS "${device_type}"
                        INCLUDE_DIRS include
                        REQUIRES "${requires}")
