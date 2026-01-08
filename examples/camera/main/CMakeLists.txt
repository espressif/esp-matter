set(SRC_DIRS_LIST "." "clusters" "common" "webrtc")
set(PRIV_INCLUDE_DIRS_LIST "." "clusters" "common" "webrtc")

idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                       PRIV_INCLUDE_DIRS  ${PRIV_INCLUDE_DIRS_LIST} "${ESP_MATTER_PATH}/examples/common/utils")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
