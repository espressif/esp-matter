# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

macro(get_target_rtos_variables rtos)
    set(RTOS_STRING ${${rtos}-STRING})
    set(RTOS_FETCH ${${rtos}-FETCH})
    set(RTOS_LIBS ${${rtos}-LIBS})
    set(RTOS_HEADER_INCLUDE ${${rtos}-HEADER_INCLUDE})
endmacro()

# CMSIS RTX
set(cmsis-rtx-STRING "CMSIS RTX")
set(cmsis-rtx-FETCH "set(IOTSDK_CMSIS_RTX ON)")
set(cmsis-rtx-LIBS "cmsis-rtx")
set(cmsis-rtx-HEADER_INCLUDE
    "target_include_directories(cmsis-core INTERFACE cmsis-config)"
)

# CMSIS RTOS v2 wrapper on FreeRTOS
set(cmsis-freertos-STRING "CMSIS RTOS API wrapper on FreeRTOS")
set(cmsis-freertos-FETCH
"set(IOTSDK_FREERTOS ON)
set(IOTSDK_CMSIS_RTOS_API ON)"
)
set(cmsis-freertos-LIBS "freertos-cmsis-rtos
    freertos-kernel-heap-2")
set(cmsis-freertos-HEADER_INCLUDE
"target_include_directories(cmsis-core INTERFACE cmsis-config)
target_include_directories(freertos-kernel INTERFACE freertos-config)"
)

# CMSIS RTOS v2 wrapper on ThreadX
set(cmsis-threadx-STRING "CMSIS RTOS API wrapper on ThreadX")
set(cmsis-threadx-FETCH
"set(IOTSDK_THREADX_CDI_PORT ON)
set(IOTSDK_CMSIS_RTOS_API ON)
set(TX_USER_FILE \"\$\{CMAKE_CURRENT_LIST_DIR\}/threadx-config/tx_user.h\")"
)
set(cmsis-threadx-LIBS "threadx-cdi-port")
set(cmsis-threadx-HEADER_INCLUDE
    "target_include_directories(cmsis-core INTERFACE cmsis-config)"
)

# FreeRTOS
set(freertos-STRING "FreeRTOS")
set(freertos-FETCH "set(IOTSDK_FREERTOS ON)")
set(freertos-LIBS "freertos-kernel
    freertos-kernel-heap-2")
set(freertos-HEADER_INCLUDE
    "target_include_directories(freertos-kernel INTERFACE freertos-config)"
)

# FreeRTOS wrapper on ThreadX
set(freertos-threadx-STRING "FreeRTOS API wrapper on ThreadX")
set(freertos-threadx-FETCH
"set(IOTSDK_THREADX ON)
set(TX_USER_FILE \"\$\{CMAKE_CURRENT_LIST_DIR\}/threadx-config/tx_user.h\")"
)
set(freertos-threadx-LIBS "freertos-threadx")
set(freertos-threadx-HEADER_INCLUDE
    "target_include_directories(threadx INTERFACE freertos-config)"
)

# ThreadX
set(threadx-STRING "ThreadX")
set(threadx-FETCH
"set(IOTSDK_THREADX ON)
set(TX_USER_FILE \"\$\{CMAKE_CURRENT_LIST_DIR\}/threadx-config/tx_user.h\")")
set(threadx-LIBS "threadx")
