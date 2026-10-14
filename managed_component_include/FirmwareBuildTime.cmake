# Generating FirmwareBuildTime.h
execute_process(
    COMMAND date "+%s"
    OUTPUT_VARIABLE BUILD_SECONDS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
math(EXPR BUILD_SECONDS "${BUILD_SECONDS} - (946684800)")
# Not using esp_matter_write_if_changed: the timestamp changes on every
# configure. Only ConfigurationManagerImpl.cpp includes this header.
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/FirmwareBuildTime.h
"#pragma once\n
#define CHIP_DEVICE_CONFIG_FIRMWARE_BUILD_TIME_MATTER_EPOCH_S ${BUILD_SECONDS}
")
