# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    CMSIS_5
    GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5.git
    GIT_TAG         105cfde9cce81a3d165a9810492c75dbcfc69710
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(CMSIS_5)
FetchContent_GetProperties(CMSIS_5)
