# Generating minmdns/MinMdnsBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/minmdns/MinMdnsBuildConfig.h
"#pragma once\n
#define CHIP_MINMDNS_USE_EPHEMERAL_UNICAST_PORT 0
#define CHIP_MINMDNS_HIGH_VERBOSITY 0
#define CHIP_MINMDNS_DEFAULT_POLICY 1
")
