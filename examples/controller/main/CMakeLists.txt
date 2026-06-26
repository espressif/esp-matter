set(SRC_DIRS_LIST ".")
if (CONFIG_CUSTOM_REVOKED_DAC_CHAIN_CHECK)
    list(APPEND SRC_DIRS_LIST "revocation_set")
endif()

idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                       INCLUDE_DIRS      "."
                       PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils"
                       LDFRAGMENTS "linker.lf")

if(CONFIG_SPIFFS_ATTESTATION_TRUST_STORE)
    spiffs_create_partition_image(paa_cert ${CMAKE_SOURCE_DIR}/paa_cert FLASH_IN_PROJECT)
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
