# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    add_library(parson EXCLUDE_FROM_ALL
        ${azure-sdk_SOURCE_DIR}/deps/parson/parson.c
    )

    target_include_directories(parson
        PUBLIC
            ${azure-sdk_SOURCE_DIR}/deps/parson/
    )
endif()
