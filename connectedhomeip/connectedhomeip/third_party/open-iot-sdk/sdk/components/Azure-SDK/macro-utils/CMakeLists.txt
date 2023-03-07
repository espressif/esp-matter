# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    add_library(azure-macro-utils INTERFACE EXCLUDE_FROM_ALL)

    target_include_directories(azure-macro-utils
        INTERFACE
            ${azure-sdk_SOURCE_DIR}/deps/azure-macro-utils-c/inc
    )
endif()
