#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

set(TFM_BUILD_IN_SPE ON)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

set(SECURE_FW_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(TFM_ROOT_DIR  "${SECURE_FW_DIR}/..")
set(TEST_DIR      "${TFM_ROOT_DIR}/test")
set(INTERFACE_DIR "${TFM_ROOT_DIR}/interface")
set(PROJECT_DIR "${TFM_ROOT_DIR}/../../../..")
set(OUT_DIR "${OUTPATH}")
set(OUT_DRIVER_DIR "${OUT_DIR}/obj/${OUT_FILE_NAME}/driver/chip/mt7933")
set(OUT_BOOTLOADER_DIR "${OUT_DIR}/obj/bootloader/project/mt7933_hdk/apps/bootloader")
set(OUT_PROJECT_DIR "${OUT_DIR}/obj/${OUT_FILE_NAME}/project/mt7933_hdk/apps/${BOARD_TYPE}")

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_s LANGUAGES ASM C)
embedded_project_fixup()

#Include functionality to enable building the documentation.
include("Common/BuildDoxygenDoc")
include("Common/BuildSphinxDoc")

if (NOT DEFINED TFM_LVL)
	message(FATAL_ERROR "Incomplete build configuration: TFM_LVL is undefined. ")
endif()

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

if (NOT DEFINED TFM_PARTITION_TEST_CORE)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_CORE is undefined. ")
endif()

if (NOT DEFINED TFM_PARTITION_TEST_CORE_IPC)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_CORE_IPC is undefined.")
endif()

if (NOT DEFINED TFM_PARTITION_TEST_SECURE_SERVICES)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_SECURE_SERVICES is undefined. ")
endif()

if (NOT DEFINED TFM_PARTITION_TEST_SST)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_SST is undefined.")
endif()

if (NOT DEFINED TEST_FRAMEWORK_S)
	message(FATAL_ERROR "Incomplete build configuration: TEST_FRAMEWORK_S is undefined.")
endif()

if (NOT DEFINED TEST_FRAMEWORK_NS)
	message(FATAL_ERROR "Incomplete build configuration: TEST_FRAMEWORK_NS is undefined.")
endif()

if (NOT DEFINED BL2)
	message(FATAL_ERROR "Incomplete build configuration: BL2 is undefined. ")
endif()

if (NOT DEFINED TFM_PSA_API)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PSA_API is undefined. ")
endif()

if(NOT DEFINED PLATFORM_LINK_INCLUDES)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: PLATFORM_LINK_INCLUDES is not defined.")
endif()

if (NOT DEFINED TFM_ENABLE_IRQ_TEST)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: TFM_ENABLE_IRQ_TEST is not defined.")
endif()

include(${SECURE_FW_DIR}/spm/CMakeLists.inc)
if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
	include(${SECURE_FW_DIR}/ns_callable/CMakeLists.inc)
endif ()
#Involve all IPC related sources in ipc's CMakeLists.inc, and switch core between IPC and Library.
if(TFM_PSA_API)
	include(${SECURE_FW_DIR}/core/ipc/CMakeLists.inc)
else()
	include(${SECURE_FW_DIR}/core/CMakeLists.inc)
endif()
include(${SECURE_FW_DIR}/core/arch/CMakeLists.inc)

set(BUILD_CMSIS_CORE On)
set(BUILD_RETARGET On)
set(BUILD_NATIVE_DRIVERS On)
set(BUILD_STARTUP On)
set(BUILD_TARGET_CFG On)
# FIXME: The following TARGET flags are platform dependent.
#        It is required to add a mechanism to expose the
#        target capabilities and, based on them, set the
#        flags properly.
set(BUILD_TARGET_HARDWARE_KEYS On)
set(BUILD_TARGET_NV_COUNTERS On)
set(BUILD_CMSIS_DRIVERS On)
set(BUILD_TIME Off)
set(BUILD_UART_STDOUT On)
set(BUILD_FLASH On)
set(BUILD_PLAT_TEST Off)
set(TFM_INIT_PSRAM Off)
set(TFM_DEEPSLEEP_WAKEUP On)

if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()

if(NOT DEFINED S_SCATTER_FILE_NAME)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: S_SCATTER_FILE_NAME not defined, Include this file from a Config*.cmake")
endif()
embedded_set_target_linker_file(TARGET ${PROJECT_NAME} PATH "${S_SCATTER_FILE_NAME}")

embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)
#Create an object library to avoid compiling all source files twice, when two executables
#with different memory map need to be linked(BL2 non-swapping)
set(PROJECT_OBJ_LIB ${PROJECT_NAME}_obj_lib)
add_library(${PROJECT_OBJ_LIB} OBJECT ${ALL_SRC_C} ${ALL_SRC_C_S} ${ALL_SRC_ASM} ${ALL_SRC_ASM_S})

#Add platform specific definitions in SPE
if (DEFINED TFM_PLATFORM_SECURE_DEFS)
	embedded_set_target_compile_defines(TARGET ${PROJECT_OBJ_LIB} LANGUAGE C DEFINES ${TFM_PLATFORM_SECURE_DEFS} APPEND)
	embedded_set_target_compile_defines(TARGET ${PROJECT_OBJ_LIB} LANGUAGE ASM DEFINES ${TFM_PLATFORM_SECURE_DEFS} APPEND)
endif()

#Set common compiler flags
if (DEFINED CMSE_FLAGS)
	embedded_set_target_compile_flags(TARGET ${PROJECT_OBJ_LIB} LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()
config_setting_shared_compiler_flags(${PROJECT_OBJ_LIB})

if(NOT DEFINED TARGET_NV_COUNTERS_ENABLE)
	set(TARGET_NV_COUNTERS_ENABLE OFF)
endif()

if(TARGET_NV_COUNTERS_ENABLE)
	embedded_set_target_compile_defines(TARGET ${PROJECT_OBJ_LIB} LANGUAGE C DEFINES TFM_NVCOUNTERS_ENABLE APPEND)
endif()

if(BOOT_DATA_AVAILABLE)
	embedded_set_target_compile_defines(TARGET ${PROJECT_OBJ_LIB} LANGUAGE C DEFINES BOOT_DATA_AVAILABLE APPEND)
endif()

if (NOT DEFINED CORE_TEST)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST is undefined.")
elseif(CORE_TEST)
	embedded_set_target_compile_defines(TARGET ${PROJECT_OBJ_LIB} LANGUAGE C DEFINES TFM_CORE_DEBUG APPEND)
endif()

if (NOT DEFINED TFM_NS_CLIENT_IDENTIFICATION)
	message(FATAL_ERROR "Incomplete build configuration: TFM_NS_CLIENT_IDENTIFICATION is undefined.")
elseif (TFM_NS_CLIENT_IDENTIFICATION)
	target_compile_definitions(${PROJECT_OBJ_LIB} PRIVATE TFM_NS_CLIENT_IDENTIFICATION)
endif()

if (NOT DEFINED DEBUG_AUTHENTICATION)
	set(DEBUG_AUTHENTICATION "DAUTH_CHIP_DEFAULT")
endif()

#Set include directories
embedded_target_include_directories(TARGET ${PROJECT_OBJ_LIB} PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)

# For the non-swapping BL2 configuration two executables need to be built.
# One can be executed from the primary slot flash partition and other from the
# the secondary slot. Only the linking phase is different. This function
# captures common settings and eliminates copy-paste.
function(set_up_secure_fw_build)
	set( _OPTIONS_ARGS)                                #Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS S_TARGET VENEER_NAME POSTFIX) #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS LINK_DEFINES)               #List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN})

	if (NOT DEFINED _MY_PARAMS_S_TARGET)
		message(FATAL_ERROR "set_up_secure_fw_build(): mandatory parameter 'S_TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_VENEER_NAME)
		message(FATAL_ERROR "set_up_secure_fw_build(): mandatory parameter 'VENEER_NAME' missing.")
	endif()

	set(EXE_NAME ${_MY_PARAMS_S_TARGET}${_MY_PARAMS_POSTFIX})
	set(VENEER_NAME ${_MY_PARAMS_VENEER_NAME}${_MY_PARAMS_POSTFIX}.o)

    if (TFM_INIT_PSRAM)
        set (PSRAM_INIT_FILE
            "${OUT_DRIVER_DIR}/src/hal_psram.o"
            "${OUT_DRIVER_DIR}/src/hal_cache.o"
            "${OUT_DRIVER_DIR}/src/hal_wdt.o"
            "${OUT_DRIVER_DIR}/src/hal_wdt_internal.o"
            "${OUT_DRIVER_DIR}/src/hal_clk.o"
            "${OUT_DRIVER_DIR}/src/hal_uart.o"
            "${OUT_DRIVER_DIR}/src/hal_gpt.o"
            "${OUT_DRIVER_DIR}/src/hal_asic_mpu.o"
            "${OUT_DRIVER_DIR}/src/hal_gpt_internal.o"
            "${OUT_DRIVER_DIR}/src/psramc/non_uhs_psram.o"
            "${OUT_DRIVER_DIR}/src/psramc/psramc_pi_main.o"
            "${OUT_DRIVER_DIR}/src/hal_nvic.o"
            "${OUT_DRIVER_DIR}/src/psramc/Hal_io.o"
            "${OUT_DRIVER_DIR}/src/psramc/pemi.o"
            "${OUT_DRIVER_DIR}/src/psramc/psramc_pi_basic_api.o"
            "${OUT_DRIVER_DIR}/src/hal_nvic_internal.o"
            "${OUT_DRIVER_DIR}/src/psramc/psramc_pi_calibration_api.o"
            "${OUT_DRIVER_DIR}/src_core/src/hal_ls_api.o"
            "${OUT_DRIVER_DIR}/src_core/src/hal_security_api.o"
            "${OUT_DRIVER_DIR}/src_core/src/hal_boot.o"
            "${OUT_DRIVER_DIR}/src_core/src/hal_spm.o"
            "${OUT_DRIVER_DIR}/src_core/src/hal_sleep_manager_internal.o"
            "${OUT_BOOTLOADER_DIR}/src/hw_uart.o"
            )
        add_library(tfm_psram STATIC ${PSRAM_INIT_FILE})
        # Drivers in tfm_psram.a have gobal datas, during PSRAM init flow, the data/bss section will be loaded on SYSRAM and need to be cleared after PSRAM init,
        # it will cause the driver can't be used after PSRAM init any more, so we need two driver codes, one for PSRAM init and another used after PSRAM init over,
        # but if we include the same driver codes, it will happen symbol collision(symbol multiple defined), so we add the prefix sysram_ to the driver symbols in tfm_psram.a
        # and keep the symbol names which in gcc libraries.
        add_custom_command(TARGET tfm_psram
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} ARGS --prefix-symbols=sysram_ $<TARGET_FILE:tfm_psram>
            COMMAND ${CMAKE_OBJCOPY} ARGS --redefine-sym sysram_printf=printf $<TARGET_FILE:tfm_psram>
            COMMAND ${CMAKE_OBJCOPY} ARGS --redefine-sym sysram_memset=memset $<TARGET_FILE:tfm_psram>
            COMMAND ${CMAKE_OBJCOPY} ARGS --redefine-sym sysram_memcpy=memcpy $<TARGET_FILE:tfm_psram>
            COMMAND ${CMAKE_OBJCOPY} ARGS --redefine-sym sysram_memcmp=memcmp $<TARGET_FILE:tfm_psram>
            COMMENT "Add prefix sysram_ to tfm_psram.a")

        SET_TARGET_PROPERTIES(tfm_psram PROPERTIES LINKER_LANGUAGE C)

        #Create linker target: add object library to executable
        #add_executable(${EXE_NAME} $<TARGET_OBJECTS:${PROJECT_OBJ_LIB}>)
        #target_link_libraries(${EXE_NAME} tfm_psram)
    endif()

    set (TFM_DRIVER_LIB
        "${OUT_DRIVER_DIR}/src/hal_flash.o"
        "${OUT_DRIVER_DIR}/src/hal_gpt.o"
        "${OUT_DRIVER_DIR}/src/hal_gpt_internal.o"
        "${OUT_DRIVER_DIR}/src/hal_sej_gpt.o"
        "${OUT_DRIVER_DIR}/src/hal_clk.o"
        "${OUT_DRIVER_DIR}/src/hal_uart.o"
        "${OUT_DRIVER_DIR}/src/hal_nvic_internal.o"
        "${OUT_DRIVER_DIR}/src/hal_asic_mpu.o"
        "${OUT_DRIVER_DIR}/src/hal_devapc.o"
        "${OUT_DRIVER_DIR}/src/hal_cache.o"
        "${OUT_DRIVER_DIR}/src/hal_cache_internal.o"
        "${OUT_PROJECT_DIR}/src/sys_init.o"
        "${OUT_DRIVER_DIR}/src/hal_wdt.o"
        "${OUT_DRIVER_DIR}/src/hal_wdt_internal.o"
        "${OUT_DRIVER_DIR}/src/hal_aes.o"
        "${OUT_DRIVER_DIR}/src/hal_sys.o"
    )

    # internal enviroment
    if(EXISTS "${OUT_DRIVER_DIR}/src_core")
        list(APPEND TFM_DRIVER_LIB "${OUT_DRIVER_DIR}/src_core/src/hal_ls_api.o")
        list(APPEND TFM_DRIVER_LIB "${OUT_DRIVER_DIR}/src_core/src/hal_boot.o")
        list(APPEND TFM_DRIVER_LIB "${OUT_DRIVER_DIR}/src_core/src/hal_efuse_get_grp1.o")
        list(APPEND TFM_DRIVER_LIB "${OUT_DRIVER_DIR}/src_core/src/hal_efuse_get_grp2.o")
    endif()

    add_library(tfm_driver_lib STATIC ${TFM_DRIVER_LIB})
    if(NOT EXISTS "${OUT_DRIVER_DIR}/src_core")
        target_link_libraries(tfm_driver_lib ${PROJECT_DIR}/prebuilt/driver/chip/mt7933/lib/libhal_core_CM33_TFM_S_GCC.a)
    endif()

    SET_TARGET_PROPERTIES(tfm_driver_lib PROPERTIES LINKER_LANGUAGE C)

    #Create linker target: add object library to executable
    add_executable(${EXE_NAME} $<TARGET_OBJECTS:${PROJECT_OBJ_LIB}>)
    target_link_libraries(${EXE_NAME} tfm_driver_lib)
    if(EXISTS "${OUT_DRIVER_DIR}/src_core")
        target_link_libraries(${EXE_NAME} ${PROJECT_DIR}/driver/chip/mt7933/src_core/lib/lib_gcpu.a)
    else()
        target_link_libraries(${EXE_NAME} ${PROJECT_DIR}/prebuilt/driver/chip/mt7933/lib/lib_gcpu.a)
    endif()

	#Set common linker flags
	config_setting_shared_linker_flags(${EXE_NAME})

	#Set individual linker flags per linker target/executable
	foreach(flag ${_MY_PARAMS_LINK_DEFINES})
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "${flag}")
	endforeach(flag)

	embedded_set_target_linker_file(TARGET ${EXE_NAME} PATH "${S_SCATTER_FILE_NAME}")

	#Set macro definitions for the project.
	set(TARGET_COMPILE_DEFINITIONS __thumb2__ __DOMAIN_NS=0 DOMAIN_NS=__DOMAIN_NS TFM_LVL=${TFM_LVL} ${DEBUG_AUTHENTICATION})
	embedded_set_target_compile_defines(TARGET ${PROJECT_OBJ_LIB} LANGUAGE C DEFINES ${TARGET_COMPILE_DEFINITIONS} APPEND)

	if (TFM_BUILD_MODE_DEBUG)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_DEBUG_BUILD")
	elseif (TFM_BUILD_MODE_NORMAL)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_NORMAL_BUILD")
	endif()

	# The order of the libraries linked below is important for GCC
	# Because the GCC linker will search an archive only once.
	# If the a library has some symbols that are defined in the library which is linked before it,
	# it will not cause the linker to search again.
	# So please put a library before what it relies on.
	# If the link dependency is complicated or it is difficult to sort out link
	# dependency by only adjusting the order, it is recommended to explicitly
	# add the dependency in the dedicated CMake file of corresponding module, to
	# simplify the ordering and avoid any potential linking issue.
	if (CORE_TEST OR TFM_PARTITION_TEST_SECURE_SERVICES)
		target_link_libraries(${EXE_NAME} tfm_secure_tests)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_SECURE_SERVICES")
	endif()

	if (TFM_PARTITION_INITIAL_ATTESTATION)
		target_link_libraries(${EXE_NAME} tfm_attest)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_INITIAL_ATTESTATION")
	endif()

	if (TFM_PARTITION_SECURE_STORAGE)
		target_link_libraries(${EXE_NAME} tfm_storage)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_SECURE_STORAGE")
	endif()

	if (TFM_PARTITION_CRYPTO)
		target_link_libraries(${EXE_NAME} tfm_crypto)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_CRYPTO")
	endif()

	if (TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
		target_link_libraries(${EXE_NAME} tfm_internal_trusted_storage)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_INTERNAL_TRUSTED_STORAGE")
	endif()

	if (TFM_PARTITION_AUDIT_LOG)
		target_link_libraries(${EXE_NAME} tfm_audit)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_AUDIT_LOG")
	endif()

	if (TFM_PARTITION_PLATFORM)
		target_link_libraries(${EXE_NAME} tfm_platform)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_PLATFORM")
	endif()

    if (TFM_INIT_PSRAM)
        embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_INIT_PSRAM")
    endif()

    if (TARGET_PLATFORM_MT793X)
        embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TARGET_PLATFORM_MT793X")
    endif()

    if (MTK_AUDIO_SUPPORT)
        embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "MTK_AUDIO_SUPPORT")
    endif()

	#Add the PSA API compliance test secure libraries
	if(PSA_API_TEST_IPC)
		add_definitions(-DPSA_API_TEST_IPC)
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/partition/client_partition.a")
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/partition/server_partition.a")
		target_link_libraries(${EXE_NAME} "${PSA_API_TEST_BUILD_PATH}/partition/driver_partition.a")
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "PSA_API_TEST_IPC")
	endif()

	target_link_libraries(${EXE_NAME} libtfmsprt)

    target_link_libraries(${EXE_NAME} lib_mtk_drv)

	embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_LVL=${TFM_LVL}")

	if (TFM_PARTITION_TEST_CORE)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_CORE")
	endif()

	if (TFM_PARTITION_TEST_CORE_IPC)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_CORE_IPC")
	endif()

	if (TFM_PARTITION_TEST_SST)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_SST")
	endif()

	if (TEST_FRAMEWORK_S)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TEST_FRAMEWORK_S")
	endif()

	if (TEST_FRAMEWORK_NS)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TEST_FRAMEWORK_NS")
	endif()

	if (BL2)
		#Add BL2 and MCUBOOT_IMAGE_NUMBER defines to linker to resolve symbols in region_defs.h and flash_layout.h
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "BL2" "MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}")
	endif()

	if (TFM_PSA_API)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PSA_API")
		if (DEFINED TFM_MULTI_CORE_TOPOLOGY AND TFM_MULTI_CORE_TOPOLOGY)
			embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_MULTI_CORE_TOPOLOGY")
		endif()
	endif()

	if(CORE_TEST)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_PARTITION_TEST_CORE")
	endif()

	embedded_set_target_link_includes(TARGET ${EXE_NAME} INCLUDES "${PLATFORM_LINK_INCLUDES}")

	if (TFM_BUILD_MODE_DEBUG)
		if(EXISTS "${HADRON_SDK_DIR}/${PROJECT_TYPE}/GCC/debug/ld")
			embedded_set_target_link_includes(TARGET ${EXE_NAME} INCLUDES "${HADRON_SDK_DIR}/${BOARD_TYPE}/GCC/debug/ld")
		else()
			embedded_set_target_link_includes(TARGET ${EXE_NAME} INCLUDES "${HADRON_SDK_DIR}/${BOARD_TYPE}/GCC/ld")
		endif()
	else()
		embedded_set_target_link_includes(TARGET ${EXE_NAME} INCLUDES "${HADRON_SDK_DIR}/${BOARD_TYPE}/GCC/ld")
	endif()

	if (TFM_ENABLE_IRQ_TEST)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_ENABLE_IRQ_TEST")
	endif()

	if (TFM_MULTI_CORE_TEST)
		add_definitions(-DTFM_MULTI_CORE_TEST)
		embedded_set_target_link_defines(TARGET ${EXE_NAME} DEFINES "TFM_MULTI_CORE_TEST")
	endif()

	#Generate binary file from executable
	compiler_generate_binary_output(${EXE_NAME})


	if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
		#Configure where we put the CMSE veneers generated by the compiler.
		set(S_VENEER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${VENEER_NAME}")
		compiler_set_cmse_output(${EXE_NAME} "${S_VENEER_FILE}")
	endif()

	#Configure what file shall be installed.
	#Set install location. Keep original value to avoid overriding command line settings.
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install location for secure_fw." FORCE)
	endif()

	#Export files needed to interface external applications at: <build_dir>/install/export/tfm/
	set(INTERFACE_INC_DIR "${INTERFACE_DIR}/include")
	set(INTERFACE_SRC_DIR "${INTERFACE_DIR}/src")
	set(EXPORT_INC_DIR "export/tfm/include")
	set(EXPORT_SRC_DIR "export/tfm/src")
	#Headers
	install(FILES       ${INTERFACE_INC_DIR}/psa/client.h
						${INTERFACE_INC_DIR}/psa/error.h
			DESTINATION ${EXPORT_INC_DIR}/psa)

	install(FILES       ${INTERFACE_INC_DIR}/os_wrapper/common.h
						${INTERFACE_INC_DIR}/os_wrapper/mutex.h
			DESTINATION ${EXPORT_INC_DIR}/os_wrapper)

	install(FILES       ${INTERFACE_INC_DIR}/psa_manifest/sid.h
			DESTINATION ${EXPORT_INC_DIR}/psa_manifest)

	install(FILES       ${INTERFACE_INC_DIR}/tfm_api.h
						${INTERFACE_INC_DIR}/tfm_ns_interface.h
						${INTERFACE_INC_DIR}/tfm_ns_svc.h
			DESTINATION ${EXPORT_INC_DIR})

	if (TFM_MULTI_CORE_TOPOLOGY)
		install(FILES       ${INTERFACE_INC_DIR}/tfm_multi_core_api.h
							${INTERFACE_INC_DIR}/tfm_ns_mailbox.h
							${INTERFACE_INC_DIR}/tfm_mailbox.h
				DESTINATION ${EXPORT_INC_DIR})
	else()
		install(FILES       ${INTERFACE_INC_DIR}/tfm_veneers.h
				DESTINATION ${EXPORT_INC_DIR})
	endif()

	#Sources
	if (TFM_MULTI_CORE_TOPOLOGY)
		install(FILES       ${INTERFACE_SRC_DIR}/tfm_ns_mailbox.c
							${INTERFACE_SRC_DIR}/tfm_multi_core_api.c
							${INTERFACE_SRC_DIR}/tfm_multi_core_psa_ns_api.c
				DESTINATION ${EXPORT_SRC_DIR})
	else()
		install(FILES       ${INTERFACE_SRC_DIR}/tfm_ns_interface.c
				DESTINATION ${EXPORT_SRC_DIR})

		if(TFM_PSA_API)
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_psa_ns_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		endif()
	endif()

	if (TFM_NS_CLIENT_IDENTIFICATION)
		install(FILES       ${INTERFACE_INC_DIR}/tfm_nspm_api.h
							${INTERFACE_INC_DIR}/tfm_nspm_svc_handler.h
				DESTINATION ${EXPORT_INC_DIR})
		install(FILES       ${INTERFACE_SRC_DIR}/tfm_nspm_api.c
							${INTERFACE_SRC_DIR}/tfm_nspm_svc_handler.c
				DESTINATION ${EXPORT_SRC_DIR})
	endif()

	if (TFM_PARTITION_SECURE_STORAGE)
		install(FILES       ${INTERFACE_INC_DIR}/psa/protected_storage.h
				DESTINATION ${EXPORT_INC_DIR}/psa)
		if (TFM_PSA_API)
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_sst_ipc_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		else()
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_sst_func_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		endif()
	endif()

	if (TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
		install(FILES       ${INTERFACE_INC_DIR}/psa/internal_trusted_storage.h
							${INTERFACE_INC_DIR}/psa/storage_common.h
				DESTINATION ${EXPORT_INC_DIR}/psa)
		if (TFM_PSA_API)
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_its_ipc_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		else()
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_its_func_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		endif()
	endif()

	if (TFM_PARTITION_CRYPTO)
		install(FILES       ${INTERFACE_INC_DIR}/psa/crypto_extra.h
							${INTERFACE_INC_DIR}/psa/crypto_compat.h
							${INTERFACE_INC_DIR}/psa/crypto.h
							${INTERFACE_INC_DIR}/psa/crypto_platform.h
							${INTERFACE_INC_DIR}/psa/crypto_sizes.h
							${INTERFACE_INC_DIR}/psa/crypto_struct.h
							${INTERFACE_INC_DIR}/psa/crypto_types.h
							${INTERFACE_INC_DIR}/psa/crypto_values.h
				DESTINATION ${EXPORT_INC_DIR}/psa)
		install(FILES       ${INTERFACE_INC_DIR}/tfm_crypto_defs.h
				DESTINATION ${EXPORT_INC_DIR})
		if (TFM_PSA_API)
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_crypto_ipc_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		else()
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_crypto_func_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		endif()
	endif()

	if (TFM_PARTITION_INITIAL_ATTESTATION)
		install(FILES       ${INTERFACE_INC_DIR}/psa/initial_attestation.h
				DESTINATION ${EXPORT_INC_DIR}/psa)
		if (TFM_PSA_API)
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_initial_attestation_ipc_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		else()
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_initial_attestation_func_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		endif()
	endif()

	if(TFM_PARTITION_AUDIT_LOG)
		install(FILES       ${INTERFACE_INC_DIR}/psa_audit_api.h
							${INTERFACE_INC_DIR}/psa_audit_defs.h
				DESTINATION ${EXPORT_INC_DIR})
		install(FILES       ${INTERFACE_SRC_DIR}/tfm_audit_func_api.c
				DESTINATION ${EXPORT_SRC_DIR})
	endif()

	if(TFM_PARTITION_PLATFORM)
		install(FILES       ${INTERFACE_INC_DIR}/tfm_platform_api.h
				DESTINATION ${EXPORT_INC_DIR})
		if(TFM_PSA_API)
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_platform_ipc_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		else()
			install(FILES       ${INTERFACE_SRC_DIR}/tfm_platform_func_api.c
					DESTINATION ${EXPORT_SRC_DIR})
		endif()
	endif()

	if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
		install(FILES ${S_VENEER_FILE} DESTINATION export/tfm/veneers)
	endif()

	#Collect executables to common location: <build_dir>/install/outputs/
	if (DEFINED SECURE_AXF_DIR_PREFIX)
		set(MY_BINARY_DIR ${SECURE_AXF_DIR_PREFIX})
	else()
		set(MY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	install(FILES ${MY_BINARY_DIR}/${EXE_NAME}.axf
				  ${MY_BINARY_DIR}/${EXE_NAME}.bin
			DESTINATION outputs/${TARGET_PLATFORM}/)

	install(FILES ${MY_BINARY_DIR}/${EXE_NAME}.axf
				  ${MY_BINARY_DIR}/${EXE_NAME}.bin
			DESTINATION outputs/fvp/)
endfunction()

#Adds the test directory
add_subdirectory(${TFM_ROOT_DIR}/test ${CMAKE_BINARY_DIR}/test/secure_test)

#Add the crypto library target
if (TFM_PARTITION_CRYPTO)
	add_subdirectory(${SECURE_FW_DIR}/services/crypto)
endif()

#Add the secure storage library target
if (TFM_PARTITION_SECURE_STORAGE)
	add_subdirectory(${SECURE_FW_DIR}/services/secure_storage)
endif()

#Add the internal trusted storage library target
if (TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
	add_subdirectory(${SECURE_FW_DIR}/services/internal_trusted_storage)
endif()

#Add the platform service library target
if (TFM_PARTITION_PLATFORM)
	add_subdirectory(${SECURE_FW_DIR}/services/platform)
endif()

#Add the initial attestation service library target
if (TFM_PARTITION_INITIAL_ATTESTATION)
	add_subdirectory(${SECURE_FW_DIR}/services/initial_attestation)
endif()

#Add the audit logging library target
if (TFM_PARTITION_AUDIT_LOG)
	add_subdirectory(${SECURE_FW_DIR}/services/audit_logging)
endif()

#Add the secure runtime library target
add_subdirectory(${SECURE_FW_DIR}/lib/sprt)

#Add the MTK platform drivers build
add_subdirectory(${SECURE_FW_DIR}/lib/mtk_platform_driver)

if (NOT DEFINED TFM_MULTI_CORE_TOPOLOGY OR NOT TFM_MULTI_CORE_TOPOLOGY)
	#Broadcast veneer path in bundled building case
	set(S_VENEER_PATH "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "")
endif()

if (LINK_TO_BOTH_MEMORY_REGION)
	#Link to primary memory region
	set_up_secure_fw_build(S_TARGET      ${PROJECT_NAME}
						   VENEER_NAME   s_veneers)

	#Link to secondary memory region(add extra linker flag)
	set_up_secure_fw_build(S_TARGET      ${PROJECT_NAME}
						   LINK_DEFINES  "LINK_TO_SECONDARY_PARTITION"
						   VENEER_NAME   s_veneers
						   POSTFIX       "_1")
else()
	#Link to primary memory region only
	set_up_secure_fw_build(S_TARGET      ${PROJECT_NAME}
						   VENEER_NAME   s_veneers)
endif()

#Finally let CMake system apply changes after the whole project is defined.
if (TARGET ${PROJECT_NAME})
	embedded_project_end(${PROJECT_NAME})
endif()

if (TARGET ${PROJECT_NAME}_1)
	embedded_project_end(${PROJECT_NAME}_1)
endif()

embedded_project_end(${PROJECT_OBJ_LIB})
