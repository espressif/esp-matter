#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
# Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file gathers all Cypress PSoC 6 specific files in the application.

include("Common/MultiCore")

# Select configuration of multi-core topology
enable_multi_core_topology_config()

# Enable multiple outstanding NS PSA Client calls
enable_multi_core_multi_client_call()

set(TFM_NS_CLIENT_IDENTIFICATION OFF)

add_definitions(-DCYB0644ABZI_S2D44=1)

add_definitions(-DTFM_CORE_DEBUG)

# psoc64 platform doesn't yet use TF-M BL2
set(BL2 False)
remove_definitions(-DBL2)

# Skip peripheral access test
set(TFM_ENABLE_PERIPH_ACCESS_TEST OFF)
if (REGRESSION)
    # Enable multi-core specific test cases
    set(TFM_MULTI_CORE_TEST ON)
endif()

# Set Cortex-M0plus as secure core
set_secure_cpu_type("CpuM0p")
# Set Cortex-M4 as non-secure core
set_ns_cpu_type("CpuM4")

# Set PSoC62 specific secure definitions
add_platform_secure_definitions(CY_PSOC6_CM0P=1)

set(PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR})

#Specify the location of platform specific build dependencies.
if(COMPILER STREQUAL "ARMCLANG")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/armclang/tfm_common_s.sct")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/armclang/psoc6_ns.sct")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/ARM/RTX_CM4F.lib")
    endif()
elseif(COMPILER STREQUAL "GNUARM")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/gcc/tfm_common_s.ld")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/gcc/psoc6_ns.ld")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      # [libRTX_CM3.a should be used for CM4 without FPU]
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/GCC/libRTX_CM3.a")
    endif()
elseif(COMPILER STREQUAL "IARARM")
    set (S_SCATTER_FILE_NAME   "${PLATFORM_DIR}/common/iar/tfm_common_s.icf")
    set (NS_SCATTER_FILE_NAME  "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/iar/psoc6_ns.icf")
    if (DEFINED CMSIS_5_DIR)
      # not all project defines CMSIS_5_DIR, only the ones that use it.
      # [RTX_CM3.a should be used for CM4 without FPU]
      set (RTX_LIB_PATH "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Library/IAR/RTX_CM3.a")
    endif()
else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
endif()
set (FLASH_LAYOUT          "${PLATFORM_DIR}/target/cypress/psoc64/partition/flash_layout.h")
set (PLATFORM_LINK_INCLUDES "${PLATFORM_DIR}/target/cypress/psoc64/partition")

embedded_include_directories(PATH "${PLATFORM_DIR}/cmsis" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/CMSIS_Driver/Config" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/Device/Config" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/Device/Include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/include" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/mailbox" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/partition" ABSOLUTE)
embedded_include_directories(PATH "${TFM_ROOT_DIR}/interface/include" ABSOLUTE)
embedded_include_directories(PATH "${TFM_ROOT_DIR}/platform/include" ABSOLUTE)
embedded_include_directories(PATH "${TFM_ROOT_DIR}/secure_fw/core/arch/include" ABSOLUTE)
embedded_include_directories(PATH "${TFM_ROOT_DIR}/secure_fw/core/ipc/include" ABSOLUTE)

#Gather all source files we need.
list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/mailbox/platform_multicore.c")
list(APPEND ALL_SRC_C_NS "${PLATFORM_DIR}/target/cypress/psoc64/mailbox/platform_ns_mailbox.c")
list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/mailbox/mailbox_ipc_intr.c")
list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/mailbox/platform_spe_mailbox.c")

install(FILES ${PLATFORM_DIR}/target/cypress/psoc64/mailbox/ns_ipc_config.h
        ${PLATFORM_DIR}/target/cypress/psoc64/mailbox/platform_multicore.h
        DESTINATION export/tfm/include)

install(FILES ${PLATFORM_DIR}/target/cypress/psoc64/mailbox/platform_multicore.c
        ${PLATFORM_DIR}/target/cypress/psoc64/mailbox/platform_ns_mailbox.c
        DESTINATION export/tfm/src)

if (NOT DEFINED BUILD_CMSIS_CORE)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_CORE (true|false) is undefined!")
elseif(BUILD_CMSIS_CORE)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/system_psoc6_cm0plus.c")
  list(APPEND ALL_SRC_C_NS "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/system_psoc6_cm4.c")
endif()

if (NOT DEFINED BUILD_RETARGET)
  message(FATAL_ERROR "Configuration variable BUILD_RETARGET (true|false) is undefined!")
elseif(BUILD_RETARGET)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/device_definition.c")
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
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_device.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_flash.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_gpio.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_prot.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_ipc_drv.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_ipc_pipe.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_ipc_sema.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_scb_common.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_scb_uart.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_sysclk.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_sysint.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_syslib.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_syspm.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_systick.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_tcpwm_counter.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_trigmux.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/source/cy_wdt.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg_clocks.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg_clocks.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg_peripherals.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg_pins.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg_routing.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/Native_Driver/generated_source/cycfg_system.c")
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
	  list(APPEND ALL_SRC_ASM "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/armclang/cy_syslib_mdk.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
	  list(APPEND ALL_SRC_ASM "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/gcc/cy_syslib_gcc.S")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "IARARM")
	  list(APPEND ALL_SRC_ASM "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/iar/cy_syslib_iar.c")
  else()
    message(FATAL_ERROR "No cy_syslib is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TIME)
  message(FATAL_ERROR "Configuration variable BUILD_TIME (true|false) is undefined!")
elseif(BUILD_TIME)
endif()

if (NOT DEFINED BUILD_STARTUP)
  message(FATAL_ERROR "Configuration variable BUILD_STARTUP (true|false) is undefined!")
elseif(BUILD_STARTUP)
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMCLANG")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/armclang/startup_psoc64_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/armclang/startup_psoc64_ns.s")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNUARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/gcc/startup_psoc64_s.S")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/gcc/startup_psoc64_ns.S")
    set_property(SOURCE "${ALL_SRC_ASM_S}" "${ALL_SRC_ASM_NS}" APPEND
      PROPERTY COMPILE_DEFINITIONS "__STARTUP_CLEAR_BSS_MULTIPLE" "__STARTUP_COPY_MULTIPLE")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "IARARM")
    list(APPEND ALL_SRC_ASM_S "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/iar/startup_psoc64_s.s")
    list(APPEND ALL_SRC_ASM_NS "${PLATFORM_DIR}/target/cypress/psoc64/Device/Source/iar/startup_psoc64_ns.s")
  else()
    message(FATAL_ERROR "No startup file is available for compiler '${CMAKE_C_COMPILER_ID}'.")
  endif()
endif()

if (NOT DEFINED BUILD_TARGET_CFG)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_CFG (true|false) is undefined!")
elseif(BUILD_TARGET_CFG)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/target_cfg.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/spm_hal.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/attest_hal.c")
  if (TFM_PARTITION_PLATFORM)
    list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/services/src/tfm_platform_system.c")
  endif()
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/tfm_platform.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/common" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_PLAT_TEST)
    message(FATAL_ERROR "Configuration variable BUILD_PLAT_TEST (true|false) is undefined!")
elseif(BUILD_PLAT_TEST)
    list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/plat_test.c")
endif()

if (NOT DEFINED BUILD_TARGET_HARDWARE_KEYS)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_HARDWARE_KEYS (true|false) is undefined!")
elseif(BUILD_TARGET_HARDWARE_KEYS)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/template/tfm_initial_attestation_key_material.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/common/template/tfm_rotpk.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/dummy_crypto_keys.c")
endif()

if (NOT DEFINED BUILD_TARGET_NV_COUNTERS)
  message(FATAL_ERROR "Configuration variable BUILD_TARGET_NV_COUNTERS (true|false) is undefined!")
elseif(BUILD_TARGET_NV_COUNTERS)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/nv_counters.c")
  set(TARGET_NV_COUNTERS_ENABLE ON)
  # Sets SST_ROLLBACK_PROTECTION flag to compile in the SST services
  # rollback protection code as the target supports nv counters.
  set (SST_ROLLBACK_PROTECTION ON)
endif()

if (NOT DEFINED BUILD_CMSIS_DRIVERS)
  message(FATAL_ERROR "Configuration variable BUILD_CMSIS_DRIVERS (true|false) is undefined!")
elseif(BUILD_CMSIS_DRIVERS)
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/driver_smpu.c")
  list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/driver_ppu.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/driver_dap.c")
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/CMSIS_Driver/Driver_USART.c")
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/CMSIS_Driver" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

if (NOT DEFINED BUILD_FLASH)
  message(FATAL_ERROR "Configuration variable BUILD_FLASH (true|false) is undefined!")
elseif(BUILD_FLASH)
  list(APPEND ALL_SRC_C "${PLATFORM_DIR}/target/cypress/psoc64/CMSIS_Driver/Driver_Flash.c")
  # As the SST area is going to be in RAM, it is required to set SST_CREATE_FLASH_LAYOUT
  # to be sure the SST service knows that when it starts the SST area does not contain any
  # valid SST flash layout and it needs to create one.
  set(SST_CREATE_FLASH_LAYOUT ON)
  set(ITS_CREATE_FLASH_LAYOUT ON)
  embedded_include_directories(PATH "${PLATFORM_DIR}/target/cypress/psoc64/CMSIS_Driver" ABSOLUTE)
  embedded_include_directories(PATH "${PLATFORM_DIR}/driver" ABSOLUTE)
endif()

list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/dummy_boot_seed.c")
list(APPEND ALL_SRC_C_S "${PLATFORM_DIR}/target/cypress/psoc64/dummy_device_id.c")
