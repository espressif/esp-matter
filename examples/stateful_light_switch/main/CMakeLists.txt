set(SRC_DIRS_LIST "." "${ESP_MATTER_PATH}/examples/common/app_reset")

if (CONFIG_DYNAMIC_PASSCODE_COMMISSIONABLE_DATA_PROVIDER)
    list(APPEND SRC_DIRS_LIST "custom_provider")
endif()


idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                       PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils" "${ESP_MATTER_PATH}/examples/common/app_reset")

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
