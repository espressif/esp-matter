include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)
set(button_requires )
idf_component_register(SRCS ${button_type}/button_driver.c
                    INCLUDE_DIRS include
                    REQUIRES ${button_requires})
