#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the cc312 libraries with
#the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

if (CMAKE_HOST_WIN32)
	#CC312 can only be build from a posix environment (cygwin or msys).
	#Verify environment.
	set(_CC312_OK False)

	find_program(CMAKE_UNAME_COMMAND uname)
	if (CMAKE_UNAME_COMMAND)
		execute_process(COMMAND ${CMAKE_UNAME_COMMAND} -o
						OUTPUT_VARIABLE _UNAME_RES
						OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(_UNAME_RES STREQUAL "Msys" OR _UNAME_RES STREQUAL "Cygwin")
			set(_CC312_OK True)
		endif()
	else()
		set(_CC312_OK False)
	endif()

	if (NOT _CC312_OK)
		set(_MSG "The CC312 build system currently only supports POSIX")
		set(_MSG "${_MSG} build environments. Please start the build from")
		set(_MSG "${_MSG} Cygwin or Msys (Mingw) shell.")
		message(FATAL_ERROR ${_MSG})
	endif()
	unset(_CC312_OK)
endif()

if (NOT DEFINED CC312_SOURCE_DIR)
	message(FATAL_ERROR "Please set CC312_SOURCE_DIR before including this file.")
endif()

if (NOT DEFINED CC312_TARGET_NAME)
	set(CC312_TARGET_NAME "${PROJECT_NAME}_cc312_lib" PARENT_SCOPE)
	set(CC312_TARGET_NAME "${PROJECT_NAME}_cc312_lib")
	message(WARNING "Using default CC312_TARGET_NAME ${CC312_TARGET_NAME}")
endif()

if (NOT DEFINED CC312_BUILD_DIR)
	set(CC312_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/cryptocell/build")
	message(WARNING "Using default CC312_BUILD_DIR ${CC312_BUILD_DIR}")
endif()

if (NOT DEFINED CC312_INSTALL_DIR)
	set(CC312_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/cryptocell/install PARENT_SCOPE)
	set(CC312_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/cryptocell/install)
	message(WARNING "Using default CC312_INSTALL_DIR ${CC312_INSTALL_DIR}")
endif()


# CC312 needs to know what config mbedtls was built with
if (NOT DEFINED MBEDTLS_CONFIG_FILE)
	message(FATAL_ERROR "Please set MBEDTLS_CONFIG_FILE before including this file.")
endif()
if (NOT DEFINED MBEDTLS_CONFIG_PATH)
	message(FATAL_ERROR "Please set MBEDTLS_CONFIG_PATH before including this file.")
endif()

#FIXME This is bad, but it _does_ work.
if (${PROJECT_NAME} STREQUAL "mcuboot")
	# because these are used in the mbedtls config they need to be defined for
	# CC312 as well (as it includes the config
	if (MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-3072")
		string(APPEND CC312_C_FLAGS " -DMCUBOOT_SIGN_RSA_LEN=3072")
	elseif(MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-2048")
		string(APPEND CC312_C_FLAGS " -DMCUBOOT_SIGN_RSA_LEN=2048")
	endif()
endif()

if (CRYPTO_HW_ACCELERATOR)
	list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/cc312/cc312.c")
	string(APPEND MBEDCRYPTO_C_FLAGS " -DUSE_MBEDTLS_CRYPTOCELL")
	string(APPEND MBEDCRYPTO_C_FLAGS " -DCRYPTO_HW_ACCELERATOR")
endif()

if (CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "PROVISIONING")
	list(APPEND ALL_SRC_C_BL2 "${PLATFORM_DIR}/common/cc312/cc312_provisioning.c")
	string(APPEND MBEDCRYPTO_C_FLAGS " -DCRYPTO_HW_ACCELERATOR_OTP_PROVISIONING")
	string(APPEND CC312_C_FLAGS " -DCRYPTO_HW_ACCELERATOR_OTP_PROVISIONING")
elseif (CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "ENABLED")
	string(APPEND MBEDCRYPTO_C_FLAGS " -DCRYPTO_HW_ACCELERATOR_OTP_ENABLED")
	string(APPEND CC312_C_FLAGS " -DCRYPTO_HW_ACCELERATOR_OTP_ENABLED")
endif()


embedded_include_directories(PATH "${PLATFORM_DIR}/common/cc312/" ABSOLUTE)

embedded_include_directories(PATH "${CC312_INSTALL_DIR}/include")
string(APPEND MBEDCRYPTO_C_FLAGS " -I ${CC312_INSTALL_DIR}/include")
string(APPEND CC312_INC_DIR " ${CC312_INSTALL_DIR}/include")

string(APPEND MBEDCRYPTO_C_FLAGS " -I ${PLATFORM_DIR}/common/cc312")
string(APPEND MBEDCRYPTO_C_FLAGS " -DMBEDTLS_ECDH_LEGACY_CONTEXT")
string(APPEND MBEDCRYPTO_C_FLAGS " -DCC_IOT")

string(APPEND CC312_C_FLAGS " -DMBEDTLS_CONFIG_FILE=\\\"${MBEDTLS_CONFIG_FILE}\\\"")
string(APPEND CC312_INC_DIR " ${MBEDTLS_CONFIG_PATH}")
string(APPEND CC312_INC_DIR " ${PLATFORM_DIR}/common/cc312")


if (MBEDCRYPTO_DEBUG)
	if (${COMPILER} STREQUAL "GNUARM")
		list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/cc312/cc312_log.c")
		string(APPEND CC312_C_FLAGS " -DDEBUG -DCC_PAL_MAX_LOG_LEVEL=3")
	else()
		# Can't set DEBUG (because of stdout issues)
		message(WARNING "${COMPILER} does not support CC312 debug logging")
	endif()
	string(APPEND CC312_C_FLAGS " -g -O0")
endif()

set(CC312_COMPILER ${CMAKE_C_COMPILER})

if (${COMPILER} STREQUAL "ARMCLANG")
	set(CC312_CROSSCOMPILE armclang)
elseif(${COMPILER} STREQUAL "GNUARM")
	set(CC312_CROSSCOMPILE arm-none-eabi-)
else()
	message(FATAL_ERROR "Compiler ${COMPILER} is not supported by CC312")
endif()

if (TARGET ${CC312_TARGET_NAME})
	message(FATAL_ERROR "A target with name ${CC312_TARGET_NAME} is already\
defined. Please set CC312_TARGET_NAME to a unique value.")
endif()

#Build CC312 as external project.
include(ExternalProject)

#On windows the maximum command line length makes compilation with logging fail.
if (CMAKE_HOST_WIN32)
	set(LOGFILE "-")
else()
	set(LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/cc312_makelog.txt")
endif()

configure_file("${PLATFORM_DIR}/common/cc312/cc312_proj_cfg.mk.in"
			   "${CC312_BUILD_DIR}/cc312_proj_cfg.mk"
			   @ONLY)

externalproject_add(${CC312_TARGET_NAME}
	SOURCE_DIR ${CC312_SOURCE_DIR}/host/src
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CC312_BUILD_TYPE}
	BUILD_IN_SOURCE 1
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	WORKING_DIRECTORY ${CC312_SOURCE_DIR}/host/src
	CONFIGURE_COMMAND ${CMAKE_COMMAND} -E make_directory ${CC312_SOURCE_DIR}/mbedtls
			  COMMAND ${CMAKE_COMMAND} -E copy_directory ${MBEDCRYPTO_SOURCE_DIR} ${CC312_SOURCE_DIR}/mbedtls
	INSTALL_COMMAND ""
	BUILD_ALWAYS TRUE
	BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ARCH=arm CROSS_COMPILE=${CC312_CROSSCOMPILE}
				  PROJ_CFG_PATH=${CC312_BUILD_DIR}/cc312_proj_cfg.mk
				  LOGFILE=${LOGFILE}
)
