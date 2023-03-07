#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# This file provides functions to support multi-core building.
cmake_minimum_required(VERSION 3.7)

# Platform specific cmake script calls this function to select and enable
# building configuration in mulit-core scenario
function(enable_multi_core_topology_config)
	set(TFM_MULTI_CORE_TOPOLOGY ON PARENT_SCOPE)
endfunction(enable_multi_core_topology_config)

# Enable multiple outstanding NS PSA Client calls feature.
# It requires the multi-core platform implement the configuration and support
# of the multiple outstanding NS PSA Client calls feature.
function(enable_multi_core_multi_client_call)
	add_definitions(-DTFM_MULTI_CORE_MULTI_CLIENT_CALL)
endfunction(enable_multi_core_multi_client_call)

# Platform specific cmake script calls this function to set secure core cpu type
# Argument CPU_TYPE_CMAKE represents the CMake file of the corresponding secure
# CPU type.
# CPU_TYPE_CMAKE value   CMake file       CPU type to be selected
# CpuM0p                 CpuM0p.cmake     Cortex-M0+
# CpuM4                  CpuM4.cmake      Cortex-M4
# CpuM23                 CpuM23.cmake     Cortex-M23
# CpuM33                 CpuM33.cmake     Cortex-M33
# Other CPU types are not yet supported.
function(set_secure_cpu_type CPU_TYPE_CMAKE)
	include("Common/${CPU_TYPE_CMAKE}")

	if (NOT DEFINED ARM_CPU_TYPE)
		message(FATAL_ERROR "Error: Fail to set secure cpu type.")
	else ()
		set(SECURE_ARM_CPU_TYPE ${ARM_CPU_TYPE} PARENT_SCOPE)
	endif ()
endfunction(set_secure_cpu_type)

# Platform specific cmake script calls this function to set non-secure core cpu
# type
# Argument CPU_TYPE_CMAKE represents the CMake file of the corresponding
# non-secure CPU type.
# CPU_TYPE_CMAKE value   CMake file       CPU type to be selected
# CpuM0p                 CpuM0p.cmake     Cortex-M0+
# CpuM4                  CpuM4.cmake      Cortex-M4
# CpuM23                 CpuM23.cmake     Cortex-M23
# CpuM33                 CpuM33.cmake     Cortex-M33
# Other CPU types are not yet supported.
function(set_ns_cpu_type CPU_TYPE_CMAKE)
	include("Common/${CPU_TYPE_CMAKE}")

	if (NOT DEFINED ARM_CPU_TYPE)
		message(FATAL_ERROR "Error: Fail to set non-secure cpu type.")
	else ()
		set(NS_ARM_CPU_TYPE ${ARM_CPU_TYPE} PARENT_SCOPE)
	endif ()
endfunction(set_ns_cpu_type)

# Platform specific cmake script calls this function to add platform specific
# secure definitions.
# Multiple definitions and options can be organized in argument PLATFORM_DEFS,
# separated by spaces:
#     add_platform_secure_definitions(DEF1 DEF2=VAL DEF3)
# The `-D` option flag before preprocessor macros should be skipped.
function(add_platform_secure_definitions PLATFORM_DEFS)
	# Check if the same definition is already set
	string(FIND TFM_PLATFORM_SECURE_DEFS PLATFORM_DEFS find_output)

	if (find_output EQUAL -1)
		# Not set yet. Add it into secure definition list.
		set(TFM_PLATFORM_SECURE_DEFS ${TFM_PLATFORM_SECURE_DEFS}
			${PLATFORM_DEFS}
			PARENT_SCOPE)
	endif ()
endfunction(add_platform_secure_definitions)

function(select_arm_cpu_type BUILD_IN_SPE_FLAG)
	if (BUILD_IN_SPE_FLAG)
		if (NOT DEFINED SECURE_ARM_CPU_TYPE)
			message(FATAL_ERROR "Error: cannot find definition of SECURE_ARM_CPU_TYPE")
		else ()
			set(ARM_CPU_TYPE ${SECURE_ARM_CPU_TYPE} PARENT_SCOPE)
		endif ()
	else ()
		if (NOT DEFINED NS_ARM_CPU_TYPE)
			message(FATAL_ERROR "Error: cannot find definition of NS_ARM_CPU_TYPE")
		else ()
			set(ARM_CPU_TYPE ${NS_ARM_CPU_TYPE} PARENT_SCOPE)
		endif ()
	endif ()
endfunction(select_arm_cpu_type)
