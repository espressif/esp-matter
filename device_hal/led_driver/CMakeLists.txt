include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(led_requires driver)
if ("${led_type}" STREQUAL "ws2812")
    list(APPEND led_requires led_strip)
elseif ("${led_type}" STREQUAL "vled")
    list(APPEND led_requires tft spidriver)
endif()

idf_component_register(SRC_DIRS ${led_type} utils
                    INCLUDE_DIRS include
                    PRIV_REQUIRES ${led_requires})
