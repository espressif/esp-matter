# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(lwip)

include(FetchContent)

# LwIP CMake build files require Doxygen to have valid CMake targets.
# If Doxygen is not available in the system, create a fake lwipdocs target.
find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
add_custom_target(lwipdocs)
endif()

FetchContent_Declare(
    lwip
    GIT_REPOSITORY  https://github.com/lwip-tcpip/lwip.git
    GIT_TAG         STABLE-2_1_3_RELEASE
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(lwip)
