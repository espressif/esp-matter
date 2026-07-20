set(EXCLUDE_SRCS_LIST)

if (NOT CONFIG_ENABLE_ALIRO_OVER_NFC)
    list(APPEND EXCLUDE_SRCS_LIST "lock/aliro_door_lock_delegate.cpp")
endif()

idf_component_register(SRC_DIRS "." "./lock"
                       EXCLUDE_SRCS ${EXCLUDE_SRCS_LIST}
                       PRIV_INCLUDE_DIRS
                         "." "./lock" "${ESP_MATTER_PATH}/examples/common/utils")

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
