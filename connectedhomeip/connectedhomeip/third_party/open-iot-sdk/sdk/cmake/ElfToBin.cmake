# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

function(target_elf_to_bin target)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(elf_to_bin arm-none-eabi-objcopy -O binary $<TARGET_FILE:${target}> $<TARGET_FILE_DIR:${target}>/${target}.bin)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
        set(elf_to_bin fromelf --bin --output $<TARGET_FILE_DIR:${target}>/${target}.bin $<TARGET_FILE:${target}>)
    endif()

    add_custom_command(
        TARGET
            ${target}
        POST_BUILD
        DEPENDS
            $<TARGET_FILE:${target}>
        COMMAND
            ${elf_to_bin}
        COMMAND
            ${CMAKE_COMMAND} -E echo "-- built: $<TARGET_FILE_DIR:${target}>/${target}.bin"
        VERBATIM
    )
endfunction()
