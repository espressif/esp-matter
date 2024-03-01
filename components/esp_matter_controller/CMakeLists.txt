set(src_dirs_list )
set(include_dirs_list )
set(exclude_srcs_list )

if (CONFIG_ESP_MATTER_CONTROLLER_ENABLE)
    list(APPEND src_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/logger/zap-generated")
    list(APPEND include_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/logger")

    if (CONFIG_ESP_MATTER_CONTROLLER_CUSTOM_CLUSTER_ENABLE)
        list(APPEND src_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}/controller_custom_cluster")
        list(APPEND include_dirs_list "${CMAKE_CURRENT_SOURCE_DIR}/controller_custom_cluster")
    endif()
    if (NOT CONFIG_ESP_MATTER_COMMISSIONER_ENABLE)
        list(APPEND exclude_srcs_list "${CMAKE_CURRENT_SOURCE_DIR}/esp_matter_commissioner.cpp"
                                      "${CMAKE_CURRENT_SOURCE_DIR}/esp_matter_controller_pairing_command.cpp"
                                      "${CMAKE_CURRENT_SOURCE_DIR}/esp_matter_attestation_trust_store.cpp"
                                      "${CMAKE_CURRENT_SOURCE_DIR}/esp_matter_controller_group_settings.cpp")
    endif()
endif()

idf_component_register(SRC_DIRS ${src_dirs_list}
    EXCLUDE_SRCS ${exclude_srcs_list}
    INCLUDE_DIRS ${include_dirs_list}
    REQUIRES chip esp_matter esp_matter_console json_parser spiffs esp_http_client json_generator)

idf_build_set_property(COMPILE_OPTIONS "-Wno-write-strings" APPEND)
