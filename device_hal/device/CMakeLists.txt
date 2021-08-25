include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)
idf_component_register(SRC_DIRS "${device_name}"
                        INCLUDE_DIRS include
                        PRIV_REQUIRES ${used_driver})
