# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(iotsdk-example-flashiapblockdevice
    main.cpp
)
target_include_directories(iotsdk-example-flashiapblockdevice
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(iotsdk-example-flashiapblockdevice
    PRIVATE
        iotsdk-blockdevice
        storage-serial-printf
        mcu-driver-bootstrap
        mdh-platform-startup
        mdh-reference-platforms-for-arm
)

add_test(
    NAME    iotsdk-example-flashiapblockdevice
    COMMAND htrun
        --image-path=iotsdk-example-flashiapblockdevice${HTRUN_IMAGE_EXT}
        --compare-log=${CMAKE_CURRENT_LIST_DIR}/test.log
        ${HTRUN_ARGUMENTS}
    COMMAND_EXPAND_LISTS
)

add_executable(iotsdk-example-flashiapblockdevice-cdi-experimental
    main.cpp
)
target_include_directories(iotsdk-example-flashiapblockdevice-cdi-experimental
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(iotsdk-example-flashiapblockdevice-cdi-experimental
    PRIVATE
        iotsdk-cdi-blockdevice-experimental
        storage-serial-printf
        mcu-driver-bootstrap
        mdh-platform-startup
        mdh-reference-platforms-for-arm
)

add_test(
    NAME    iotsdk-example-flashiapblockdevice-cdi-experimental
    COMMAND htrun
        --image-path=iotsdk-example-flashiapblockdevice-cdi-experimental${HTRUN_IMAGE_EXT}
        --compare-log=${CMAKE_CURRENT_LIST_DIR}/test.log
        ${HTRUN_ARGUMENTS}
    COMMAND_EXPAND_LISTS
)
