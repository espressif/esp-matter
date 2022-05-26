include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(src_dirs )
set(requires driver)

if ("${button_type}" STREQUAL "hollow_button")
    list(APPEND src_dirs hollow_button)
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                    INCLUDE_DIRS include
                    REQUIRES ${requires})
