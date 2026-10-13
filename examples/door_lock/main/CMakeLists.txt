set(EXCLUDE_SRCS_LIST)
set(PRIV_REQUIRES_LIST)

if (NOT CONFIG_ENABLE_ALIRO_OVER_NFC)
    list(APPEND EXCLUDE_SRCS_LIST "lock/aliro_door_lock_delegate.cpp")
endif()

if (NOT CONFIG_ENABLE_CHIP_SHELL)
    list(APPEND EXCLUDE_SRCS_LIST "lock/door_lock_console.cpp")
endif()

if (CONFIG_ENABLE_RFID_NFC)
    list(APPEND PRIV_REQUIRES_LIST "rfid_nfc_reader")
else()
    list(APPEND EXCLUDE_SRCS_LIST "lock/rfid_door_lock_handler.cpp")
endif()

idf_component_register(SRC_DIRS "." "./lock"
                       EXCLUDE_SRCS ${EXCLUDE_SRCS_LIST}
                       PRIV_INCLUDE_DIRS
                         "." "./lock" "${ESP_MATTER_PATH}/examples/common/utils"
                       PRIV_REQUIRES ${PRIV_REQUIRES_LIST})

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u retain_ember_door_lock_callbacks")
