#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "ipc" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - directory where secure FW sourec is located.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list.
#	  This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list.
#	  This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this
#	  list. This shall be added to your add_executable or add_library
#	  command.
#	Include directories will be modified by using the include_directories()
#	  commands as needed.

#Get the current directory where this file is located.
set(SS_IPC_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR
		"Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED TFM_PSA_API)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PSA_API is undefined. ")
elseif (TFM_PSA_API)
	set (SS_IPC_C_SRC "${SS_IPC_DIR}/tfm_thread.c"
			"${SS_IPC_DIR}/tfm_wait.c"
			"${SS_IPC_DIR}/tfm_message_queue.c"
			"${SS_IPC_DIR}/tfm_pools.c"
			"${SS_IPC_DIR}/../tfm_core.c"
			"${SS_IPC_DIR}/../tfm_secure_api.c"
			"${SS_IPC_DIR}/../tfm_spm_services.c"
			"${SS_IPC_DIR}/../tfm_utils.c"
			"${SS_IPC_DIR}/../tfm_core_svcalls_ipc.c"
			"${SS_IPC_DIR}/../tfm_boot_data.c"
			"${SS_IPC_DIR}/../tfm_core_utils.c"
			)

	# Add multi-core topology support
	if (DEFINED TFM_MULTI_CORE_TOPOLOGY AND TFM_MULTI_CORE_TOPOLOGY)
		list(APPEND SS_IPC_C_SRC "${SS_IPC_DIR}/tfm_rpc.c"
				"${SS_IPC_DIR}/tfm_spe_mailbox.c"
				"${SS_IPC_DIR}/tfm_multi_core.c"
				"${SS_IPC_DIR}/tfm_multi_core_mem_check.c"
				)
	else ()
		list(APPEND SS_IPC_C_SRC "${SS_IPC_DIR}/../tfm_nspm_ipc.c"
				"${SS_IPC_DIR}/../tfm_core_mem_check.c"
				)
	endif ()
endif()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SS_IPC_C_SRC})
unset(SS_IPC_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/ipc ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/ipc/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/arch/include ABSOLUTE)

if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()
