idf_component_register(SRCS         "esp_matter_ota.cpp"
                       INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}"
                       REQUIRES     esp_matter app_update esp32_mbedtls esp_system)
