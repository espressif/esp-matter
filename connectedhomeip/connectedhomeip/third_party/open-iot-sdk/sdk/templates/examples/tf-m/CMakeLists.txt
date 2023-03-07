# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# TF-M example

set(example tf-m)
set(supported_platforms corstone-300 corstone-310)
set(supported_rtoses cmsis-rtx cmsis-threadx)

foreach(platform IN LISTS supported_platforms)
    get_tfm_target_platform_variables(${platform})
    foreach(rtos IN LISTS supported_rtoses)
        get_target_rtos_variables(${rtos})
        set(EXAMPLE_TITLE "Trusted Firmware-M (TF-M) example")
        set(EXAMPLE_INTRO "This example shows the usage of Trusted Firmware-M (TF-M). RTOS: ${RTOS_STRING}, platform: ${PLATFORM_STRING}.")
        string(CONCAT EXAMPLE_RUN_COMMAND
            "# PSA Crypto API\n"
            "${PLATFORM_VHT} -a __build/iotsdk-example-tf-m-crypto_merged.elf\n"
            "# PSA Firmware Update API\n"
            "${PLATFORM_VHT} -a __build/iotsdk-example-tf-m-firmware-update_merged.elf\n"
        )
        set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${rtos}/${platform})
        configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
        configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
        configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
        configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
        file(COPY
            main.c
            crypto.c
            firmware_update.c
            cmsis-config
            crypto.log
            firmware_update.log
            DESTINATION ${example_dir}
        )
        if(rtos STREQUAL "cmsis-threadx")
            file(COPY threadx-config DESTINATION ${example_dir})
        endif()
    endforeach()
endforeach()
