# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    NimBLE
    GIT_REPOSITORY  https://github.com/apache/mynewt-nimble.git
    GIT_TAG         f9eb3c8d27b07d441dc7a77de513ef940ad7da1f
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(NimBLE)
FetchContent_GetProperties(NimBLE)
