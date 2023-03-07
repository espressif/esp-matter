# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(iotsdk-example-buffered-blockdevice
    main.cpp
)
target_include_directories(iotsdk-example-buffered-blockdevice
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(iotsdk-example-buffered-blockdevice
    PRIVATE
        iotsdk-blockdevice
        storage-serial-printf
        mcu-driver-bootstrap
        mdh-platform-startup
        mdh-reference-platforms-for-arm
)

add_test(
    NAME    iotsdk-example-buffered-blockdevice
    COMMAND htrun
        --image-path=iotsdk-example-buffered-blockdevice${HTRUN_IMAGE_EXT}
        --compare-log=${CMAKE_CURRENT_LIST_DIR}/test.log
        ${HTRUN_ARGUMENTS}
    COMMAND_EXPAND_LISTS
)
