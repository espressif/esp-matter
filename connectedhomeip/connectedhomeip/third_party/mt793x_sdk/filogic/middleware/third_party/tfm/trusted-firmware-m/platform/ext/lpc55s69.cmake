#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
# Copyright (c) 2020, Linaro. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all LPCXpresso55S69 specific files in the application.

# LPC55S69 has a Cortex M33 CPU.
include("Common/CpuM33")

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

add_definitions(-DCPU_LPC55S69JBD100_cm33_core0)
add_definitions(-DSERIAL_PORT_TYPE_UART=1)
# set(SST_ENCRYPTION ON)
set(SST_RAM_FS ON) # Disabled with BUILD_FLASH
set(SST_CREATE_FLASH_LAYOUT ON)
set(SST_ROLLBACK_PROTECTION ON)
set(ITS_RAM_FS ON) # Disabled with BUILD_FLASH
set(ITS_CREATE_FLASH_LAYOUT ON)
set(DAUTH_CHIP_DEFAULT ON)
set(TFM_PARTITION_PLATFORM ON)

#Specify the location of platform specific build dependencies.
#FIXME use CMAKE_C_COMPILER_ID here instead?
if(COMPILER STREQUAL "ARMCLANG")
    # Throw error for ARMCLANG until support can be added/tested.
    message(FATAL_ERROR "ARMCLANG is not currently supported on this platform. Use GNUARM instead.")
    #set(BL2_SCATTER_FILE_NAME "${PLATFORM_DIR}target/nxp/lpcxpresso55s69/Device/Source/armclang/LPC55S69_cm33_core0_bl2.sct")
    #set(S_SCATTER_FILE_NAME   "${PLATFORM_DIR}target/nxp/lpcxpresso55s69/Device/Source/armclang/LPC55S69_cm33_core0_s.sct")
    #set(NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}target/nxp/lpcxpresso55s69/Device/Source/armclang/LPC55S69_cm33_core0_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # Not all projects defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_V8MMN.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set(S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/gcc/tfm_common_s.ld")
    set(NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armgcc/LPC55S69_cm33_core0_ns.ld")
    if (DEFINED CMSIS_5_DIR)
        # Not all projects define CMSIS_5_DIR, only the ones that use it.
        set(RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT           "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/partition")

if (BL2)
  message(FATAL_ERROR "ERROR: BL2 is currently not supported on target '${TARGET_PLATFORM}'.")
  # Section below maintained for ref. with future BL2 support.
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
embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/lists" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/serial_manager" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/uart" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities/debug_console" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities/str" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Config" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/s" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/partition" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/../include" ABSOLUTE)

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
elseif (BUILD_NATIVE_DRIVERS)
    list(APPEND ALL_SRC_C    "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/lists/generic_list.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/serial_manager/serial_manager.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/serial_manager/serial_port_uart.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/components/uart/usart_adapter.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_casper.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_clock.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_common.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_ctimer.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_flexcomm.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_gpio.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_hashcrypt.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_iap.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_power.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_reset.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_rng.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/drivers/fsl_usart.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities/fsl_assert.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities/fsl_sbrk.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities/debug_console/fsl_debug_console.c"
                             "${PLATFORM_DIR}/target/nxp/common/Native_Driver/utilities/str/fsl_str.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/system_LPC55S69_cm33_core0.c")
    list(APPEND ALL_SRC_C_S  "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/s/board.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/s/hardware_init.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/s/clock_config.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/s/peripherals.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/s/pin_mux.c")
    list(APPEND ALL_SRC_C_NS "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/ns/board.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/ns/hardware_init.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/ns/clock_config.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/ns/peripherals.c"
                             "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Native_Driver/project_template/ns/pin_mux.c")

endif()

if (NOT DEFINED BUILD_TIME)
    message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif (BUILD_TIME)
    embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_STARTUP)
    message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif (BUILD_STARTUP)
    if (CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
        list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armclang/startup_LPC55S69_cm33_core0_s.s")
        list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armclang/startup_LPC55S69_cm33_core0_ns.s")
        #list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armclang/startup_LPC55S69_cm33_core0_bl2.s")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
        list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armgcc/startup_LPC55S69_cm33_core0_s.S")
        list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armgcc/startup_LPC55S69_cm33_core0_ns.S")
        #list(APPEND ALL_SRC_ASM_BL2 "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/Device/Source/armgcc/startup_LPC55S69_cm33_core0_bl2.S")
        set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" "${ALL_SRC_ASM_BL2}"
                     APPEND PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
    else()
        message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
    endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/target_cfg.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/spm_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/template/attest_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/nxp/common/Native_Driver/mpu_armv8m_drv.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/Native_Driver" ABSOLUTE)
  if (TFM_PARTITION_PLATFORM)
    list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/services/src/tfm_platform_system.c")
  endif()
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/tfm_platform.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_PLAT_TEST)
  message(FATAL_ERROR "Configuration variable BUILD_PLAT_TEST (true|false) is undefined!")
elseif(BUILD_PLAT_TEST)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/plat_test.c")
endif()

# TODO: Implement bootloader support!
if (NOT DEFINED BUILD_BOOT_HAL)
  message(FATAL_ERROR "Configuration variable BUILD_BOOT_HAL (true|false) is undefined!")
elseif(BUILD_BOOT_HAL)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/boot_hal.c")
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
  #list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/nxp/lpcxpresso55s69/cmsis_drivers/Driver_MPC.c"
  #  "${PLATFORM_DIR}/target/nxp/common/CMSIS_Driver/Driver_PPC.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/nxp/common/CMSIS_Driver/Driver_USART.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/CMSIS_Driver" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/nxp/common/CMSIS_Driver/Driver_Flash.c")

  # As the SST area is going to be in RAM, it is required to set
  # SST_CREATE_FLASH_LAYOUT to be sure the SST service knows that when it
  # starts the SST area does not contain any valid SST flash layout and it
  # needs to create one.
  set(SST_CREATE_FLASH_LAYOUT ON)
  set(SST_RAM_FS OFF)
  set(ITS_CREATE_FLASH_LAYOUT ON)
  set(ITS_RAM_FS OFF)
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/nxp/common/CMSIS_Driver" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()
