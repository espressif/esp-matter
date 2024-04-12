set(src_dirs_list )
set(include_dirs_list )
set(exclude_srcs_list )
set(requires_list chip esp_matter esp_matter_console spiffs)

if (CONFIG_ESP_MATTER_CONTROLLER_ENABLE)
    list(APPEND src_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}/core"
                              "${CMAKE_CURRENT_SOURCE_DIR}/commands"
                              "${CMAKE_CURRENT_SOURCE_DIR}/logger/zap-generated")
    list(APPEND include_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}/core"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/commands"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/logger")

    if (CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER)
        list(APPEND exclude_srcs_list "${CMAKE_CURRENT_SOURCE_DIR}/core/esp_matter_controller_client.cpp"
                                      "${CMAKE_CURRENT_SOURCE_DIR}/core/esp_matter_controller_credentials_issuer.cpp"
                                      "${CMAKE_CURRENT_SOURCE_DIR}/core/esp_matter_controller_group_settings.cpp")
    endif()

    if (CONFIG_ESP_MATTER_COMMISSIONER_ENABLE)
        list(APPEND src_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}/attestation_store")
        list(APPEND include_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}/attestation_store")
    else()
        list(APPEND exclude_srcs_list "${CMAKE_CURRENT_SOURCE_DIR}/commands/esp_matter_controller_pairing_command.cpp")
    endif()
endif()

idf_component_register(SRC_DIRS ${src_dirs_list}
    EXCLUDE_SRCS ${exclude_srcs_list}
    INCLUDE_DIRS ${include_dirs_list}
    REQUIRES ${requires_list})

idf_build_set_property(COMPILE_OPTIONS "-Wno-write-strings" APPEND)
