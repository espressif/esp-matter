# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    littlefs
    GIT_REPOSITORY  https://github.com/littlefs-project/littlefs.git
    GIT_TAG         v2.5.0
    GIT_SHALLOW     ON
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(littlefs)
FetchContent_GetProperties(littlefs)
