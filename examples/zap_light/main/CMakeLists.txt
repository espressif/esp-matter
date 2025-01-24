idf_component_register(SRC_DIRS          "."
                                         "${MATTER_SDK_PATH}/src/app/util"
                                         "${MATTER_SDK_PATH}/zzz_generated/app-common/app-common/zap-generated/attributes"
                                         # TODO Remove this from source as the zap doesn't use binding cluster
                                         "${MATTER_SDK_PATH}/src/app/clusters/bindings"
                       PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils")

# We must set CHIP_ROOT to include chip_data_model.cmake
get_filename_component(CHIP_ROOT "${MATTER_SDK_PATH}" REALPATH)
include(${CHIP_ROOT}/src/app/chip_data_model.cmake)
chip_configure_data_model(${COMPONENT_LIB} 
    ZAP_FILE ${CHIP_ROOT}/examples/lighting-app/lighting-common/lighting-app.zap)

add_prebuilt_library(matterlib "${CMAKE_BINARY_DIR}/esp-idf/chip/lib/libCHIP.a"
                     REQUIRES esp_matter main)
target_link_libraries(${COMPONENT_LIB} INTERFACE matterlib)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
