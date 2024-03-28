set(SRC_DIRS_LIST ".")
set(INCLUDE_DIRS_LIST ".")
set(ldfragments linker.lf)
# set(PRIV_REQUIRES_LIST device esp_matter esp_matter_console esp_matter_thread_br app_reset console fatfs hal)

if(CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S3)
    list(APPEND SRC_DIRS_LIST "driver/fan")
    list(APPEND INCLUDE_DIRS_LIST "driver/fan/include")
endif()

idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                      PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils"
                    #   PRIV_REQUIRES      ${PRIV_REQUIRES_LIST}
                      INCLUDE_DIRS       ${INCLUDE_DIRS_LIST}
                      LDFRAGMENTS        "${ldfragments}")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")

if(CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S3)
    idf_build_get_property(build_components BUILD_COMPONENTS)
    #To distinguish between a local component and a component manager
    if("espressif__esp_sensorless_bldc_control" IN_LIST build_components)
        idf_component_get_property(bldc_lib espressif__esp_sensorless_bldc_control COMPONENT_LIB)
    else()
        idf_component_get_property(bldc_lib esp_sensorless_bldc_control COMPONENT_LIB)
    endif()

    cmake_policy(SET CMP0079 NEW)
    target_link_libraries(${bldc_lib} PUBLIC ${COMPONENT_LIB})
endif()
