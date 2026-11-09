# Copyright 2026 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Replaces the GN chip component. Examples list this directory later in
# EXTRA_COMPONENT_DIRS, and IDF keeps the last component of a given name.
# `gn gen` still chooses the sources; this component compiles them.

set(chip_gn_component_dir "${MATTER_SDK_PATH}/config/esp32/components/chip")

# Same requirements as the GN chip component.
idf_component_register(SRCS          "${chip_gn_component_dir}/chip.c"
                                     "${chip_gn_component_dir}/chip.cpp"
                       REQUIRES      lwip freertos console bt mdns
                       PRIV_REQUIRES esp_eth mbedtls fatfs app_update openthread nvs_flash spi_flash)

# IDF reads requirements before MATTER_SDK_PATH is set.
if (CMAKE_BUILD_EARLY_EXPANSION)
    return()
endif()

if (CONFIG_ENABLE_PW_RPC OR CONFIG_BUILD_CHIP_TESTS)
    message(FATAL_ERROR "chip: pw_rpc and CHIP tests need the GN-based chip component; list "
                        "${MATTER_SDK_PATH}/config/esp32/components after ${ESP_MATTER_PATH}/components "
                        "in EXTRA_COMPONENT_DIRS")
endif()

include(${chip_gn_component_dir}/ota-image.cmake)

# Copy the GN component's argument block so both stay in sync.
file(READ "${chip_gn_component_dir}/CMakeLists.txt" chip_gn_cmake)
string(FIND "${chip_gn_cmake}" "set(chip_gn_args" chip_gn_args_begin)
string(FIND "${chip_gn_cmake}" "set(args_gn_input" chip_gn_args_end)
if (chip_gn_args_begin EQUAL -1 OR chip_gn_args_end EQUAL -1)
    message(FATAL_ERROR "chip: GN arguments not found in ${chip_gn_component_dir}/CMakeLists.txt")
endif()
math(EXPR chip_gn_args_length "${chip_gn_args_end} - ${chip_gn_args_begin}")
string(SUBSTRING "${chip_gn_cmake}" ${chip_gn_args_begin} ${chip_gn_args_length} chip_gn_args_cmake)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/chip_gn_args.cmake" "${chip_gn_args_cmake}")

# Names that argument block reads.
set(CHIP_ROOT "${MATTER_SDK_PATH}")
if (CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE)
    set(is_debug FALSE)
else()
    set(is_debug TRUE)
endif()
include("${CMAKE_CURRENT_BINARY_DIR}/chip_gn_args.cmake")

# GN only forwards these flags; this component uses IDF's.
string(APPEND chip_gn_args "target_cflags_c = []\ntarget_cflags_cc = []\n")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/args.gn" "${chip_gn_args}")

find_program(GN_EXECUTABLE gn)
if (NOT GN_EXECUTABLE)
    message(FATAL_ERROR "chip: 'gn' not found; set up the environment as described in BUILDING.md")
endif()

set(chip_gn_gen_args gen --check)
if (NOT CHIP_CODEGEN_PREGEN_DIR)
    list(APPEND chip_gn_gen_args --fail-on-unused-args)
endif()
execute_process(COMMAND ${GN_EXECUTABLE} --root=${MATTER_SDK_PATH}/config/esp32 ${chip_gn_gen_args}
                        ${CMAKE_CURRENT_BINARY_DIR}
                RESULT_VARIABLE chip_result)
if (NOT chip_result EQUAL 0)
    message(FATAL_ERROR "chip: gn gen failed")
endif()

set(chip_gn_libraries "lib/libCHIP.a")
if (CONFIG_CHIP_ENABLE_ESP_DIAGNOSTICS)
    list(APPEND chip_gn_libraries "lib/libEsp32DiagnosticsBackend.a")
endif()
idf_build_get_property(python PYTHON)
execute_process(COMMAND ${python} ${CMAKE_CURRENT_LIST_DIR}/gn_sources.py ${CMAKE_CURRENT_BINARY_DIR}
                        ${CMAKE_CURRENT_BINARY_DIR}/chip_gn_sources.cmake ${chip_gn_libraries}
                RESULT_VARIABLE chip_result)
if (NOT chip_result EQUAL 0)
    message(FATAL_ERROR "chip: failed to export the CHIP sources from the GN build plan")
endif()

include("${CMAKE_CURRENT_BINARY_DIR}/chip_gn_sources.cmake")  # CHIP_GN_SRCS, CHIP_GN_DEPENDS, CHIP_GN_GENERATED_*
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
             ${CHIP_GN_DEPENDS}
             "${chip_gn_component_dir}/CMakeLists.txt"
             "${CMAKE_CURRENT_LIST_DIR}/gn_sources.py")

if (CHIP_GN_GENERATED_TARGETS)
    # Check the CHIP generated headers on every build and regenerate missing or stale
    # ones. Declaring the outputs also orders consumers in components that depend on chip.
    add_custom_target(chip_codegen
        COMMAND ninja -C "${CMAKE_CURRENT_BINARY_DIR}" --quiet ${CHIP_GN_GENERATED_TARGETS}
        BYPRODUCTS ${CHIP_GN_GENERATED_FILES}
        VERBATIM)
    add_dependencies(${COMPONENT_LIB} chip_codegen)
endif()

target_sources(${COMPONENT_LIB} PRIVATE ${CHIP_GN_SRCS})
# Name the esp_matter linker fragment looks for.
set_target_properties(${COMPONENT_LIB} PROPERTIES OUTPUT_NAME "CHIP")
# `matterlib` is created in project_include.cmake. Apps like zap_light used to create it
# with `REQUIRES esp_matter main`, which puts those libraries after libCHIP when linking.
target_link_libraries(matterlib INTERFACE ${COMPONENT_LIB})
idf_build_get_property(build_components BUILD_COMPONENTS)
foreach(req esp_matter main)
    if(req IN_LIST build_components)
        idf_component_get_property(req_lib ${req} COMPONENT_LIB)
        target_link_libraries(matterlib INTERFACE ${req_lib})
    endif()
endforeach()

# Warnings GN accepts that IDF's -Werror does not.
target_compile_options(${COMPONENT_LIB} PRIVATE
                       "-Wno-error=uninitialized" "-Wno-error=maybe-uninitialized"
                       "-Wno-missing-field-initializers" "-Wno-error=array-bounds"
                       "-Wno-format-nonliteral" "-Wno-format-security" "-Wno-write-strings"
                       "-Wno-error=deprecated-declarations" "-Wno-cast-function-type" "-Wno-psabi")

# Same include paths and optional requirements as the GN component.
idf_component_get_property(freertos_dir freertos COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE
                           "${freertos_dir}/include/freertos"
                           "${freertos_dir}/FreeRTOS-Kernel/include/freertos")
if (CONFIG_CHIP_USE_OT_ENDPOINT)
    idf_component_get_property(openthread_dir openthread COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${openthread_dir}/private_include")
endif()

target_include_directories(${COMPONENT_LIB} PUBLIC
                           "${MATTER_SDK_PATH}/src/platform/ESP32"
                           "${MATTER_SDK_PATH}/src/platform/OpenThread"
                           "${MATTER_SDK_PATH}/src/include"
                           "${MATTER_SDK_PATH}/src/lib"
                           "${MATTER_SDK_PATH}/src"
                           "${MATTER_SDK_PATH}/zzz_generated/app-common"
                           "${MATTER_SDK_PATH}/examples/platform/esp32"
                           "${MATTER_SDK_PATH}/third_party/nlassert/repo/include"
                           "${MATTER_SDK_PATH}/third_party/nlio/repo/include"
                           "${CMAKE_CURRENT_BINARY_DIR}/src/include"
                           "${CMAKE_CURRENT_BINARY_DIR}/include"
                           "${CMAKE_CURRENT_BINARY_DIR}/gen/include"
                           "${MATTER_SDK_PATH}/config/esp32/${CONFIG_CHIP_EXTERNAL_PLATFORM_DIR}"
                           "${MATTER_SDK_PATH}/config/esp32/${CONFIG_CHIP_EXTERNAL_PLATFORM_DIR}/../../")

if (CONFIG_SEC_CERT_DAC_PROVIDER)
    idf_component_optional_requires(PUBLIC espressif__esp_secure_cert_mgr)
endif()
if (CONFIG_USE_ESP32_TEE_DAC_KEY_PBKDF2)
    idf_component_get_property(tee_sec_storage_dir tee_sec_storage COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${tee_sec_storage_dir}/include")
    idf_component_optional_requires(PUBLIC tee_sec_storage)
endif()
if (CONFIG_ENABLE_ENCRYPTED_OTA)
    idf_component_optional_requires(PUBLIC espressif__esp_encrypted_img)
endif()
if (CONFIG_ENABLE_DELTA_OTA)
    idf_component_optional_requires(PUBLIC espressif__esp_delta_ota)
endif()

if (CONFIG_CHIP_OTA_IMAGE_BUILD)
    if (CONFIG_OPENTHREAD_BORDER_ROUTER AND CONFIG_AUTO_UPDATE_RCP)
        chip_ota_image(chip-ota-image
                       INPUT_FILES ${BUILD_DIR}/ota_with_rcp_image
                       OUTPUT_FILE ${BUILD_DIR}/ota_with_rcp_image-ota.bin)
        add_dependencies(chip-ota-image gen_ota_image)
    else()
        chip_ota_image(chip-ota-image
                       INPUT_FILES ${BUILD_DIR}/${CMAKE_PROJECT_NAME}.bin
                       OUTPUT_FILE ${BUILD_DIR}/${CMAKE_PROJECT_NAME}-ota.bin)
        add_dependencies(chip-ota-image app)
    endif()
endif()
