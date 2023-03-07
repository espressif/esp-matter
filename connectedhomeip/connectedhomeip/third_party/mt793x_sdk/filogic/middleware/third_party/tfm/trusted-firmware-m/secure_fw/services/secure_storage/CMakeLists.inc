#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "secure_storage" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR		- root directory of the TF-M repository.
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(SECURE_STORAGE_DIR ${CMAKE_CURRENT_LIST_DIR})

#Check input variables
if (NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED SST_ENCRYPTION)
	message(FATAL_ERROR "Incomplete build configuration: SST_ENCRYPTION is undefined. ")
endif()

if (NOT DEFINED SST_ROLLBACK_PROTECTION)
	message(FATAL_ERROR "Incomplete build configuration: SST_ROLLBACK_PROTECTION is undefined.")
endif()

if (NOT DEFINED SST_CREATE_FLASH_LAYOUT)
	message(FATAL_ERROR "Incomplete build configuration: SST_CREATE_FLASH_LAYOUT is undefined. ")
endif()

if (NOT DEFINED SST_VALIDATE_METADATA_FROM_FLASH)
	message(FATAL_ERROR "Incomplete build configuration: SST_VALIDATE_METADATA_FROM_FLASH is undefined. ")
endif()

if (NOT DEFINED SST_RAM_FS)
	message(FATAL_ERROR "Incomplete build configuration: SST_RAM_FS is undefined. ")
endif()

if (NOT DEFINED SST_TEST_NV_COUNTERS)
	message(FATAL_ERROR "Incomplete build configuration: SST_TEST_NV_COUNTERS is undefined.")
endif()

set (SECURE_STORAGE_C_SRC
	"${SECURE_STORAGE_DIR}/tfm_sst_secure_api.c"
	"${SECURE_STORAGE_DIR}/tfm_sst_req_mngr.c"
	"${SECURE_STORAGE_DIR}/tfm_protected_storage.c"
	"${SECURE_STORAGE_DIR}/sst_object_system.c"
	"${SECURE_STORAGE_DIR}/sst_object_table.c"
	"${SECURE_STORAGE_DIR}/sst_utils.c"
)

if (SST_ENCRYPTION)
	list(APPEND SECURE_STORAGE_C_SRC
		"${SECURE_STORAGE_DIR}/crypto/sst_crypto_interface.c"
		"${SECURE_STORAGE_DIR}/sst_encrypted_object.c"
	)
	set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_ENCRYPTION)

	if (SST_ROLLBACK_PROTECTION)
		# Only build the NV counters implementation if the SST_TEST_NV_COUNTERS
		# flag is off. When this flag is on, a virtual implementation of the SST
		# NV counters interface is used instead. Full documentation for this
		# flag can be found in the SST Integration Guide.
		if (NOT SST_TEST_NV_COUNTERS)
			list(APPEND SECURE_STORAGE_C_SRC
				"${SECURE_STORAGE_DIR}/nv_counters/sst_nv_counters.c")
		endif()
		set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_ROLLBACK_PROTECTION)
	endif()
endif()

if (SST_VALIDATE_METADATA_FROM_FLASH)
	set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_VALIDATE_METADATA_FROM_FLASH)
endif()

if (SST_CREATE_FLASH_LAYOUT)
	set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_CREATE_FLASH_LAYOUT)
endif()

if (SST_RAM_FS)
	set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_RAM_FS)
endif()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SECURE_STORAGE_C_SRC})
unset(SECURE_STORAGE_C_SRC)

#Inform the user about SST service features selected based on the SST service cmake flags
message("The SST service compile configuration is as follows:")
message("- SST_ENCRYPTION: " ${SST_ENCRYPTION})
if (SST_ENCRYPTION)
	message("- SST_ROLLBACK_PROTECTION: " ${SST_ROLLBACK_PROTECTION})
else()
	message("- SST_ROLLBACK_PROTECTION: N/A")
endif()
message("- SST_VALIDATE_METADATA_FROM_FLASH: " ${SST_VALIDATE_METADATA_FROM_FLASH})
message("- SST_CREATE_FLASH_LAYOUT: " ${SST_CREATE_FLASH_LAYOUT})
message("- SST_RAM_FS: " ${SST_RAM_FS})
message("- SST_TEST_NV_COUNTERS: " ${SST_TEST_NV_COUNTERS})

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/ext/common ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/ext/driver ABSOLUTE)
set(BUILD_CMSIS_CORE Off)
set(BUILD_RETARGET Off)
set(BUILD_NATIVE_DRIVERS Off)
set(BUILD_STARTUP Off)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_HARDWARE_KEYS Off)
set(BUILD_TARGET_NV_COUNTERS Off)
set(BUILD_CMSIS_DRIVERS Off)
set(BUILD_TIME Off)
set(BUILD_UART_STDOUT Off)
set(BUILD_FLASH Off)
set(BUILD_PLAT_TEST Off)
if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()
