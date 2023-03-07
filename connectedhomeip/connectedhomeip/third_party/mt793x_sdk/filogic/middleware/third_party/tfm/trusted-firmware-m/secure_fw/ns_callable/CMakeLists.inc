#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "NS Callable" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - directory where secure FW source is located.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list.
#                  This shall be added to the add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(SS_NS_CALLABLE_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (TFM_PSA_API)
	set (SS_NS_CALLABLE_C_SRC "${CMAKE_CURRENT_LIST_DIR}/tfm_psa_api_veneers.c")
else()
	set (SS_NS_CALLABLE_C_SRC "${CMAKE_CURRENT_LIST_DIR}/tfm_veneers.c")
endif()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SS_NS_CALLABLE_C_SRC})
unset(SS_NS_CALLABLE_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE)
