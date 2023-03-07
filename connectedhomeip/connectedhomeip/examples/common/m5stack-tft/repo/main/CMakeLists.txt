set(SOURCES tft_demo.c)
idf_component_register(
        SRCS ${SOURCES}
        REQUIRES
            tft
            spiffs
        PRIV_REQUIRES
            nvs_flash
)
