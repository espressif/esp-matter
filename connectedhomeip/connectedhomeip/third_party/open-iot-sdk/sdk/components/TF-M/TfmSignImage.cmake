# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(ExternalProject)
include(ElfToBin)

function(iotsdk_tf_m_sign_image target)
    ExternalProject_Get_Property(tf-m-build BINARY_DIR)
    add_custom_command(
        TARGET
            ${target}
        POST_BUILD
        DEPENDS
            $<TARGET_FILE_DIR:${target}>/${target}.bin
        COMMAND
            # Sign the non-secure (application) image for TF-M bootloader (BL2)"
            python3 ${BINARY_DIR}/install/image_signing/scripts/wrapper/wrapper.py
                --layout ${BINARY_DIR}/install/image_signing/layout_files/signing_layout_ns.o
                -v ${MCUBOOT_IMAGE_VERSION_NS}
                -k ${BINARY_DIR}/install/image_signing/keys/root-RSA-3072_1.pem
                --public-key-format full
                --align 1 --pad --pad-header -H 0x400 -s auto -d "(0, 0.0.0+0)"
                $<TARGET_FILE_DIR:${target}>/${target}.bin
                --overwrite-only
                $<TARGET_FILE_DIR:${target}>/${target}_signed.bin
        COMMAND
            # Copy the bootloader and TF-M secure image for the user's convenience
            ${CMAKE_COMMAND} -E copy
                ${BINARY_DIR}/install/outputs/bl2.elf
                ${BINARY_DIR}/install/outputs/tfm_s_signed.bin
                $<TARGET_FILE_DIR:${target}>/
        COMMAND
            ${CMAKE_COMMAND} -E echo "-- signed: $<TARGET_FILE_DIR:${target}>/${target}_signed.bin"
        VERBATIM
    )
endfunction()

# To merge the bootloader, TF-M secure image and non-secure user application image into
# one, their addresses are needed. As the addresses are defined in their respective
# linker scripts, there is no simple way to programmatically get them, so they need to
# be specified by the user project.
function(iotsdk_tf_m_merge_images target bl2_address tfm_s_address ns_address)
    find_program(srec_cat NAMES srec_cat REQUIRED)
    find_program(objcopy NAMES arm-none-eabi-objcopy objcopy REQUIRED)
    ExternalProject_Get_Property(tf-m-build BINARY_DIR)
    add_custom_command(
        TARGET
            ${target}
        POST_BUILD
        DEPENDS
            $<TARGET_FILE_DIR:${target}>/${target}_signed.bin
        COMMAND
            ${srec_cat} ${BINARY_DIR}/install/outputs/bl2.bin -Binary -offset ${bl2_address}
                ${BINARY_DIR}/install/outputs/tfm_s_signed.bin -Binary -offset ${tfm_s_address}
                $<TARGET_FILE_DIR:${target}>/${target}_signed.bin -Binary -offset ${ns_address}
                -o $<TARGET_FILE_DIR:${target}>/${target}_merged.hex
        COMMAND
            ${objcopy} -I ihex -O elf32-little
                $<TARGET_FILE_DIR:${target}>/${target}_merged.hex
                $<TARGET_FILE_DIR:${target}>/${target}_merged.elf
        COMMAND
            ${CMAKE_COMMAND} -E echo "-- merged: $<TARGET_FILE_DIR:${target}>/${target}_merged.elf"
        VERBATIM
    )
endfunction()
