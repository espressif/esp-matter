include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

if("${led_type}" STREQUAL "ws2812")
    set(led_backend_source "backends/ws2812.c")
    set(led_backend_supports_color 1)
elseif("${led_type}" STREQUAL "gpio")
    set(led_backend_source "backends/ledc.c")
    set(led_backend_supports_color 0)
elseif("${led_type}" STREQUAL "hollow_led")
    set(led_backend_source "backends/hollow.c")
    set(led_backend_supports_color 1)
else()
    message(FATAL_ERROR "Unsupported device HAL led_type '${led_type}'; expected ws2812, gpio, or hollow_led")
endif()

idf_component_register(SRCS "led_driver.c" "led_driver_utils.c" "${led_backend_source}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include"
                       PRIV_REQUIRES esp_driver_ledc)

# Compile unsupported color setter bodies out of the LEDC backend. This prevents
# color conversion tables, floating-point helpers, and libm from entering the binary.
target_compile_definitions(${COMPONENT_LIB} PRIVATE
                           LED_DRIVER_BACKEND_SUPPORTS_COLOR=${led_backend_supports_color})
