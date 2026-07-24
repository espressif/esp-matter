# Generating setup_payload/CHIPAdditionalDataPayloadBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/setup_payload/CHIPAdditionalDataPayloadBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#ifdef CONFIG_ENABLE_ROTATING_DEVICE_ID
#define CHIP_ENABLE_ROTATING_DEVICE_ID 1
#else
#define CHIP_ENABLE_ROTATING_DEVICE_ID 0
#endif
")
