# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# IoT Socket API on LwIP example

set(example cmsis-lwip)
set(supported_platforms corstone-300 corstone-310)
set(supported_rtoses cmsis-rtx cmsis-freertos)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    foreach(rtos IN LISTS supported_rtoses)
        get_target_rtos_variables(${rtos})
        set(EXAMPLE_TITLE "Example for IoT Socket API on LwIP")
        set(EXAMPLE_INTRO "This example shows the usage of IoT Socket API implemented on LwIP. RTOS: ${RTOS_STRING}, platform: ${PLATFORM_STRING}.")
        set(EXAMPLE_RUN_COMMAND "${PLATFORM_VHT} -a __build/iotsdk-example-lwip.elf -C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1")
        set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${rtos}/${platform})
        configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
        configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
        configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
        configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
        file(COPY
            main.c
            cmsis-config
            test.log
            DESTINATION ${example_dir}
        )
        if(rtos STREQUAL "cmsis-freertos")
            file(COPY freertos-config DESTINATION ${example_dir})
        endif()
    endforeach()
endforeach()
