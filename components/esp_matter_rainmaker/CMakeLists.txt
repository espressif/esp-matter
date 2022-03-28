set(SRCS_LIST )
set(REQUIRES_LIST       esp_matter)

idf_build_get_property(rainmaker_enabled RAINMAKER_ENABLED)
if (${rainmaker_enabled})
    list(APPEND SRCS_LIST       esp_matter_rainmaker.cpp)
    list(APPEND REQUIRES_LIST   esp_rainmaker)
endif()

idf_component_register(SRCS             ${SRCS_LIST}
                       INCLUDE_DIRS     "."
                       REQUIRES         ${REQUIRES_LIST})
