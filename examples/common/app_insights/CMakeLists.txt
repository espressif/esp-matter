# App Insights

if(CONFIG_ESP_MATTER_ENABLE_INSIGHTS)
    set(srcs app_insights.c)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS .
                    PRIV_REQUIRES )

if(CONFIG_ESP_MATTER_ENABLE_INSIGHTS)
    idf_component_get_property(esp_insights_lib esp_insights COMPONENT_LIB)
    idf_component_get_property(esp_diag_lib esp_diagnostics COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PRIVATE ${esp_insights_lib})
    target_link_libraries(${COMPONENT_LIB} PRIVATE ${esp_diag_lib})
endif()
