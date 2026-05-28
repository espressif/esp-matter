set(SRC_DIRS_LIST       "."
                        "utils"
                        "utils/jsontlv"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
)

set(INCLUDE_DIRS_LIST   "."
                        "utils"
                        "utils/jsontlv"
                        "data_model_provider"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common"
                        "${MATTER_SDK_PATH}/third_party/nlfaultinjection/include"
		                "${MATTER_SDK_PATH}/third_party/uriparser/repo/include"
                        "${MATTER_SDK_PATH}/src")

set(PRIV_INCLUDE_DIRS_LIST )

if (CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)
    list(APPEND SRC_DIRS_LIST    "${MATTER_SDK_PATH}/src/app/server"
                                 "${MATTER_SDK_PATH}/src/app/reporting"
                                 "${MATTER_SDK_PATH}/src/app/persistence"
                                 "${MATTER_SDK_PATH}/src/app/server-cluster")
    if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
        include("utils/cluster_select/cluster_dir.cmake")
        get_supported_cluster_dirs(SUPPORTED_CLUSTER_DIRS)
        foreach(CLUSTER_DIR ${SUPPORTED_CLUSTER_DIRS})
            # If the cluster directory has .c/.cpp file, add it to SRC_DIRS_LIST
            file(GLOB C_CPP_FILES "${CLUSTER_DIR}/*.c" "${CLUSTER_DIR}/*.cpp")
            if (C_CPP_FILES)
                list(APPEND SRC_DIRS_LIST "${CLUSTER_DIR}")
            endif()
            file(GLOB_RECURSE CODEGEN_FILES "${CLUSTER_DIR}/Codegen*.cpp")
            if (CODEGEN_FILES)
                list(APPEND EXCLUDE_SRCS_LIST ${CODEGEN_FILES})
            endif()
        endforeach()

        list(APPEND SRC_DIRS_LIST "${MATTER_SDK_PATH}/src/app/clusters/on-off-server/codegen"
                                  "${MATTER_SDK_PATH}/src/app/clusters/level-control/codegen")

        file(GLOB ESP_MATTER_CLUSTER_DIRS_LIST "data_model_provider/clusters/*")
        foreach(ESP_MATTER_CLUSTER_DIR ${ESP_MATTER_CLUSTER_DIRS_LIST})
            list(APPEND SRC_DIRS_LIST ${ESP_MATTER_CLUSTER_DIR})
        endforeach()

        list(APPEND SRC_DIRS_LIST "data_model"
                                  "data_model_provider"
                                  "data_model/private"
                                  "data_model_provider/private")

        if (NOT CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES)
            list(APPEND EXCLUDE_SRCS_LIST "data_model/esp_matter_optional_attribute.cpp")
        endif()
        list(APPEND INCLUDE_DIRS_LIST "zap_common"
                                      "data_model")
        list(APPEND PRIV_INCLUDE_DIRS_LIST "data_model/private"
                                           "data_model_provider/private")

        if(CONFIG_ESP_MATTER_ENABLE_GENERATED_DATA_MODEL)
            foreach(GENERATED_DATA_MODEL_PATH "device_types" "clusters")
                list(APPEND SRC_DIRS_LIST "data_model/generated/${GENERATED_DATA_MODEL_PATH}")
                list(APPEND INCLUDE_DIRS_LIST "data_model/generated/${GENERATED_DATA_MODEL_PATH}")

                file(GLOB DIRS CONFIGURE_DEPENDS "data_model/generated/${GENERATED_DATA_MODEL_PATH}/*")
                foreach(DIR ${DIRS})
                    if(IS_DIRECTORY ${DIR})
                        list(APPEND SRC_DIRS_LIST ${DIR})
                        list(APPEND INCLUDE_DIRS_LIST ${DIR})
                    endif()
                endforeach()
            endforeach()

            list(APPEND PRIV_INCLUDE_DIRS_LIST "data_model/generated")
            list(APPEND SRC_DIRS_LIST "data_model/generated/")
        else()
            list(APPEND SRC_DIRS_LIST "data_model/legacy")
            list(APPEND INCLUDE_DIRS_LIST "data_model/legacy")
        endif()
    endif(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
else()
    list(APPEND EXCLUDE_SRCS_LIST "esp_matter_identify.cpp"
                                  "esp_matter_ota.cpp")
endif(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)



set(REQUIRES_LIST       chip bt esp_matter_console nvs_flash app_update esp_secure_cert_mgr mbedtls esp_system openthread json)

set(PRIV_REQUIRES_LIST "")
if (CONFIG_OPENTHREAD_BORDER_ROUTER AND CONFIG_AUTO_UPDATE_RCP AND CONFIG_ENABLE_OTA_REQUESTOR)
    list(APPEND PRIV_REQUIRES_LIST esp_rcp_update)
endif()

idf_component_register( SRC_DIRS        ${SRC_DIRS_LIST}
                        INCLUDE_DIRS    ${INCLUDE_DIRS_LIST}
                        EXCLUDE_SRCS    ${EXCLUDE_SRCS_LIST}
                        PRIV_INCLUDE_DIRS ${PRIV_INCLUDE_DIRS_LIST}
                        PRIV_REQUIRES   ${PRIV_REQUIRES_LIST}
                        REQUIRES        ${REQUIRES_LIST})

# This has been added to fix the error and should be removed once fixed:
# esp-matter/connectedhomeip/connectedhomeip/src/app/EventManagement.cpp:467:23: error: 'writer' is
# used uninitialized in this function
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=uninitialized;-Wno-error=maybe-uninitialized;-Wno-missing-field-initializers;" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wno-error=array-bounds" APPEND)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wstringop-truncation" "-Wstringop-overflow")

if (CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)
    if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u data_model_utils_impl")
    else()
        target_include_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_BINARY_DIR}/gen/__idf_main-codegen/cpp-app"
                                                           "${CMAKE_BINARY_DIR}/gen/__idf_main-zapgen/zapgen/app-templates")
    endif(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
endif(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)

target_sources(${COMPONENT_LIB} PRIVATE ${MATTER_SDK_PATH}/src/app/StorageDelegateWrapper.cpp
                                        ${MATTER_SDK_PATH}/src/app/SafeAttributePersistenceProvider.cpp)

target_compile_options(${COMPONENT_LIB} PUBLIC
                       "-DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=<lib/address_resolve/AddressResolve_DefaultImpl.h>")

# Time sync client feature depends on read client, disable it when read client is disabled.
if (CONFIG_DISABLE_READ_CLIENT)
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DTIME_SYNC_ENABLE_TSC_FEATURE=0")
endif()
