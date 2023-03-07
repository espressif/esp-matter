# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0


add_executable(iotsdk-example-kvstore-tdbstore
    main.cpp
)
target_include_directories(iotsdk-example-kvstore-tdbstore
    PRIVATE
        ${PROJECT_SOURCE_DIR}/common/include
)
target_link_libraries(iotsdk-example-kvstore-tdbstore
    PRIVATE
        iotsdk-tdbstore
        storage-serial-printf
        mcu-driver-bootstrap
        mdh-reference-platforms-for-arm
)
if(TARGET mdh-arm-an552-mps3)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(linker_script ${CMAKE_CURRENT_SOURCE_DIR}/platforms/an552_gcc.ld)
        target_link_options(iotsdk-example-kvstore-tdbstore PRIVATE -T ${linker_script})
        target_sources(iotsdk-example-kvstore-tdbstore PRIVATE ${mcu-driver-reference-platforms-for-arm_SOURCE_DIR}/platforms/cs300/toolchains/gcc/startup_cmsdk_cs300.S)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
        set(linker_script ${CMAKE_CURRENT_SOURCE_DIR}/platforms/an552_armclang.sct)
        target_link_options(iotsdk-example-kvstore-tdbstore PRIVATE --scatter=${linker_script})
        target_sources(iotsdk-example-kvstore-tdbstore PRIVATE ${mcu-driver-reference-platforms-for-arm_SOURCE_DIR}/platforms/cs300/toolchains/armclang/startup_cmsdk_cs300.c)
    endif()
    set_target_properties(iotsdk-example-kvstore-tdbstore PROPERTIES LINK_DEPENDS ${linker_script})
endif()
add_test(
    NAME    iotsdk-example-kvstore-tdbstore
    COMMAND htrun
        --image-path=iotsdk-example-kvstore-tdbstore${HTRUN_IMAGE_EXT}
        --compare-log=${CMAKE_CURRENT_LIST_DIR}/test.log
        ${HTRUN_ARGUMENTS}
    COMMAND_EXPAND_LISTS
)
