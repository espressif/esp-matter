include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)
idf_component_register(SRC_DIRS "${device_type}"
                        INCLUDE_DIRS include
                        REQUIRES button_driver led_driver)
