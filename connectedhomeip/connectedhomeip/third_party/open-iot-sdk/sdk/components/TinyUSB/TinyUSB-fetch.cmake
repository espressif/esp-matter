# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    TinyUSB
    GIT_REPOSITORY  https://github.com/hathach/tinyusb.git
    GIT_TAG         251ce9899039fbdf8e749342bc6504e7438d2e88
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(TinyUSB)
FetchContent_GetProperties(TinyUSB)
