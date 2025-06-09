idf_component_register(SRC_DIRS          "."
                       PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")

if (CONFIG_IDF_TARGET_ESP32C5)
    idf_component_get_property(openthread_lib openthread COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC $<TARGET_FILE:${openthread_lib}>)
endif()
