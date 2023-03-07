# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(iotsdk_threadx)

include(ThreadX-fetch.cmake)

if(TARGET threadx)
    # Assembly files in ThreadX ports do not include tx_user.h but require
    # configuration macros, so we force include tx_user.h
    target_compile_options(threadx
        PRIVATE
            # ThreadX always makes a copy of TX_USER_FILE here and uses it
            $<$<COMPILE_LANGUAGE:ASM>:-include ${threadx_BINARY_DIR}/custom_inc/tx_user.h>
    )
endif()
