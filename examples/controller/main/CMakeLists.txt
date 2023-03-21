set(PRIV_REQUIRES_LIST device esp_matter esp_matter_console esp_matter_controller route_hook app_reset)

idf_component_register(SRC_DIRS          "."
                      PRIV_INCLUDE_DIRS  "."
                      PRIV_REQUIRES      ${PRIV_REQUIRES_LIST})

spiffs_create_partition_image(paa_cert ${CMAKE_SOURCE_DIR}/paa_cert FLASH_IN_PROJECT)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 14)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DLWIP_IPV6_SCOPES=0" "-DCHIP_HAVE_CONFIG_H")
