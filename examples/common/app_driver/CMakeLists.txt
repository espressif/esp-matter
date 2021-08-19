include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)
idf_component_register(SRCS app_driver.c
                    INCLUDE_DIRS .
                    PRIV_REQUIRES device ${used_driver} esp_matter esp_matter_console)
