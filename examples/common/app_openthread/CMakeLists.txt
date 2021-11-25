if(CONFIG_OPENTHREAD_ENABLED)
    set(srcs app_openthread.c)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS .
                       PRIV_REQUIRES openthread)
