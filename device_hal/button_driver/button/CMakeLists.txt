include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(src_dirs )
set(requires driver)

if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
    list(APPEND requires esp_adc)
endif()

if ("${button_type}" STREQUAL "hollow_button")
    list(APPEND src_dirs hollow_button)
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                    INCLUDE_DIRS include
                    REQUIRES ${requires})
