idf_component_register(SRC_DIRS          "."
                                         "${MATTER_SDK_PATH}/src/app/clusters/access-control-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/administrator-commissioning-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/basic-information"
                                         "${MATTER_SDK_PATH}/src/app/clusters/bindings"
                                         "${MATTER_SDK_PATH}/src/app/clusters/color-control-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/descriptor"
                                         "${MATTER_SDK_PATH}/src/app/clusters/diagnostic-logs-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/ethernet-network-diagnostics-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/fixed-label-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/general-commissioning-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/general-diagnostics-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/group-key-mgmt-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/groups-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/identify-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/level-control"
                                         "${MATTER_SDK_PATH}/src/app/clusters/localization-configuration-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/network-commissioning"
                                         "${MATTER_SDK_PATH}/src/app/clusters/on-off-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/operational-credentials-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/ota-requestor"
                                         "${MATTER_SDK_PATH}/src/app/clusters/scenes-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/software-diagnostics-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/switch-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/thread-network-diagnostics-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/time-format-localization-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/user-label-server"
                                         "${MATTER_SDK_PATH}/src/app/clusters/wifi-network-diagnostics-server"
                                         "${MATTER_SDK_PATH}/src/app/util"
                                         "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
                       PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils")

# We must set CHIP_ROOT to include esp32_codegen.cmake
get_filename_component(CHIP_ROOT "${MATTER_SDK_PATH}" REALPATH)
include("${CHIP_ROOT}/build/chip/esp32/esp32_codegen.cmake")
chip_app_component_codegen("${CHIP_ROOT}/examples/lighting-app/lighting-common/lighting-app.matter")
chip_app_component_zapgen("${CHIP_ROOT}/examples/lighting-app/lighting-common/lighting-app.zap")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
