if (CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED)
set(srcs            "src/esp_matter_ota_bdx_sender.cpp"
                    "src/esp_matter_ota_candidates.cpp"
                    "src/esp_matter_ota_http_downloader.cpp"
                    "src/esp_matter_ota_provider.cpp")

set(include_dirs    "include")

set(priv_include_dirs "private_include")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       REQUIRES esp_matter esp_http_client json_parser)
