cmake_minimum_required(VERSION 3.5)

message(IDF_VERSION ": $ENV{IDF_VERSION}; idf version number: ${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
get_filename_component(SDKCONFIG_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig.defaults" ABSOLUTE)

# Sequence of loading default sdkconfig files are based on
# https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html#custom-sdkconfig-defaults
#
# First default file should be sdkconfig.defaults,
# if that is present then sdkconfig.defaults.target will be auto loaded
set(SDKCONFIG_FILES "sdkconfig.defaults")

# Append the configurations externally provided using `SDKCONFIG_DEFAULTS` variable
list(APPEND SDKCONFIG_FILES ${SDKCONFIG_DEFAULTS})

# Then append any C2 specific files
# checking if IDF_TARGET is esp32c2
if(IDF_TARGET STREQUAL "esp32c2")
    get_filename_component(SDKCONFIG_TARGET "${SDKCONFIG_COMMON_DIR}.${IDF_TARGET}" ABSOLUTE)

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

get_filename_component(SDKCONFIG_SHELL_IDF_VER "${SDKCONFIG_COMMON_DIR}.shell.v4.4" ABSOLUTE)
if(EXISTS ${SDKCONFIG_SHELL_IDF_VER} AND ${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH} STRLESS "5.0.0")
    list(APPEND SDKCONFIG_FILES "${SDKCONFIG_SHELL_IDF_VER}")
endif()

# setting the final sdkconfig files list to SDKCONFIG_DEFAULTS
set(SDKCONFIG_DEFAULTS ${SDKCONFIG_FILES})

message("Sequence of SDKCONFIG files: ${SDKCONFIG_DEFAULTS}")
