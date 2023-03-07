# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(iotsdk-example-blockdevice
    source/cs3xx_cmsis5_flash.c
    source/main.c
)

target_include_directories(iotsdk-example-blockdevice
    PUBLIC
        include
)

target_link_libraries(iotsdk-example-blockdevice
    PRIVATE
        iotsdk-blockdevice
        storage-serial-printf
        mcu-driver-bootstrap
        mdh-platform-startup
        mdh-reference-platforms-for-arm
)

add_test(
    NAME    iotsdk-example-blockdevice
    COMMAND htrun
        --image-path=iotsdk-example-blockdevice${HTRUN_IMAGE_EXT}
        --compare-log=${CMAKE_CURRENT_LIST_DIR}/test.log
        ${HTRUN_ARGUMENTS}
    COMMAND_EXPAND_LISTS
)
