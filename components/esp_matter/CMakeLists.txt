set(SRC_DIRS_LIST       "."
                        "client"
                        "client/private"
                        "utils"
                        "utils/jsontlv"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
)

set(INCLUDE_DIRS_LIST   "."
                        "client"
                        "utils"
                        "utils/jsontlv"
                        "data_model_provider"
                        # zap_common holds the app-specific ZAP headers (gen_config.h,
                        # endpoint_config.h) that upstream's util/config.h pulls in. The
                        # unconditionally-built Accessors.cpp reaches config.h via attribute-storage.h
                        # (Matter 1.7 GetDefault accessors), so this must be on the path for every
                        # build, including no-data-model configs like controller and mfg_test_app.
                        "zap_common"
                        "${MATTER_SDK_PATH}/zzz_generated/app-common"
                        "${MATTER_SDK_PATH}/third_party/nlfaultinjection/include"
		                "${MATTER_SDK_PATH}/third_party/uriparser/repo/include"
                        "${MATTER_SDK_PATH}/src")

# Some public cluster headers (e.g. commodity-tariff-server) use pigweed container types and
# the chip component does not export pigweed include paths. These headers are reachable through
# esp_matter's public MATTER_SDK_PATH/src include (e.g. an app delegate that includes the server
# header), so the pigweed paths must be public too, not private.
list(APPEND INCLUDE_DIRS_LIST   "${MATTER_SDK_PATH}/third_party/pigweed/repo/pw_containers/public"
                                "${MATTER_SDK_PATH}/third_party/pigweed/repo/pw_assert/public"
                                "${MATTER_SDK_PATH}/third_party/pigweed/repo/pw_assert/assert_compatibility_public_overrides"
                                "${MATTER_SDK_PATH}/third_party/pigweed/repo/pw_preprocessor/public"
                                "${MATTER_SDK_PATH}/third_party/pigweed/repo/pw_toolchain/public"
                                "${MATTER_SDK_PATH}/third_party/pigweed/repo/pw_polyfill/public")

# data_model/compat carries stand-ins for headers upstream generates per app from ZAP.
set(PRIV_INCLUDE_DIRS_LIST "data_model/compat" "client/private")

if (NOT CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)
    list(APPEND EXCLUDE_SRCS_LIST "esp_matter_identify.cpp"
                                  "esp_matter_ota.cpp")
endif()

if (CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)
    # server, reporting, persistence, and server-cluster are already in libCHIP.a. CHIP_APP_SRCS
    # collects the CHIP files it leaves to the app; they are added after idf_component_register().
    set(CHIP_APP_SRCS "${MATTER_SDK_PATH}/src/app/server/DefaultTermsAndConditionsProvider.cpp"
                      "${MATTER_SDK_PATH}/src/app/server/TermsAndConditionsManager.cpp"
                      "${MATTER_SDK_PATH}/src/app/server/JointFabricAdministrator.cpp"
                      "${MATTER_SDK_PATH}/src/app/server/JointFabricDatastore.cpp"
                      "${MATTER_SDK_PATH}/src/app/server/ThreadRendezvousAnnouncement.cpp"
                      "${MATTER_SDK_PATH}/src/app/persistence/DeferredAttributePersistenceProvider.cpp")
    list(APPEND SRC_DIRS_LIST "client/binding")
    list(APPEND INCLUDE_DIRS_LIST "client/binding")
    if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
        include("utils/cluster_select/cluster_dir.cmake")
        get_supported_cluster_dirs(SUPPORTED_CLUSTER_DIRS)
        set(CHIP_CLUSTERS_DIR "${MATTER_SDK_PATH}/src/app/clusters")

        # CHIP_LIB_CLUSTER_DIRS: dir names from source_set("clusters") public_deps (already in libCHIP.a).
        # Re-run configure when that list changes (e.g. CHIP update).
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CHIP_CLUSTERS_DIR}/BUILD.gn")
        file(READ "${CHIP_CLUSTERS_DIR}/BUILD.gn" CHIP_CLUSTERS_BUILD_GN)
        string(REGEX MATCH "source_set\\(\"clusters\"\\)[^]]*public_deps = \\[([^]]*)\\]" _ "${CHIP_CLUSTERS_BUILD_GN}")
        if (NOT CMAKE_MATCH_1)
            message(FATAL_ERROR "esp_matter: could not read the \"clusters\" group from "
                                "${CHIP_CLUSTERS_DIR}/BUILD.gn; update the parser above")
        endif()
        string(REGEX MATCHALL "\"[a-z0-9-]+\"" CHIP_LIB_CLUSTER_DIRS "${CMAKE_MATCH_1}")
        list(TRANSFORM CHIP_LIB_CLUSTER_DIRS REPLACE "\"" "")

        foreach(CLUSTER_DIR ${SUPPORTED_CLUSTER_DIRS})
            get_filename_component(CLUSTER_NAME "${CLUSTER_DIR}" NAME)
            if (CLUSTER_NAME IN_LIST CHIP_LIB_CLUSTER_DIRS)
                # libCHIP.a leaves out the app-side files CHIP lists in app_config_dependent_sources.cmake.
                # Compile those, except Codegen* (esp_matter has its own integration) and codegen/
                # (added as SRC_DIRS below).
                set(APP_CFG_FILE "${CLUSTER_DIR}/app_config_dependent_sources.cmake")
                if (EXISTS "${APP_CFG_FILE}")
                    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${APP_CFG_FILE}")
                    file(STRINGS "${APP_CFG_FILE}" APP_CFG_SRCS REGEX "\"\\$\\{CLUSTER_DIR\\}/[^\"]+\\.(c|cpp)\"")
                    foreach(APP_CFG_SRC ${APP_CFG_SRCS})
                        string(REGEX REPLACE ".*\"\\$\\{CLUSTER_DIR\\}/([^\"]+)\".*" "\\1" APP_CFG_SRC "${APP_CFG_SRC}")
                        if (NOT APP_CFG_SRC MATCHES "^(Codegen|codegen/)")
                            list(APPEND CHIP_APP_SRCS "${CLUSTER_DIR}/${APP_CFG_SRC}")
                        endif()
                    endforeach()
                endif()
                continue()
            endif()
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

        # Not in any CHIP source list; apps add it themselves.
        if (CONFIG_SUPPORT_GENERAL_DIAGNOSTICS_CLUSTER)
            list(APPEND CHIP_APP_SRCS "${CHIP_CLUSTERS_DIR}/general-diagnostics-server/GenericFaultTestEventTriggerHandler.cpp")
        endif()

        get_supported_esp_matter_cluster_dirs(ESP_MATTER_CLUSTER_DIRS_LIST)
        list(APPEND SRC_DIRS_LIST ${ESP_MATTER_CLUSTER_DIRS_LIST})

        list(APPEND SRC_DIRS_LIST "data_model"
                                  "data_model_provider"
                                  "data_model/private"
                                  "data_model_provider/private")

        if (NOT CONFIG_ESP_MATTER_ENABLE_OPTIONAL_ATTRIBUTES)
            list(APPEND EXCLUDE_SRCS_LIST "data_model/esp_matter_optional_attribute.cpp")
        endif()
        list(APPEND INCLUDE_DIRS_LIST "data_model")
        list(APPEND PRIV_INCLUDE_DIRS_LIST "data_model/private"
                                           "data_model_provider/private")

        if(CONFIG_ESP_MATTER_ENABLE_GENERATED_DATA_MODEL)
            # Weak no-op ESPMatter*ClusterServer{Init,Shutdown}Callback stubs referenced by the
            # generated cluster files; provider integrations override them with strong definitions.
            list(APPEND SRC_DIRS_LIST "zap_common/app")

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
endif(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)

set(REQUIRES_LIST   chip bt nvs_flash app_update esp_secure_cert_mgr mbedtls esp_system openthread espressif__cjson)

idf_component_register( SRC_DIRS        ${SRC_DIRS_LIST}
                        INCLUDE_DIRS    ${INCLUDE_DIRS_LIST}
                        EXCLUDE_SRCS    ${EXCLUDE_SRCS_LIST}
                        PRIV_INCLUDE_DIRS ${PRIV_INCLUDE_DIRS_LIST}
                        REQUIRES        ${REQUIRES_LIST}
                        LDFRAGMENTS     "linker.lf")

# esp_matter_ota.cpp includes esp_rcp_ota.h (from espressif__esp_rcp_update) when the RCP-OTA path
# is enabled. That managed component is pulled in by the example, not esp_matter, and component
# requirements are expanded before Kconfig and before the component graph is final, so we cannot
# gate on CONFIG_* or on BUILD_COMPONENTS at REQUIRES time. idf_component_optional_requires runs
# after registration, when BUILD_COMPONENTS is complete, and links the component only if the app
# actually pulled it in (a no-op for apps like light that don't use RCP OTA).
idf_component_optional_requires(PRIVATE espressif__esp_rcp_update)

# Precompile the shared esp_matter/CHIP headers to avoid reparsing them in every
# source file.
if(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER AND CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    target_precompile_headers(${COMPONENT_LIB} PRIVATE
        <esp_matter.h>
        <esp_matter_core.h>
        <app-common/zap-generated/cluster-enums.h>
        <app-common/zap-generated/callback.h>
    )
endif()

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

# Added here because idf_component_register() ignores SRC_DIRS when SRCS is given.
if (CHIP_APP_SRCS)
    target_sources(${COMPONENT_LIB} PRIVATE ${CHIP_APP_SRCS})
endif()

target_compile_options(${COMPONENT_LIB} PUBLIC
                       "-DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=<lib/address_resolve/AddressResolve_DefaultImpl.h>")

# Time sync client feature depends on read client, disable it when read client is disabled.
if (CONFIG_DISABLE_READ_CLIENT)
    target_compile_options(${COMPONENT_LIB} PUBLIC "-DTIME_SYNC_ENABLE_TSC_FEATURE=0")
endif()
