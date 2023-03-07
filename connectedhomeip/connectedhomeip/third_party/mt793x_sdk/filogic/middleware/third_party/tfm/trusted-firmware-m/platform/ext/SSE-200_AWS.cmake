#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all SSE-200_AWS specific files in the application.

#SSE-200_AWS has a Cortex M33 CPU.
include("Common/CpuM33")

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

#Specify the location of platform specific build dependencies.
#FIXME use CMAKE_C_COMPILER_ID here instead?
if(COMPILER STREQUAL "ARMCLANG")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/armclang/tfm_common_s.sct")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/sse-200_aws/armclang/sse-200_aws_bl2.sct")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/sse-200_aws/armclang/sse-200_aws_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_V8MMN.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/gcc/tfm_common_s.ld")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/sse-200_aws/gcc/sse-200_aws_bl2.ld")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/sse-200_aws/gcc/sse-200_aws_ns.ld")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a")
    endif()
elseif(COMPILER STREQUAL "IARARM")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/iar/tfm_common_s.icf")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/sse-200_aws/iar/sse-200_aws_bl2.icf")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/sse-200_aws/iar/sse-200_aws_ns.icf")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/IAR/RTX_V8MBN.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT           "${PLATFORM_DIR}/target/sse-200_aws/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${PLATFORM_DIR}/target/sse-200_aws/partition/")

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
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/cmsis_core" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/retarget" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/native_drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/partition" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../include" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/cmsis_core/system_cmsdk_sse-200_aws.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/retarget/platform_retarget_dev.c")
endif()

if (NOT DEFINED BUILD_UART_STDOUT)
  message(FATAL_ERROR "Configuration variable BUILD_UART_STDOUT (true|false) is undefined!")
elseif(BUILD_UART_STDOUT)
  if (NOT DEFINED SECURE_UART1)
    message(FATAL_ERROR "Configuration variable SECURE_UART1 (true|false) is undefined!")
  elseif(SECURE_UART1)
    add_definitions(-DSECURE_UART1)
  endif()
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/uart_stdout.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
  set(BUILD_NATIVE_DRIVERS true)
  set(BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED BUILD_NATIVE_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(BUILD_NATIVE_DRIVERS)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/native_drivers/uart_cmsdk_drv.c")

  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/sse-200_aws/native_drivers/mpc_sie_drv.c"
              "${PLATFORM_DIR}/target/sse-200_aws/native_drivers/ppc_sse200_drv.c"
              )
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/native_drivers/timer_cmsdk/timer_cmsdk_drv.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/native_drivers/timer_cmsdk" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/sse-200_aws/armclang/startup_cmsdk_sse-200_aws_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/sse-200_aws/armclang/startup_cmsdk_sse-200_aws_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/sse-200_aws/armclang/startup_cmsdk_sse-200_aws_bl2.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/sse-200_aws/gcc/startup_cmsdk_sse-200_aws_s.S")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/sse-200_aws/gcc/startup_cmsdk_sse-200_aws_ns.S")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/sse-200_aws/gcc/startup_cmsdk_sse-200_aws_bl2.S")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}" APPEND
      PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "IARARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/sse-200_aws/iar/startup_cmsdk_sse-200_aws_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/sse-200_aws/iar/startup_cmsdk_sse-200_aws_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/sse-200_aws/iar/startup_cmsdk_sse-200_aws_bl2.s")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}" APPEND
      PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/target_cfg.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/sse-200_aws/spm_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/template/attest_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/sse-200_aws/native_drivers/mpu_armv8m_drv.c")
  if (TFM_PARTITION_PLATFORM)
    list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/sse-200_aws/services/src/tfm_platform_system.c")
  endif()
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/tfm_platform.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_PLAT_TEST)
  message(FATAL_ERROR "Configuration variable BUILD_PLAT_TEST (true|false) is undefined!")
elseif(BUILD_PLAT_TEST)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/plat_test.c")
endif()

if (NOT DEFINED BUILD_BOOT_HAL)
  message(FATAL_ERROR "Configuration variable BUILD_BOOT_HAL (true|false) is undefined!")
elseif(BUILD_BOOT_HAL)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/boot_hal.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/boot_hal.c")
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
elseif(BUILD_TARGET_NV_COUNTERS)
  # NOTE: This non-volatile counters implementation is a dummy
  #       implementation. Platform vendors have to implement the
  #       API ONLY if the target has non-volatile counters.
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/template/nv_counters.c")
  set(TARGET_NV_COUNTERS_ENABLE ON)
  # Sets SST_ROLLBACK_PROTECTION flag to compile in the SST services
  # rollback protection code as the target supports nv counters.
  set (SST_ROLLBACK_PROTECTION ON)
endif()

if (NOT DEFINED BUILD_CMSIS_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(BUILD_CMSIS_DRIVERS)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/sse-200_aws/cmsis_drivers/Driver_SSE-200_AWS_MPC.c"
    "${PLATFORM_DIR}/target/sse-200_aws/cmsis_drivers/Driver_PPC.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/cmsis_drivers/Driver_USART.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/sse-200_aws/cmsis_drivers/Driver_Flash.c")
  # There is no real flash memory for code on MPS2 board. Instead a code SRAM is
  # used for code storage: ZBT SSRAM1. The Driver_Flash driver just emulates a flash
  # interface and behaviour on top of the SRAM memory.
  # As the SST area is going to be in RAM, it is required to set SST_CREATE_FLASH_LAYOUT
  # to be sure the SST service knows that when it starts the SST area does not contain any
  # valid SST flash layout and it needs to create one. The same for ITS.
  set(SST_CREATE_FLASH_LAYOUT ON)
  set(SST_RAM_FS OFF)
  set(ITS_CREATE_FLASH_LAYOUT ON)
  set(ITS_RAM_FS OFF)
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse-200_aws/cmsis_drivers" ABSOLUTE)
endif()
