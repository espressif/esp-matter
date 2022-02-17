idf_component_register(SRCS         "esp_matter_factory.c"
                       INCLUDE_DIRS .
                       REQUIRES     chip nvs_flash esp32_mbedtls)
