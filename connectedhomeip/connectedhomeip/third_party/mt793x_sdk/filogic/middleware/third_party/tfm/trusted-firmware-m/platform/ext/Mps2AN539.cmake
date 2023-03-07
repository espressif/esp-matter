#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all MPS2/AN539 specific files in the application.

#MPS2/AN539 has a Cortex-M23 CPU.
include("Common/CpuM23")

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

set(AN539_DIR ${PLATFORM_DIR}/target/mps2/an539)

#Specify the location of platform specific build dependencies.
if(COMPILER STREQUAL "ARMCLANG")
    set (BL2_SCATTER_FILE_NAME "${AN539_DIR}/device/source/armclang/an539_mps2_bl2.sct")
    set (S_SCATTER_FILE_NAME "${PLATFORM_DIR}/common/armclang/tfm_common_s.sct")
    set (NS_SCATTER_FILE_NAME "${AN539_DIR}/device/source/armclang/an539_mps2_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_V8MBN.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set (BL2_SCATTER_FILE_NAME "${AN539_DIR}/device/source/gcc/an539_mps2_bl2.ld")
    set (S_SCATTER_FILE_NAME "${PLATFORM_DIR}/common/gcc/tfm_common_s.ld")
    set (NS_SCATTER_FILE_NAME "${AN539_DIR}/device/source/gcc/an539_mps2_ns.ld")
    if (DEFINED CMSIS_5_DIR)
        # Not all projects define CMSIS_5_DIR, only the ones that use it.
        set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_V8MBN.a")
    endif()
elseif(COMPILER STREQUAL "IARARM")
    set (BL2_SCATTER_FILE_NAME "${AN539_DIR}/device/source/iar/an539_mps2_bl2.icf")
    set (S_SCATTER_FILE_NAME "${PLATFORM_DIR}/common/iar/tfm_common_s.icf")
    set (NS_SCATTER_FILE_NAME "${AN539_DIR}/device/source/iar/an539_mps2_ns.icf")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/IAR/RTX_V8MBN.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT "${AN539_DIR}/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${AN539_DIR}/partition/")

if (BL2)
  set (BL2_LINKER_CONFIG ${BL2_SCATTER_FILE_NAME})
  if (${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "RAM_LOADING")
      message(FATAL_ERROR "ERROR: RAM_LOADING upgrade strategy is not supported on target '${TARGET_PLATFORM}'.")
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
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mps2" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../include" ABSOLUTE)
embedded_include_directories(PATH "${AN539_DIR}" ABSOLUTE)
embedded_include_directories(PATH "${AN539_DIR}/partition" ABSOLUTE)
embedded_include_directories(PATH "${AN539_DIR}/device/include" ABSOLUTE)
embedded_include_directories(PATH "${AN539_DIR}/device/config" ABSOLUTE)
embedded_include_directories(PATH "${AN539_DIR}/cmsis_drivers/config" ABSOLUTE)
embedded_include_directories(PATH "${AN539_DIR}/native_drivers" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C "${AN539_DIR}/device/source/system_core_init.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${AN539_DIR}/device/source/device_definition.c")
endif()

if (NOT DEFINED BUILD_UART_STDOUT)
  message(FATAL_ERROR "Configuration variable BUILD_UART_STDOUT (true|false) is undefined!")
elseif(BUILD_UART_STDOUT)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/uart_stdout.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
  set(BUILD_NATIVE_DRIVERS true)
  set(BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED BUILD_NATIVE_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(BUILD_NATIVE_DRIVERS)
  list(APPEND ALL_SRC_C "${AN539_DIR}/native_drivers/uart_cmsdk_drv.c")

  list(APPEND ALL_SRC_C_S "${AN539_DIR}/native_drivers/mpc_sie200_drv.c"
                          "${AN539_DIR}/native_drivers/ppc_sse123_drv.c")
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
  list(APPEND ALL_SRC_C "${AN539_DIR}/native_drivers/systimer_armv8-m_drv.c")
  list(APPEND ALL_SRC_C "${AN539_DIR}/native_drivers/syscounter_armv8-m_cntrl_drv.c")
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${AN539_DIR}/device/source/armclang/startup_cmsdk_an539_mps2_s.s")
    list(APPEND ALL_SRC_ASM_NS "${AN539_DIR}/device/source/armclang/startup_cmsdk_an539_mps2_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${AN539_DIR}/device/source/armclang/startup_cmsdk_an539_mps2_bl2.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
    list(APPEND ALL_SRC_ASM_S "${AN539_DIR}/device/source/gcc/startup_cmsdk_an539_mps2_s.S")
    list(APPEND ALL_SRC_ASM_NS "${AN539_DIR}/device/source/gcc/startup_cmsdk_an539_mps2_ns.S")
    list(APPEND ALL_SRC_ASM_BL2 "${AN539_DIR}/device/source/gcc/startup_cmsdk_an539_mps2_bl2.S")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}" APPEND
        PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "IARARM")
    list(APPEND ALL_SRC_ASM_S "${AN539_DIR}/device/source/iar/startup_cmsdk_an539_mps2_s.s")
    list(APPEND ALL_SRC_ASM_NS "${AN539_DIR}/device/source/iar/startup_cmsdk_an539_mps2_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${AN539_DIR}/device/source/iar/startup_cmsdk_an539_mps2_bl2.s")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${AN539_DIR}/target_cfg.c")
  list(APPEND ALL_SRC_C_S "${AN539_DIR}/spm_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/template/attest_hal.c")
  list(APPEND ALL_SRC_C_S "${AN539_DIR}/native_drivers/mpu_armv8m_drv.c")
  if (TFM_PARTITION_PLATFORM)
    list(APPEND ALL_SRC_C_S "${AN539_DIR}/services/src/tfm_platform_system.c")
  endif()
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/tfm_platform.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
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
  list(APPEND ALL_SRC_C_S "${AN539_DIR}/cmsis_drivers/Driver_MPC.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_Flash.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_AHB_EXP0.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_APB.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_APB_EXP0.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_APB_EXP1.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_APB_EXP2.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_APB_EXP3.c"
                          "${AN539_DIR}/cmsis_drivers/Driver_SSE123_PPC_Common.c")
  list(APPEND ALL_SRC_C "${AN539_DIR}/cmsis_drivers/Driver_USART.c")
  embedded_include_directories(PATH "${AN539_DIR}/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_PLAT_TEST)
  message(FATAL_ERROR "Configuration variable BUILD_PLAT_TEST (true|false) is undefined!")
elseif(BUILD_PLAT_TEST)
  list(APPEND ALL_SRC_C "${AN539_DIR}/plat_test.c")
endif()

if (NOT DEFINED BUILD_BOOT_HAL)
  message(FATAL_ERROR "Configuration variable BUILD_BOOT_HAL (true|false) is undefined!")
elseif(BUILD_BOOT_HAL)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/boot_hal.c")
  list(APPEND ALL_SRC_C "${AN539_DIR}/boot_hal.c")
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${AN539_DIR}/cmsis_drivers/Driver_Flash.c")
  # There is no real flash memory for code on MPS2 board. Instead a code SRAM is used for code
  # storage: SSRAM1. The Driver_Flash driver just emulates a flash interface and behaviour on
  # top of the SRAM memory.
  # As the SST area is going to be in RAM, it is required to set SST_CREATE_FLASH_LAYOUT
  # to be sure the SST service knows that when it starts the SST area does not contain any
  # valid SST flash layout and it needs to create one. The same for ITS.
  set(SST_CREATE_FLASH_LAYOUT ON)
  set(SST_RAM_FS OFF)
  set(ITS_CREATE_FLASH_LAYOUT ON)
  set(ITS_RAM_FS OFF)
  embedded_include_directories(PATH "${AN539_DIR}/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()
