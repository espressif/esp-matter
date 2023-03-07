# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# littlefs example

set(example littlefs)
set(supported_platforms corstone-300 corstone-310)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    set(EXAMPLE_TITLE "littlefs example")
    set(EXAMPLE_INTRO "This example shows the usage of littlefs on ${PLATFORM_STRING}.")
    set(EXAMPLE_RUN_COMMAND "${PLATFORM_VHT} -a __build/iotsdk-example-littlefs.elf")
    set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${platform})
    configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
    configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
    configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
    configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
    file(COPY
        main.c
        test.log
        ${PLATFORM_FLASH}
        DESTINATION ${example_dir}
    )
endforeach()
