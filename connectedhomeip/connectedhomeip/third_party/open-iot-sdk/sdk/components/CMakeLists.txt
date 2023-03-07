# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Components

add_subdirectory(Storage)

include(${CMAKE_CURRENT_LIST_DIR}/MCU-Driver-HAL/MCU-Driver-HAL.cmake)

if(IOTSDK_CMSIS_RTOS_API OR IOTSDK_CMSIS_RTX OR IOTSDK_CMSIS_DEVICE)
    add_subdirectory(CMSIS_5)
endif()

if(IOTSDK_FREERTOS)
    add_subdirectory(FreeRTOS-Kernel)
endif()

add_subdirectory(azure-rtos)

add_subdirectory(Connectivity)

if(IOTSDK_TFM)
    add_subdirectory(TF-M)
endif()

if(IOTSDK_ML)
    include(${CMAKE_CURRENT_LIST_DIR}/ML-embedded-evaluation-kit/ML-embedded-evaluation-kit.cmake)
endif()

if(IOTSDK_TINYUSB)
    add_subdirectory(TinyUSB)
endif()

if(IOTSDK_NIMBLE)
    add_subdirectory(NimBLE)
endif()


add_subdirectory(Pigweed)


if(IOTSDK_MBEDTLS)
    add_subdirectory(mbedtls)
endif()

if(IOTSDK_AWS_CLIENT)
    add_subdirectory(AWS-IOT-device-sdk-embedded-C)
endif()

add_subdirectory(Azure-SDK)
