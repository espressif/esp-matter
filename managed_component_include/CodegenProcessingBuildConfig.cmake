# Generating codegen/CodegenProcessingBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/codegen/CodegenProcessingBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#define CHIP_CODEGEN_CONFIG_ENABLE_CODEGEN_INTEGRATION_LOOKUP_ERRORS 0
")
