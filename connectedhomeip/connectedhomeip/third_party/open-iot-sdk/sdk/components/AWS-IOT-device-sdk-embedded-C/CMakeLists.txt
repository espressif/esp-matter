# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include("${CMAKE_CURRENT_SOURCE_DIR}/aws-iot-device-sdk-embedded-C-fetch.cmake")

# coreMQTT

# Import source and include definitions from the library
include("${aws-iot-device-sdk_SOURCE_DIR}/libraries/standard/coreMQTT/mqttFilePaths.cmake")

add_library(coreMQTT)

target_include_directories(coreMQTT
    PUBLIC
        ${MQTT_INCLUDE_PUBLIC_DIRS}
)

target_sources(coreMQTT
    PRIVATE
        ${MQTT_SOURCES}
        ${MQTT_SERIALIZER_SOURCES}
)

# Add empty config library. The application must either use the default
# configuration by providing the MQTT_DO_NOT_USE_CUSTOM_CONFIG or provide a
# custom config file named core_mqtt_config.h.
add_library(coreMQTT-config INTERFACE)

target_link_libraries(coreMQTT
    PUBLIC
        coreMQTT-config
)

# coreHTTP

# Import source and include definitions from the library
include("${aws-iot-device-sdk_SOURCE_DIR}/libraries/standard/coreHTTP/httpFilePaths.cmake")

add_library(coreHTTP)

target_include_directories(coreHTTP
    PUBLIC
        ${HTTP_INCLUDE_PUBLIC_DIRS}
)

target_sources(coreHTTP
    PRIVATE
        ${HTTP_SOURCES}
)

# Add empty config library. The application must either use the default
# configuration by providing the HTTP_DO_NOT_USE_CUSTOM_CONFIG or provide a
# custom config file named core_htto_config.h.
add_library(coreHTTP-config INTERFACE)

target_link_libraries(coreHTTP
    PUBLIC
        coreHTTP-config
)


# coreJSON
include("${aws-iot-device-sdk_SOURCE_DIR}/libraries/standard/coreJSON/jsonFilePaths.cmake")

add_library(coreJSON)

target_include_directories(coreJSON
    PUBLIC
        ${JSON_INCLUDE_PUBLIC_DIRS}
)

target_sources(coreJSON
    PRIVATE
        ${JSON_SOURCES}
)


# backoff_algorithm
include("${aws-iot-device-sdk_SOURCE_DIR}/libraries/standard/backoffAlgorithm/backoffAlgorithmFilePaths.cmake")

add_library(backoffAlgorithm)

target_include_directories(backoffAlgorithm
    PUBLIC
        ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS}
)

target_sources(backoffAlgorithm
    PRIVATE
        ${BACKOFF_ALGORITHM_SOURCES}
)

add_subdirectory(AwsNetworkManager)
