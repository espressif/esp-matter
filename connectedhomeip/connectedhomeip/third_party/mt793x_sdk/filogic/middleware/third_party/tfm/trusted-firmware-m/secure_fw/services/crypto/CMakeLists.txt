#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Some project global settings
set (CRYPTO_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${CRYPTO_DIR}/../../.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_crypto LANGUAGES ASM C)
embedded_project_fixup()

#Get the definition of what files we need to build
set (ENABLE_CRYPTO ON)
#The backend of the service is based on Mbed Crypto
set (CRYPTO_ENGINE_MBEDTLS ON)

if (CRYPTO_ENGINE_MBEDTLS)
	set (MBEDTLS_CONFIG_FILE "tfm_mbedcrypto_config.h")
	set (MBEDTLS_CONFIG_PATH "${PLATFORM_DIR}/common")
	#Define location of Mbed Crypto source, build, and installation directory.
	get_filename_component(MBEDCRYPTO_SOURCE_DIR "${TFM_ROOT_DIR}/../mbed-crypto" ABSOLUTE)
	set (MBEDCRYPTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbed-crypto/build")
	set (MBEDCRYPTO_INSTALL_DIR ${MBEDCRYPTO_BINARY_DIR}/../install)
	set (MBEDCRYPTO_TARGET_NAME "mbedcrypto_lib")
endif()

include(CMakeLists.inc)

if (CRYPTO_HW_ACCELERATOR)
	if(NOT DEFINED CRYPTO_HW_ACCELERATOR_CMAKE_BUILD)
		message(FATAL_ERROR "CRYPTO_HW_ACCELERATOR_CMAKE_BUILD not defined.")
	endif()
	include(${CRYPTO_HW_ACCELERATOR_CMAKE_BUILD})
endif()

#Create a list of the C defines
list(APPEND TFM_CRYPTO_C_DEFINES_LIST __thumb2__ TFM_LVL=${TFM_LVL})

if (CRYPTO_ENGINE_MBEDTLS)
  list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_ENGINE_MBEDTLS MBEDTLS_CONFIG_FILE="${MBEDTLS_CONFIG_FILE}")
  if (DEFINED MBEDTLS_USER_CONFIG_FILE)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST MBEDTLS_USER_CONFIG_FILE="${MBEDTLS_USER_CONFIG_FILE}")
  endif()
endif()

#Add module configuration parameters in case they are provided during CMake configuration step
if (CRYPTO_KEY_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_KEY_MODULE_DISABLED)
endif()
if (CRYPTO_AEAD_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_AEAD_MODULE_DISABLED)
endif()
if (CRYPTO_MAC_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_MAC_MODULE_DISABLED)
endif()
if (CRYPTO_CIPHER_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_CIPHER_MODULE_DISABLED)
endif()
if (CRYPTO_HASH_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_HASH_MODULE_DISABLED)
endif()
if (CRYPTO_KEY_DERIVATION_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED)
endif()
if (CRYPTO_ASYMMETRIC_MODULE_DISABLED)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_ASYMMETRIC_MODULE_DISABLED)
endif()

if (DEFINED CRYPTO_ENGINE_BUF_SIZE)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_ENGINE_BUF_SIZE=${CRYPTO_ENGINE_BUF_SIZE})
endif()
if (DEFINED CRYPTO_CONC_OPER_NUM)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_CONC_OPER_NUM=${CRYPTO_CONC_OPER_NUM})
endif()
if (TFM_PSA_API AND DEFINED CRYPTO_IOVEC_BUFFER_SIZE)
	list(APPEND TFM_CRYPTO_C_DEFINES_LIST TFM_CRYPTO_IOVEC_BUFFER_SIZE=${CRYPTO_IOVEC_BUFFER_SIZE})
endif()

if (CRYPTO_ENGINE_MBEDTLS)
	#Set Mbed Crypto compiler flags
	string(APPEND MBEDCRYPTO_C_FLAGS " ${MBEDCRYPTO_C_FLAGS_SERVICES}")
	string(APPEND MBEDCRYPTO_C_FLAGS " -DMBEDTLS_CONFIG_FILE=\\\\\\\"${MBEDTLS_CONFIG_FILE}\\\\\\\""
	                                 " -I${CMAKE_CURRENT_LIST_DIR}")
	if ((DEFINED MBEDTLS_USER_CONFIG_FILE) AND (DEFINED MBEDTLS_USER_CONFIG_PATH))
		string(APPEND MBEDCRYPTO_C_FLAGS " -DMBEDTLS_USER_CONFIG_FILE=\\\\\\\"${MBEDTLS_USER_CONFIG_FILE}\\\\\\\""
		" -I${MBEDTLS_USER_CONFIG_PATH}")
	endif()
	#Add TF-M include directory so Mbed Crypto can include PSA ITS headers
	string(APPEND MBEDCRYPTO_C_FLAGS " -I${TFM_ROOT_DIR}/interface/include")

	#Build Mbed Crypto as external project.
	#This ensures Mbed Crypto is built with exactly defined settings.
	#Mbed Crypto will be used from its install location
	include(${TFM_ROOT_DIR}/BuildMbedCrypto.cmake)
endif()

#Specify what we build (for the crypto service, build as a static library)
add_library(tfm_crypto STATIC ${ALL_SRC_ASM} ${ALL_SRC_C})
embedded_set_target_compile_defines(TARGET tfm_crypto LANGUAGE C DEFINES ${TFM_CRYPTO_C_DEFINES_LIST})

if (DEFINED CMSE_FLAGS)
	embedded_set_target_compile_flags(TARGET tfm_crypto LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()

if (CRYPTO_ENGINE_MBEDTLS)
	#Add a dependency on the Mbed Crypto install target.
	add_dependencies(tfm_crypto ${MBEDCRYPTO_TARGET_NAME}_install)
	#Ask the compiler to merge the Mbed Crypto and crypto service libraries.
	compiler_merge_library(DEST tfm_crypto LIBS "${MBEDCRYPTO_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX_C}")

	#Link crypto accelerator libraries if applicable
	if (CRYPTO_HW_ACCELERATOR)
		if(NOT DEFINED CRYPTO_HW_ACCELERATOR_CMAKE_LINK)
			message(FATAL_ERROR "CRYPTO_HW_ACCELERATOR_CMAKE_LINK not defined.")
		endif()
		include(${CRYPTO_HW_ACCELERATOR_CMAKE_LINK})
	endif()
endif()

#Persistent key requires ITS service
if (NOT CRYPTO_KEY_MODULE_DISABLED)
	target_link_libraries(tfm_crypto PRIVATE tfm_internal_trusted_storage)
endif()

#Set common compiler and linker flags
config_setting_shared_compiler_flags(tfm_crypto)
config_setting_shared_linker_flags(tfm_crypto)

embedded_project_end(tfm_crypto)
