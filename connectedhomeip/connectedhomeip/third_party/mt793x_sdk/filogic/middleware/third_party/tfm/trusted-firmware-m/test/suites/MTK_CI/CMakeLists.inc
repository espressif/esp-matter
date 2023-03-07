#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the T_COSE module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - root directory of the TF-M repo.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(MTK_CI_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})

#Verify input parameters
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED MTK_CI_TEST)
	message(FATAL_ERROR "Incomplete build configuration: MTK_CI_TEST is undefined.")
elseif(MTK_CI_TEST)
	list(APPEND ALL_SRC_C_NS
		"${MTK_CI_TEST_DIR}/non_secure/mtk_ci_test.c"
	)
endif()

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
