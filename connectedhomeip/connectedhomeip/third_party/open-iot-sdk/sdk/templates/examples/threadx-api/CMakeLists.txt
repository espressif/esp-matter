# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# ThreadX API example

set(example threadx-api)
set(supported_platforms corstone-300 corstone-310)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    get_target_rtos_variables(threadx)
    set(EXAMPLE_TITLE "ThreadX API example")
    set(EXAMPLE_INTRO "This example shows the usage of the ThreadX API on ${PLATFORM_STRING}.")
    set(EXAMPLE_RUN_COMMAND "${PLATFORM_VHT} -a __build/iotsdk-example-threadx.elf")
    set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${platform})
    configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
    configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
    configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
    configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
    file(COPY
        main.c
        test.log
        threadx-config
        DESTINATION ${example_dir}
    )
endforeach()
