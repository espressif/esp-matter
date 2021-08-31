include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)
set(light_requires )
if ("${light_type}" STREQUAL "ws2812")
    list(APPEND light_requires led_strip)
elseif ("${light_type}" STREQUAL "vled")
    list(APPEND light_requires tft spidriver)
endif()
idf_component_register(SRC_DIRS ${light_type} utils
                    INCLUDE_DIRS include
                    PRIV_REQUIRES ${light_requires})
