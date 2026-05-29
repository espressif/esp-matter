list(APPEND srcs_list "attribute_get_val.cpp")
list(APPEND srcs_list "attribute_create_value_persistence.cpp")
list(APPEND srcs_list "attribute_get_val_type.cpp")
list(APPEND srcs_list "attribute_report.cpp")
list(APPEND srcs_list "cluster_lifecycle_basic.cpp")
list(APPEND srcs_list "cluster_lifecycle_managed_delegate.cpp")
list(APPEND srcs_list "jsontlv.cpp")

idf_component_register(SRCS ${srcs_list}
                       INCLUDE_DIRS "."
                       REQUIRES unity esp_matter)
