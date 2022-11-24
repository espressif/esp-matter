set(src_dirs_list )
set(include_dirs_list )
set(exclude_srcs_list )
if (CONFIG_ESP_MATTER_CONTROLLER_ENABLE)
    list(APPEND src_dirs_list . logger/zap-generated)
    list(APPEND include_dirs_list . logger)
endif()

if (NOT CONFIG_ESP_MATTER_COMMISSIONER_ENABLE)
    list(APPEND exclude_srcs_list esp_matter_commissioner.cpp esp_matter_controller_pairing_command.cpp)
endif()

idf_component_register(SRC_DIRS ${src_dirs_list}
    EXCLUDE_SRCS ${exclude_srcs_list}
    INCLUDE_DIRS ${include_dirs_list}
    REQUIRES chip esp_matter esp_matter_console json_parser)
