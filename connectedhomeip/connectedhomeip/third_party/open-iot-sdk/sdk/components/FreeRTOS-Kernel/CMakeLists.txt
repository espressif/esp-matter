# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(iotsdk_freertos LANGUAGES C CXX ASM)

include(FreeRTOS-fetch.cmake)

# FreeRTOS kernel

add_library(freertos-kernel INTERFACE)

target_sources(freertos-kernel
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/croutine.c
        ${freertos-kernel_SOURCE_DIR}/Source/event_groups.c
        ${freertos-kernel_SOURCE_DIR}/Source/list.c
        ${freertos-kernel_SOURCE_DIR}/Source/queue.c
        ${freertos-kernel_SOURCE_DIR}/Source/stream_buffer.c
        ${freertos-kernel_SOURCE_DIR}/Source/tasks.c
        ${freertos-kernel_SOURCE_DIR}/Source/timers.c
)

target_include_directories(freertos-kernel
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/include
)

# FreeRTOS porting layer. Note: ARMClang and GCC both use the GCC port
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m55 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m85)
    # M55 support uses the M33 port in FreeRTOS. Currently we only support non secure mode.
    # TODO: Add config to enable trustzone support.
    target_sources(freertos-kernel
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
            ${freertos-kernel_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
    )
    target_include_directories(freertos-kernel
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure
    )
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m4)
    target_compile_definitions(freertos-kernel INTERFACE CPU_CORTEX_M4)
    target_sources(freertos-kernel
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F/port.c
    )
    target_include_directories(freertos-kernel
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F
    )
endif()

# Enable FreeRTOS TF-M secure services support.
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m33 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m55 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m85)
    add_library(freertos-tfm-os-wrapper INTERFACE)
    target_sources(freertos-tfm-os-wrapper
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/Source/portable/ThirdParty/GCC/ARM_CM33_TFM/os_wrapper_freertos.c
    )
endif()

# Allow user to select a heap implementation.
add_library(freertos-kernel-heap-1 INTERFACE)
target_sources(freertos-kernel-heap-1
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/portable/MemMang/heap_1.c
)

add_library(freertos-kernel-heap-2 INTERFACE)
target_sources(freertos-kernel-heap-2
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/portable/MemMang/heap_2.c
)

add_library(freertos-kernel-heap-3 INTERFACE)
target_sources(freertos-kernel-heap-3
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/portable/MemMang/heap_3.c
)

add_library(freertos-kernel-heap-4 INTERFACE)
target_sources(freertos-kernel-heap-4
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/portable/MemMang/heap_4.c
)

add_library(freertos-kernel-heap-5 INTERFACE)
target_sources(freertos-kernel-heap-5
    INTERFACE
        ${freertos-kernel_SOURCE_DIR}/Source/portable/MemMang/heap_5.c
)

if(TARGET cmsis-rtos-api)
    # CMSIS-RTOS FreeRTOS implementation

    add_library(freertos-cmsis-rtos INTERFACE)

    target_sources(freertos-cmsis-rtos
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c
            ${freertos-kernel_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/freertos_evr.c
            #${freertos-kernel_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/handlers.c - Cortex A only
            ${freertos-kernel_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/os_systick.c
            ${freertos-kernel_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/ARM/clib_arm.c
    )
    target_include_directories(freertos-cmsis-rtos
        INTERFACE
            ${freertos-kernel_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Include
    )
    target_link_libraries(freertos-cmsis-rtos INTERFACE cmsis-rtos-api freertos-kernel)
endif()
