if (NOT CONFIG_OPENTHREAD_ENABLED)
    set(src_dirs src)
endif()

idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS include
                       PRIV_INCLUDE_DIRS private_include
                       REQUIRES lwip esp_netif)
