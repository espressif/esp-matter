#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file contains settings to specify how ARMCLANG shall be used

#Include some dependencies
Include(Common/CompilerArmClangCommon)
Include(Common/Utils)

check_armclang_input_vars("6.13")

if(NOT DEFINED ARM_CPU_ARCHITECTURE)
	set(_NO_ARM_CPU_ARCHITECTURE true)
elseif(${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8.1-M.MAIN")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv8.1-m.main+mve")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv8.1-m.main+mve")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=8.1-M.Main.dsp")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=8.1-M.Main.dsp")

# FIXME
# When compiling for a given architecture, compilers define a flag identifying
# the architecture like __ARM_ARCH_7M__ or __ARM_ARCH_8M_MAIN__ or
# __ARM_ARCH_8_1M_MAIN__. Those flags are used in CMSIS headers like
# cmsis_armclang.h or cmsis_gcc.h to restrict the definition of some symbols
# to particular architectures. For the new Armv8.1-M.Main architecture, some
# CMSIS headers like cmsis_armclang.h or cmsis_gcc.h have not been updated yet
# to use __ARM_ARCH_8_1M_MAIN__. To mitigate this, core_armv81mml.h header
# defines __ARM_ARCH_8M_MAIN__: #define __ARM_ARCH_8M_MAIN__ 1. Concerning
# cmsis_armclang.h or cmsis_gcc.h includes, for this solution to work, wherever
# cmsis_compiler.h (which includes cmsis_armclang.h or cmsis_gcc.h) is included
# core_armv81mml.h has to be included as well. This is not the case in several
# places in TF-M code leading to compilation failures for some C files for
# Armv8.1-M architecture. When the CMSIS headers are updated to fully support
# the __ARM_ARCH_8_1M_MAIN__ flag, the compilation issues should disappear thus
# we don't want to change the header inclusions in TF-M code. Thus the below
# definition as a compile parameter of _ARM_ARCH_8M_MAIN__ that must be removed
# in the future.

	add_definitions(-D__ARM_ARCH_8M_MAIN__=1)

elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.BASE")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.base")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.base")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=8-M.Base")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=8-M.Base")
elseif(${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.MAIN")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.main")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.main")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=8-M.Main")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=8-M.Main")
elseif(${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv7-M")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=7-M")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=7-M")
elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv6-M")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=6-M")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=6-M")
else()
	message(FATAL_ERROR "Unknown or unsupported ARM cpu architecture setting.")
endif()

#Prefer architecture definition over cpu type.
if(NOT DEFINED ARM_CPU_ARCHITECTURE)
	if(NOT DEFINED ARM_CPU_TYPE)
		string_append_unique_item(_NO_ARM_CPU_TYPE true)
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M3")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m3")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m3")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu" VAL "--cpu=Cortex-M3")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu" VAL "--cpu=Cortex-M3")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M33")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m33")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m33")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu" VAL "--cpu=Cortex-M33")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu" VAL "--cpu=Cortex-M33")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M23")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m23")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m23")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu" VAL "--cpu=Cortex-M23")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu" VAL "--cpu=Cortex-M23")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M4")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M4")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M4")
		set(ARM_CPU_ARCHITECTURE "ARMv7-M")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M0p")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "--target=" VAL "--target=arm-arm-none-eabi")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M0plus")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M0plus")
		set(ARM_CPU_ARCHITECTURE "ARMv6-M")
	else()
		message(FATAL_ERROR "Unknown ARM cpu setting.")
	endif()
endif()

if (_NO_ARM_CPU_TYPE AND _NO_ARM_CPU_ARCHITECTURE)
	message(FATAL_ERROR "Can not set CPU specific compiler flags: neither the ARM CPU type nor the architecture is set.")
endif()
