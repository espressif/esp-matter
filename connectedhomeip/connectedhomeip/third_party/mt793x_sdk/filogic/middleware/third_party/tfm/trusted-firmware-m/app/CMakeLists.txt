#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

set(TFM_BUILD_IN_SPE OFF)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

set(APP_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(TFM_ROOT_DIR ${APP_DIR}/.. ABSOLUTE)
set(INTERFACE_DIR ${TFM_ROOT_DIR}/interface)
set(TEST_DIR ${TFM_ROOT_DIR}/test)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_ns LANGUAGES ASM C)
embedded_project_fixup()

#Include BL2 bootloader related functions
set(MCUBOOT_DIR "${TFM_ROOT_DIR}/bl2/ext/mcuboot")
include("${MCUBOOT_DIR}/MCUBoot.cmake")

#Set variables
get_filename_component(CMSIS_5_DIR ${TFM_ROOT_DIR}/../CMSIS_5 ABSOLUTE)

if(NOT EXISTS ${CMSIS_5_DIR})
#	message(FATAL_ERROR "Missing CMSIS_5. Please clone the CMSIS_5 repo to directory \"${CMSIS_5_DIR}\".")
endif()

if (NOT DEFINED BL2)
	message(FATAL_ERROR "Incomplete build configuration: BL2 is undefined. ")
endif ()

if (NOT DEFINED TFM_PARTITION_AUDIT_LOG)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_AUDIT_LOG is undefined.")
endif()

if (NOT DEFINED TFM_PARTITION_PLATFORM)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_PLATFORM is undefined.")
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

if (NOT DEFINED TFM_PSA_API)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PSA_API is undefined.")
endif()

if(EXISTS ${CMSIS_5_DIR})
embedded_include_directories(PATH ${TFM_ROOT_DIR}/app ABSOLUTE)

set(NS_APP_SRC "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Config/RTX_Config.c"
	"${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Source/rtx_lib.c"
	"${APP_DIR}/main_ns.c"
	"${APP_DIR}/tfm_integ_test.c"
	"${APP_DIR}/os_wrapper_cmsis_rtos_v2.c"
	"${TFM_ROOT_DIR}/interface/src/log/tfm_log_raw.c"
	)
endif()

set(NS_APPINT_SRC "${APP_DIR}/os_wrapper_cmsis_rtos_v2.c")

if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
	list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_ns_interface.c")
endif()

if (TFM_PARTITION_AUDIT_LOG)
	if (TFM_PSA_API)
		message(FATAL_ERROR "Audit log has not been supported in IPC model yet.")
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_audit_func_api.c")
	endif()
endif()

if (TFM_PARTITION_PLATFORM)
	if (TFM_PSA_API)
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_platform_ipc_api.c")
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_platform_func_api.c")
	endif()
endif()

if (TFM_PARTITION_SECURE_STORAGE)
	if (TFM_PSA_API)
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_sst_ipc_api.c")
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_sst_func_api.c")
	endif()
endif()

if (TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
	if (TFM_PSA_API)
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_its_ipc_api.c")
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_its_func_api.c")
	endif()
endif()

if (TFM_PARTITION_CRYPTO)
	if (TFM_PSA_API)
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_crypto_ipc_api.c")
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_crypto_func_api.c")
	endif()
endif()

if (TFM_PARTITION_INITIAL_ATTESTATION)
	if (TFM_PSA_API)
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_initial_attestation_ipc_api.c")
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_initial_attestation_func_api.c")
	endif()
endif()

if (NOT DEFINED TFM_NS_CLIENT_IDENTIFICATION)
	message(FATAL_ERROR "Incomplete build configuration: TFM_NS_CLIENT_IDENTIFICATION is undefined.")
elseif (TFM_NS_CLIENT_IDENTIFICATION)
	list(APPEND NS_APPINT_SRC
		"${INTERFACE_DIR}/src/tfm_nspm_svc_handler.c"
		"${INTERFACE_DIR}/src/tfm_nspm_api.c"
		)
endif()

if (PSA_API_TEST_NS)
	list(APPEND NS_APP_SRC "${APP_DIR}/psa_api_test.c")
endif()

if (TFM_PSA_API)
	if (DEFINED TFM_MULTI_CORE_TOPOLOGY AND TFM_MULTI_CORE_TOPOLOGY)
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_ns_mailbox.c"
			"${INTERFACE_DIR}/src/tfm_multi_core_api.c"
			"${INTERFACE_DIR}/src/tfm_multi_core_psa_ns_api.c"
			)

		if (TFM_MULTI_CORE_TEST)
			add_definitions(-DTFM_MULTI_CORE_TEST)
		endif()
	else()
		list(APPEND NS_APPINT_SRC "${INTERFACE_DIR}/src/tfm_psa_ns_api.c")
	endif()
endif()

set(BUILD_CMSIS_CORE On)
set(BUILD_RETARGET On)
set(BUILD_NATIVE_DRIVERS On)
set(BUILD_TIME On)
set(BUILD_STARTUP On)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_HARDWARE_KEYS Off)
set(BUILD_TARGET_NV_COUNTERS Off)
set(BUILD_CMSIS_DRIVERS On)
set(BUILD_UART_STDOUT Off)
set(BUILD_FLASH Off)
if(CORE_TEST_POSITIVE)
	set(BUILD_PLAT_TEST On)
	set(BUILD_TIME On)
else()
	set(BUILD_PLAT_TEST Off)
	set(BUILD_TIME Off)
endif()
if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()

if(NOT DEFINED NS_SCATTER_FILE_NAME)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: NS_SCATTER_FILE_NAME not defined, Include this file from a Config*.cmake")
endif()
embedded_set_target_linker_file(TARGET ${PROJECT_NAME} PATH  "${NS_SCATTER_FILE_NAME}")

#Create an object library to avoid compiling all source files twice, when two executables
#with different memory map need to be linked(BL2 non-swapping)
set(PROJECT_OBJ_LIB ${PROJECT_NAME}_obj_lib)
set(PROJECT_INT_LIB ${PROJECT_NAME}_int_lib)
add_library(${PROJECT_OBJ_LIB} OBJECT ${ALL_SRC_C} ${ALL_SRC_C_NS} ${ALL_SRC_ASM} ${ALL_SRC_ASM_NS} ${NS_APP_SRC} ${NS_APPINT_SRC})
add_library(${PROJECT_INT_LIB} STATIC ${NS_APPINT_SRC})

#Set common compiler flags
config_setting_shared_compiler_flags(${PROJECT_OBJ_LIB})
config_setting_shared_compiler_flags(${PROJECT_INT_LIB})
#Set macro definitions
set(TARGET_COMPILE_DEFINITIONS __thumb2__ __DOMAIN_NS=1 DOMAIN_NS=__DOMAIN_NS)
target_compile_definitions(${PROJECT_OBJ_LIB} PRIVATE ${TARGET_COMPILE_DEFINITIONS})
target_compile_definitions(${PROJECT_INT_LIB} PRIVATE ${TARGET_COMPILE_DEFINITIONS})

#Set include directories.
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${TEST_INTERFACE_DIR}/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${INTERFACE_DIR}/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${TFM_ROOT_DIR}/secure_fw/core/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/Include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Config  ABSOLUTE APPEND)

embedded_include_directories(PATH ${TEST_INTERFACE_DIR}/include ABSOLUTE APPEND)
embedded_include_directories(PATH ${INTERFACE_DIR}/include ABSOLUTE APPEND)
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE APPEND)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/include ABSOLUTE APPEND)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../build/inc ABSOLUTE APPEND)

if (NOT DEFINED TFM_NS_CLIENT_IDENTIFICATION)
	message(FATAL_ERROR "Incomplete build configuration: TFM_NS_CLIENT_IDENTIFICATION is undefined.")
elseif (TFM_NS_CLIENT_IDENTIFICATION)
	target_compile_definitions(${PROJECT_OBJ_LIB} PRIVATE TFM_NS_CLIENT_IDENTIFICATION)
	target_compile_definitions(${PROJECT_INT_LIB} PRIVATE TFM_NS_CLIENT_IDENTIFICATION)
endif()

add_subdirectory(${TFM_ROOT_DIR}/test ${CMAKE_BINARY_DIR}/test/non_secure_test)

# For the non-swapping BL2 configuration two executables need to be built.
# One can be executed from the primary slot flash partition and other from the
# secondary slot. Only the linking phase is different. This function captures
# common settings and eliminates copy-paste.
function(set_up_app_build)
	set( _OPTIONS_ARGS)                                                            #Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS NS_TARGET S_TARGET FULL_BIN SIGN_BIN VENEER_NAME POSTFIX) #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS LINK_DEFINES)                                           #List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN})

	if (NOT DEFINED _MY_PARAMS_NS_TARGET)
		message(FATAL_ERROR "set_up_app_build(): mandatory parameter 'NS_TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_S_TARGET)
		message(FATAL_ERROR "set_up_app_build(): mandatory parameter 'S_TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_FULL_BIN)
		message(FATAL_ERROR "set_up_app_build(): mandatory parameter 'FULL_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_SIGN_BIN)
		message(FATAL_ERROR "set_up_app_build(): mandatory parameter 'SIGN_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_VENEER_NAME)
		message(FATAL_ERROR "set_up_app_build(): mandatory parameter 'VENEER_NAME' missing.")
	endif()

	set(EXE_NAME ${_MY_PARAMS_NS_TARGET}${_MY_PARAMS_POSTFIX})
	set(S_BIN ${_MY_PARAMS_S_TARGET}${_MY_PARAMS_POSTFIX})
	set(FULL_NAME ${_MY_PARAMS_FULL_BIN}${_MY_PARAMS_POSTFIX})
	set(SIGN_NAME ${_MY_PARAMS_SIGN_BIN}${_MY_PARAMS_POSTFIX})
	set(VENEER_NAME ${_MY_PARAMS_VENEER_NAME}${_MY_PARAMS_POSTFIX}.o)

	#Create linker target: add object library to executable
	add_executable(${EXE_NAME} $<TARGET_OBJECTS:${PROJECT_OBJ_LIB}>)

	#Set common linker flags
	config_setting_shared_linker_flags(${EXE_NAME})

	#Set individual linker flags per linker target/executable
	foreach(flag ${_MY_PARAMS_LINK_DEFINES})
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "${flag}")
	endforeach(flag)

	embedded_set_target_linker_file(TARGET ${EXE_NAME} PATH "${NS_SCATTER_FILE_NAME}")

	#Add the RTX library
	if(NOT DEFINED RTX_LIB_PATH)
		message(FATAL_ERROR "ERROR: Incomplete Configuration: RTX_LIB_PATH is not defined.")
	endif()
	target_link_libraries(${EXE_NAME} "${RTX_LIB_PATH}")

	#Add the PSA API compliance test libraries
	if(PSA_API_TEST_NS)
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/val/val_nspe.a")
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/platform/pal_nspe.a")
	endif()
	if(PSA_API_TEST_NS AND (PSA_API_TEST_INTERNAL_TRUSTED_STORAGE OR PSA_API_TEST_PROTECTED_STORAGE OR PSA_API_TEST_STORAGE))
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/dev_apis/storage/test_combine.a")
	endif()
	if(PSA_API_TEST_NS AND PSA_API_TEST_CRYPTO)
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/dev_apis/crypto/test_combine.a")
	endif()
	if(PSA_API_TEST_NS AND PSA_API_TEST_INITIAL_ATTESTATION)
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/dev_apis/initial_attestation/test_combine.a")
	endif()
	if(PSA_API_TEST_NS AND PSA_API_TEST_IPC)
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/ff/ipc/test_combine.a")
	endif()

	if(NOT DEFINED PLATFORM_LINK_INCLUDES)
		message(FATAL_ERROR "ERROR: Incomplete Configuration: PLATFORM_LINK_INCLUDES is not defined.")
	endif()
	embedded_set_target_link_includes(TARGET ${EXE_NAME} INCLUDES "${PLATFORM_LINK_INCLUDES}")

	#Generate binary file from axf
	compiler_generate_binary_output(${EXE_NAME})

	#Generate MCUBoot compatible payload
	if (BL2)
		mcuboot_create_boot_payload(S_BIN    ${S_BIN}
									NS_BIN   ${EXE_NAME}
									FULL_BIN ${FULL_NAME}
									SIGN_BIN ${SIGN_NAME}
									POSTFIX  ${_MY_PARAMS_POSTFIX})
	endif()

	if (NOT DEFINED TFM_PARTITION_TEST_CORE)
		message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_CORE is undefined. ")
	elseif (TFM_PARTITION_TEST_CORE)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_CORE")
	endif()

	if (NOT DEFINED TFM_PARTITION_TEST_CORE_IPC)
		message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_CORE_IPC is undefined.")
	elseif (TFM_PARTITION_TEST_CORE_IPC)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_CORE_IPC")
	endif()

	if (NOT DEFINED TFM_PARTITION_TEST_SECURE_SERVICES)
		message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_SECURE_SERVICES is undefined. ")
	elseif (TFM_PARTITION_TEST_SECURE_SERVICES)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_SECURE_SERVICES")
	endif()

	if (NOT DEFINED TEST_FRAMEWORK_S)
		message(FATAL_ERROR "Incomplete build configuration: TEST_FRAMEWORK_S is undefined.")
	elseif (TEST_FRAMEWORK_S)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TEST_FRAMEWORK_S")
	endif()

	if (NOT DEFINED TEST_FRAMEWORK_NS)
		message(FATAL_ERROR "Incomplete build configuration: TEST_FRAMEWORK_NS is undefined.")
	elseif (TEST_FRAMEWORK_NS)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TEST_FRAMEWORK_NS")
	endif()

	#Set BL2 specific settings.
	if (BL2)
		#Add BL2 and MCUBOOT_IMAGE_NUMBER defines to linker to resolve symbols in region_defs.h and flash_layout.h
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "BL2" "MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}")
	endif()

	#We depend on the non secure tests. See if the library target is available.
	if(TARGET tfm_non_secure_tests)
		#If yes, then use the library.
		target_link_libraries(${EXE_NAME} tfm_non_secure_tests)
		#Ensure library is built first.
		add_dependencies(${EXE_NAME} tfm_non_secure_tests)
	endif()

	#Ensure secure_fw is built before our executable.
	add_dependencies(${EXE_NAME} ${S_BIN})

	if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
		if (NOT DEFINED S_VENEER_PATH)
			if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../secure_fw)
				set (S_VENEER_PATH "${CMAKE_CURRENT_BINARY_DIR}/../secure_fw")
			else()
				message(FATAL_ERROR "No valid path for S_VENEER_PATH, secure_fw is built?")
			endif()
		endif()

		#Add the veneers to the executable.
		set(S_VENEER_FILE "${S_VENEER_PATH}/${VENEER_NAME}")
		set_property(TARGET ${EXE_NAME} APPEND PROPERTY LINK_LIBRARIES ${S_VENEER_FILE})
	endif()

	#Collect executables to common location: build/install/outputs/
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.axf
				  ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.bin
			DESTINATION outputs/${TARGET_PLATFORM}/)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.axf
				  ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.bin
			DESTINATION outputs/fvp/)
endfunction()
if(EXISTS ${CMSIS_5_DIR})
if (LINK_TO_BOTH_MEMORY_REGION)
	#Link to primary memory region
	set_up_app_build(NS_TARGET     ${PROJECT_NAME}
					 S_TARGET      tfm_s
					 FULL_BIN      tfm_full
					 SIGN_BIN      tfm_sign
					 VENEER_NAME   s_veneers)

	#Link to secondary memory region(add extra linker flag)
	set_up_app_build(NS_TARGET     ${PROJECT_NAME}
					 LINK_DEFINES  "LINK_TO_SECONDARY_PARTITION"
					 S_TARGET      tfm_s
					 FULL_BIN      tfm_full
					 SIGN_BIN      tfm_sign
					 VENEER_NAME   s_veneers
					 POSTFIX       "_1")
else()
	#Link to primary memory region only
	set_up_app_build(NS_TARGET     ${PROJECT_NAME}
					 S_TARGET      tfm_s
					 FULL_BIN      tfm_full
					 SIGN_BIN      tfm_sign
					 VENEER_NAME   s_veneers)
endif()

#If the tfm_non_secure_tests target is not available
if(NOT TARGET tfm_non_secure_tests)
	#Add the test source to the build.
	#As of today since secure_fw is built as a sub-project this code will never execute.
	option(ENABLE_SECURE_STORAGE_SERVICE_TESTS "" TRUE)
	include(../test/CMakeLists.inc)
	target_sources(${PROJECT_OBJ_LIB} PUBLIC ${ALL_SRC_C} ${ALL_SRC_C_NS})
endif()

endif()
#Finally let CMake system apply changes after the whole project is defined.
if (TARGET ${PROJECT_NAME})
	embedded_project_end(${PROJECT_NAME})
endif()

if (TARGET ${PROJECT_NAME}_1)
	embedded_project_end(${PROJECT_NAME}_1)
endif()

embedded_project_end(${PROJECT_OBJ_LIB})
