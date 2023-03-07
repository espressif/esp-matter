#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the cc312 libraries with
#the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

if (CRYPTO_HW_ACCELERATOR)
    list(APPEND ALL_SRC_C "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/mt7933_GCPU_accelerator.c")
    list(APPEND ALL_SRC_C "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/crypto_alt/aes_alt.c")
    list(APPEND ALL_SRC_C "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/entropy_hardware_poll.c")

	string(APPEND MBEDCRYPTO_C_FLAGS " -DCRYPTO_HW_ACCELERATOR")
    string(APPEND MBEDCRYPTO_C_FLAGS " -I ${CRYPTO_HW_ACCELERATOR_BUILD_PATH}")
    string(APPEND MBEDCRYPTO_C_FLAGS " -I ${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/crypto_alt")
endif()

embedded_include_directories(PATH "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}" ABSOLUTE)
embedded_include_directories(PATH "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/crypto_alt/" ABSOLUTE)
