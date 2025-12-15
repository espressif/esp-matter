set(requires esp_http_client json mbedtls esp_wifi wifi_provisioning nvs_flash protobuf-c esp_app_format pthread)

idf_component_register(SRCS "rainmaker_api.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires})
