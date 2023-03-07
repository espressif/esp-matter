# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    cmsis-sockets-api
    GIT_REPOSITORY  https://github.com/MDK-Packs/IoT_Socket.git
    GIT_TAG         1e15f55ae08c2169c0e4a59216e78a4906a66af8
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(cmsis-sockets-api)
