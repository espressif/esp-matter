#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Built in compiler identification does not work for embedded targets, so
#override it here.

#Stop built in CMakeDetermine<lang>.cmake scripts to run.
set (CMAKE_CXX_COMPILER_ID_RUN 1)
#Stop cmake run compiler tests.
set (CMAKE_CXX_COMPILER_FORCED true)

if(NOT DEFINED CMAKE_CXX_COMPILER)
	message(FATAL_ERROR "Please set CMAKE_CXX_COMPILER to hold the full path \
of your compiler executable")
endif(NOT DEFINED CMAKE_CXX_COMPILER)

get_filename_component(_CXX_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME)

if (_CXX_COMPILER_NAME MATCHES "^.*armclang(\\.exe)?$")
	set(CMAKE_CXX_COMPILER_ID "ARMCLANG" CACHE INTERNAL "C++ compiler ID" FORCE)
	set(ARM_TOOLCHAIN_FILE "Compiler/ARMClang-CXX")
elseif (_CXX_COMPILER_NAME MATCHES "^.*gcc(\\.exe)?$")
	set(CMAKE_CXX_COMPILER_ID "GNUARM" CACHE INTERNAL "C++ compiler ID" FORCE)
	set(ARM_TOOLCHAIN_FILE "Compiler/GNUARM-CXX")
elseif (_CXX_COMPILER_NAME MATCHES "^.*iccarm(\\.exe)?$")
	set(CMAKE_CXX_COMPILER_ID "IARARM" CACHE INTERNAL "C++ compiler ID" FORCE)
	set(ARM_TOOLCHAIN_FILE "Compiler/IARARM-CXX")
else()
	message(FATAL_ERROR "C++ Compiler executable ${_C_COMPILER_NAME} is \
unknown. Please add needed settings to ${CMAKE_CURRENT_LIST_FILE}")
endif ()

include(${ARM_TOOLCHAIN_FILE})

