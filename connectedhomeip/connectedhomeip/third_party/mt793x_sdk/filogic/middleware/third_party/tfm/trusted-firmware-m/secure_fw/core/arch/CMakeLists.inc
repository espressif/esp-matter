#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "arch" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - directory where TF-M soure code is located.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C_S: C source files to be compiled will be added to this list.
#	  This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM_S: assembly source files to be compiled will be added to this
#	  list. This shall be added to your add_executable or add_library
#	  command.
#	Include directories will be modified by using the include_directories()
#	  commands as needed.

#Get the current directory where this file is located.
set(TFM_ARCH_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR
		"Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED ARM_CPU_ARCHITECTURE)
	message(FATAL_ERROR "Incomplete build configuration: architecture is not specified.")
elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8.1-M.MAIN")
	set(TFM_ARCH_C_SRC "${TFM_ARCH_DIR}/tfm_arch_v8m_main.c")
elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.MAIN")
	set(TFM_ARCH_C_SRC "${TFM_ARCH_DIR}/tfm_arch_v8m_main.c")
elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.BASE")
	set(TFM_ARCH_C_SRC "${TFM_ARCH_DIR}/tfm_arch_v8m_base.c")
elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv7-M" OR
		${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv6-M" OR
		${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv6S-M")
	if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
		message(FATAL_ERROR "Armv6-M/Armv7-M can only support multi-core TF-M.")
	endif()

	set(TFM_ARCH_C_SRC "${TFM_ARCH_DIR}/tfm_arch_v6m_v7m.c")
else ()
	message(FATAL_ERROR "Unsupported architecture.")
endif()

list(APPEND TFM_ARCH_C_SRC "${TFM_ARCH_DIR}/tfm_arch.c")

#Append all our source files to global secure source code lists.
list(APPEND ALL_SRC_C_S ${TFM_ARCH_C_SRC})
unset(TFM_ARCH_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/ext/cmsis ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/ipc ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/ipc/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ARCH_DIR}/include ABSOLUTE)
