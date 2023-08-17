idf_component_register(SRC_DIRS          "."
                      PRIV_INCLUDE_DIRS  "."
                                         "${ESP_MATTER_PATH}/examples/common/blemesh_platform")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 14)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
