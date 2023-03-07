#------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

set(TFM_BUILD_IN_SPE ON)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
get_filename_component(TFM_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(mcuboot LANGUAGES ASM C)
embedded_project_fixup()

#Set the appropriate MCUBoot path
if (MCUBOOT_REPO STREQUAL "TF-M")
	get_filename_component(MCUBOOT_DIR ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
else()
	get_filename_component(MCUBOOT_DIR "${TFM_ROOT_DIR}/../mcuboot/boot" ABSOLUTE)
	if (NOT EXISTS ${MCUBOOT_DIR})
		message(FATAL_ERROR "Missing MCUBoot. Please clone the MCUBoot repo to directory \"${MCUBOOT_DIR}\".")
	endif()
endif()

#Check input variables
if (NOT DEFINED BL2)
   message(FATAL ERROR "Incomplete build configuration: BL2 is undefined.")
elseif(NOT BL2)
    #If mcuboot is not need to be built then stop further processing.
    return()
endif()

if (NOT DEFINED MBEDCRYPTO_C_FLAGS_BL2)
	message(FATAL_ERROR "Incomplete build configuration: MBEDCRYPTO_C_FLAGS_BL2 is undefined.")
endif()

set(BUILD_CMSIS_CORE On)
set(BUILD_RETARGET On)
set(BUILD_NATIVE_DRIVERS On)
set(BUILD_STARTUP On)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_NV_COUNTERS On)
set(BUILD_CMSIS_DRIVERS On)
set(BUILD_TIME Off)
set(BUILD_UART_STDOUT On)
set(BUILD_FLASH On)
set(BUILD_PLAT_TEST Off)
set(BUILD_BOOT_HAL On)

if (MCUBOOT_HW_KEY)
	set(BUILD_TARGET_HARDWARE_KEYS On)
else()
	set(BUILD_TARGET_HARDWARE_KEYS Off)
endif()

if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()

#Add platform specific definitions in SPE
if (DEFINED TFM_PLATFORM_SECURE_DEFS)
	embedded_set_target_compile_defines(TARGET ${PROJECT_NAME} LANGUAGE C DEFINES ${TFM_PLATFORM_SECURE_DEFS} APPEND)
	embedded_set_target_compile_defines(TARGET ${PROJECT_NAME} LANGUAGE ASM DEFINES ${TFM_PLATFORM_SECURE_DEFS} APPEND)
endif()

if (DEFINED CMSE_FLAGS)
	embedded_set_target_compile_flags(TARGET ${PROJECT_NAME} LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C
		"${TFM_ROOT_DIR}/bl2/ext/mcuboot/bl2_main.c"
		"${TFM_ROOT_DIR}/bl2/ext/mcuboot/flash_map_extended.c"
		"${TFM_ROOT_DIR}/bl2/ext/mcuboot/flash_map_legacy.c"
		"${TFM_ROOT_DIR}/bl2/ext/mcuboot/keys.c"
		"${TFM_ROOT_DIR}/bl2/src/flash_map.c"
		"${MCUBOOT_DIR}/bootutil/src/loader.c"
		"${MCUBOOT_DIR}/bootutil/src/bootutil_misc.c"
		"${MCUBOOT_DIR}/bootutil/src/image_validate.c"
		"${MCUBOOT_DIR}/bootutil/src/image_rsa.c"
		"${MCUBOOT_DIR}/bootutil/src/tlv.c"
	)

if (MCUBOOT_REPO STREQUAL "TF-M")
	list(APPEND ALL_SRC_C
			"${TFM_ROOT_DIR}/bl2/src/boot_record.c"
			"${TFM_ROOT_DIR}/bl2/src/security_cnt.c"
		)
else()
	list(APPEND ALL_SRC_C
			"${MCUBOOT_DIR}/bootutil/src/swap_scratch.c"
			"${MCUBOOT_DIR}/bootutil/src/swap_move.c"
			"${MCUBOOT_DIR}/bootutil/src/swap_misc.c"
	)
endif()

#Define location of Mbed Crypto source, build, and installation directory.
set(MBEDTLS_CONFIG_FILE "config-rsa.h")
set(MBEDTLS_CONFIG_PATH "${TFM_ROOT_DIR}/bl2/ext/mcuboot/include")
get_filename_component(MBEDCRYPTO_SOURCE_DIR "${TFM_ROOT_DIR}/../mbed-crypto" ABSOLUTE)
if(NOT EXISTS ${MBEDCRYPTO_SOURCE_DIR})
    message(FATAL_ERROR "Missing mbed-crypto. Please clone the mbed-crypto repo to directory \"${MBEDCRYPTO_SOURCE_DIR}\".")
endif()
set (MBEDCRYPTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbed-crypto/build")
set (MBEDCRYPTO_INSTALL_DIR ${MBEDCRYPTO_BINARY_DIR}/../install)

if (CRYPTO_HW_ACCELERATOR OR CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "PROVISIONING")
	if(NOT DEFINED CRYPTO_HW_ACCELERATOR_CMAKE_BUILD)
		message(FATAL_ERROR "CRYPTO_HW_ACCELERATOR_CMAKE_BUILD not defined.")
	endif()
	include(${CRYPTO_HW_ACCELERATOR_CMAKE_BUILD})
endif()

#Build Mbed Crypto as external project.
#This ensures Mbed Crypto is built with exactly defined settings.
#Mbed Crypto will be used from its install location
string(APPEND MBEDCRYPTO_C_FLAGS " ${MBEDCRYPTO_C_FLAGS_BL2}")
set(MBEDCRYPTO_TARGET_NAME "mbedcrypto_mcuboot_lib")
include(${TFM_ROOT_DIR}/BuildMbedCrypto.cmake)

#Setting include directories
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR}/bl2/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR}/bl2/ext/mcuboot/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${MCUBOOT_DIR}/bootutil/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${MBEDCRYPTO_INSTALL_DIR}/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${CMAKE_CURRENT_BINARY_DIR} ABSOLUTE APPEND)

#Define linker file
if(NOT DEFINED BL2_LINKER_CONFIG)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: BL2_LINKER_CONFIG is not defined.")
endif()
embedded_set_target_linker_file(TARGET ${PROJECT_NAME} PATH "${BL2_LINKER_CONFIG}")

if(NOT DEFINED PLATFORM_LINK_INCLUDES)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: PLATFORM_LINK_INCLUDES is not defined.")
endif()
embedded_set_target_link_includes(TARGET ${PROJECT_NAME} INCLUDES "${PLATFORM_LINK_INCLUDES}")

add_executable(${PROJECT_NAME} ${ALL_SRC_ASM} ${ALL_SRC_C_BL2} ${ALL_SRC_ASM_BL2} ${ALL_SRC_C} ${ALL_SRC_CXX})

#Set common compiler and linker flags
config_setting_shared_compiler_flags(${PROJECT_NAME})
config_setting_shared_linker_flags(${PROJECT_NAME})

#Add BL2 and MCUBOOT_IMAGE_NUMBER defines to linker to resolve symbols in region_defs.h and flash_layout.h
embedded_set_target_link_defines(TARGET ${PROJECT_NAME} DEFINES "BL2" "MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}")

if(NOT DEFINED TEST_FRAMEWORK_S)
	message(FATAL_ERROR "Incomplete build configuration: TEST_FRAMEWORK_S is undefined.")
elseif(TEST_FRAMEWORK_S)
	embedded_set_target_link_defines(TARGET ${PROJECT_NAME} DEFINES "TEST_FRAMEWORK_S")
endif()

if(NOT DEFINED TEST_FRAMEWORK_NS)
	message(FATAL_ERROR "Incomplete build configuration: TEST_FRAMEWORK_NS is undefined.")
elseif(TEST_FRAMEWORK_NS)
	embedded_set_target_link_defines(TARGET ${PROJECT_NAME} DEFINES "TEST_FRAMEWORK_NS")
endif()

#Link mbedcrypto library to project
target_link_libraries(${PROJECT_NAME} "${MBEDCRYPTO_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
add_dependencies(${PROJECT_NAME} ${MBEDCRYPTO_TARGET_NAME}_install)

#Link crypto accelerator libraries if applicable
if (CRYPTO_HW_ACCELERATOR OR CRYPTO_HW_ACCELERATOR_OTP_STATE STREQUAL "PROVISIONING")
	if(NOT DEFINED CRYPTO_HW_ACCELERATOR_CMAKE_LINK)
		message(FATAL_ERROR "CRYPTO_HW_ACCELERATOR_CMAKE_LINK not defined.")
	endif()
	include(${CRYPTO_HW_ACCELERATOR_CMAKE_LINK})
endif()

#Generate binary file from axf
compiler_generate_binary_output(${PROJECT_NAME})

message("- MCUBOOT_REPO: '${MCUBOOT_REPO}'.")
message("- MCUBOOT_IMAGE_NUMBER: '${MCUBOOT_IMAGE_NUMBER}'.")
message("- MCUBOOT_UPGRADE_STRATEGY: '${MCUBOOT_UPGRADE_STRATEGY}'.")
message("- MCUBOOT_SIGNATURE_TYPE: '${MCUBOOT_SIGNATURE_TYPE}'.")
message("- MCUBOOT_HW_KEY: '${MCUBOOT_HW_KEY}'.")
message("- MCUBOOT_LOG_LEVEL: '${MCUBOOT_LOG_LEVEL}'.")

get_property(_log_levels CACHE MCUBOOT_LOG_LEVEL PROPERTY STRINGS)
list(FIND _log_levels ${MCUBOOT_LOG_LEVEL} LOG_LEVEL_ID)

if (MCUBOOT_REPO STREQUAL "UPSTREAM")
	set(MCUBOOT_USE_UPSTREAM On)
endif()

if(MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-3072")
	set(MCUBOOT_SIGN_RSA On)
	set(MCUBOOT_SIGN_RSA_LEN 3072)
elseif(MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-2048")
	set(MCUBOOT_SIGN_RSA On)
	set(MCUBOOT_SIGN_RSA_LEN 2048)
endif()

if (${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "OVERWRITE_ONLY")
	set(MCUBOOT_OVERWRITE_ONLY On)
elseif(${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "NO_SWAP")
	set(MCUBOOT_NO_SWAP On)
elseif (${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "RAM_LOADING")
	set(MCUBOOT_RAM_LOADING On)
endif()

configure_file("${CMAKE_CURRENT_LIST_DIR}/include/mcuboot_config/mcuboot_config.h.in"
			   "${CMAKE_CURRENT_BINARY_DIR}/mcuboot_config/mcuboot_config.h"
			   @ONLY)

target_compile_definitions(${PROJECT_NAME} PRIVATE MBEDTLS_CONFIG_FILE="${MBEDTLS_CONFIG_FILE}")

if (NOT MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-3072" AND NOT MCUBOOT_SIGNATURE_TYPE STREQUAL "RSA-2048")
	message(FATAL_ERROR "${MCUBOOT_SIGNATURE_TYPE} is not supported as firmware signing algorithm")
endif()

validate_cache_value(MCUBOOT_UPGRADE_STRATEGY)

#Set install location. Keep original value to avoid overriding command line settings.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install location for MCUBoot." FORCE)
endif()

#Collect executables to common location: build/install/outputs/
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.axf
			  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
		DESTINATION outputs/${TARGET_PLATFORM}/)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.axf
			  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
		DESTINATION outputs/fvp/)

#Finally let cmake system apply changes after the whole project is defined.
embedded_project_end(${PROJECT_NAME})
