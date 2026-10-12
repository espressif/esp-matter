# Generating lwip/lwip_buildconfig.h
esp_matter_write_if_changed(${CMAKE_CURRENT_BINARY_DIR}/lwip/lwip_buildconfig.h
"#pragma once\n
#include <sdkconfig.h>\n
")
