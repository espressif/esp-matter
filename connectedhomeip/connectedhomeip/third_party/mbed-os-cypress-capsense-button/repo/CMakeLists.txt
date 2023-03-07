# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(capsense INTERFACE)

if(${MBED_TOOLCHAIN} STREQUAL "GCC_ARM")
    set(CY_CAPSENSE_LIB COMPONENT_SOFTFP/TOOLCHAIN_GCC_ARM/libcy_capsense.a)
elseif(${MBED_TOOLCHAIN} STREQUAL "ARM")
    set(CY_CAPSENSE_LIB COMPONENT_SOFTFP/TOOLCHAIN_ARM/libcy_capsense.ar)
elseif(${MBED_TOOLCHAIN} STREQUAL "IAR")
    set(CY_CAPSENSE_LIB COMPONENT_SOFTFP/TOOLCHAIN_IAR/libcy_capsense.a)
endif()

target_include_directories(capsense
    INTERFACE
        .
)

target_sources(capsense
    INTERFACE
        capsense.cpp
        cycfg_capsense.c
        cy_capsense_control.c
        cy_capsense_tuner.c
        cy_capsense_centroid.c
        cy_capsense_csx_v2.c
        cy_capsense_csd_v2.c
        cy_capsense_filter.c
        cy_capsense_generator_v3.c
        cy_capsense_processing.c
        cy_capsense_selftest.c
        cy_capsense_sensing_v2.c
        cy_capsense_sensing_v3.c
        cy_capsense_structure.c
)

target_compile_definitions(capsense
    INTERFACE
        CAPSENSE_ENABLED=1
)

target_link_libraries(capsense 
    INTERFACE
        mbed-core 
        mbed-events 
        mbed-rtos
        ${CMAKE_CURRENT_SOURCE_DIR}/${CY_CAPSENSE_LIB}
)
