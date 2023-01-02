set(SRC_DIRS_LIST       "."
                        "zap_common/zap-generated"
                        "${ZAP_GENERATED_PATH}"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
                        "${MATTER_SDK_PATH}/src/app"
                        "${MATTER_SDK_PATH}/src/app/server"
                        "${MATTER_SDK_PATH}/src/app/util"
                        "${MATTER_SDK_PATH}/src/app/reporting"
                        "${MATTER_SDK_PATH}/src/app/clusters/access-control-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/account-login-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/administrator-commissioning-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/application-basic-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/application-launcher-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/audio-output-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/barrier-control-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/basic-information"
                        "${MATTER_SDK_PATH}/src/app/clusters/bindings"
                        "${MATTER_SDK_PATH}/src/app/clusters/bridged-device-basic-information-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/channel-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/color-control-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/content-launch-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/descriptor"
                        "${MATTER_SDK_PATH}/src/app/clusters/diagnostic-logs-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/door-lock-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/ethernet-network-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/fixed-label-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/general-commissioning-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/general-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/group-key-mgmt-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/groups-client"
                        "${MATTER_SDK_PATH}/src/app/clusters/groups-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/identify-client"
                        "${MATTER_SDK_PATH}/src/app/clusters/identify-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/keypad-input-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/level-control"
                        "${MATTER_SDK_PATH}/src/app/clusters/localization-configuration-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/low-power-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/media-input-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/media-playback-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/mode-select-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/network-commissioning"
                        "${MATTER_SDK_PATH}/src/app/clusters/occupancy-sensor-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/on-off-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/operational-credentials-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/ota-provider"
                        "${MATTER_SDK_PATH}/src/app/clusters/ota-requestor"
                        "${MATTER_SDK_PATH}/src/app/clusters/power-source-configuration-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/power-source-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/pump-configuration-and-control-client"
                        "${MATTER_SDK_PATH}/src/app/clusters/pump-configuration-and-control-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/scenes"
                        "${MATTER_SDK_PATH}/src/app/clusters/scenes-client"
                        "${MATTER_SDK_PATH}/src/app/clusters/software-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/switch-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/target-navigator-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/test-cluster-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/thermostat-client"
                        "${MATTER_SDK_PATH}/src/app/clusters/thermostat-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/thermostat-user-interface-configuration-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/thread-network-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/time-format-localization-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/user-label-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/wake-on-lan-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/wifi-network-diagnostics-server"
                        "${MATTER_SDK_PATH}/src/app/clusters/window-covering-server"
)

set(INCLUDE_DIRS_LIST   "."
                        "zap_common"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common"
                        "${MATTER_SDK_PATH}/src"
                        "${ZAP_GENERATED_PATH}/../")

set(REQUIRES_LIST       chip bt esp_matter_console nvs_flash app_update esp_secure_cert_mgr esp32_mbedtls esp_system route_hook)

if ("${IDF_TARGET}" STREQUAL "esp32h2")
    list(APPEND REQUIRES_LIST openthread esp_matter_openthread)
endif()

idf_component_register( SRC_DIRS        ${SRC_DIRS_LIST}
                        INCLUDE_DIRS    ${INCLUDE_DIRS_LIST}
                        REQUIRES        ${REQUIRES_LIST})

# This has been added to fix the error and should be removed once fixed:
# esp-matter/connectedhomeip/connectedhomeip/src/app/EventManagement.cpp:467:23: error: 'writer' is
# used uninitialized in this function
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=uninitialized;-Wno-error=maybe-uninitialized;-Wno-missing-field-initializers;" APPEND)
target_compile_options(${COMPONENT_LIB} PUBLIC
                       "-DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=<lib/address_resolve/AddressResolve_DefaultImpl.h>")
