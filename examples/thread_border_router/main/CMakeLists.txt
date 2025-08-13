idf_component_register(SRC_DIRS          "."
                       INCLUDE_DIRS      "."
                       LDFRAGMENTS "linker.lf")

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
