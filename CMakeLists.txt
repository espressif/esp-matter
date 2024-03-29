get_filename_component(MATTER_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/connectedhomeip/connectedhomeip/ REALPATH)

get_filename_component(CHIP_ROOT ${CMAKE_CURRENT_LIST_DIR}/connectedhomeip/connectedhomeip/ REALPATH)
include(${CMAKE_CURRENT_LIST_DIR}/connectedhomeip/connectedhomeip/config/esp32/components/chip/ota-image.cmake)

set(ESP_COMPONENTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/components)

set(REQUIRE_COMPONENTS esp_eth freertos lwip bt mbedtls fatfs app_update console openthread nvs_flash spi_flash
    esp_secure_cert_mgr esp_system esp_timer json_parser spiffs esp_http_client json_generator esp_netif vfs json)

if(NOT "${IDF_TARGET}" STREQUAL "esp32h2")
    list(APPEND REQUIRE_COMPONENTS mdns)
endif()

function(collect_exclude_files PATH INCLUDE_FILES EXCLUDE_LIST)
    file(GLOB ALL_FILES ${PATH}/*.cpp ${PATH}/*.c)
    set(EXCLUDE_FILES "")
    foreach(FILE ${ALL_FILES})
        list(FIND INCLUDE_FILES ${FILE} EXCLUDE_INDEX)
        if(EXCLUDE_INDEX LESS 0)
            list(APPEND EXCLUDE_FILES ${FILE})
        endif()
    endforeach()
    set(${EXCLUDE_LIST} ${EXCLUDE_FILES} PARENT_SCOPE)
endfunction()

set(SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/access"
                  "${MATTER_SDK_PATH}/src/access/examples"
                  "${MATTER_SDK_PATH}/src/app"
                  "${MATTER_SDK_PATH}/src/app/MessageDef"
                  "${MATTER_SDK_PATH}/src/app/reporting"
                  "${MATTER_SDK_PATH}/src/app/server"
                  "${MATTER_SDK_PATH}/src/ble"
                  "${MATTER_SDK_PATH}/src/credentials"
                  "${MATTER_SDK_PATH}/src/credentials/attestation_verifier"
                  "${MATTER_SDK_PATH}/src/credentials/examples"
                  "${MATTER_SDK_PATH}/src/credentials/tests"
                  "${MATTER_SDK_PATH}/src/crypto"
                  "${MATTER_SDK_PATH}/src/inet"
                  "${MATTER_SDK_PATH}/src/lib/address_resolve"
                  "${MATTER_SDK_PATH}/src/lib/asn1"
                  "${MATTER_SDK_PATH}/src/lib/core"
                  "${MATTER_SDK_PATH}/src/lib/dnssd"
                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns"
                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/core"
                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/records"
                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/responders"
                  "${MATTER_SDK_PATH}/src/lib/support"
                  "${MATTER_SDK_PATH}/src/lib/support/logging"
                  "${MATTER_SDK_PATH}/src/lib/support/verhoeff"
                  "${MATTER_SDK_PATH}/src/messaging"
                  "${MATTER_SDK_PATH}/src/platform"
                  "${MATTER_SDK_PATH}/src/platform/OpenThread"
                  "${MATTER_SDK_PATH}/src/protocols"
                  "${MATTER_SDK_PATH}/src/protocols/bdx"
                  "${MATTER_SDK_PATH}/src/protocols/echo"
                  "${MATTER_SDK_PATH}/src/protocols/secure_channel"
                  "${MATTER_SDK_PATH}/src/protocols/user_directed_commissioning"
                  "${MATTER_SDK_PATH}/src/setup_payload"
                  "${MATTER_SDK_PATH}/src/system"
                  "${MATTER_SDK_PATH}/src/tracing"
                  "${MATTER_SDK_PATH}/src/transport"
                  "${MATTER_SDK_PATH}/src/transport/raw"
                  "${MATTER_SDK_PATH}/third_party/nlfaultinjection/src"
                  "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated")

set(INCLUDE_DIRS_LIST "${MATTER_SDK_PATH}/src"
                      "${MATTER_SDK_PATH}/src/include"
                      "${MATTER_SDK_PATH}/src/lib"
                      "${MATTER_SDK_PATH}/src/platform/OpenThread"
                      "${MATTER_SDK_PATH}/third_party/nlfaultinjection/include"
                      "${MATTER_SDK_PATH}/third_party/nlassert/repo/include"
                      "${MATTER_SDK_PATH}/third_party/nlio/repo/include"
                      "${MATTER_SDK_PATH}/zzz_generated/app-common")

set(EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/controller/ExamplePersistentStorage.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/attestation_verifier/FileAttestationTrustStore.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/CHIPCert_error_test_vectors.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/CHIPCert_test_vectors.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/CHIPCert_unit_test_vectors.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/FuzzChipCert.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestCertificationDeclaration.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestChipCert.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestCommissionerDUTVectors.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestDeviceAttestationConstruction.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestDeviceAttestationCredentials.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestFabricTable.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestGroupDataProvider.cpp"
                      "${MATTER_SDK_PATH}/src/credentials/tests/TestPersistentStorageOpCertStore.cpp"
                      "${MATTER_SDK_PATH}/src/crypto/CHIPCryptoPALOpenSSL.cpp"
                      "${MATTER_SDK_PATH}/src/crypto/CHIPCryptoPALPSA.cpp"
                      "${MATTER_SDK_PATH}/src/crypto/PSAOperationalKeystore.cpp"
                      "${MATTER_SDK_PATH}/src/crypto/PSASessionKeystore.cpp"
                      "${MATTER_SDK_PATH}/src/inet/TCPEndPointImplOpenThread.cpp"
                      "${MATTER_SDK_PATH}/src/inet/TCPEndPointImplSockets.cpp"
                      "${MATTER_SDK_PATH}/src/inet/UDPEndPointImplNetworkFramework.cpp"
                      "${MATTER_SDK_PATH}/src/inet/UDPEndPointImplOpenThread.cpp"
                      "${MATTER_SDK_PATH}/src/inet/UDPEndPointImplSockets.cpp"
                      "${MATTER_SDK_PATH}/src/lib/address_resolve/tool.cpp"
                      "${MATTER_SDK_PATH}/src/lib/dnssd/Advertiser_ImplNone.cpp"
                      "${MATTER_SDK_PATH}/src/lib/dnssd/Resolver_ImplNone.cpp"
                      "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/AddressPolicy_LibNlImpl.cpp"
                      "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/Logging.cpp"
                      "${MATTER_SDK_PATH}/src/lib/support/UnitTestRegistration.cpp"
                      "${MATTER_SDK_PATH}/src/lib/support/UnitTest.cpp"
                      "${MATTER_SDK_PATH}/src/lib/support/UnitTestUtils.cpp"
                      "${MATTER_SDK_PATH}/src/lib/support/JniReferences.cpp"
                      "${MATTER_SDK_PATH}/src/lib/support/CHIPMem-Simple.cpp"
                      "${MATTER_SDK_PATH}/src/lib/shell/commands/NFC.cpp"
                      "${MATTER_SDK_PATH}/src/platform/DeviceSafeQueue.cpp"
                      "${MATTER_SDK_PATH}/src/platform/SyscallStubs.cpp"
                      "${MATTER_SDK_PATH}/src/platform/OpenThread/GenericThreadStackManagerImpl_OpenThread_LwIP.cpp"
                      "${MATTER_SDK_PATH}/src/system/SystemLayerImplSelect.cpp"
                      "${MATTER_SDK_PATH}/src/transport/TraceMessage.cpp")

list(APPEND INCLUDE_DIRS_LIST "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB CLUSTER_DIR_LIST true ${MATTER_SDK_PATH}/src/app/clusters/*)
foreach(CLUSTER_DIR ${CLUSTER_DIR_LIST})
    file(GLOB_RECURSE C_CPP_FILES "${CLUSTER_DIR}/*.c" "${CLUSTER_DIR}/*.cpp")
    if (C_CPP_FILES)
        list(APPEND SRC_DIRS_LIST "${CLUSTER_DIR}")
    endif()
endforeach()

list(APPEND SRC_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter"
                          "${ESP_COMPONENTS_PATH}/esp_matter/private"
                          "${ESP_COMPONENTS_PATH}/esp_matter/utils"
                          "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
                          "${MATTER_SDK_PATH}/src/app/util"
                          "${ESP_COMPONENTS_PATH}/esp_matter_bridge"
                          "${ESP_COMPONENTS_PATH}/esp_matter_console")

list(APPEND INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter"
                              "${ESP_COMPONENTS_PATH}/esp_matter/utils"
                              "${ESP_COMPONENTS_PATH}/esp_matter_bridge"
                              "${ESP_COMPONENTS_PATH}/esp_matter_console")

if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    list(APPEND SRC_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter/zap_common/app")
    list(APPEND INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter/zap_common")
endif()

set(PRIV_INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter/private")

if (CONFIG_ESP_MATTER_CONTROLLER_ENABLE)
    list(APPEND SRC_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_controller"
                              "${ESP_COMPONENTS_PATH}/esp_matter_controller/logger/zap-generated")
    list(APPEND INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_controller"
                                  "${ESP_COMPONENTS_PATH}/esp_matter_controller/logger")

    if (CONFIG_ESP_MATTER_CONTROLLER_CUSTOM_CLUSTER_ENABLE)
        list(APPEND SRC_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_controller/controller_custom_cluster")
        list(APPEND INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_controller/controller_custom_cluster")
    endif()
    if (NOT CONFIG_ESP_MATTER_COMMISSIONER_ENABLE)
        list(APPEND EXCLUDE_SRCS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_controller/esp_matter_commissioner.cpp"
                                      "${ESP_COMPONENTS_PATH}/esp_matter_controller/esp_matter_controller_pairing_command.cpp"
                                      "${ESP_COMPONENTS_PATH}/esp_matter_controller/esp_matter_attestation_trust_store.cpp"
                                      "${ESP_COMPONENTS_PATH}/esp_matter_controller/esp_matter_controller_group_settings.cpp")
    endif()
endif()

idf_build_get_property(rainmaker_enabled RAINMAKER_ENABLED)
if (${rainmaker_enabled})
    list(APPEND SRC_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_rainmaker")
    list(APPEND INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_rainmaker")
    list(APPEND REQUIRE_COMPONENTS esp_rainmaker)
endif()

if (CONFIG_OPENTHREAD_BORDER_ROUTER)
    list(APPEND INCLUDE_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_thread_br")
    list(APPEND SRC_DIRS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_thread_br")
    if ((NOT CONFIG_ENABLE_CHIP_SHELL) OR (NOT CONFIG_OPENTHREAD_CLI))
        list(APPEND EXCLUDE_SRCS_LIST "${ESP_COMPONENTS_PATH}/esp_matter_thread_br/esp_matter_thread_br_console.cpp")
    endif()
endif()

# ESP-IDF lets user set software version string by two ways:
# 1. Project's CMakeLists.txt file and 2. Config option
# It depends on CONFIG_APP_PROJECT_VER_FROM_CONFIG option
# So, below makes the same provision for software version number
if (CONFIG_APP_PROJECT_VER_FROM_CONFIG)
    set(chip_config_software_version_number ${CONFIG_DEVICE_SOFTWARE_VERSION_NUMBER})
elseif (DEFINED PROJECT_VER_NUMBER)
    set(chip_config_software_version_number ${PROJECT_VER_NUMBER})
else()
    set(chip_config_software_version_number 0)
endif()

if(CONFIG_DISABLE_READ_CLIENT)
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/app/BufferedReadCallback.cpp"
                                  "${MATTER_SDK_PATH}/src/app/ClusterStateCache.cpp"
                                  "${MATTER_SDK_PATH}/src/app/ReadClient.cpp")
endif()

if (NOT CONFIG_BUILD_CHIP_TESTS)
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/inet/InetFaultInjection.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/support/CHIPFaultInjection.cpp"
                                  "${MATTER_SDK_PATH}/src/system/SystemFaultInjection.cpp"
                                  "${MATTER_SDK_PATH}/third_party/nlfaultinjection/src/nlfaultinjection.cpp")
endif()

if (NOT CONFIG_USE_MINIMAL_MDNS)
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/lib/dnssd/ActiveResolveAttempts.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/Advertiser_ImplMinimalMdns.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/IncrementalResolve.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/MinimalMdnsServer.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/Resolver_ImplMinimalMdns.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/Parser.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/RecordData.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/ResponseSender.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/Server.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/AddressPolicy.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/core/QName.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/core/QNameString.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/core/RecordWriter.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/responders/IP.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/responders/QueryResponder.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/records/IP.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/records/ResourceRecord.cpp"
                                  "${MATTER_SDK_PATH}/src/lib/dnssd/minimal_mdns/AddressPolicy_DefaultImpl.cpp")
else()
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/lib/dnssd/Discovery_ImplPlatform.cpp")
endif()

if (CONFIG_ENABLE_CHIP_SHELL)
    list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/lib/shell")
    list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/lib/shell/commands")    
    set(ESP_SHELL_FILES "${MATTER_SDK_PATH}/src/lib/shell/MainLoopESP32.cpp"
                        "${MATTER_SDK_PATH}/src/lib/shell/streamer_esp32.cpp"
                        "${MATTER_SDK_PATH}/src/lib/shell/Engine.cpp"
                        "${MATTER_SDK_PATH}/src/lib/shell/streamer.cpp")
    collect_exclude_files("${MATTER_SDK_PATH}/src/lib/shell" "${ESP_SHELL_FILES}" SHELL_EXCLUDE_FILES)
    list(APPEND EXCLUDE_SRCS_LIST "${SHELL_EXCLUDE_FILES}")
endif()

if (CONFIG_ENABLE_CHIP_CONTROLLER_BUILD)
    list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/controller")
    if (CONFIG_DISABLE_READ_CLIENT)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/controller/CHIPDeviceController.cpp"
                                      "${MATTER_SDK_PATH}/src/controller/CommissioningWindowOpener.cpp"
                                      "${MATTER_SDK_PATH}/src/controller/CurrentFabricRemover.cpp")
    endif()
endif()

if((NOT CONFIG_ENABLE_WIFI_STATION) AND (NOT CONFIG_ENABLE_WIFI_AP))
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/lib/shell/commands/WiFi.cpp")
endif()

if (NOT CONFIG_ENABLE_MATTER_OVER_THREAD)
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/OpenThread/DnssdImpl.cpp"
                                  "${MATTER_SDK_PATH}/src/platform/OpenThread/GenericNetworkCommissioningThreadDriver.cpp"
                                  "${MATTER_SDK_PATH}/src/platform/OpenThread/OpenThreadUtils.cpp"
                                  "${MATTER_SDK_PATH}/src/platform/OpenThread/OpenThreadDnssdImpl.cpp")
endif()

# Set up CHIP project configuration file
if (CONFIG_CHIP_PROJECT_CONFIG)
    get_filename_component(CHIP_PROJECT_CONFIG
        ${CONFIG_CHIP_PROJECT_CONFIG}
        REALPATH
        BASE_DIR ${CMAKE_SOURCE_DIR}
    )
    set(CHIP_PROJECT_CONFIG "<${CHIP_PROJECT_CONFIG}>")
else()
    set(CHIP_PROJECT_CONFIG "")
endif()

if (CONFIG_ENABLE_ESP_INSIGHTS_TRACE)
    list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/tracing/esp32_trace")
    list(APPEND INCLUDE_DIRS_LIST "${MATTER_SDK_PATH}/src/tracing/esp32_trace/include")
endif()

if (CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM)
    # check the source file dir if external platform is enable
    if ((EXISTS ${CONFIG_CHIP_EXTERNAL_PLATFORM_DIR}/external_platform.cmake))
        include(${CONFIG_CHIP_EXTERNAL_PLATFORM_DIR}/external_platform.cmake)
        list(APPEND SRC_DIRS_LIST ${EXPLANT_SRC_DIRS_LIST})
        list(APPEND INCLUDE_DIRS_LIST ${EXPLANT_INCLUDE_DIRS_LIST})
        list(APPEND EXCLUDE_SRCS_LIST ${EXPLANT_EXCLUDE_SRCS_LIST})
    else()
        message(FATAL_ERROR "CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM is set, but CONFIG_CHIP_EXTERNAL_PLATFORM_DIR is not set correctly!")
    endif()
else()
    list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32"
                              "${MATTER_SDK_PATH}/src/platform/ESP32/route_hook")

    list(APPEND INCLUDE_DIRS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32"
                                  "${MATTER_SDK_PATH}/src/platform/ESP32/bluedroid"
                                  "${MATTER_SDK_PATH}/src/platform/ESP32/nimble"
                                  "${MATTER_SDK_PATH}/src/platform/ESP32/route_hook")

    if (CONFIG_USE_MINIMAL_MDNS)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/DnssdImpl.cpp")
    endif()

    if ((CONFIG_USE_MINIMAL_MDNS) OR ((NOT CONFIG_ENABLE_WIFI_STATION) AND (NOT CONFIG_ENABLE_WIFI_AP)))
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/WiFiDnssdImpl.cpp")
    endif()

    if((NOT CONFIG_ENABLE_WIFI_STATION) AND (NOT CONFIG_ENABLE_WIFI_AP))
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/ConnectivityManagerImpl_WiFi.cpp"
                                      "${MATTER_SDK_PATH}/src/platform/ESP32/NetworkCommissioningDriver.cpp"
                                      "${MATTER_SDK_PATH}/src/platform/ESP32/route_hook/ESP32RouteHook.c"
                                      "${MATTER_SDK_PATH}/src/platform/ESP32/route_hook/ESP32RouteTable.c")
    endif()

    if(NOT CONFIG_ENABLE_ROUTE_HOOK)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/route_hook/ESP32RouteHook.c"
                                      "${MATTER_SDK_PATH}/src/platform/ESP32/route_hook/ESP32RouteTable.c")
    endif()

    if ((CONFIG_BT_ENABLED) AND (CONFIG_ENABLE_CHIPOBLE))
        if (CONFIG_BT_NIMBLE_ENABLED)
            list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/nimble")
            list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/bluedroid/BLEManagerImpl.cpp")
            if(NOT CONFIG_ENABLE_ESP32_BLE_CONTROLLER)
                list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/bluedroid/ChipDeviceScanner.cpp")
            endif()
        else()
            list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/bluedroid")
            list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/nimble/BLEManagerImpl.cpp")
            if(NOT CONFIG_ENABLE_ESP32_BLE_CONTROLLER)
                list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/nimble/ChipDeviceScanner.cpp"
                                              "${MATTER_SDK_PATH}/src/platform/ESP32/nimble/misc.c"
                                              "${MATTER_SDK_PATH}/src/platform/ESP32/nimble/peer.c")
            endif()
        endif()
    endif()

    if (NOT CONFIG_ENABLE_ETHERNET_TELEMETRY)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/ConnectivityManagerImpl_Ethernet.cpp"
                                      "${MATTER_SDK_PATH}/src/platform/ESP32/NetworkCommissioningDriver_Ethernet.cpp")
    endif()

    if ((NOT CONFIG_OPENTHREAD_ENABLED) OR (NOT CONFIG_ENABLE_MATTER_OVER_THREAD))
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/OpenthreadLauncher.cpp"
                                      "${MATTER_SDK_PATH}/src/platform/ESP32/ThreadStackManagerImpl.cpp")
    endif()

    if (NOT CONFIG_ENABLE_OTA_REQUESTOR)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/OTAImageProcessorImpl.cpp")
    endif()

    if (NOT CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/ESP32FactoryDataProvider.cpp")
    endif()

    if (NOT CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/ESP32DeviceInfoProvider.cpp")
    endif()

    if (NOT CONFIG_SEC_CERT_DAC_PROVIDER)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/ESP32SecureCertDACProvider.cpp")
    endif()

    if (NOT CONFIG_USE_ESP32_ECDSA_PERIPHERAL)
        list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/platform/ESP32/ESP32CHIPCryptoPAL.cpp")
    endif()
endif()

if (NOT CONFIG_ENABLE_CHIPOBLE)
    list(APPEND EXCLUDE_SRCS_LIST "${MATTER_SDK_PATH}/src/ble/BLEEndPoint.cpp"
                                  "${MATTER_SDK_PATH}/src/ble/BleError.cpp"
                                  "${MATTER_SDK_PATH}/src/ble/BleLayer.cpp"
                                  "${MATTER_SDK_PATH}/src/ble/BleUUID.cpp"
                                  "${MATTER_SDK_PATH}/src/ble/BtpEngine.cpp")
endif()

if(CONFIG_ENABLE_ICD_SERVER)
    list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/app/icd/server/")
endif()

idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                       EXCLUDE_SRCS      ${EXCLUDE_SRCS_LIST}
                       INCLUDE_DIRS      ${INCLUDE_DIRS_LIST}
                       PRIV_INCLUDE_DIRS ${PRIV_INCLUDE_DIRS_LIST}
                       REQUIRES          ${REQUIRE_COMPONENTS})

target_compile_options(${COMPONENT_LIB} PUBLIC
                       "-DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=<lib/address_resolve/AddressResolve_DefaultImpl.h>"
                       "-DCHIP_CONFIG_SOFTWARE_VERSION_NUMBER=${chip_config_software_version_number}")

if (CHIP_PROJECT_CONFIG)
    target_compile_options(${COMPONENT_LIB} PUBLIC
                           "-DCHIP_PROJECT_CONFIG_INCLUDE=${CHIP_PROJECT_CONFIG}"
                           "-DSYSTEM_PROJECT_CONFIG_INCLUDE=${CHIP_PROJECT_CONFIG}")
endif()

idf_build_set_property(COMPILE_OPTIONS "-Wno-error=uninitialized;-Wno-error=maybe-uninitialized;-Wno-format-nonliteral;-Wno-missing-field-initializers;" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=array-bounds" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-write-strings" APPEND)

# For Xtensa chips, uint32_t was defined as 'unsigned' before v5.0, and after IDF v5.0 it is defined
# as 'unsigned long', same as RISC-V. add this compile option to avoid format errors.
# https://github.com/espressif/esp-idf/issues/6906#issuecomment-1207373706
if (CONFIG_IDF_TARGET_ARCH_XTENSA AND (${IDF_VERSION_MAJOR} LESS 5))
    idf_build_set_property(COMPILE_OPTIONS "-Wno-format" APPEND)
endif()

# Time sync client feature depends on read client, disable it when read client is disabled.
if (CONFIG_DISABLE_READ_CLIENT)
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DTIME_SYNC_ENABLE_TSC_FEATURE=0")
endif()

if (CONFIG_USE_MINIMAL_MDNS)
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DCHIP_MINMDNS_DEFAULT_POLICY=1"
                                                   "-DCHIP_MINMDNS_USE_EPHEMERAL_UNICAST_PORT=0"
                                                   "-DCHIP_MINMDNS_HIGH_VERBOSITY=0")
endif()

if (NOT CMAKE_BUILD_EARLY_EXPANSION)
    if (CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE)
        target_compile_options(${COMPONENT_LIB} PUBLIC "-DLWIP_DEBUG=1")
    endif()
endif()

if (NOT CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    target_include_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_BINARY_DIR}/gen/app-codegen/cpp-app"
                                                       "${CMAKE_BINARY_DIR}/gen/app-zapgen/zapgen/app-templates")
endif()

idf_component_get_property(freertos_dir freertos COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE
    "${freertos_dir}/include/freertos"
    "${freertos_dir}/FreeRTOS-Kernel/include/freertos")

target_include_directories(${COMPONENT_LIB} INTERFACE
    "${MATTER_SDK_PATH}/examples/platform/esp32")

#Generate some include files
include(${CMAKE_CURRENT_LIST_DIR}/generate-include-files.cmake)

idf_component_get_property(main_lib main COMPONENT_LIB)
set(chip_libraries $<TARGET_FILE:${main_lib}>)
target_link_libraries(${COMPONENT_LIB} INTERFACE -Wl,--start-group
                                                ${chip_libraries}
                                                -Wl,--end-group)

# TODO: Build OTA image need a python package, leading a file change and distorying the hash of managed_components.
# Build Matter OTA image
if (false AND CONFIG_CHIP_OTA_IMAGE_BUILD)
    chip_ota_image(chip-ota-image
        INPUT_FILES ${BUILD_DIR}/${CMAKE_PROJECT_NAME}.bin
        OUTPUT_FILE ${BUILD_DIR}/${CMAKE_PROJECT_NAME}-ota.bin
    )
    # Adding dependecy as app target so that this runs after images are ready
    add_dependencies(chip-ota-image app)
endif()
