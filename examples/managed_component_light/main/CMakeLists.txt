idf_component_register(SRC_DIRS          "."
                       INCLUDE_DIRS      "."
                       PRIV_INCLUDE_DIRS "../../common/utils")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")