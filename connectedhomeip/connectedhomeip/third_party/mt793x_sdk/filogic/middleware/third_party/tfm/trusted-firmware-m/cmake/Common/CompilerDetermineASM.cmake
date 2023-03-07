#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Built in compiler identification does not work for embedded targets, so
#override it here.

if(NOT DEFINED CMAKE_ASM_COMPILER)
	message(FATAL_ERROR "Please set CMAKE_ASM_COMPILER to hold the full path of\
 your compiler executable")
endif()

get_filename_component(_ASM_COMPILER_NAME ${CMAKE_ASM_COMPILER} NAME)

#Based on the name of the compiler executable select which tool we use.
if (_ASM_COMPILER_NAME MATCHES "^.*armclang(\\.exe)?$")
	set(ARM_TOOLCHAIN_FILE "Compiler/ARMClang-ASM")
elseif(_ASM_COMPILER_NAME MATCHES "^.*armasm(\\.exe)?$")
	set(ARM_TOOLCHAIN_FILE "Compiler/ARMCC-ASM")
elseif (_ASM_COMPILER_NAME MATCHES "^.*gcc(\\.exe)?$")
	set(ARM_TOOLCHAIN_FILE "Compiler/GNUARM-ASM")
elseif (_ASM_COMPILER_NAME MATCHES "^.*iasmarm(\\.exe)?$")
	set(ARM_TOOLCHAIN_FILE "Compiler/IARARM-ASM")
else()
	message(FATAL_ERROR "ASM Compiler executable ${_ASM_COMPILER_NAME} is \
unknown. Please add needed settings to ${CMAKE_CURRENT_LIST_FILE}")
endif ()

include(${ARM_TOOLCHAIN_FILE})
