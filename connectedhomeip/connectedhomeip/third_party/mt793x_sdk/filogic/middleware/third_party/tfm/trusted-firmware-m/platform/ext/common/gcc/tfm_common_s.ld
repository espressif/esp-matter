;/*
; * Copyright (c) 2009-2020 Arm Limited
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; *
; *
; * This file is derivative of CMSIS V5.00 gcc_arm.ld
; */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

/* Linker script to configure memory regions. */
/* This file will be run trough the pre-processor. */

#include "region_defs.h"

MEMORY
{
  FLASH    (rx)  : ORIGIN = S_CODE_START, LENGTH = S_CODE_SIZE
  RAM      (rwx) : ORIGIN = S_DATA_START, LENGTH = S_DATA_SIZE
#if defined(S_CODE_SRAM_ALIAS_BASE)
  CODE_RAM (rwx) : ORIGIN = S_CODE_SRAM_ALIAS_BASE, LENGTH = TOTAL_CODE_SRAM_SIZE
#endif
#ifndef TFM_MULTI_CORE_TOPOLOGY
  VENEERS  (rx)  : ORIGIN = CMSE_VENEER_REGION_START, LENGTH = CMSE_VENEER_REGION_SIZE
#endif
}

__heap_size__  = S_HEAP_SIZE;
__psp_stack_size__ = S_PSP_STACK_SIZE;
__msp_init_stack_size__ = S_MSP_STACK_SIZE_INIT;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a libc_nano.a)

ENTRY(Reset_Handler)

SECTIONS
{
    .TFM_VECTORS : ALIGN(4)
    {
        __vectors_start__ = .;
        KEEP(*(.vectors))
        *startup*(.text*)
        . = ALIGN(4);
        __vectors_end__ = .;
    } > FLASH

    .copy.table : ALIGN(4)
    {
        __copy_table_start__ = .;
        LONG (LOADADDR(.TFM_DATA))
        LONG (ADDR(.TFM_DATA))
        LONG (SIZEOF(.TFM_DATA))
#if defined(TFM_PSA_API)
        LONG (LOADADDR(.TFM_UNPRIV_DATA))
        LONG (ADDR(.TFM_UNPRIV_DATA))
        LONG (SIZEOF(.TFM_UNPRIV_DATA))
#endif /* defined(TFM_PSA_API) */
#ifdef TFM_PARTITION_SECURE_STORAGE
        LONG (LOADADDR(.TFM_SP_STORAGE_LINKER_DATA))
        LONG (ADDR(.TFM_SP_STORAGE_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_STORAGE_LINKER_DATA))
#endif /* TFM_PARTITION_SECURE_STORAGE */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
        LONG (LOADADDR(.TFM_SP_ITS_LINKER_DATA))
        LONG (ADDR(.TFM_SP_ITS_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_ITS_LINKER_DATA))
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
#ifdef TFM_PARTITION_AUDIT_LOG
        LONG (LOADADDR(.TFM_SP_AUDIT_LOG_LINKER_DATA))
        LONG (ADDR(.TFM_SP_AUDIT_LOG_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_AUDIT_LOG_LINKER_DATA))
#endif /* TFM_PARTITION_AUDIT_LOG */
#ifdef TFM_PARTITION_CRYPTO
        LONG (LOADADDR(.TFM_SP_CRYPTO_LINKER_DATA))
        LONG (ADDR(.TFM_SP_CRYPTO_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_CRYPTO_LINKER_DATA))
#endif /* TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
        LONG (LOADADDR(.TFM_SP_PLATFORM_LINKER_DATA))
        LONG (ADDR(.TFM_SP_PLATFORM_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_PLATFORM_LINKER_DATA))
#endif /* TFM_PARTITION_PLATFORM */
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
        LONG (LOADADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA))
        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA))
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (LOADADDR(.TFM_SP_CORE_TEST_LINKER_DATA))
        LONG (ADDR(.TFM_SP_CORE_TEST_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (LOADADDR(.TFM_SP_CORE_TEST_2_LINKER_DATA))
        LONG (ADDR(.TFM_SP_CORE_TEST_2_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_2_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (LOADADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA))
        LONG (ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (LOADADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_DATA))
        LONG (ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (LOADADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_DATA))
        LONG (ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_ENABLE_IRQ_TEST
        LONG (LOADADDR(.TFM_IRQ_TEST_1_LINKER_DATA))
        LONG (ADDR(.TFM_IRQ_TEST_1_LINKER_DATA))
        LONG (SIZEOF(.TFM_IRQ_TEST_1_LINKER_DATA))
#endif /* TFM_ENABLE_IRQ_TEST */
#ifdef TFM_PARTITION_TEST_SST
        LONG (LOADADDR(.TFM_SP_SST_TEST_LINKER_DATA))
        LONG (ADDR(.TFM_SP_SST_TEST_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_SST_TEST_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_SST */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (LOADADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_DATA))
        LONG (ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER_DATA))
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_MULTI_CORE_TEST
        LONG (LOADADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_DATA))
        LONG (ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER_DATA))
#endif /* TFM_MULTI_CORE_TEST */
#ifdef TFM_ENABLE_MT7933_IRQ
        LONG (LOADADDR(.TFM_IRQ_MT7933_LINKER_DATA))
        LONG (ADDR(.TFM_IRQ_MT7933_LINKER_DATA))
        LONG (SIZEOF(.TFM_IRQ_MT7933_LINKER_DATA))
#endif /* TFM_ENABLE_MT7933_IRQ */
#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
        LONG (LOADADDR(.TFM_SP_PLATFORM_MTK_LINKER_DATA))
        LONG (ADDR(.TFM_SP_PLATFORM_MTK_LINKER_DATA))
        LONG (SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER_DATA))
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */
#ifdef MTK_CI_TEST
        LONG (LOADADDR(.TFM_MTK_CI_TEST_LINKER_DATA))
        LONG (ADDR(.TFM_MTK_CI_TEST_LINKER_DATA))
        LONG (SIZEOF(.TFM_MTK_CI_TEST_LINKER_DATA))
#endif /* MTK_CI_TEST */
#if defined (S_RAM_CODE_START)
        LONG (LOADADDR(.TFM_RAM_CODE))
        LONG (ADDR(.TFM_RAM_CODE))
        LONG (SIZEOF(.TFM_RAM_CODE))
#endif
#if defined(S_CODE_SRAM_ALIAS_BASE)
        LONG (LOADADDR(.ER_CODE_SRAM))
        LONG (ADDR(.ER_CODE_SRAM))
        LONG (SIZEOF(.ER_CODE_SRAM))
#endif
        __copy_table_end__ = .;
    } > FLASH

    .zero.table : ALIGN(4)
    {
        __zero_table_start__ = .;
        LONG (ADDR(.TFM_BSS))
        LONG (SIZEOF(.TFM_BSS))
#if !defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SECURE_STACK))
        LONG (SIZEOF(.TFM_SECURE_STACK))
#endif /* !defined(TFM_PSA_API) */
#ifdef TFM_PARTITION_SECURE_STORAGE
        LONG (ADDR(.TFM_SP_STORAGE_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_STORAGE_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_STORAGE_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_STORAGE_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_SECURE_STORAGE */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
        LONG (ADDR(.TFM_SP_ITS_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_ITS_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_ITS_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_ITS_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
#ifdef TFM_PARTITION_AUDIT_LOG
        LONG (ADDR(.TFM_SP_AUDIT_LOG_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_AUDIT_LOG_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_AUDIT_LOG_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_AUDIT_LOG_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_AUDIT_LOG */
#ifdef TFM_PARTITION_CRYPTO
        LONG (ADDR(.TFM_SP_CRYPTO_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_CRYPTO_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_CRYPTO_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_CRYPTO_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
        LONG (ADDR(.TFM_SP_PLATFORM_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_PLATFORM_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_PLATFORM_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_PLATFORM_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_PLATFORM */
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (ADDR(.TFM_SP_CORE_TEST_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_CORE_TEST_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (ADDR(.TFM_SP_CORE_TEST_2_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_2_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_CORE_TEST_2_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_2_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_ENABLE_IRQ_TEST
        LONG (ADDR(.TFM_IRQ_TEST_1_LINKER_BSS))
        LONG (SIZEOF(.TFM_IRQ_TEST_1_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_IRQ_TEST_1_LINKER_STACK))
        LONG (SIZEOF(.TFM_IRQ_TEST_1_LINKER_STACK))
#endif
#endif /* TFM_ENABLE_IRQ_TEST */
#ifdef TFM_PARTITION_TEST_SST
        LONG (ADDR(.TFM_SP_SST_TEST_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_SST_TEST_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_SST_TEST_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_SST_TEST_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_SST */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER_STACK))
#endif
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_MULTI_CORE_TEST
        LONG (ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER_STACK))
#endif
#endif /* TFM_MULTI_CORE_TEST */
#ifdef TFM_ENABLE_MT7933_IRQ
        LONG (ADDR(.TFM_IRQ_MT7933_LINKER_BSS))
        LONG (SIZEOF(.TFM_IRQ_MT7933_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_IRQ_MT7933_LINKER_STACK))
        LONG (SIZEOF(.TFM_IRQ_MT7933_LINKER_STACK))
#endif
#endif /* TFM_ENABLE_MT7933_IRQ */
#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
        LONG (ADDR(.TFM_SP_PLATFORM_MTK_LINKER_BSS))
        LONG (SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_SP_PLATFORM_MTK_LINKER_STACK))
        LONG (SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER_STACK))
#endif
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */
#ifdef MTK_CI_TEST
        LONG (ADDR(.TFM_MTK_CI_TEST_LINKER_BSS))
        LONG (SIZEOF(.TFM_MTK_CI_TEST_LINKER_BSS))
#if defined(TFM_PSA_API)
        LONG (ADDR(.TFM_MTK_CI_TEST_LINKER_STACK))
        LONG (SIZEOF(.TFM_MTK_CI_TEST_LINKER_STACK))
#endif
#endif /* MTK_CI_TEST */
        __zero_table_end__ = .;
    } > FLASH

    .TFM_UNPRIV_CODE : ALIGN(32)
    {
        *libc_nano*:*(.text*)
        *libc_nano*:*(.rodata*)
        *tfm_spm_services.*(.text*)
        *tfm_spm_services.*(.rodata*)
        *platform_retarget_dev.*(.text*)
        *platform_retarget_dev.*(.rodata*)
        *device_definition.*(.text*)
        *device_definition.*(.rodata*)
        *(SFN)
        *libgcc*:*(.text*)
        *libgcc*:*(.rodata*)
        *libtfmsprt*:*(.text*)
        *libtfmsprt*:*(.rodata*)
        *psa_client.*(.text*)              /* NXP */
        *psa_client.*(.rodata*)
        *psa_service.*(.text*)             /* NXP */
        *psa_service.*(.rodata*)
        *psa_lifecycle.*(.text*)             /* NXP */
        *psa_lifecycle.*(.rodata*)
        *tfm_log_raw.*(.text*)             /* NXP */
        *tfm_log_raw.*(.rodata*)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_UNPRIV_CODE$$RO$$Base = ADDR(.TFM_UNPRIV_CODE);
    Image$$TFM_UNPRIV_CODE$$RO$$Limit = ADDR(.TFM_UNPRIV_CODE) + SIZEOF(.TFM_UNPRIV_CODE);

    /**** PSA RoT RO part (CODE + RODATA) start here */
    Image$$TFM_PSA_CODE_START$$Base = .;

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    .TFM_SP_ITS_LINKER : ALIGN(32)
    {
        *tfm_internal_trusted_storage*:*(.text*)
        *tfm_internal_trusted_storage*:*(.rodata*)
        *(TFM_SP_ITS_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_ITS_LINKER$$RO$$Base = ADDR(.TFM_SP_ITS_LINKER);
    Image$$TFM_SP_ITS_LINKER$$RO$$Limit = ADDR(.TFM_SP_ITS_LINKER) + SIZEOF(.TFM_SP_ITS_LINKER);
    Image$$TFM_SP_ITS_LINKER$$Base = ADDR(.TFM_SP_ITS_LINKER);
    Image$$TFM_SP_ITS_LINKER$$Limit = ADDR(.TFM_SP_ITS_LINKER) + SIZEOF(.TFM_SP_ITS_LINKER);
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
    .TFM_SP_AUDIT_LOG_LINKER : ALIGN(32)
    {
        *tfm_audit*:*(.text*)
        *tfm_audit*:*(.rodata*)
        *(TFM_SP_AUDIT_LOG_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_AUDIT_LOG_LINKER$$RO$$Base = ADDR(.TFM_SP_AUDIT_LOG_LINKER);
    Image$$TFM_SP_AUDIT_LOG_LINKER$$RO$$Limit = ADDR(.TFM_SP_AUDIT_LOG_LINKER) + SIZEOF(.TFM_SP_AUDIT_LOG_LINKER);
    Image$$TFM_SP_AUDIT_LOG_LINKER$$Base = ADDR(.TFM_SP_AUDIT_LOG_LINKER);
    Image$$TFM_SP_AUDIT_LOG_LINKER$$Limit = ADDR(.TFM_SP_AUDIT_LOG_LINKER) + SIZEOF(.TFM_SP_AUDIT_LOG_LINKER);
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
    .TFM_SP_CRYPTO_LINKER : ALIGN(32)
    {
        *tfm_crypto*:*(.text*)
        *tfm_crypto*:*(.rodata*)
        *(TFM_SP_CRYPTO_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CRYPTO_LINKER$$RO$$Base = ADDR(.TFM_SP_CRYPTO_LINKER);
    Image$$TFM_SP_CRYPTO_LINKER$$RO$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER) + SIZEOF(.TFM_SP_CRYPTO_LINKER);
    Image$$TFM_SP_CRYPTO_LINKER$$Base = ADDR(.TFM_SP_CRYPTO_LINKER);
    Image$$TFM_SP_CRYPTO_LINKER$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER) + SIZEOF(.TFM_SP_CRYPTO_LINKER);
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
    .TFM_SP_PLATFORM_LINKER : ALIGN(32)
    {
        *tfm_platform*:*(.text*)
        *tfm_platform*:*(.rodata*)
        *(TFM_SP_PLATFORM_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_PLATFORM_LINKER$$RO$$Base = ADDR(.TFM_SP_PLATFORM_LINKER);
    Image$$TFM_SP_PLATFORM_LINKER$$RO$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER) + SIZEOF(.TFM_SP_PLATFORM_LINKER);
    Image$$TFM_SP_PLATFORM_LINKER$$Base = ADDR(.TFM_SP_PLATFORM_LINKER);
    Image$$TFM_SP_PLATFORM_LINKER$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER) + SIZEOF(.TFM_SP_PLATFORM_LINKER);
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    .TFM_SP_INITIAL_ATTESTATION_LINKER : ALIGN(32)
    {
        *tfm_attest*:*(.text*)
        *tfm_attest*:*(.rodata*)
        *(TFM_SP_INITIAL_ATTESTATION_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$RO$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$RO$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER);
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
    .TFM_SP_CORE_TEST_LINKER : ALIGN(32)
    {
        *tfm_ss_core_test.*(.text*)
        *tfm_ss_core_test.*(.rodata*)
        *(TFM_SP_CORE_TEST_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CORE_TEST_LINKER$$RO$$Base = ADDR(.TFM_SP_CORE_TEST_LINKER);
    Image$$TFM_SP_CORE_TEST_LINKER$$RO$$Limit = ADDR(.TFM_SP_CORE_TEST_LINKER) + SIZEOF(.TFM_SP_CORE_TEST_LINKER);
    Image$$TFM_SP_CORE_TEST_LINKER$$Base = ADDR(.TFM_SP_CORE_TEST_LINKER);
    Image$$TFM_SP_CORE_TEST_LINKER$$Limit = ADDR(.TFM_SP_CORE_TEST_LINKER) + SIZEOF(.TFM_SP_CORE_TEST_LINKER);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_TEST_PARTITION_LINKER : ALIGN(32)
    {
        *tfm_secure_client_service.*(.text*)
        *tfm_secure_client_service.*(.rodata*)
        *test_framework*(.text*)
        *test_framework*(.rodata*)
        *uart_stdout.*(.text*)
        *uart_stdout.*(.rodata*)
        *Driver_USART.*(.text*)
        *Driver_USART.*(.rodata*)
        *arm_uart_drv.*(.text*)
        *arm_uart_drv.*(.rodata*)
        *uart_pl011_drv.*(.text*)
        *uart_pl011_drv.*(.rodata*)
        *uart_cmsdk_drv*(.text*)
        *uart_cmsdk_drv*(.rodata*)
        *secure_suites.*(.text*)
        *secure_suites.*(.rodata*)
        *attestation_s_interface_testsuite.*(.text*)
        *attestation_s_interface_testsuite.*(.rodata*)
        *(TFM_SP_SECURE_TEST_PARTITION_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER$$RO$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER);
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER$$RO$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER);
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER);
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    .TFM_SP_IPC_SERVICE_TEST_LINKER : ALIGN(32)
    {
        *ipc_service_test.*(.text*)
        *ipc_service_test.*(.rodata*)
        *(TFM_SP_IPC_SERVICE_TEST_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER$$RO$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER);
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER$$RO$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER);
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER);
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_SST
    .TFM_SP_SST_TEST_LINKER : ALIGN(32)
    {
        *tfm_sst_test_service.*(.text*)
        *tfm_sst_test_service.*(.rodata*)
        *(TFM_SP_SST_TEST_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_SST_TEST_LINKER$$RO$$Base = ADDR(.TFM_SP_SST_TEST_LINKER);
    Image$$TFM_SP_SST_TEST_LINKER$$RO$$Limit = ADDR(.TFM_SP_SST_TEST_LINKER) + SIZEOF(.TFM_SP_SST_TEST_LINKER);
    Image$$TFM_SP_SST_TEST_LINKER$$Base = ADDR(.TFM_SP_SST_TEST_LINKER);
    Image$$TFM_SP_SST_TEST_LINKER$$Limit = ADDR(.TFM_SP_SST_TEST_LINKER) + SIZEOF(.TFM_SP_SST_TEST_LINKER);
#endif /* TFM_PARTITION_TEST_SST */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    .TFM_SP_PLATFORM_MTK_LINKER : ALIGN(32)
    {
        *tfm_platform_MTK.*(.text*)
        *tfm_platform_MTK.*(.rodata*)
        *(TFM_SP_PLATFORM_MTK_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_PLATFORM_MTK_LINKER$$RO$$Base = ADDR(.TFM_SP_PLATFORM_MTK_LINKER);
    Image$$TFM_SP_PLATFORM_MTK_LINKER$$RO$$Limit = ADDR(.TFM_SP_PLATFORM_MTK_LINKER) + SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER);
    Image$$TFM_SP_PLATFORM_MTK_LINKER$$Base = ADDR(.TFM_SP_PLATFORM_MTK_LINKER);
    Image$$TFM_SP_PLATFORM_MTK_LINKER$$Limit = ADDR(.TFM_SP_PLATFORM_MTK_LINKER) + SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER);
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

    /**** PSA RoT RO part (CODE + RODATA) end here */
    Image$$TFM_PSA_CODE_END$$Base = .;

    /**** APPLICATION RoT RO part (CODE + RODATA) start here */
    Image$$TFM_APP_CODE_START$$Base = .;

#ifdef TFM_PARTITION_SECURE_STORAGE
    .TFM_SP_STORAGE_LINKER : ALIGN(32)
    {
        *tfm_storage*:*(.text*)
        *tfm_storage*:*(.rodata*)
        *test_sst_nv_counters.*(.text*)
        *test_sst_nv_counters.*(.rodata*)
        *(TFM_SP_STORAGE_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_STORAGE_LINKER$$RO$$Base = ADDR(.TFM_SP_STORAGE_LINKER);
    Image$$TFM_SP_STORAGE_LINKER$$RO$$Limit = ADDR(.TFM_SP_STORAGE_LINKER) + SIZEOF(.TFM_SP_STORAGE_LINKER);
    Image$$TFM_SP_STORAGE_LINKER$$Base = ADDR(.TFM_SP_STORAGE_LINKER);
    Image$$TFM_SP_STORAGE_LINKER$$Limit = ADDR(.TFM_SP_STORAGE_LINKER) + SIZEOF(.TFM_SP_STORAGE_LINKER);
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_TEST_CORE
    .TFM_SP_CORE_TEST_2_LINKER : ALIGN(32)
    {
        *tfm_ss_core_test_2.*(.text*)
        *tfm_ss_core_test_2.*(.rodata*)
        *(TFM_SP_CORE_TEST_2_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CORE_TEST_2_LINKER$$RO$$Base = ADDR(.TFM_SP_CORE_TEST_2_LINKER);
    Image$$TFM_SP_CORE_TEST_2_LINKER$$RO$$Limit = ADDR(.TFM_SP_CORE_TEST_2_LINKER) + SIZEOF(.TFM_SP_CORE_TEST_2_LINKER);
    Image$$TFM_SP_CORE_TEST_2_LINKER$$Base = ADDR(.TFM_SP_CORE_TEST_2_LINKER);
    Image$$TFM_SP_CORE_TEST_2_LINKER$$Limit = ADDR(.TFM_SP_CORE_TEST_2_LINKER) + SIZEOF(.TFM_SP_CORE_TEST_2_LINKER);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    .TFM_SP_IPC_CLIENT_TEST_LINKER : ALIGN(32)
    {
        *ipc_client_test.*(.text*)
        *ipc_client_test.*(.rodata*)
        *(TFM_SP_IPC_CLIENT_TEST_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER$$RO$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER);
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER$$RO$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER);
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER);
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
    .TFM_IRQ_TEST_1_LINKER : ALIGN(32)
    {
        *tfm_irq_test_service_1.*(.text*)
        *tfm_irq_test_service_1.*(.rodata*)
        *timer_cmsdk*(.text*)
        *timer_cmsdk*(.rodata*)
        *(TFM_IRQ_TEST_1_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_IRQ_TEST_1_LINKER$$RO$$Base = ADDR(.TFM_IRQ_TEST_1_LINKER);
    Image$$TFM_IRQ_TEST_1_LINKER$$RO$$Limit = ADDR(.TFM_IRQ_TEST_1_LINKER) + SIZEOF(.TFM_IRQ_TEST_1_LINKER);
    Image$$TFM_IRQ_TEST_1_LINKER$$Base = ADDR(.TFM_IRQ_TEST_1_LINKER);
    Image$$TFM_IRQ_TEST_1_LINKER$$Limit = ADDR(.TFM_IRQ_TEST_1_LINKER) + SIZEOF(.TFM_IRQ_TEST_1_LINKER);
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_CLIENT_2_LINKER : ALIGN(32)
    {
        *tfm_secure_client_2.*(.text*)
        *tfm_secure_client_2.*(.rodata*)
        *(TFM_SP_SECURE_CLIENT_2_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER$$RO$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER);
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER$$RO$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER);
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER);
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
    .TFM_SP_MULTI_CORE_TEST_LINKER : ALIGN(32)
    {
        *multi_core_test.*(.text*)
        *multi_core_test.*(.rodata*)
        *(TFM_SP_MULTI_CORE_TEST_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER$$RO$$Base = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER);
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER$$RO$$Limit = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER) + SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER);
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER$$Base = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER);
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER$$Limit = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER) + SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER);
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
    .TFM_IRQ_MT7933_LINKER : ALIGN(32)
    {
        *tfm_irq_mt7933.*(.text*)
        *tfm_irq_mt7933.*(.rodata*)
        *(TFM_IRQ_MT7933_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_IRQ_MT7933_LINKER$$RO$$Base = ADDR(.TFM_IRQ_MT7933_LINKER);
    Image$$TFM_IRQ_MT7933_LINKER$$RO$$Limit = ADDR(.TFM_IRQ_MT7933_LINKER) + SIZEOF(.TFM_IRQ_MT7933_LINKER);
    Image$$TFM_IRQ_MT7933_LINKER$$Base = ADDR(.TFM_IRQ_MT7933_LINKER);
    Image$$TFM_IRQ_MT7933_LINKER$$Limit = ADDR(.TFM_IRQ_MT7933_LINKER) + SIZEOF(.TFM_IRQ_MT7933_LINKER);
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_CI_TEST
    .TFM_MTK_CI_TEST_LINKER : ALIGN(32)
    {
        *tfm_MTK_CI_test.*(.text*)
        *tfm_MTK_CI_test.*(.rodata*)
        *(TFM_MTK_CI_TEST_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_MTK_CI_TEST_LINKER$$RO$$Base = ADDR(.TFM_MTK_CI_TEST_LINKER);
    Image$$TFM_MTK_CI_TEST_LINKER$$RO$$Limit = ADDR(.TFM_MTK_CI_TEST_LINKER) + SIZEOF(.TFM_MTK_CI_TEST_LINKER);
    Image$$TFM_MTK_CI_TEST_LINKER$$Base = ADDR(.TFM_MTK_CI_TEST_LINKER);
    Image$$TFM_MTK_CI_TEST_LINKER$$Limit = ADDR(.TFM_MTK_CI_TEST_LINKER) + SIZEOF(.TFM_MTK_CI_TEST_LINKER);
#endif /* MTK_CI_TEST */

    /**** APPLICATION RoT RO part (CODE + RODATA) end here */
    Image$$TFM_APP_CODE_END$$Base = .;

#if defined(S_CODE_SRAM_ALIAS_BASE)
    .ER_CODE_SRAM : ALIGN(4)
    {
        *Driver_GFC100_EFlash.o(.text*)
        *Driver_GFC100_EFlash.o(.rodata*)
        *gfc100_eflash_drv.o(.text*)
        *gfc100_eflash_drv.o(.rodata*)
        *musca_b1_eflash_drv.o(.text*)
        *musca_b1_eflash_drv.o(.rodata*)
        . = ALIGN(4); /* This alignment is needed to make the section size 4 bytes aligned */
    } > CODE_RAM AT > FLASH
    Image$$ER_CODE_SRAM$$RO$$Base = ADDR(.ER_CODE_SRAM);
    Image$$ER_CODE_SRAM$$RO$$Limit = ADDR(.ER_CODE_SRAM) + SIZEOF(.ER_CODE_SRAM);
    Image$$ER_CODE_SRAM$$Base = ADDR(.ER_CODE_SRAM);
    Image$$ER_CODE_SRAM$$Limit = ADDR(.ER_CODE_SRAM) + SIZEOF(.ER_CODE_SRAM);
#endif

#if TFM_LVL != 1
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

#endif /* TFM_LVL != 1 */

    .ER_TFM_CODE : ALIGN(4)
    {
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))


        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
         *crtbegin.o(.dtors)
         *crtbegin?.o(.dtors)
         *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
         *(SORT(.dtors.*))
         *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    /**** Base address of secure data area */
    .tfm_secure_data_start :
    {
        . = ABSOLUTE(S_DATA_START) ;
    } > RAM

    /*
     * MPU on Armv6-M/v7-M core in multi-core topology may require more strict
     * alignment that MPU region base address must align with the MPU region
     * size.
     * As a result, in multi-core topology, to save memory resource and MPU
     * regions, unprivileged data sections and privileged data sections are
     * separated and gathered in unprivileged/privileged data area respectively.
     * Keep BL2 shared data and MSP stack at the beginning of the secure data
     * area in single Armv8-M topology, while move the two areas to the
     * beginning of privileged data region in multi-core topology.
     */
#ifndef TFM_MULTI_CORE_TOPOLOGY
    /* shared_data and msp_stack are overlapping on purpose when
     * msp_stack is extended until the beginning of RAM, when shared_date
     * was read out by partitions
     */
    .tfm_bl2_shared_data : ALIGN(32)
    {
        . += BOOT_TFM_SHARED_DATA_SIZE;
    } > RAM

    .msp_stack : ALIGN(32)
    {
        . += __msp_init_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK_MSP$$ZI$$Base = ADDR(.msp_stack);
    Image$$ARM_LIB_STACK_MSP$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);

    /* PSP is unprivileged in single-core topology */
    .psp_stack : ALIGN(32)
    {
        . += __psp_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.psp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.psp_stack) + SIZEOF(.psp_stack);
#endif

#if !defined(TFM_PSA_API)
    .TFM_SECURE_STACK : ALIGN(128)
    {
        . += 0x2000;
    } > RAM
    Image$$TFM_SECURE_STACK$$ZI$$Base = ADDR(.TFM_SECURE_STACK);
    Image$$TFM_SECURE_STACK$$ZI$$Limit = ADDR(.TFM_SECURE_STACK) + SIZEOF(.TFM_SECURE_STACK);
#endif /* !defined(TFM_PSA_API) */

#if (TFM_LVL == 1)
    .heap : ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM
#endif /* TFM_LVL == 1 */

#if defined(TFM_PSA_API)
    .TFM_UNPRIV_DATA : ALIGN(32)
    {
    } > RAM AT> FLASH
    Image$$TFM_UNPRIV_DATA$$RW$$Base = ADDR(.TFM_UNPRIV_DATA);
    Image$$TFM_UNPRIV_DATA$$RW$$Limit = ADDR(.TFM_UNPRIV_DATA) + SIZEOF(.TFM_UNPRIV_DATA);

    .TFM_UNPRIV_BSS : ALIGN(32)
    {
    } > RAM AT> RAM
    Image$$TFM_UNPRIV_DATA$$ZI$$Base = ADDR(.TFM_UNPRIV_BSS);
    Image$$TFM_UNPRIV_DATA$$ZI$$Limit = ADDR(.TFM_UNPRIV_BSS) + SIZEOF(.TFM_UNPRIV_BSS);
#endif /* defined(TFM_PSA_API) */

    /**** APPLICATION RoT DATA start here */
    Image$$TFM_APP_RW_STACK_START$$Base = .;

#ifdef TFM_PARTITION_SECURE_STORAGE
    .TFM_SP_STORAGE_LINKER_DATA : ALIGN(32)
    {
        *tfm_storage*:*(.data*)
        *test_sst_nv_counters.*(.data*)
        *(TFM_SP_STORAGE_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_STORAGE_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_STORAGE_LINKER_DATA);
    Image$$TFM_SP_STORAGE_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_STORAGE_LINKER_DATA) + SIZEOF(.TFM_SP_STORAGE_LINKER_DATA);

    .TFM_SP_STORAGE_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_STORAGE_LINKER = .;
        *tfm_storage*:*(.bss*)
        *tfm_storage*:*(COMMON)
        *test_sst_nv_counters.*(.bss*)
        *test_sst_nv_counters.*(COMMON)
        *(TFM_SP_STORAGE_ATTR_ZI)
        . += (. - start_of_TFM_SP_STORAGE_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_STORAGE_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_STORAGE_LINKER_BSS);
    Image$$TFM_SP_STORAGE_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_STORAGE_LINKER_BSS) + SIZEOF(.TFM_SP_STORAGE_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_STORAGE_LINKER_STACK : ALIGN(128)
    {
        . += 0x800;
    } > RAM
    Image$$TFM_SP_STORAGE_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_STORAGE_LINKER_STACK);
    Image$$TFM_SP_STORAGE_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_STORAGE_LINKER_STACK) + SIZEOF(.TFM_SP_STORAGE_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_TEST_CORE
    .TFM_SP_CORE_TEST_2_LINKER_DATA : ALIGN(32)
    {
        *tfm_ss_core_test_2.*(.data*)
        *(TFM_SP_CORE_TEST_2_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_CORE_TEST_2_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_CORE_TEST_2_LINKER_DATA);
    Image$$TFM_SP_CORE_TEST_2_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_CORE_TEST_2_LINKER_DATA) + SIZEOF(.TFM_SP_CORE_TEST_2_LINKER_DATA);

    .TFM_SP_CORE_TEST_2_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_CORE_TEST_2_LINKER = .;
        *tfm_ss_core_test_2.*(.bss*)
        *tfm_ss_core_test_2.*(COMMON)
        *(TFM_SP_CORE_TEST_2_ATTR_ZI)
        . += (. - start_of_TFM_SP_CORE_TEST_2_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_CORE_TEST_2_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_CORE_TEST_2_LINKER_BSS);
    Image$$TFM_SP_CORE_TEST_2_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_CORE_TEST_2_LINKER_BSS) + SIZEOF(.TFM_SP_CORE_TEST_2_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_CORE_TEST_2_LINKER_STACK : ALIGN(128)
    {
        . += 0x0280;
    } > RAM
    Image$$TFM_SP_CORE_TEST_2_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_CORE_TEST_2_LINKER_STACK);
    Image$$TFM_SP_CORE_TEST_2_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_CORE_TEST_2_LINKER_STACK) + SIZEOF(.TFM_SP_CORE_TEST_2_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    .TFM_SP_IPC_CLIENT_TEST_LINKER_DATA : ALIGN(32)
    {
        *ipc_client_test.*(.data*)
        *(TFM_SP_IPC_CLIENT_TEST_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_DATA);
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_DATA) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER_DATA);

    .TFM_SP_IPC_CLIENT_TEST_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_IPC_CLIENT_TEST_LINKER = .;
        *ipc_client_test.*(.bss*)
        *ipc_client_test.*(COMMON)
        *(TFM_SP_IPC_CLIENT_TEST_ATTR_ZI)
        . += (. - start_of_TFM_SP_IPC_CLIENT_TEST_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_BSS);
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_BSS) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_IPC_CLIENT_TEST_LINKER_STACK : ALIGN(128)
    {
        . += 0x0300;
    } > RAM
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_STACK);
    Image$$TFM_SP_IPC_CLIENT_TEST_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_LINKER_STACK) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
    .TFM_IRQ_TEST_1_LINKER_DATA : ALIGN(32)
    {
        *tfm_irq_test_service_1.*(.data*)
        *timer_cmsdk*(.data*)
        *(TFM_IRQ_TEST_1_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_IRQ_TEST_1_LINKER_DATA$$RW$$Base = ADDR(.TFM_IRQ_TEST_1_LINKER_DATA);
    Image$$TFM_IRQ_TEST_1_LINKER_DATA$$RW$$Limit = ADDR(.TFM_IRQ_TEST_1_LINKER_DATA) + SIZEOF(.TFM_IRQ_TEST_1_LINKER_DATA);

    .TFM_IRQ_TEST_1_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_IRQ_TEST_1_LINKER = .;
        *tfm_irq_test_service_1.*(.bss*)
        *tfm_irq_test_service_1.*(COMMON)
        *timer_cmsdk*(.bss*)
        *timer_cmsdk*(COMMON)
        *(TFM_IRQ_TEST_1_ATTR_ZI)
        . += (. - start_of_TFM_IRQ_TEST_1_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_IRQ_TEST_1_LINKER_DATA$$ZI$$Base = ADDR(.TFM_IRQ_TEST_1_LINKER_BSS);
    Image$$TFM_IRQ_TEST_1_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_IRQ_TEST_1_LINKER_BSS) + SIZEOF(.TFM_IRQ_TEST_1_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_IRQ_TEST_1_LINKER_STACK : ALIGN(128)
    {
        . += 0x0400;
    } > RAM
    Image$$TFM_IRQ_TEST_1_LINKER_STACK$$ZI$$Base = ADDR(.TFM_IRQ_TEST_1_LINKER_STACK);
    Image$$TFM_IRQ_TEST_1_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_IRQ_TEST_1_LINKER_STACK) + SIZEOF(.TFM_IRQ_TEST_1_LINKER_STACK);
#endif

#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_CLIENT_2_LINKER_DATA : ALIGN(32)
    {
        *tfm_secure_client_2.*(.data*)
        *(TFM_SP_SECURE_CLIENT_2_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_DATA);
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_DATA) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER_DATA);

    .TFM_SP_SECURE_CLIENT_2_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_SECURE_CLIENT_2_LINKER = .;
        *tfm_secure_client_2.*(.bss*)
        *tfm_secure_client_2.*(COMMON)
        *(TFM_SP_SECURE_CLIENT_2_ATTR_ZI)
        . += (. - start_of_TFM_SP_SECURE_CLIENT_2_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_BSS);
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_BSS) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_SECURE_CLIENT_2_LINKER_STACK : ALIGN(128)
    {
        . += 0x300;
    } > RAM
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_STACK);
    Image$$TFM_SP_SECURE_CLIENT_2_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_LINKER_STACK) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
    .TFM_SP_MULTI_CORE_TEST_LINKER_DATA : ALIGN(32)
    {
        *multi_core_test.*(.data*)
        *(TFM_SP_MULTI_CORE_TEST_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_DATA);
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_DATA) + SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER_DATA);

    .TFM_SP_MULTI_CORE_TEST_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_MULTI_CORE_TEST_LINKER = .;
        *multi_core_test.*(.bss*)
        *multi_core_test.*(COMMON)
        *(TFM_SP_MULTI_CORE_TEST_ATTR_ZI)
        . += (. - start_of_TFM_SP_MULTI_CORE_TEST_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_BSS);
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_BSS) + SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_MULTI_CORE_TEST_LINKER_STACK : ALIGN(128)
    {
        . += 0x0100;
    } > RAM
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_STACK);
    Image$$TFM_SP_MULTI_CORE_TEST_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_MULTI_CORE_TEST_LINKER_STACK) + SIZEOF(.TFM_SP_MULTI_CORE_TEST_LINKER_STACK);
#endif

#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
    .TFM_IRQ_MT7933_LINKER_DATA : ALIGN(32)
    {
        *tfm_irq_mt7933.*(.data*)
        *(TFM_IRQ_MT7933_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_IRQ_MT7933_LINKER_DATA$$RW$$Base = ADDR(.TFM_IRQ_MT7933_LINKER_DATA);
    Image$$TFM_IRQ_MT7933_LINKER_DATA$$RW$$Limit = ADDR(.TFM_IRQ_MT7933_LINKER_DATA) + SIZEOF(.TFM_IRQ_MT7933_LINKER_DATA);

    .TFM_IRQ_MT7933_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_IRQ_MT7933_LINKER = .;
        *tfm_irq_mt7933.*(.bss*)
        *tfm_irq_mt7933.*(COMMON)
        *(TFM_IRQ_MT7933_ATTR_ZI)
        . += (. - start_of_TFM_IRQ_MT7933_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_IRQ_MT7933_LINKER_DATA$$ZI$$Base = ADDR(.TFM_IRQ_MT7933_LINKER_BSS);
    Image$$TFM_IRQ_MT7933_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_IRQ_MT7933_LINKER_BSS) + SIZEOF(.TFM_IRQ_MT7933_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_IRQ_MT7933_LINKER_STACK : ALIGN(128)
    {
        . += 0x0400;
    } > RAM
    Image$$TFM_IRQ_MT7933_LINKER_STACK$$ZI$$Base = ADDR(.TFM_IRQ_MT7933_LINKER_STACK);
    Image$$TFM_IRQ_MT7933_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_IRQ_MT7933_LINKER_STACK) + SIZEOF(.TFM_IRQ_MT7933_LINKER_STACK);
#endif

#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_CI_TEST
    .TFM_MTK_CI_TEST_LINKER_DATA : ALIGN(32)
    {
        *tfm_MTK_CI_test.*(.data*)
        *(TFM_MTK_CI_TEST_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_MTK_CI_TEST_LINKER_DATA$$RW$$Base = ADDR(.TFM_MTK_CI_TEST_LINKER_DATA);
    Image$$TFM_MTK_CI_TEST_LINKER_DATA$$RW$$Limit = ADDR(.TFM_MTK_CI_TEST_LINKER_DATA) + SIZEOF(.TFM_MTK_CI_TEST_LINKER_DATA);

    .TFM_MTK_CI_TEST_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_MTK_CI_TEST_LINKER = .;
        *tfm_MTK_CI_test.*(.bss*)
        *tfm_MTK_CI_test.*(COMMON)
        *(TFM_MTK_CI_TEST_ATTR_ZI)
        . += (. - start_of_TFM_MTK_CI_TEST_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_MTK_CI_TEST_LINKER_DATA$$ZI$$Base = ADDR(.TFM_MTK_CI_TEST_LINKER_BSS);
    Image$$TFM_MTK_CI_TEST_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_MTK_CI_TEST_LINKER_BSS) + SIZEOF(.TFM_MTK_CI_TEST_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_MTK_CI_TEST_LINKER_STACK : ALIGN(128)
    {
        . += 0x0400;
    } > RAM
    Image$$TFM_MTK_CI_TEST_LINKER_STACK$$ZI$$Base = ADDR(.TFM_MTK_CI_TEST_LINKER_STACK);
    Image$$TFM_MTK_CI_TEST_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_MTK_CI_TEST_LINKER_STACK) + SIZEOF(.TFM_MTK_CI_TEST_LINKER_STACK);
#endif

#endif /* MTK_CI_TEST */

    /**** APPLICATION RoT DATA end here */
    Image$$TFM_APP_RW_STACK_END$$Base = .;

#if TFM_LVL != 1
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA : ALIGN(32)
    {
        *libc_nano*:*(.data*)
        . = ALIGN(32);
    } > RAM AT> FLASH

    .TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS : ALIGN(32)
    {
        /* FixMe:
         * Test framework use printf to print out test result. Implementation of
         * printf in GCC libc use static data and heap as well. To be able to
         * execute test suites with TFM_LVL=3 this workaround is needed to
         * allocate libc static data and heap within the data section of secure
         * test partition. This can be removed if test service will be executed
         * in privileged mode.
         */
        *libc_nano*:*(.bss*)
        *libc_nano*:*(COMMON)

        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */

        . = ALIGN(32);
    } > RAM AT> RAM
#else /* TFM_PARTITION_TEST_SECURE_SERVICES */
    .heap : ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM AT> RAM
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#endif /* TFM_LVL != 1 */

#ifdef TFM_MULTI_CORE_TOPOLOGY
#ifdef S_DATA_PRIV_START
    /**** Privileged data area base address specified by multi-core platform */
    .tfm_secure_priv_data_boundary :
    {
        . = ABSOLUTE(S_DATA_PRIV_START) ;
    } > RAM
#endif

    /*
     * Move BL2 shared area and MSP stack to the beginning of privileged data
     * area in multi-core topology.
     */

    /* shared_data and msp_stack are overlapping on purpose when
     * msp_stack is extended until the beginning of RAM, when shared_date
     * was read out by partitions
     */
    .tfm_bl2_shared_data : ALIGN(32)
    {
        . += BOOT_TFM_SHARED_DATA_SIZE;
    } > RAM AT> RAM

    .msp_stack : ALIGN(32)
    {
        . += __msp_init_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK_MSP$$ZI$$Base = ADDR(.msp_stack);
    Image$$ARM_LIB_STACK_MSP$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);

    /* PSP is privileged in multi-core topology */
    .psp_stack : ALIGN(32)
    {
        . += __psp_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.psp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.psp_stack) + SIZEOF(.psp_stack);
#endif

    /**** PSA RoT DATA start here */
    Image$$TFM_PSA_RW_STACK_START$$Base = .;

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    .TFM_SP_ITS_LINKER_DATA : ALIGN(32)
    {
        *tfm_internal_trusted_storage*:*(.data*)
        *(TFM_SP_ITS_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_ITS_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_ITS_LINKER_DATA);
    Image$$TFM_SP_ITS_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_ITS_LINKER_DATA) + SIZEOF(.TFM_SP_ITS_LINKER_DATA);

    .TFM_SP_ITS_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_ITS_LINKER = .;
        *tfm_internal_trusted_storage*:*(.bss*)
        *tfm_internal_trusted_storage*:*(COMMON)
        *(TFM_SP_ITS_ATTR_ZI)
        . += (. - start_of_TFM_SP_ITS_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_ITS_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_ITS_LINKER_BSS);
    Image$$TFM_SP_ITS_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_ITS_LINKER_BSS) + SIZEOF(.TFM_SP_ITS_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_ITS_LINKER_STACK : ALIGN(128)
    {
        . += 0x680;
    } > RAM
    Image$$TFM_SP_ITS_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_ITS_LINKER_STACK);
    Image$$TFM_SP_ITS_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_ITS_LINKER_STACK) + SIZEOF(.TFM_SP_ITS_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
    .TFM_SP_AUDIT_LOG_LINKER_DATA : ALIGN(32)
    {
        *tfm_audit*:*(.data*)
        *(TFM_SP_AUDIT_LOG_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_AUDIT_LOG_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_AUDIT_LOG_LINKER_DATA);
    Image$$TFM_SP_AUDIT_LOG_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_AUDIT_LOG_LINKER_DATA) + SIZEOF(.TFM_SP_AUDIT_LOG_LINKER_DATA);

    .TFM_SP_AUDIT_LOG_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_AUDIT_LOG_LINKER = .;
        *tfm_audit*:*(.bss*)
        *tfm_audit*:*(COMMON)
        *(TFM_SP_AUDIT_LOG_ATTR_ZI)
        . += (. - start_of_TFM_SP_AUDIT_LOG_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_AUDIT_LOG_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_AUDIT_LOG_LINKER_BSS);
    Image$$TFM_SP_AUDIT_LOG_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_AUDIT_LOG_LINKER_BSS) + SIZEOF(.TFM_SP_AUDIT_LOG_LINKER_BSS);


#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
    .TFM_SP_CRYPTO_LINKER_DATA : ALIGN(32)
    {
        *tfm_crypto*:*(.data*)
        *(TFM_SP_CRYPTO_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_CRYPTO_LINKER_DATA);
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER_DATA) + SIZEOF(.TFM_SP_CRYPTO_LINKER_DATA);

    .TFM_SP_CRYPTO_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_CRYPTO_LINKER = .;
        *tfm_crypto*:*(.bss*)
        *tfm_crypto*:*(COMMON)
        *(TFM_SP_CRYPTO_ATTR_ZI)
        . += (. - start_of_TFM_SP_CRYPTO_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_CRYPTO_LINKER_BSS);
    Image$$TFM_SP_CRYPTO_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER_BSS) + SIZEOF(.TFM_SP_CRYPTO_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_CRYPTO_LINKER_STACK : ALIGN(128)
    {
        . += 0x2000;
    } > RAM
    Image$$TFM_SP_CRYPTO_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_CRYPTO_LINKER_STACK);
    Image$$TFM_SP_CRYPTO_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_CRYPTO_LINKER_STACK) + SIZEOF(.TFM_SP_CRYPTO_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
    .TFM_SP_PLATFORM_LINKER_DATA : ALIGN(32)
    {
        *tfm_platform*:*(.data*)
        *(TFM_SP_PLATFORM_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_PLATFORM_LINKER_DATA);
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER_DATA) + SIZEOF(.TFM_SP_PLATFORM_LINKER_DATA);

    .TFM_SP_PLATFORM_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_PLATFORM_LINKER = .;
        *tfm_platform*:*(.bss*)
        *tfm_platform*:*(COMMON)
        *(TFM_SP_PLATFORM_ATTR_ZI)
        . += (. - start_of_TFM_SP_PLATFORM_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_LINKER_BSS);
    Image$$TFM_SP_PLATFORM_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER_BSS) + SIZEOF(.TFM_SP_PLATFORM_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_PLATFORM_LINKER_STACK : ALIGN(128)
    {
        . += 0x0400;
    } > RAM
    Image$$TFM_SP_PLATFORM_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_LINKER_STACK);
    Image$$TFM_SP_PLATFORM_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_LINKER_STACK) + SIZEOF(.TFM_SP_PLATFORM_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    .TFM_SP_INITIAL_ATTESTATION_LINKER_DATA : ALIGN(32)
    {
        *tfm_attest*:*(.data*)
        *(TFM_SP_INITIAL_ATTESTATION_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_DATA);

    .TFM_SP_INITIAL_ATTESTATION_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_INITIAL_ATTESTATION_LINKER = .;
        *tfm_attest*:*(.bss*)
        *tfm_attest*:*(COMMON)
        *(TFM_SP_INITIAL_ATTESTATION_ATTR_ZI)
        . += (. - start_of_TFM_SP_INITIAL_ATTESTATION_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_INITIAL_ATTESTATION_LINKER_STACK : ALIGN(128)
    {
        . += 0x0A80;
    } > RAM
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK);
    Image$$TFM_SP_INITIAL_ATTESTATION_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
    .TFM_SP_CORE_TEST_LINKER_DATA : ALIGN(32)
    {
        *tfm_ss_core_test.*(.data*)
        *(TFM_SP_CORE_TEST_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_CORE_TEST_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_CORE_TEST_LINKER_DATA);
    Image$$TFM_SP_CORE_TEST_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_CORE_TEST_LINKER_DATA) + SIZEOF(.TFM_SP_CORE_TEST_LINKER_DATA);

    .TFM_SP_CORE_TEST_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_CORE_TEST_LINKER = .;
        *tfm_ss_core_test.*(.bss*)
        *tfm_ss_core_test.*(COMMON)
        *(TFM_SP_CORE_TEST_ATTR_ZI)
        . += (. - start_of_TFM_SP_CORE_TEST_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_CORE_TEST_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_CORE_TEST_LINKER_BSS);
    Image$$TFM_SP_CORE_TEST_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_CORE_TEST_LINKER_BSS) + SIZEOF(.TFM_SP_CORE_TEST_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_CORE_TEST_LINKER_STACK : ALIGN(128)
    {
        . += 0x0380;
    } > RAM
    Image$$TFM_SP_CORE_TEST_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_CORE_TEST_LINKER_STACK);
    Image$$TFM_SP_CORE_TEST_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_CORE_TEST_LINKER_STACK) + SIZEOF(.TFM_SP_CORE_TEST_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA : ALIGN(32)
    {
        *tfm_secure_client_service.*(.data*)
        *test_framework*(.data*)
        *uart_stdout.*(.data*)
        *Driver_USART.*(.data*)
        *arm_uart_drv.*(.data*)
        *uart_pl011_drv.*(.data*)
        *uart_cmsdk_drv*(.data*)
        *secure_suites.*(.data*)
        *attestation_s_interface_testsuite.*(.data*)
        *(TFM_SP_SECURE_TEST_PARTITION_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA);
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA);

    .TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_SECURE_TEST_PARTITION_LINKER = .;
        *tfm_secure_client_service.*(.bss*)
        *tfm_secure_client_service.*(COMMON)
        *test_framework*(.bss*)
        *test_framework*(COMMON)
        *uart_stdout.*(.bss*)
        *uart_stdout.*(COMMON)
        *Driver_USART.*(.bss*)
        *Driver_USART.*(COMMON)
        *arm_uart_drv.*(.bss*)
        *arm_uart_drv.*(COMMON)
        *uart_pl011_drv.*(.bss*)
        *uart_pl011_drv.*(COMMON)
        *uart_cmsdk_drv*(.bss*)
        *uart_cmsdk_drv*(COMMON)
        *secure_suites.*(.bss*)
        *secure_suites.*(COMMON)
        *attestation_s_interface_testsuite.*(.bss*)
        *attestation_s_interface_testsuite.*(COMMON)
        *(TFM_SP_SECURE_TEST_PARTITION_ATTR_ZI)
        . += (. - start_of_TFM_SP_SECURE_TEST_PARTITION_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS);
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK : ALIGN(128)
    {
        . += 0x0D00;
    } > RAM
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK);
    Image$$TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    .TFM_SP_IPC_SERVICE_TEST_LINKER_DATA : ALIGN(32)
    {
        *ipc_service_test.*(.data*)
        *(TFM_SP_IPC_SERVICE_TEST_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_DATA);
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_DATA) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER_DATA);

    .TFM_SP_IPC_SERVICE_TEST_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_IPC_SERVICE_TEST_LINKER = .;
        *ipc_service_test.*(.bss*)
        *ipc_service_test.*(COMMON)
        *(TFM_SP_IPC_SERVICE_TEST_ATTR_ZI)
        . += (. - start_of_TFM_SP_IPC_SERVICE_TEST_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_BSS);
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_BSS) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_IPC_SERVICE_TEST_LINKER_STACK : ALIGN(128)
    {
        . += 0x0220;
    } > RAM
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_STACK);
    Image$$TFM_SP_IPC_SERVICE_TEST_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_LINKER_STACK) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_SST
    .TFM_SP_SST_TEST_LINKER_DATA : ALIGN(32)
    {
        *tfm_sst_test_service.*(.data*)
        *(TFM_SP_SST_TEST_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_SST_TEST_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_SST_TEST_LINKER_DATA);
    Image$$TFM_SP_SST_TEST_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_SST_TEST_LINKER_DATA) + SIZEOF(.TFM_SP_SST_TEST_LINKER_DATA);

    .TFM_SP_SST_TEST_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_SST_TEST_LINKER = .;
        *tfm_sst_test_service.*(.bss*)
        *tfm_sst_test_service.*(COMMON)
        *(TFM_SP_SST_TEST_ATTR_ZI)
        . += (. - start_of_TFM_SP_SST_TEST_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_SST_TEST_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_SST_TEST_LINKER_BSS);
    Image$$TFM_SP_SST_TEST_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_SST_TEST_LINKER_BSS) + SIZEOF(.TFM_SP_SST_TEST_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_SST_TEST_LINKER_STACK : ALIGN(128)
    {
        . += 0x500;
    } > RAM
    Image$$TFM_SP_SST_TEST_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_SST_TEST_LINKER_STACK);
    Image$$TFM_SP_SST_TEST_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_SST_TEST_LINKER_STACK) + SIZEOF(.TFM_SP_SST_TEST_LINKER_STACK);
#endif

#endif /* TFM_PARTITION_TEST_SST */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    .TFM_SP_PLATFORM_MTK_LINKER_DATA : ALIGN(32)
    {
        *tfm_platform_MTK.*(.data*)
        *(TFM_SP_PLATFORM_MTK_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_SP_PLATFORM_MTK_LINKER_DATA$$RW$$Base = ADDR(.TFM_SP_PLATFORM_MTK_LINKER_DATA);
    Image$$TFM_SP_PLATFORM_MTK_LINKER_DATA$$RW$$Limit = ADDR(.TFM_SP_PLATFORM_MTK_LINKER_DATA) + SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER_DATA);

    .TFM_SP_PLATFORM_MTK_LINKER_BSS : ALIGN(32)
    {
        start_of_TFM_SP_PLATFORM_MTK_LINKER = .;
        *tfm_platform_MTK.*(.bss*)
        *tfm_platform_MTK.*(COMMON)
        *(TFM_SP_PLATFORM_MTK_ATTR_ZI)
        . += (. - start_of_TFM_SP_PLATFORM_MTK_LINKER) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_SP_PLATFORM_MTK_LINKER_DATA$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_MTK_LINKER_BSS);
    Image$$TFM_SP_PLATFORM_MTK_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_MTK_LINKER_BSS) + SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER_BSS);

#if defined (TFM_PSA_API)
    .TFM_SP_PLATFORM_MTK_LINKER_STACK : ALIGN(128)
    {
        . += 0x0400;
    } > RAM
    Image$$TFM_SP_PLATFORM_MTK_LINKER_STACK$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_MTK_LINKER_STACK);
    Image$$TFM_SP_PLATFORM_MTK_LINKER_STACK$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_MTK_LINKER_STACK) + SIZEOF(.TFM_SP_PLATFORM_MTK_LINKER_STACK);
#endif

#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

    /**** PSA RoT DATA end here */
    Image$$TFM_PSA_RW_STACK_END$$Base = .;

    .TFM_DATA : ALIGN(4)
    {
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);

    } > RAM AT> FLASH
    Image$$ER_TFM_DATA$$RW$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$RW$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA);

    .TFM_BSS : ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM AT> RAM
    Image$$ER_TFM_DATA$$ZI$$Base = ADDR(.TFM_BSS);
    Image$$ER_TFM_DATA$$ZI$$Limit = ADDR(.TFM_BSS) + SIZEOF(.TFM_BSS);

    Image$$ER_TFM_DATA$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA) + SIZEOF(.TFM_BSS);

#if defined (S_RAM_CODE_START)
    /* Code executed from RAM */
    .TFM_RAM_CODE S_RAM_CODE_START :
    {
        KEEP(*(.ramfunc))
    } > RAM AT> FLASH
#endif

#ifndef TFM_MULTI_CORE_TOPOLOGY
    /*
     * Place the CMSE Veneers (containing the SG instruction) after the code, in a
     * separate 32 bytes aligned region so that the SAU can programmed to just set
     * this region as Non-Secure Callable.
     */
    .gnu.sgstubs : ALIGN(32)
    {
        *(.gnu.sgstubs*)
        . = ALIGN(32);
    } > VENEERS AT> VENEERS
    Load$$LR$$LR_VENEER$$Base = ADDR(.gnu.sgstubs);
    Load$$LR$$LR_VENEER$$Limit = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);
#endif

    Load$$LR$$LR_NS_PARTITION$$Base = NS_PARTITION_START;

#ifdef BL2
    Load$$LR$$LR_SECONDARY_PARTITION$$Base = SECONDARY_PARTITION_START;
#endif /* BL2 */

    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);
}
