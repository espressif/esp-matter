idf_component_register(SRCS esp_matter_console.cpp esp_matter_console_diagnostics.cpp
                    INCLUDE_DIRS .
                    PRIV_REQUIRES chip esp32_mbedtls)
