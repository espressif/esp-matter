# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(lwip-cmsis-sys)

target_include_directories(lwip-cmsis-sys
    PUBLIC
        include
)

target_sources(lwip-cmsis-sys
    PRIVATE
        src/sys_arch.c
)

target_link_libraries(lwip-cmsis-sys
    PRIVATE
        lwipcore
        lwipopts
        cmsis-core
    PUBLIC
        cmsis-rtos-api
)

# GCC is sensitive to linker argument order so we must provide the RTOS library here
if(TARGET cmsis-rtx)
    target_link_libraries(lwip-cmsis-sys
        PRIVATE
            cmsis-rtx
    )
elseif(TARGET freertos-cmsis-rtos)
    target_link_libraries(lwip-cmsis-sys
        PRIVATE
            freertos-cmsis-rtos
    )
else()
    message(FATAL_ERROR "Must provide an RTOS for lwip-cmsis-sys")
endif()
