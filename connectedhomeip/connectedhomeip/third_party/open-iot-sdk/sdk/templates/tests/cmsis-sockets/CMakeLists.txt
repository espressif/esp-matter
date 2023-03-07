# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# IoT Socket API on LwIP test

set(test cmsis-sockets)
set(supported_platforms corstone-300 corstone-310)
set(supported_rtoses cmsis-rtx cmsis-freertos)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    foreach(rtos IN LISTS supported_rtoses)
        get_target_rtos_variables(${rtos})
        set(test_dir ${TESTS_OUTPUT_DIR}/${test}/${rtos}/${platform})
        configure_file(CMakeLists.txt.in ${test_dir}/CMakeLists.txt @ONLY)
        configure_file(${LINKER_SCRIPT_GCC} ${test_dir}/gcc.ld @ONLY)
        configure_file(${LINKER_SCRIPT_ARM} ${test_dir}/armclang.sct @ONLY)
        file(COPY
            main.c
            cmsis-config
            test.log
            DESTINATION ${test_dir}
        )
        if(rtos STREQUAL "cmsis-freertos")
            file(COPY freertos-config DESTINATION ${test_dir})
        endif()
    endforeach()
endforeach()
