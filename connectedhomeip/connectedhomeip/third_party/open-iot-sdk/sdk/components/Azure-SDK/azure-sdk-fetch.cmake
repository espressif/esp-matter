# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    azure-sdk
    GIT_REPOSITORY  https://github.com/Azure/azure-iot-sdk-c
    GIT_TAG         LTS_01_2022_Ref01
    GIT_SHALLOW     ON
    GIT_PROGRESS    ON
)

FetchContent_GetProperties(azure-sdk)
FetchContent_Populate(azure-sdk)
