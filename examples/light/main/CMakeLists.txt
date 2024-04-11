idf_component_register(SRCS
                           "app_driver.cpp"
                           "app_main.cpp"
                           "${MATTER_SDK_PATH}/examples/temperature-measurement-app/esp32/main/diagnostic-logs-provider-delegate-impl.cpp"
                       PRIV_INCLUDE_DIRS
                           "."
                           "${ESP_MATTER_PATH}/examples/common/utils"
                           "${MATTER_SDK_PATH}/examples/temperature-measurement-app/esp32/main/include"
                       EMBED_FILES
                           "${MATTER_SDK_PATH}/examples/temperature-measurement-app/esp32/main/diagnostic_logs/end_user_support.log"
                           "${MATTER_SDK_PATH}/examples/temperature-measurement-app/esp32/main/diagnostic_logs/network_diag.log")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
