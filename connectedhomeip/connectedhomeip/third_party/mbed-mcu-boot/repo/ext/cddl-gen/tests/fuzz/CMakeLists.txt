#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.13.1)

project(NONE)

add_subdirectory(../../ ${PROJECT_BINARY_DIR}/cddl_gen)

add_executable(fuzz_pet
    main_entry.c
    fuzz_pet.c)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/fuzz_input)

target_cddl_source(fuzz_pet ../cases/pet.cddl ENTRY_TYPES Pet DECODE)

execute_process(
    COMMAND python3 ../../../cddl_gen/cddl_gen.py
    --verbose
    --cddl ../../cases/pet.cddl
    convert
    --input ../../cases/pet0.yaml
    -t Pet
    --output ${PROJECT_BINARY_DIR}/fuzz_input/input0.cbor
    --output-as cbor
    )
