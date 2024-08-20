set(src_dirs)
if (CONFIG_ENABLE_CHIP_SHELL)
    list(APPEND src_dirs ".")
endif()
idf_component_register(SRC_DIRS ${src_dirs}
                    INCLUDE_DIRS .
                    PRIV_REQUIRES chip mbedtls esp_timer bt openthread esp_rcp_update)
