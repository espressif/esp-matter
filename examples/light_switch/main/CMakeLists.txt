set(SRC_DIRS_LIST ".")

if (CONFIG_DYNAMIC_PASSCODE_COMMISSIONABLE_DATA_PROVIDER)
    list(APPEND SRC_DIRS_LIST "custom_provider")
endif()

idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                       PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
