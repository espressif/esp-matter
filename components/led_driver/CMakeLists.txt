set(priv_requires_list )

# Chip specific components. Try and remove this later.
if(${IDF_TARGET} STREQUAL "esp32")
    list(APPEND priv_requires_list tft spidriver)
elseif(${IDF_TARGET} STREQUAL "esp32c3")
    list(APPEND priv_requires_list led_strip)
endif()

idf_component_register(SRCS ${CONFIG_LED_DRIVER}/led_driver.c color_format/color_format.c
                    INCLUDE_DIRS include
                    PRIV_REQUIRES ${priv_requires_list})
