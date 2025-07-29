set(src_dirs_list )
set(include_dirs_list )

if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    list(APPEND src_dirs_list "${CMAKE_CURRENT_LIST_DIR}")
    list(APPEND include_dirs_list "${CMAKE_CURRENT_LIST_DIR}")
endif()
idf_component_register(SRC_DIRS        ${src_dirs_list}
                       INCLUDE_DIRS    ${include_dirs_list}
                       REQUIRES        esp_matter)
