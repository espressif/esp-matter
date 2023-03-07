# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(ExternalProject)
set(ml_toolchain_file "" "Set the toolchain file to use")

# Set the toolchain configuration file based on CMAKE_C_COMPILER_ID use
# provided toolchain file as part of ml-embedded-evaluation-kit.
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(ml_toolchain_file "<SOURCE_DIR>/scripts/cmake/toolchains/bare-metal-gcc.cmake")
elseif(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
    set(ml_toolchain_file "<SOURCE_DIR>/scripts/cmake/toolchains/bare-metal-armclang.cmake")
endif()

ExternalProject_Add(
    ml-embedded-evaluation-kit
    GIT_REPOSITORY  https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit
    GIT_TAG         22.05
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
    PREFIX          ml
    INSTALL_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${ml_toolchain_file}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        ${ML_CMAKE_ARGS}
    PATCH_COMMAND
        ${ML_PATCH_COMMAND}
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target ${ML_TARGETS}
)

ExternalProject_Get_property(ml-embedded-evaluation-kit SOURCE_DIR)

ExternalProject_Add_Step(ml-embedded-evaluation-kit
    download_model
    COMMAND python3 ${SOURCE_DIR}/set_up_default_resources.py
    DEPENDEES patch
    DEPENDERS configure
)
