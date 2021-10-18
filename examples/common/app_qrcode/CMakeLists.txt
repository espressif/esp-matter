set(priv_requires_list chip bt esp32_mbedtls qrcode)

if ("${IDF_TARGET}" STREQUAL "esp32h2")
    list(APPEND priv_requires_list openthread)
endif()
idf_component_register(SRCS app_qrcode.cpp
                    INCLUDE_DIRS .
                    PRIV_REQUIRES ${priv_requires_list})
