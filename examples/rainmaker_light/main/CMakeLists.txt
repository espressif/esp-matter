set(SRC_DIRS_LIST          "${CMAKE_CURRENT_LIST_DIR}"
                           "${CMAKE_CURRENT_LIST_DIR}/gen"
                           "${MATTER_SDK_PATH}/src/app/server"
                           "${MATTER_SDK_PATH}/src/app/util"
                           "${MATTER_SDK_PATH}/src/app/reporting"
                           "${MATTER_SDK_PATH}/src/app/clusters/basic"
                           "${MATTER_SDK_PATH}/src/app/clusters/general-commissioning-server"
                           "${MATTER_SDK_PATH}/src/app/clusters/network-commissioning"
                           "${MATTER_SDK_PATH}/src/app/clusters/on-off-server"
                           "${MATTER_SDK_PATH}/src/app/clusters/operational-credentials-server"
                           "${MATTER_SDK_PATH}/src/app/clusters/level-control"
                           "${MATTER_SDK_PATH}/src/app/clusters/color-control-server")

set(PRIV_INCLUDE_DIRS_LIST      "${CMAKE_CURRENT_LIST_DIR}"
                           "${MATTER_SDK_PATH}/src")

set(PRIV_REQUIRES_LIST chip bt esp32_mbedtls app_driver esp_rainmaker console)

idf_component_register(SRC_DIRS         ${SRC_DIRS_LIST}
                      PRIV_INCLUDE_DIRS      ${PRIV_INCLUDE_DIRS_LIST}
                      PRIV_REQUIRES     ${PRIV_REQUIRES_LIST})

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 14)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DLWIP_IPV6_SCOPES=0" "-DCHIP_HAVE_CONFIG_H")
