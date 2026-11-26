set(srcs_list )

if (CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    list(APPEND srcs_list "bridge_persistence.cpp")
endif()

# cluster_lifecycle_common.h has the helpers to create the node and start Matter in the unit test app.
idf_component_register(SRCS ${srcs_list}
                       PRIV_INCLUDE_DIRS "../../esp_matter/test"
                       REQUIRES unity esp_matter esp_matter_bridge nvs_flash)
