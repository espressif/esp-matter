# Generating lwip/lwip_buildconfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lwip/lwip_buildconfig.h
"#pragma once\n
#include <sdkconfig.h>\n
")
