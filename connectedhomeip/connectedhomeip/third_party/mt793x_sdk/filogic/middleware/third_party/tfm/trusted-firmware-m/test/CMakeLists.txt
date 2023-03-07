#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

#Tell TFM Root before calling sub cmake
get_filename_component(TFM_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_tests LANGUAGES ASM C)
embedded_project_fixup()

#Check incoming configuration options
if (NOT DEFINED SERVICES_TEST_ENABLED)
	message(FATAL_ERROR "Incomplete build configuration: SERVICES_TEST_ENABLED is undefined. ")
endif()

if (NOT DEFINED CORE_TEST_INTERACTIVE)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST_INTERACTIVE is undefined. ")
endif()

if (NOT DEFINED CORE_TEST_POSITIVE)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST_POSITIVE is undefined. ")
endif()

if (NOT DEFINED TFM_LVL)
	message(FATAL_ERROR "Incomplete build configuration: TFM_LVL is undefined. ")
endif()

if (NOT DEFINED TFM_PARTITION_AUDIT_LOG)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_AUDIT_LOG is undefined.")
endif()

if (NOT DEFINED TFM_PARTITION_SECURE_STORAGE)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_SECURE_STORAGE is undefined.")
endif()

if (NOT DEFINED TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_INTERNAL_TRUSTED_STORAGE is undefined.")
endif()

if (NOT DEFINED TFM_PARTITION_CRYPTO)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_CRYPTO is undefined.")
endif()

if (NOT DEFINED TFM_PARTITION_INITIAL_ATTESTATION)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_INITIAL_ATTESTATION is undefined.")
endif()

if (NOT DEFINED TFM_ENABLE_IRQ_TEST)
	message(FATAL_ERROR "Incomplete build configuration: TFM_ENABLE_IRQ_TEST is undefined.")
endif()

#Configure our options as needed.
if (CORE_TEST_INTERACTIVE OR CORE_TEST_POSITIVE)
	set(ENABLE_CORE_TESTS True)
	set(ENABLE_CORE_TESTS_2 True)
else()
	set(ENABLE_CORE_TESTS False)
	set(ENABLE_CORE_TESTS_2 False)
endif()

if (TFM_ENABLE_IRQ_TEST)
	set(ENABLE_IRQ_TEST_SERVICES ON)
else()
	set(ENABLE_IRQ_TEST_SERVICES OFF)
endif()

if (ENABLE_CORE_TESTS)
	# If the platform doesn't specify whether the peripheral test is enabled
	# or not, select it by default.
	if (NOT DEFINED TFM_ENABLE_PERIPH_ACCESS_TEST)
		set(TFM_ENABLE_PERIPH_ACCESS_TEST TRUE)
	endif()

	if (TFM_ENABLE_PERIPH_ACCESS_TEST)
		add_definitions(-DTFM_ENABLE_PERIPH_ACCESS_TEST)
	endif()
else()
	set(TFM_ENABLE_PERIPH_ACCESS_TEST FALSE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/TestConfig.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/CMakeLists.inc)

if (ENABLE_SECURE_STORAGE_SERVICE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_SECURE_STORAGE_SERVICE_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_SECURE_STORAGE_SERVICE_TESTS APPEND)
endif()

if (ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS APPEND)
endif()

if (ENABLE_CRYPTO_SERVICE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_CRYPTO_SERVICE_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_CRYPTO_SERVICE_TESTS APPEND)
endif()

if (ENABLE_ATTESTATION_SERVICE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_ATTESTATION_SERVICE_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_ATTESTATION_SERVICE_TESTS APPEND)
endif()

if (ENABLE_PLATFORM_SERVICE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_PLATFORM_SERVICE_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_PLATFORM_SERVICE_TESTS APPEND)
endif()

if (ENABLE_QCBOR_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_QCBOR_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_QCBOR_TESTS APPEND)
endif()

if (ENABLE_T_COSE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_T_COSE_TESTS APPEND)
endif()

if (ENABLE_AUDIT_LOGGING_SERVICE_TESTS)
	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES ENABLE_AUDIT_LOGGING_SERVICE_TESTS APPEND)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ENABLE_AUDIT_LOGGING_SERVICE_TESTS APPEND)
endif()

if (TFM_MULTI_CORE_TEST)
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES TFM_MULTI_CORE_TEST APPEND)
endif()

if (NOT DEFINED TFM_BUILD_IN_SPE)
	message(FATAL_ERROR "TFM_BUILD_IN_SPE is not set. Cannot specify current building status")
endif()

if (NOT TARGET tfm_t_cose_verify AND (ENABLE_ATTESTATION_SERVICE_TESTS OR ENABLE_T_COSE_TESTS))
	add_subdirectory(${TFM_ROOT_DIR}/lib/ext/t_cose ${CMAKE_CURRENT_BINARY_DIR}/t_cose)
endif()

if ((NOT TARGET tfm_qcbor_encode OR NOT TARGET tfm_qcbor_decode) AND (ENABLE_ATTESTATION_SERVICE_TESTS OR ENABLE_QCBOR_TESTS OR ENABLE_T_COSE_TESTS))
	add_subdirectory(${TFM_ROOT_DIR}/lib/ext/qcbor ${CMAKE_CURRENT_BINARY_DIR}/qcbor)
endif()

if (TFM_BUILD_IN_SPE)
	#Build the secure library. Even though secure tests files depend on
	#tfm_qcbor, this is not expressed here as the tfm_attest library is expected
	#to hold the compiled tfm_qcbor files.
	add_library(tfm_secure_tests STATIC ${ALL_SRC_C} ${ALL_SRC_C_S})
	if (ENABLE_ATTESTATION_SERVICE_TESTS)
        target_sources(tfm_secure_tests PRIVATE $<TARGET_OBJECTS:tfm_t_cose_verify> PRIVATE $<TARGET_OBJECTS:tfm_t_cose_utility> $<TARGET_OBJECTS:tfm_qcbor_decode> $<TARGET_OBJECTS:tfm_qcbor_utility>)
	endif()

	#Set common compiler and linker flags
	if (DEFINED CMSE_FLAGS)
		embedded_set_target_compile_flags(TARGET tfm_secure_tests LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
	endif()
	config_setting_shared_compiler_flags(tfm_secure_tests)
	config_setting_shared_linker_flags(tfm_secure_tests)

	embedded_set_target_compile_defines(TARGET tfm_secure_tests LANGUAGE C DEFINES __thumb2__ TFM_LVL=${TFM_LVL} APPEND)

	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE
			PATH "Default install location for tfm_storage."
			FORCE)
	endif()

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/framework/test_framework_integ_test.h
			${CMAKE_CURRENT_SOURCE_DIR}/test_services/tfm_secure_client_service/tfm_secure_client_service_api.h
			DESTINATION export/tfm/test/inc)

	embedded_project_end(tfm_secure_tests)
else ()
	#Build the non-secure library
	set(CMAKE_STATIC_LIBRARY_PREFIX_C "lib")
	add_library(tfm_non_secure_tests STATIC ${ALL_SRC_C} ${ALL_SRC_C_NS})
	if (ENABLE_ATTESTATION_SERVICE_TESTS OR ENABLE_T_COSE_TESTS)
        target_sources(tfm_non_secure_tests PRIVATE $<TARGET_OBJECTS:tfm_t_cose_verify> PRIVATE $<TARGET_OBJECTS:tfm_t_cose_sign> PRIVATE $<TARGET_OBJECTS:tfm_t_cose_utility>)
	endif()
	if (ENABLE_ATTESTATION_SERVICE_TESTS OR ENABLE_QCBOR_TESTS OR ENABLE_T_COSE_TESTS)
        target_sources(tfm_non_secure_tests PRIVATE $<TARGET_OBJECTS:tfm_qcbor_decode> PRIVATE $<TARGET_OBJECTS:tfm_qcbor_encode> PRIVATE $<TARGET_OBJECTS:tfm_qcbor_utility>)
	endif()

	#Set common compiler and linker flags
	config_setting_shared_compiler_flags(tfm_non_secure_tests)
	config_setting_shared_linker_flags(tfm_non_secure_tests)

	#Set macro definitions
	set(TARGET_COMPILE_DEFINITIONS __thumb2__ __DOMAIN_NS=1 DOMAIN_NS=__DOMAIN_NS TFM_LVL=${TFM_LVL})
	embedded_set_target_compile_defines(TARGET tfm_non_secure_tests LANGUAGE C DEFINES ${TARGET_COMPILE_DEFINITIONS} APPEND)

	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE
			PATH "Default install location for tfm_storage."
			FORCE)
	endif()

	install(TARGETS tfm_non_secure_tests
			DESTINATION export/tfm/test/lib
			PUBLIC_HEADER DESTINATION export/tfm/test/inc)

	if(ENABLE_SECURE_STORAGE_SERVICE_TESTS)
		#only SST tests are using semaphore and thread APIs
		install(FILES ${TFM_ROOT_DIR}/interface/include/os_wrapper/semaphore.h
				${TFM_ROOT_DIR}/interface/include/os_wrapper/thread.h
			DESTINATION export/tfm/include/os_wrapper)
	endif()

	embedded_project_end(tfm_non_secure_tests)
endif()
