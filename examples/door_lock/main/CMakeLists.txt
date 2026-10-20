set(src_dirs "." "lock" "lock/storage")

if(CONFIG_ENABLE_ALIRO_OVER_NFC)
    list(APPEND src_dirs "lock/aliro")
endif()

if(CONFIG_ENABLE_TIME_SYNC)
    list(APPEND src_dirs "lock/time")
endif()

if(CONFIG_ENABLE_CHIP_SHELL)
    list(APPEND src_dirs "lock/console")
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                       PRIV_INCLUDE_DIRS
                         ${src_dirs}
                         "${ESP_MATTER_PATH}/examples/common/utils")

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u retain_ember_door_lock_callbacks")
