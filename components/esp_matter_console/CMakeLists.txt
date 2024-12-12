set(src_dirs)
if (CONFIG_ENABLE_CHIP_SHELL)
    list(APPEND src_dirs ".")
endif()

set(priv_req chip mbedtls esp_timer bt openthread esp_matter)
if ("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
    list(APPEND priv_req esp_rcp_update)
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                    INCLUDE_DIRS .
                    PRIV_REQUIRES ${priv_req})
