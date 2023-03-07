#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definations to compile the "libtfmsprt" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#   TFM_ROOT_DIR - root directory of the TF-M repository.
#Outputs:
#   Will modify include directories to make the source compile.
#   ALL_SRC_C_S: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   ALL_SRC_CXX_S: C++ source files to be compiled will be added to this list. This shaall be added to your add_executable or add_library command.
#   ALL_SRC_ASM_S: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(LIBSPRT_DIR ${CMAKE_CURRENT_LIST_DIR})

if(NOT DEFINED TFM_ROOT_DIR)
    message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

set (LIBSPRT_C_SRC
    "${LIBSPRT_DIR}/tfm_libsprt_c_memcpy.c"
    "${LIBSPRT_DIR}/tfm_libsprt_c_memmove.c"
    "${LIBSPRT_DIR}/tfm_libsprt_c_memcmp.c"
    "${TFM_ROOT_DIR}/interface/src/log/tfm_log_raw.c")

if (TFM_PSA_API)
    list(APPEND LIBSPRT_C_SRC
        "${TFM_ROOT_DIR}/interface/src/psa/psa_client.c"
        "${TFM_ROOT_DIR}/interface/src/psa/psa_service.c"
        "${TFM_ROOT_DIR}/interface/src/psa/psa_lifecycle.c"
    )
endif()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C_S ${LIBSPRT_C_SRC})
unset(LIBSPRT_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
