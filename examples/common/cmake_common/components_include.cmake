cmake_minimum_required(VERSION 3.5)

message(IDF_VERSION ": $ENV{IDF_VERSION}; idf version number: ${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
get_filename_component(SDKCONFIG_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig.defaults" ABSOLUTE)
set(SDKCONFIG_FILES "${SDKCONFIG_COMMON_DIR}")

# checking if sdkconfig.defaults.target_device exists
get_filename_component(SDKCONFIG_TARGET "${SDKCONFIG_COMMON_DIR}.${IDF_TARGET}" ABSOLUTE)
if(EXISTS ${SDKCONFIG_TARGET})
    list(APPEND SDKCONFIG_FILES "${SDKCONFIG_TARGET}")
endif()

# checking if IDF_TARGET is esp32c2
if(IDF_TARGET STREQUAL "esp32c2")
    # checking if sdkconfig.defaults.esp32c2.idf_ver exists
    get_filename_component(SDKCONFIG_ESP32C2_IDF_MINVER "${SDKCONFIG_TARGET}.v${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" ABSOLUTE)
    get_filename_component(SDKCONFIG_ESP32C2_IDF_PATVER "${SDKCONFIG_TARGET}.v${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}" ABSOLUTE)
    if(EXISTS ${SDKCONFIG_ESP32C2_IDF_MINVER})
        list(APPEND SDKCONFIG_FILES "${SDKCONFIG_ESP32C2_IDF_MINVER}")
    elseif(EXISTS ${SDKCONFIG_ESP32C2_IDF_PATVER})
        list(APPEND SDKCONFIG_FILES "${SDKCONFIG_ESP32C2_IDF_PATVER}")
    else()
        message(FATAL_ERROR "Can't support ESP32-C2 for this idf version v${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
    endif()
endif()

# setting the final sdkconfig path
message(SDKCONFIG_PATH ": ${SDKCONFIG_FILES}")
list(APPEND SDKCONFIG_DEFAULTS ${SDKCONFIG_FILES})
