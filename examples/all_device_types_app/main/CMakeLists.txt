set(PRIV_REQUIRES_LIST device esp_matter esp_matter_console esp_matter_thread_br app_reset console fatfs hal)
set(ldfragments linker.lf)
idf_component_register(SRC_DIRS          "."
                      PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils"
                      PRIV_REQUIRES      ${PRIV_REQUIRES_LIST}
                      LDFRAGMENTS        "${ldfragments}"
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
