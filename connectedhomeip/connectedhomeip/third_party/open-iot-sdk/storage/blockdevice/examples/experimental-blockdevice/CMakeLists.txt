# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(iotsdk-example-experimental-blockdevice
    main.c
    platforms/flash_cs300.c
)
target_link_libraries(iotsdk-example-experimental-blockdevice
    PRIVATE
        iotsdk-cdi-blockdevice-experimental
        storage-serial-printf
        mcu-driver-bootstrap
        mdh-platform-startup
        mdh-reference-platforms-for-arm
)

add_test(
    NAME    iotsdk-example-experimental-blockdevice
    COMMAND htrun
        --image-path=iotsdk-example-experimental-blockdevice${HTRUN_IMAGE_EXT}
        --compare-log=${CMAKE_CURRENT_LIST_DIR}/test.log
        ${HTRUN_ARGUMENTS}
    COMMAND_EXPAND_LISTS
)
