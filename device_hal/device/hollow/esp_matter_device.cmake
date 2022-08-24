cmake_minimum_required(VERSION 3.5)

SET(device_type     hollow)
SET(led_type        hollow_led)
SET(button_type     hollow_button)

SET(extra_components_dirs_append "$ENV{ESP_MATTER_DEVICE_PATH}/../../led_driver"
                                 "$ENV{ESP_MATTER_DEVICE_PATH}/../../button_driver/button")
