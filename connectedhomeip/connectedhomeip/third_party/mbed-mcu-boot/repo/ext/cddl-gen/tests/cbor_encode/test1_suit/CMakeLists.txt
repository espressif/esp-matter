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

target_cddl_source(app manifest3.cddl
  ENTRY_TYPES
  SUIT_Outer_Wrapper
  SUIT_Command_Sequence
  SUIT_Authentication_Wrapper
  ENCODE
  ${VERBOSE}
  ${CANONICAL}
  TYPE_FILE_NAME manifest3_types.h
  )

target_cddl_source(app manifest3.cddl
  ENTRY_TYPES
  SUIT_Outer_Wrapper
  SUIT_Command_Sequence
  SUIT_Authentication_Wrapper
  DECODE
  ${VERBOSE}
  ${CANONICAL}
  TYPE_FILE_NAME manifest3_types.h
  )

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE
  ${app_sources}
  )

zephyr_compile_options(-Werror)
