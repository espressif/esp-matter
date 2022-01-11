set(SRC_DIRS_LIST       "."
                        "${ZAP_GENERATED_PATH}"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
                        "${MATTER_SDK_PATH}/src/app"
                        "${MATTER_SDK_PATH}/src/app/server"
                        "${MATTER_SDK_PATH}/src/app/util"
                        "${MATTER_SDK_PATH}/src/app/reporting"
                        "${MATTER_SDK_PATH}/src/app/clusters/basic"
                        "${MATTER_SDK_PATH}/src/app/clusters/administrator-commissioning-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/application-basic-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/general-commissioning-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/general-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/fixed-label-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/user-label-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/ota-provider"
                        "${MATTER_SDK_PATH}/src/app/clusters/ota-requestor"
                        "${MATTER_SDK_PATH}/src/app/clusters/network-commissioning"
                        "${MATTER_SDK_PATH}/src/app/clusters/diagnostic-logs-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/software-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/thread-network-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/wifi-network-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/on-off-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/operational-credentials-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/descriptor"
                        "${MATTER_SDK_PATH}/src/app/clusters/level-control"
                        "${MATTER_SDK_PATH}/src/app/clusters/identify-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/groups-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/scenes"
                        "${MATTER_SDK_PATH}/src/app/clusters/color-control-server")

set(INCLUDE_DIRS_LIST   "."
                        "${MATTER_SDK_PATH}/zzz_generated/app-common"
                        "${MATTER_SDK_PATH}/src"
                        "${ZAP_GENERATED_PATH}/../")

set(REQUIRES_LIST       chip bt)

if ("${IDF_TARGET}" STREQUAL "esp32h2")
    list(APPEND REQUIRES_LIST openthread app_openthread)
endif()

idf_component_register( SRC_DIRS        ${SRC_DIRS_LIST}
                        INCLUDE_DIRS    ${INCLUDE_DIRS_LIST}
                        REQUIRES        ${REQUIRES_LIST})

# This has been added to fix the error and should be removed once fixed:
# esp-matter/connectedhomeip/connectedhomeip/src/app/EventManagement.cpp:467:23: error: 'writer' is
# used uninitialized in this function
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=uninitialized" APPEND)
