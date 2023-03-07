# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Pigweed logging example

set(example pigweed-log)
set(supported_platforms corstone-300 corstone-310)
set(supported_rtoses cmsis-rtx cmsis-freertos)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    # Baremetal
    set(EXAMPLE_TITLE "Example for Pigweed's logging module")
    set(EXAMPLE_INTRO "This example shows the usage of Pigweed's logging module on ${PLATFORM_STRING}.")
    set(EXAMPLE_RUN_COMMAND "${PLATFORM_VHT} -a __build/iotsdk-example-pigweed-log.elf")
    set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/baremetal/${platform})
    configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
    configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
    configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
    configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
    file(COPY
        main.c
        test.log
        DESTINATION ${example_dir}
    )
    # RTOSes
    foreach(rtos IN LISTS supported_rtoses)
        get_target_rtos_variables(${rtos})
        set(EXAMPLE_TITLE "Example for Pigweed's logging module")
        set(EXAMPLE_INTRO "This example shows the usage of Pigweed's logging module. RTOS: ${RTOS_STRING}, platform: ${PLATFORM_STRING}.")
        set(EXAMPLE_RUN_COMMAND "${PLATFORM_VHT} -a __build/iotsdk-example-pigweed-log.elf")
        set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${rtos}/${platform})
        configure_file(CMakeLists_cmsis_rtos.txt.in ${example_dir}/CMakeLists.txt @ONLY)
        configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
        configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
        configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
        file(COPY
            main_cmsis_rtos.c
            cmsis-config
            test_cmsis_rtos.log
            DESTINATION ${example_dir}
        )
        if(rtos STREQUAL "cmsis-freertos")
            file(COPY freertos-config DESTINATION ${example_dir})
        endif()
    endforeach()
endforeach()
