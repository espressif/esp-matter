# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

# Note: Pigweed operates in a mode where head is always functionally working.
# It still contains experimental features but they are marked as such.
# Stable features are used in production for projects like Matter.
FetchContent_Declare(
    Pigweed
    GIT_REPOSITORY  https://github.com/google/pigweed.git
    GIT_TAG         2952d3767e6a4b9511c73d63d2718fb7d1e7aaf7
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(Pigweed)
FetchContent_GetProperties(Pigweed)
