# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{ESP_MATTER_PATH})
    message(FATAL_ERROR "Please set ESP_MATTER_PATH to the path of esp-matter repo")
endif(NOT DEFINED ENV{ESP_MATTER_PATH})

if(NOT DEFINED ENV{ESP_MATTER_DEVICE_PATH})
    if("${IDF_TARGET}" STREQUAL "esp32" OR "${IDF_TARGET}" STREQUAL "")
        set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32_devkit_c)
    elseif("${IDF_TARGET}" STREQUAL "esp32c3")
        set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32c3_devkit_m)
    elseif("${IDF_TARGET}" STREQUAL "esp32c2")
        set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32c2_devkit_m)
    elseif("${IDF_TARGET}" STREQUAL "esp32h2")
        set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32h2_devkit_c)
    elseif("${IDF_TARGET}" STREQUAL "esp32s3")
        set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32s3_devkit_c)
    elseif("${IDF_TARGET}" STREQUAL "esp32c6")
        set(ENV{ESP_MATTER_DEVICE_PATH} $ENV{ESP_MATTER_PATH}/device_hal/device/esp32c6_devkit_c)
    else()
        message(FATAL_ERROR "Unsupported IDF_TARGET")
    endif()
endif(NOT DEFINED ENV{ESP_MATTER_DEVICE_PATH})

set(PROJECT_VER "1.0")
set(PROJECT_VER_NUMBER 1)

set(ESP_MATTER_PATH $ENV{ESP_MATTER_PATH})
set(MATTER_SDK_PATH ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip)

# This should be done before using the IDF_TARGET variable.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include($ENV{ESP_MATTER_DEVICE_PATH}/esp_matter_device.cmake)

set(EXTRA_COMPONENT_DIRS
    "${ESP_MATTER_PATH}/examples/common"
    "${MATTER_SDK_PATH}/config/esp32/components"
    "${ESP_MATTER_PATH}/components"
    "${ESP_MATTER_PATH}/device_hal/device"
    ${extra_components_dirs_append})

project(zap_light)
if(CONFIG_IDF_TARGET_ESP32C2)
    include(relinker)
endif()

idf_build_set_property(CXX_COMPILE_OPTIONS "-std=gnu++14;-Os;-DCHIP_HAVE_CONFIG_H" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "-Os" APPEND)
# For RISCV chips, project_include.cmake sets -Wno-format, but does not clear various
# flags that depend on -Wformat
idf_build_set_property(COMPILE_OPTIONS "-Wno-format-nonliteral;-Wno-format-security" APPEND)

# The zap generated files don't define these ENDPOINT_COUNTs
# Set these COUNTs 1 to avoid compilation errors
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_ACCOUNT_LOGIN_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_APPLICATION_BASIC_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_APPLICATION_LAUNCHER_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_AUDIO_OUTPUT_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_CONTENT_LAUNCHER_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_CHANNEL_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_DOOR_LOCK_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_LOW_POWER_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_MEDIA_INPUT_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_KEYPAD_INPUT_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_MEDIA_PLAYBACK_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_OTA_SOFTWARE_UPDATE_PROVIDER_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_TARGET_NAVIGATOR_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_WAKE_ON_LAN_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DEMBER_AF_WINDOW_COVERING_CLUSTER_SERVER_ENDPOINT_COUNT=1" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DMATTER_SCENES_TABLE_SIZE=CONFIG_ESP_MATTER_SCENES_TABLE_SIZE" APPEND)
