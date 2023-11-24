set(SRCS_LIST )

if (CONFIG_OPENTHREAD_BORDER_ROUTER)
    list(APPEND SRCS_LIST "esp_matter_thread_br_launcher.cpp")
if (CONFIG_ENABLE_CHIP_SHELL AND CONFIG_OPENTHREAD_CLI)
    list(APPEND SRCS_LIST "esp_matter_thread_br_console.cpp")
endif()
endif()

idf_component_register(SRCS             ${SRCS_LIST}
                       INCLUDE_DIRS     "."
                       REQUIRES         esp_matter openthread esp_netif vfs)
