#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all MPS3/AN524 specific files in the application.

#MPS3/AN524 has a Cortex-M33 CPU.
include("Common/CpuM33")

#Remap option for MPS3, default is BRAM
set(REMAP_QSPI True)
if (REMAP_QSPI)
    add_definitions(-DREMAP_QSPI)
endif()

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

set(AN524_DIR ${PLATFORM_DIR}/target/mps3/an524)

#Specify the location of platform specific build dependencies.
if(COMPILER STREQUAL "ARMCLANG")
    set (BL2_SCATTER_FILE_NAME "${AN524_DIR}/device/source/armclang/mps3_an524_bl2.sct")
    set (S_SCATTER_FILE_NAME "${PLATFORM_DIR}/common/armclang/tfm_common_s.sct")
    set (NS_SCATTER_FILE_NAME "${AN524_DIR}/device/source/armclang/mps3_an524_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_V8MMN.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set (BL2_SCATTER_FILE_NAME "${AN524_DIR}/device/source/gcc/mps3_an524_bl2.ld")
    set (S_SCATTER_FILE_NAME "${PLATFORM_DIR}/common/gcc/tfm_common_s.ld")
    set (NS_SCATTER_FILE_NAME "${AN524_DIR}/device/source/gcc/mps3_an524_ns.ld")
    if (DEFINED CMSIS_5_DIR)
        # Not all projects define CMSIS_5_DIR, only the ones that use it.
        set(RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a")
    endif()
elseif(COMPILER STREQUAL "IARARM")
    set (BL2_SCATTER_FILE_NAME "${AN524_DIR}/device/source/iar/mps3_an524_bl2.icf")
    set (S_SCATTER_FILE_NAME "${PLATFORM_DIR}/common/iar/tfm_common_s.icf")
    set (NS_SCATTER_FILE_NAME "${AN524_DIR}/device/source/iar/mps3_an524_ns.icf")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/IAR/RTX_V8MMN.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT "${AN524_DIR}/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${AN524_DIR}/partition/")

if (BL2)
    set (BL2_LINKER_CONFIG ${BL2_SCATTER_FILE_NAME})
    if (NOT ${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "NO_SWAP")
        message(WARNING "NO_SWAP upgrade strategy is mandatory on target '${TARGET_PLATFORM}'. Your choice was overriden.")
        mcuboot_override_upgrade_strategy("NO_SWAP")
    endif()

    #FixMe: MCUBOOT_SIGN_RSA_LEN can be removed when ROTPK won't be hard coded in platform/ext/common/template/tfm_rotpk.c
    #       instead independently loaded from secure code as a blob.
    if (${MCUBOOT_SIGNATURE_TYPE} STREQUAL "RSA-2048")
        add_definitions(-DMCUBOOT_SIGN_RSA_LEN=2048)
    endif()
    if (${MCUBOOT_SIGNATURE_TYPE} STREQUAL "RSA-3072")
        add_definitions(-DMCUBOOT_SIGN_RSA_LEN=3072)
    endif()
endif()

embedded_include_directories(PATH "${PLATFORM_DIR}/cmsis" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps3/common" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../include" ABSOLUTE)
embedded_include_directories(PATH "${AN524_DIR}" ABSOLUTE)
embedded_include_directories(PATH "${AN524_DIR}/partition" ABSOLUTE)
embedded_include_directories(PATH "${AN524_DIR}/device/include" ABSOLUTE)
embedded_include_directories(PATH "${AN524_DIR}/device/config" ABSOLUTE)
embedded_include_directories(PATH "${AN524_DIR}/cmsis_drivers/config" ABSOLUTE)
embedded_include_directories(PATH "${AN524_DIR}/native_drivers" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C "${AN524_DIR}/device/source/system_core_init.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${AN524_DIR}/device/source/device_definition.c")
endif()

if (NOT DEFINED BUILD_UART_STDOUT)
  message(FATAL_ERROR "Configuration variable BUILD_UART_STDOUT (true|false) is undefined!")
elseif(BUILD_UART_STDOUT)
  if (NOT DEFINED SECURE_UART1)
    message(FATAL_ERROR "Configuration variable SECURE_UART1 (true|false) is undefined!")
  elseif(SECURE_UART1)
    message(FATAL_ERROR "Configuration SECURE_UART1 TRUE is invalid for this target!")
  endif()
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/uart_stdout.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
  set(BUILD_NATIVE_DRIVERS true)
  set(BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED BUILD_NATIVE_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(BUILD_NATIVE_DRIVERS)
  list(APPEND ALL_SRC_C "${AN524_DIR}/native_drivers/uart_cmsdk_drv.c")

  list(APPEND ALL_SRC_C_S "${AN524_DIR}/native_drivers/mpc_sie200_drv.c"
                          "${AN524_DIR}/native_drivers/ppc_sse200_drv.c")
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
  list(APPEND ALL_SRC_C "${AN524_DIR}/native_drivers/timer_cmsdk_drv.c")
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${AN524_DIR}/device/source/armclang/startup_cmsdk_mps3_an524_s.s")
    list(APPEND ALL_SRC_ASM_NS "${AN524_DIR}/device/source/armclang/startup_cmsdk_mps3_an524_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${AN524_DIR}/device/source/armclang/startup_cmsdk_mps3_an524_bl2.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
    list(APPEND ALL_SRC_ASM_S "${AN524_DIR}/device/source/gcc/startup_cmsdk_mps3_an524_s.S")
    list(APPEND ALL_SRC_ASM_NS "${AN524_DIR}/device/source/gcc/startup_cmsdk_mps3_an524_ns.S")
    list(APPEND ALL_SRC_ASM_BL2 "${AN524_DIR}/device/source/gcc/startup_cmsdk_mps3_an524_bl2.S")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}" APPEND
        PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "IARARM")
    list(APPEND ALL_SRC_ASM_S "${AN524_DIR}/device/source/iar/startup_cmsdk_mps3_an524_s.s")
    list(APPEND ALL_SRC_ASM_NS "${AN524_DIR}/device/source/iar/startup_cmsdk_mps3_an524_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${AN524_DIR}/device/source/iar/startup_cmsdk_mps3_an524_bl2.s")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${AN524_DIR}/target_cfg.c")
  list(APPEND ALL_SRC_C_S "${AN524_DIR}/spm_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/template/attest_hal.c")
  list(APPEND ALL_SRC_C_S "${AN524_DIR}/native_drivers/mpu_armv8m_drv.c")
  list(APPEND ALL_SRC_C_S "${AN524_DIR}/services/src/tfm_platform_system.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/tfm_platform.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_PLAT_TEST)
  message(FATAL_ERROR "Configuration variable BUILD_PLAT_TEST (true|false) is undefined!")
elseif(BUILD_PLAT_TEST)
  list(APPEND ALL_SRC_C "${AN524_DIR}/plat_test.c")
endif()

if (NOT DEFINED BUILD_BOOT_HAL)
  message(FATAL_ERROR "Configuration variable BUILD_BOOT_HAL (true|false) is undefined!")
elseif(BUILD_BOOT_HAL)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/boot_hal.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mps3/an524/boot_hal.c")
endif()

if (NOT DEFINED BUILD_TARGET_HARDWARE_KEYS)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_HARDWARE_KEYS (true|false) is undefined!")
elseif(BUILD_TARGET_HARDWARE_KEYS)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/template/tfm_initial_attestation_key_material.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/template/tfm_rotpk.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/template/crypto_keys.c")
endif()

if (NOT DEFINED BUILD_TARGET_NV_COUNTERS)
    message(FATAL_ERROR "Configuration variable BUILD_TARGET_NV_COUNTERS (true|false) is undefined!")
elseif (BUILD_TARGET_NV_COUNTERS)
    # NOTE: This non-volatile counters implementation is a dummy
    #       implementation. Platform vendors have to implement the
    #       API ONLY if the target has non-volatile counters.
    list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/template/nv_counters.c")
    set(TARGET_NV_COUNTERS_ENABLE ON)
    # Sets SST_ROLLBACK_PROTECTION flag to compile in the SST services
    # rollback protection code as the target supports nv counters.
    set(SST_ROLLBACK_PROTECTION ON)
endif()

if (NOT DEFINED BUILD_CMSIS_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(BUILD_CMSIS_DRIVERS)
  list(APPEND ALL_SRC_C_S "${AN524_DIR}/cmsis_drivers/Driver_MPC.c"
                          "${AN524_DIR}/cmsis_drivers/Driver_PPC.c")
  list(APPEND ALL_SRC_C "${AN524_DIR}/cmsis_drivers/Driver_USART.c")
  embedded_include_directories(PATH "${AN524_DIR}/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${AN524_DIR}/cmsis_drivers/Driver_Flash.c")
  # For AN524 currently BRAM is used for SST The Driver_Flash driver just emulates a flash
  # interface and behaviour on top of the BRAM memory.
  # As the SST area is going to be in RAM, it is required to set SST_CREATE_FLASH_LAYOUT
  # to be sure the SST service knows that when it starts the SST area does not contain any
  # valid SST flash layout and it needs to create one. The same for ITS.
  set(SST_CREATE_FLASH_LAYOUT ON)
  set(SST_RAM_FS OFF)
  set(ITS_CREATE_FLASH_LAYOUT ON)
  set(ITS_RAM_FS OFF)
  embedded_include_directories(PATH "${AN524_DIR}/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (MCUBOOT_RAM_LOADING)
        message (FATAL_ERROR "MCUBOOT_RAM_LOADING is not supported on " ${TARGET_PLATFORM})
endif()
