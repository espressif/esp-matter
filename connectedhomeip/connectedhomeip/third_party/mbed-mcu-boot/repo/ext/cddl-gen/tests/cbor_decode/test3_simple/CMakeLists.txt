#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

set(BUILD_WITH_ZEPHYR ON)

add_subdirectory(../../.. ${PROJECT_BINARY_DIR}/cddl_gen)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE
  ${app_sources})

target_cddl_source(app pet.cddl ENTRY_TYPES Pet
  DECODE
  ${VERBOSE}
  ${CANONICAL}
  )
target_cddl_source(app serial_recovery.cddl ENTRY_TYPES Upload
  DECODE
  ${VERBOSE}
  ${CANONICAL}
  )

zephyr_compile_options(-Werror)
