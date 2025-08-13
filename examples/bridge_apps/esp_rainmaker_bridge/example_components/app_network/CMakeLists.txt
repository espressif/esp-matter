set(priv_req qrcode nvs_flash esp_event rmaker_common vfs network_provisioning openthread)

idf_component_register(SRCS "app_wifi_internal.c" "app_network.c"
                    INCLUDE_DIRS "."
                    PRIV_INCLUDE_DIRS "private_include"
                    PRIV_REQUIRES ${priv_req})

if(CONFIG_APP_WIFI_SHOW_DEMO_INTRO_TEXT)
    target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-D RMAKER_DEMO_PROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")
endif()
