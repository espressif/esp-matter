set(PRIV_REQUIRES_LIST esp_matter bootloader_support)

idf_component_register(SRC_DIRS           "."
                       PRIV_INCLUDE_DIRS  "."
                       PRIV_REQUIRES      ${PRIV_REQUIRES_LIST}
                       EMBED_FILES        "paa_cert.der")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
