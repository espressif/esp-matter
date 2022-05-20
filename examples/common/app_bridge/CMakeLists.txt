if(CONFIG_ZB_ENABLED)
    set(srcs app_zigbee_bridge_device.cpp)
endif()

idf_component_register(SRCS         "${srcs}"
                       INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}"
                       REQUIRES      esp_matter_bridge)
