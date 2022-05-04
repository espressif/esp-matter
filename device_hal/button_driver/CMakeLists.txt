include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(requires driver)
if ("${button_type}" STREQUAL "gpio")
    list(APPEND requires button)
elseif ("${button_type}" STREQUAL "adc")
    list(APPEND requires button)
endif()

idf_component_register(SRC_DIRS
                    INCLUDE_DIRS
                    REQUIRES ${requires})
