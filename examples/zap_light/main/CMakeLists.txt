set(PRIV_REQUIRES_LIST device esp_matter esp_matter_console app_reset)

idf_component_register(SRC_DIRS          "."
                      PRIV_INCLUDE_DIRS  "."
                      PRIV_REQUIRES      ${PRIV_REQUIRES_LIST})

# We must set CHIP_ROOT to include esp32_codegen.cmake
get_filename_component(CHIP_ROOT "${MATTER_SDK_PATH}" REALPATH)
include("${CHIP_ROOT}/build/chip/esp32/esp32_codegen.cmake")
chip_app_component_codegen("${CHIP_ROOT}/examples/lighting-app/lighting-common/lighting-app.matter")
chip_app_component_zapgen("${CHIP_ROOT}/examples/lighting-app/lighting-common/lighting-app.zap")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 14)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
