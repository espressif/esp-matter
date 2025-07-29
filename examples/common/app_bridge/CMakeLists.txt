set(srcs_list )
set(include_dirs_list )
if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    list(APPEND srcs_list "app_bridged_device.cpp")
    list(APPEND include_dirs_list "${CMAKE_CURRENT_LIST_DIR}")
endif()

idf_component_register(SRCS         ${srcs_list}
                       INCLUDE_DIRS ${include_dirs_list}
                       REQUIRES      esp_matter_bridge)
