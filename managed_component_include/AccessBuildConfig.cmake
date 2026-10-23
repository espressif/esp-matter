# Generating access/AccessBuildConfig.h
esp_matter_write_if_changed(${CMAKE_CURRENT_BINARY_DIR}/access/AccessBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n

#ifdef CONFIG_ENABLE_ACCESS_RESTRICTIONS
#define CHIP_CONFIG_USE_ACCESS_RESTRICTIONS 1
#else
#define CHIP_CONFIG_USE_ACCESS_RESTRICTIONS 0
#endif
")
