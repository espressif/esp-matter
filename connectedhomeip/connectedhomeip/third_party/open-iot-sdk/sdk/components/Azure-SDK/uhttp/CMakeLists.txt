# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    add_library(azure-uhttp EXCLUDE_FROM_ALL
        ${azure-sdk_SOURCE_DIR}/deps/uhttp/src/uhttp.c
    )

    target_include_directories(azure-uhttp
        PUBLIC
            ${azure-sdk_SOURCE_DIR}/uhttp/inc
    )

    target_link_libraries(azure-uhttp
        PUBLIC
            azure-c-shared-utility
            azure-macro-utils
    )
endif()
