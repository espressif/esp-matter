# Get the list of cluster directories present in ${MATTER_SDK_PATH}/src/app/clusters
file(GLOB CLUSTER_DIR_LIST true ${MATTER_SDK_PATH}/src/app/clusters/*)

set(SRC_DIRS_LIST       "."
                        "private"
                        "utils"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
                        "${MATTER_SDK_PATH}/src/app/server"
                        "${MATTER_SDK_PATH}/src/app/util"
                        "${MATTER_SDK_PATH}/src/app/reporting"
)

foreach(CLUSTER_DIR ${CLUSTER_DIR_LIST})
    file(GLOB_RECURSE C_CPP_FILES "${CLUSTER_DIR}/*.c" "${CLUSTER_DIR}/*.cpp")
    if (C_CPP_FILES)
        list(APPEND SRC_DIRS_LIST "${CLUSTER_DIR}")
    endif()
endforeach()

set(INCLUDE_DIRS_LIST   "."
                        "utils"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common"
                        "${MATTER_SDK_PATH}/third_party/nlfaultinjection/include"
                        "${MATTER_SDK_PATH}/src")

if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    list(APPEND SRC_DIRS_LIST "zap_common/app")
    list(APPEND INCLUDE_DIRS_LIST "zap_common")
endif()

set(REQUIRES_LIST       chip bt esp_matter_console nvs_flash app_update esp_secure_cert_mgr mbedtls esp_system openthread json)

idf_component_register( SRC_DIRS        ${SRC_DIRS_LIST}
                        INCLUDE_DIRS    ${INCLUDE_DIRS_LIST}
                        PRIV_INCLUDE_DIRS "private"
                        REQUIRES        ${REQUIRES_LIST})

# This has been added to fix the error and should be removed once fixed:
# esp-matter/connectedhomeip/connectedhomeip/src/app/EventManagement.cpp:467:23: error: 'writer' is
# used uninitialized in this function
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=uninitialized;-Wno-error=maybe-uninitialized;-Wno-missing-field-initializers;" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=array-bounds" APPEND)

if (NOT CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    target_include_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_BINARY_DIR}/gen/app-codegen/cpp-app"
                                                       "${CMAKE_BINARY_DIR}/gen/app-zapgen/zapgen/app-templates")
endif()

# For Xtensa chips, uint32_t was defined as 'unsigned' before v5.0, and after IDF v5.0 it is defined
# as 'unsigned long', same as RISC-V. add this compile option to avoid format errors.
# https://github.com/espressif/esp-idf/issues/6906#issuecomment-1207373706
if (CONFIG_IDF_TARGET_ARCH_XTENSA AND (${IDF_VERSION_MAJOR} LESS 5))
    idf_build_set_property(COMPILE_OPTIONS "-Wno-format" APPEND)
endif()
target_compile_options(${COMPONENT_LIB} PUBLIC
                       "-DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=<lib/address_resolve/AddressResolve_DefaultImpl.h>")

# Time sync client feature depends on read client, disable it when read client is disabled.
if (CONFIG_DISABLE_READ_CLIENT)
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DTIME_SYNC_ENABLE_TSC_FEATURE=0")
endif()
