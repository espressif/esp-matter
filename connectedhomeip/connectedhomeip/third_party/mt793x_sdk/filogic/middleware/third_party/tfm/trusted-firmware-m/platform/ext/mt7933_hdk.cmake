#-------------------------------------------------------------------------------
# Copyright (c) 2018-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all MPS2/AN521 specific files in the application.

#MPS2/AN521 has a Cortex M33 CPU.
include("Common/CpuM33")

set (TARGET_PLATFORM_MT793X ON)
add_definitions(-DTARGET_PLATFORM_MT793X)
add_definitions(-DTFM_ENABLE_MT7933_IRQ)

if (BOARD_TYPE STREQUAL "bga_tfm_demo")
    add_definitions(-DBGA_TFM_DEMO)
elseif (BOARD_TYPE STREQUAL "qfn_tfm_demo")
    add_definitions(-DQFN_TFM_DEMO)
elseif (BOARD_TYPE STREQUAL "qfn_tfm_4m")
    add_definitions(-DQFN_TFM_4M)
endif ()

if (AUDIO_EQUIP STREQUAL "y")
    set (MTK_AUDIO_SUPPORT ON)
    add_definitions(-DMTK_AUDIO_SUPPORT)
endif ()

#add_definitions(-DTFM_CORE_DEBUG)
set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})
set(TFM_ROOT_DIR ${PLATFORM_DIR}/../..)
set(HADRON_ROOT_DIR ${TFM_ROOT_DIR}/../../../..)
set(HADRON_DRIVER_DIR ${HADRON_ROOT_DIR}/driver/chip/mt7933)
set(HADRON_SDK_DIR ${HADRON_ROOT_DIR}/project/mt7933_hdk/apps)
set(HADRON_BOOTLOADER_DIR ${HADRON_SDK_DIR}/bootloader)

#Specify the location of platform specific build dependencies.
#FIXME use CMAKE_C_COMPILER_ID here instead?
if(COMPILER STREQUAL "ARMCLANG")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/armclang/tfm_common_s.sct")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/armclang/mt7933_hdk_bl2.sct")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/armclang/mt7933_hdk_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_V8MMN.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/gcc/tfm_common_s_mt7933.ld")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/gcc/mt7933_hdk_bl2.ld")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/gcc/mt7933_hdk_ns.ld")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a")
    endif()
elseif(COMPILER STREQUAL "IARARM")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/iar/tfm_common_s.icf")
    set (BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/mt7933_hdk_bl2.icf")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/mt7933_hdk_ns.icf")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/IAR/RTX_V8MMN.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT           "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/partition/")

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
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_core" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/retarget" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/native_drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/partition" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../../bl2/include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../../bl2/ext/mcuboot/include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../../bl2/ext/mcuboot/bootutil/include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../../../mbedtls/include" ABSOLUTE)
embedded_include_directories(PATH "${HADRON_DRIVER_DIR}/../inc" ABSOLUTE)
embedded_include_directories(PATH "${HADRON_DRIVER_DIR}/inc" ABSOLUTE)
embedded_include_directories(PATH "${HADRON_DRIVER_DIR}/src_core/inc" ABSOLUTE)
embedded_include_directories(PATH "${HADRON_SDK_DIR}/${BOARD_TYPE}/inc" ABSOLUTE)
embedded_include_directories(PATH "${HADRON_DRIVER_DIR}/src/psramc/inc" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/platform_isolation" ABSOLUTE)

#Gather all source files we need.
if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_core/system_cmsdk_mt7933_hdk.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/retarget/platform_retarget_dev.c")
endif()

if (NOT DEFINED BUILD_UART_STDOUT)
  message(FATAL_ERROR "Configuration variable BUILD_UART_STDOUT (true|false) is undefined!")
elseif(BUILD_UART_STDOUT)
  if (NOT DEFINED SECURE_UART1)
    message(FATAL_ERROR "Configuration variable SECURE_UART1 (true|false) is undefined!")
  elseif(SECURE_UART1)
    add_definitions(-DSECURE_UART1)
  endif()
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/services/src/uart_stdout.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
  set(BUILD_NATIVE_DRIVERS true)
  set(BUILD_CMSIS_DRIVERS true)
endif()

if (NOT DEFINED BUILD_NATIVE_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_NATIVE_DRIVERS (true|false) is undefined!")
elseif(BUILD_NATIVE_DRIVERS)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/native_drivers/arm_uart_drv.c")

  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/native_drivers/mpc_sie200_drv.c"
              "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/native_drivers/ppc_sse200_drv.c"
              )
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/tfm_hal_log.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/" ABSOLUTE)
  #Because TFM references RTOS driver headers, it needs to enable the corresponding definitions
  add_definitions(-DHAL_GPT_MODULE_ENABLED)
  add_definitions(-DHAL_SEJ_GPT_MODULE_ENABLED)
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/armclang/startup_cmsdk_mt7933_hdk_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/armclang/startup_cmsdk_mt7933_hdk_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/armclang/startup_cmsdk_mt7933_hdk_bl2.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/gcc/startup_cmsdk_mt7933_hdk_s.S")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/gcc/startup_cmsdk_mt7933_hdk_ns.S")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/gcc/startup_cmsdk_mt7933_hdk_bl2.S")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}" APPEND
      PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "IARARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/iar/startup_cmsdk_mt7933_hdk_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/iar/startup_cmsdk_mt7933_hdk_ns.s")
    list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/iar/startup_cmsdk_mt7933_hdk_bl2.s")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/tfm_hal_log.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/" ABSOLUTE)
  #Because TFM references RTOS driver headers, it needs to enable the corresponding definitions
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/target_cfg.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/sbrk.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/tfm_irq_mt7933.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/spm_hal.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/common/template/attest_hal.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/native_drivers/mpu_armv8m_drv.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/services/src/tfm_exception_handler.c")
  if (TFM_PARTITION_PLATFORM)
    list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/services/src/tfm_platform_system.c")
  endif()
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/tfm_platform.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_PLAT_TEST)
  message(FATAL_ERROR "Configuration variable BUILD_PLAT_TEST (true|false) is undefined!")
elseif(BUILD_PLAT_TEST)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/plat_test.c")
endif()

if (NOT DEFINED BUILD_BOOT_HAL)
  message(FATAL_ERROR "Configuration variable BUILD_BOOT_HAL (true|false) is undefined!")
elseif(BUILD_BOOT_HAL)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/boot_hal.c")
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
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/nv_counters.c")
  set(TARGET_NV_COUNTERS_ENABLE ON)
  # Sets SST_ROLLBACK_PROTECTION flag to compile in the SST services
  # rollback protection code as the target supports nv counters.
  set (SST_ROLLBACK_PROTECTION ON)
endif()

configure_file("${PLATFORM_DIR}/../../bl2/ext/mcuboot/include/mcuboot_config/mcuboot_config.h.in"
			   "${CMAKE_CURRENT_BINARY_DIR}/mcuboot_config/mcuboot_config.h"
			   @ONLY)

if (NOT DEFINED BUILD_CMSIS_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(BUILD_CMSIS_DRIVERS)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_drivers/Driver_MPC.c"
    "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_drivers/Driver_PPC.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_drivers/Driver_USART.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/../../bl2/src/boot_record.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_drivers" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
  embedded_include_directories(PATH "${CMAKE_CURRENT_BINARY_DIR}" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  #list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/TFM_Driver_Flash.c")
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
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/cmsis_drivers" ABSOLUTE)
endif()

if (NOT DEFINED TFM_INIT_PSRAM)
  message(FATAL_ERROR "Configuration variable TFM_INIT_PSRAM (true|false) is undefined! PSRAM will not be initialized in TFM")
elseif(TFM_INIT_PSRAM)
  add_definitions(-DTFM_INIT_PSRAM)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/tfm_psram_utility.c")
endif()

if (NOT DEFINED TFM_DEEPSLEEP_WAKEUP)
    message(FATAL_ERROR "Configuration variable TFM_DEEPSLEEP_WAKEUP (true|false) is undefined! TFM is not involved in platform sleep and wakeup flow")
elseif(TFM_DEEPSLEEP_WAKEUP)
  add_definitions(-DHAL_SLEEP_MANAGER_ENABLED)
  add_definitions(-DHAL_GPT_MODULE_ENABLED)
  add_definitions(-DHAL_TRNG_MODULE_ENABLED)
  add_definitions(-DHAL_FLASH_MODULE_ENABLED)
  add_definitions(-DHAL_CLOCK_MODULE_ENABLED)
endif()

if (NOT DEFINED TFM_ENABLE_PLATFORM_ISOLATION)
    message("Configuration variable TFM_ENABLE_PLATFORM_ISOLATION (true|false) is undefined! TFM enables the platform isolation")
elseif(TFM_ENABLE_PLATFORM_ISOLATION)
    add_definitions(-DTFM_ENABLE_PLATFORM_ISOLATION)
endif()

#Support HW crypto accelerator
if (NOT DEFINED CRYPTO_HW_ACCELERATOR)
    set (CRYPTO_HW_ACCELERATOR On)
endif()

#Enable GCPU HW accelerator
if (CRYPTO_HW_ACCELERATOR)
    set(CRYPTO_HW_ACCELERATOR_BUILD_PATH ${PLATFORM_DIR}/target/mt7933/mt7933_hdk/drivers/HW_accelerator/)
    set(CRYPTO_HW_ACCELERATOR_CMAKE_BUILD "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/Build_GCPU.cmake" PARENT_SCOPE)
    set(CRYPTO_HW_ACCELERATOR_CMAKE_LINK "${CRYPTO_HW_ACCELERATOR_BUILD_PATH}/Link_GCPU.cmake" PARENT_SCOPE)

    add_definitions("-DCRYPTO_HW_ACCELERATOR")
    add_definitions("-DHAL_AES_MODULE_ENABLED")
else()
    list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/services/platform_sp_MTK.c")
    embedded_include_directories(PATH "${PLATFORM_DIR}/target/mt7933/mt7933_hdk/services/" ABSOLUTE)

    add_definitions("-DMTK_HW_ACCELERATOR_WO_MBEDTLS")
endif()

#Enable MTK CI test
if (SERVICE_TEST_NS)
    set (MTK_CI_TEST True)
    add_definitions("-DMTK_CI_TEST")
else()
    set (MTK_CI_TEST false)
endif()

if (TFM_BUILD_MODE_DEBUG)
    add_definitions("-DTFM_DEBUG_BUILD")
elseif (TFM_BUILD_MODE_NORMAL)
    add_definitions("-DTFM_NORMAL_BUILD")
endif()
