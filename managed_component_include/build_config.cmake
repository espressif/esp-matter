# Generating matter/tracing/build_config.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/matter/tracing/build_config.h
"#pragma once\n
#include <sdkconfig.h>\n
#ifdef CONFIG_CHIP_ENABLE_ESP_DIAGNOSTICS
#define MATTER_TRACING_ENABLED 1
#else
#define MATTER_TRACING_ENABLED 0
#endif
")
