/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#ifndef __TFM_SPM_DB_INC__
#define __TFM_SPM_DB_INC__

#include "spm_api.h"
#include "psa_manifest/sid.h"

/**************************************************************************/
/** IRQ count per partition */
/**************************************************************************/
#ifdef TFM_PARTITION_SECURE_STORAGE
#define TFM_PARTITION_TFM_SP_STORAGE_IRQ_COUNT 0
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
#define TFM_PARTITION_TFM_SP_ITS_IRQ_COUNT 0
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
#define TFM_PARTITION_TFM_SP_AUDIT_LOG_IRQ_COUNT 0
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
#define TFM_PARTITION_TFM_SP_CRYPTO_IRQ_COUNT 0
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
#define TFM_PARTITION_TFM_SP_PLATFORM_IRQ_COUNT 0
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
#define TFM_PARTITION_TFM_SP_INITIAL_ATTESTATION_IRQ_COUNT 0
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_PARTITION_TFM_SP_CORE_TEST_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
#define TFM_PARTITION_TFM_SP_CORE_TEST_2_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
#define TFM_PARTITION_TFM_SP_SECURE_TEST_PARTITION_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
#define TFM_PARTITION_TFM_SP_IPC_SERVICE_TEST_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
#define TFM_PARTITION_TFM_SP_IPC_CLIENT_TEST_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
#define TFM_PARTITION_TFM_IRQ_TEST_1_IRQ_COUNT 1
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
#define TFM_PARTITION_TFM_SP_SST_TEST_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
#define TFM_PARTITION_TFM_SP_SECURE_CLIENT_2_IRQ_COUNT 0
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
#define TFM_PARTITION_TFM_SP_MULTI_CORE_TEST_IRQ_COUNT 0
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
#define TFM_PARTITION_TFM_IRQ_MT7933_IRQ_COUNT 4
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
#define TFM_PARTITION_TFM_SP_PLATFORM_MTK_IRQ_COUNT 0
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
#define TFM_PARTITION_TFM_MTK_CI_TEST_IRQ_COUNT 0
#endif /* MTK_CI_TEST */

/**************************************************************************/
/** Declarations of partition init functions */
/**************************************************************************/
#ifdef TFM_PSA_API
extern void tfm_nspm_thread_entry(void);
#endif

#ifdef TFM_PARTITION_SECURE_STORAGE
extern void tfm_sst_req_mngr_init(void);
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
extern void tfm_its_req_mngr_init(void);
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
extern void audit_core_init(void);
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
extern void tfm_crypto_init(void);
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
extern void platform_sp_init(void);
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
extern void attest_partition_init(void);
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
extern void core_test_init(void);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
extern void core_test_2_init(void);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
extern void tfm_secure_client_service_init(void);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
extern void ipc_service_test_main(void);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
extern void ipc_client_test_main(void);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
extern void tfm_irq_test_1_init(void);
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
extern void tfm_sst_test_init(void);
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
extern void tfm_secure_client_2_init(void);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
extern void multi_core_test_main(void);
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
extern void tfm_irq_mt7933_init(void);
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
extern void platform_sp_MTK_init(void);
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
extern void tfm_CI_test_init(void);
#endif /* MTK_CI_TEST */

/**************************************************************************/
/** Memory region declarations */
/**************************************************************************/
#ifdef TFM_PSA_API
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

#ifdef TFM_PARTITION_SECURE_STORAGE
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_STORAGE_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_ITS_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_AUDIT_LOG_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_CRYPTO_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_INITIAL_ATTESTATION_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_CORE_TEST_2_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_TEST_PARTITION_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_SERVICE_TEST_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_IPC_CLIENT_TEST_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_TEST_1_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_SST_TEST_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_SECURE_CLIENT_2_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_MULTI_CORE_TEST_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_IRQ_MT7933_LINKER, _STACK$$ZI$$Limit);
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_PLATFORM_MTK_LINKER, _STACK$$ZI$$Limit);
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, $$Base);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, $$Limit);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, _DATA$$RW$$Base);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, _DATA$$RW$$Limit);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, _DATA$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, _DATA$$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, _STACK$$ZI$$Base);
REGION_DECLARE(Image$$, TFM_MTK_CI_TEST_LINKER, _STACK$$ZI$$Limit);
#endif /* MTK_CI_TEST */

#endif /* defined(TFM_PSA_API) */

#ifndef TFM_PSA_API
/**************************************************************************/
/** Context stacks for IRQ handling */
/**************************************************************************/
/* The max size of the context stack can be calculated as a function of the IRQ
 * count of the secure partition:
 *
 * max_stack_size = intr_ctx_size + (IRQ_CNT * (intr_ctx_size + hndl_ctx_size))
 *
 * where:
 *   intr_ctx: Frame pushed when the partition is interrupted
 *   hndl_ctx: Frame pushed when the partition is handling an interrupt
 */
static uint32_t ns_interrupt_ctx_stack[
        sizeof(struct interrupted_ctx_stack_frame_t) / sizeof(uint32_t)];

static uint32_t tfm_core_interrupt_ctx_stack[
        sizeof(struct interrupted_ctx_stack_frame_t) / sizeof(uint32_t)];

#ifdef TFM_PARTITION_SECURE_STORAGE
static uint32_t ctx_stack_TFM_SP_STORAGE[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_STORAGE_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
static uint32_t ctx_stack_TFM_SP_ITS[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_ITS_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
static uint32_t ctx_stack_TFM_SP_AUDIT_LOG[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_AUDIT_LOG_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
static uint32_t ctx_stack_TFM_SP_CRYPTO[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_CRYPTO_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
static uint32_t ctx_stack_TFM_SP_PLATFORM[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_PLATFORM_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
static uint32_t ctx_stack_TFM_SP_INITIAL_ATTESTATION[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_INITIAL_ATTESTATION_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
static uint32_t ctx_stack_TFM_SP_CORE_TEST[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_CORE_TEST_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
static uint32_t ctx_stack_TFM_SP_CORE_TEST_2[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_CORE_TEST_2_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
static uint32_t ctx_stack_TFM_SP_SECURE_TEST_PARTITION[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_SECURE_TEST_PARTITION_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
static uint32_t ctx_stack_TFM_SP_IPC_SERVICE_TEST[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_IPC_SERVICE_TEST_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
static uint32_t ctx_stack_TFM_SP_IPC_CLIENT_TEST[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_IPC_CLIENT_TEST_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
static uint32_t ctx_stack_TFM_IRQ_TEST_1[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_IRQ_TEST_1_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
static uint32_t ctx_stack_TFM_SP_SST_TEST[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_SST_TEST_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
static uint32_t ctx_stack_TFM_SP_SECURE_CLIENT_2[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_SECURE_CLIENT_2_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
static uint32_t ctx_stack_TFM_SP_MULTI_CORE_TEST[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_MULTI_CORE_TEST_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
static uint32_t ctx_stack_TFM_IRQ_MT7933[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_IRQ_MT7933_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
static uint32_t ctx_stack_TFM_SP_PLATFORM_MTK[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_SP_PLATFORM_MTK_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
static uint32_t ctx_stack_TFM_MTK_CI_TEST[
        (sizeof(struct interrupted_ctx_stack_frame_t) +
            (TFM_PARTITION_TFM_MTK_CI_TEST_IRQ_COUNT) * (
                sizeof(struct interrupted_ctx_stack_frame_t) +
                sizeof(struct handler_ctx_stack_frame_t)
        )) / sizeof(uint32_t)];
#endif /* MTK_CI_TEST */


uint32_t *ctx_stack_list[] =
{
    ns_interrupt_ctx_stack,
    tfm_core_interrupt_ctx_stack,
#ifdef TFM_PARTITION_SECURE_STORAGE
    ctx_stack_TFM_SP_STORAGE,
#endif /* TFM_PARTITION_SECURE_STORAGE */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    ctx_stack_TFM_SP_ITS,
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
#ifdef TFM_PARTITION_AUDIT_LOG
    ctx_stack_TFM_SP_AUDIT_LOG,
#endif /* TFM_PARTITION_AUDIT_LOG */
#ifdef TFM_PARTITION_CRYPTO
    ctx_stack_TFM_SP_CRYPTO,
#endif /* TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
    ctx_stack_TFM_SP_PLATFORM,
#endif /* TFM_PARTITION_PLATFORM */
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    ctx_stack_TFM_SP_INITIAL_ATTESTATION,
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_TEST_CORE
    ctx_stack_TFM_SP_CORE_TEST,
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_CORE
    ctx_stack_TFM_SP_CORE_TEST_2,
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    ctx_stack_TFM_SP_SECURE_TEST_PARTITION,
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_TEST_CORE_IPC
    ctx_stack_TFM_SP_IPC_SERVICE_TEST,
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_PARTITION_TEST_CORE_IPC
    ctx_stack_TFM_SP_IPC_CLIENT_TEST,
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_ENABLE_IRQ_TEST
    ctx_stack_TFM_IRQ_TEST_1,
#endif /* TFM_ENABLE_IRQ_TEST */
#ifdef TFM_PARTITION_TEST_SST
    ctx_stack_TFM_SP_SST_TEST,
#endif /* TFM_PARTITION_TEST_SST */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    ctx_stack_TFM_SP_SECURE_CLIENT_2,
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_MULTI_CORE_TEST
    ctx_stack_TFM_SP_MULTI_CORE_TEST,
#endif /* TFM_MULTI_CORE_TEST */
#ifdef TFM_ENABLE_MT7933_IRQ
    ctx_stack_TFM_IRQ_MT7933,
#endif /* TFM_ENABLE_MT7933_IRQ */
#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    ctx_stack_TFM_SP_PLATFORM_MTK,
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */
#ifdef MTK_CI_TEST
    ctx_stack_TFM_MTK_CI_TEST,
#endif /* MTK_CI_TEST */
};
#endif /* !defined(TFM_PSA_API) */

/**************************************************************************/
/** Dependencies array for Secure Partition */
/**************************************************************************/
#ifdef TFM_PARTITION_SECURE_STORAGE
static int32_t dependencies_TFM_SP_STORAGE[] =
{
    TFM_CRYPTO_SID,
    TFM_ITS_SET_SID,
    TFM_ITS_GET_SID,
    TFM_ITS_GET_INFO_SID,
    TFM_ITS_REMOVE_SID,
    TFM_SP_PLATFORM_NV_COUNTER_SID,
};
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_CRYPTO
static int32_t dependencies_TFM_SP_CRYPTO[] =
{
    TFM_ITS_SET_SID,
    TFM_ITS_GET_SID,
    TFM_ITS_GET_INFO_SID,
    TFM_ITS_REMOVE_SID,
};
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
static int32_t dependencies_TFM_SP_INITIAL_ATTESTATION[] =
{
    TFM_CRYPTO_SID,
};
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
static int32_t dependencies_TFM_SP_CORE_TEST[] =
{
    SPM_CORE_TEST_2_INVERT_SID,
    SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SID,
    SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
};
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
static int32_t dependencies_TFM_SP_SECURE_TEST_PARTITION[] =
{
    TFM_SECURE_CLIENT_2_SID,
    TFM_CRYPTO_SID,
    TFM_SST_SET_SID,
    TFM_SST_GET_SID,
    TFM_SST_GET_INFO_SID,
    TFM_SST_REMOVE_SID,
    TFM_SST_GET_SUPPORT_SID,
    TFM_ITS_SET_SID,
    TFM_ITS_GET_SID,
    TFM_ITS_GET_INFO_SID,
    TFM_ITS_REMOVE_SID,
    TFM_ATTEST_GET_TOKEN_SID,
    TFM_ATTEST_GET_TOKEN_SIZE_SID,
    TFM_ATTEST_GET_PUBLIC_KEY_SID,
    TFM_SST_TEST_PREPARE_SID,
    TFM_SP_PLATFORM_SYSTEM_RESET_SID,
    TFM_SP_PLATFORM_IOCTL_SID,
};
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
static int32_t dependencies_TFM_SP_IPC_CLIENT_TEST[] =
{
    IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SID,
    IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SID,
    IPC_SERVICE_TEST_BASIC_SID,
    IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SID,
};
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_SST
static int32_t dependencies_TFM_SP_SST_TEST[] =
{
    TFM_CRYPTO_SID,
    TFM_ITS_GET_SID,
    TFM_ITS_REMOVE_SID,
};
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
static int32_t dependencies_TFM_SP_SECURE_CLIENT_2[] =
{
    TFM_ITS_GET_SID,
    TFM_CRYPTO_SID,
};
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

/**************************************************************************/
/** The static data of the partition list */
/**************************************************************************/
const struct spm_partition_static_data_t static_data_list[] =
{
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_NON_SECURE_ID,
#ifdef TFM_PSA_API
#if TFM_MULTI_CORE_TOPOLOGY
        .partition_flags      = SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_IPC,
#else
        .partition_flags      = SPM_PART_FLAG_APP_ROT | SPM_PART_FLAG_IPC,
#endif
        .partition_priority   = TFM_PRIORITY_LOW,
        .partition_init       = tfm_nspm_thread_entry,
#else
        .partition_flags      = 0,
#endif
    },

#ifndef TFM_PSA_API
    {
        .partition_id         = TFM_SP_CORE_ID,
        .partition_flags      = SPM_PART_FLAG_APP_ROT | SPM_PART_FLAG_PSA_ROT,
    },
#endif

#ifdef TFM_PARTITION_SECURE_STORAGE
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_STORAGE,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_sst_req_mngr_init,
        .dependencies_num     = 6,
        .p_dependencies       = dependencies_TFM_SP_STORAGE,
    },
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_ITS,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_its_req_mngr_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_AUDIT_LOG,
        .partition_flags      = 0
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = audit_core_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_CRYPTO,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_crypto_init,
        .dependencies_num     = 4,
        .p_dependencies       = dependencies_TFM_SP_CRYPTO,
    },
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_PLATFORM,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = platform_sp_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_INITIAL_ATTESTATION,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = attest_partition_init,
        .dependencies_num     = 1,
        .p_dependencies       = dependencies_TFM_SP_INITIAL_ATTESTATION,
    },
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_CORE_TEST,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = core_test_init,
        .dependencies_num     = 3,
        .p_dependencies       = dependencies_TFM_SP_CORE_TEST,
    },
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_CORE_TEST_2,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = core_test_2_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_SECURE_TEST_PARTITION,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_secure_client_service_init,
        .dependencies_num     = 17,
        .p_dependencies       = dependencies_TFM_SP_SECURE_TEST_PARTITION,
    },
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_IPC_SERVICE_TEST,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(HIGH),
        .partition_init       = ipc_service_test_main,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_IPC_CLIENT_TEST,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = ipc_client_test_main,
        .dependencies_num     = 4,
        .p_dependencies       = dependencies_TFM_SP_IPC_CLIENT_TEST,
    },
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_IRQ_TEST_1,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_irq_test_1_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_SST_TEST,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_sst_test_init,
        .dependencies_num     = 3,
        .p_dependencies       = dependencies_TFM_SP_SST_TEST,
    },
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_SECURE_CLIENT_2,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_secure_client_2_init,
        .dependencies_num     = 2,
        .p_dependencies       = dependencies_TFM_SP_SECURE_CLIENT_2,
    },
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_MULTI_CORE_TEST,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = multi_core_test_main,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_IRQ_MT7933,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_irq_mt7933_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_SP_PLATFORM_MTK,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_PSA_ROT | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = platform_sp_MTK_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
    {
#ifdef TFM_PSA_API
        .psa_framework_version = 0x0100,
#endif /* defined(TFM_PSA_API) */
        .partition_id         = TFM_MTK_CI_TEST,
        .partition_flags      = SPM_PART_FLAG_IPC
                              | SPM_PART_FLAG_APP_ROT
                              ,
        .partition_priority   = TFM_PRIORITY(NORMAL),
        .partition_init       = tfm_CI_test_init,
        .dependencies_num     = 0,
        .p_dependencies       = NULL,
    },
#endif /* MTK_CI_TEST */

};

/**************************************************************************/
/** The platform data of the partition list */
/**************************************************************************/
#ifdef TFM_PARTITION_AUDIT_LOG
const struct tfm_spm_partition_platform_data_t *
    platform_data_list_TFM_SP_AUDIT_LOG[] =
{
#ifdef AUDIT_UART_REDIRECTION
    TFM_PERIPHERAL_UART1,
#endif /* AUDIT_UART_REDIRECTION */
    NULL
};
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_TEST_CORE
const struct tfm_spm_partition_platform_data_t *
    platform_data_list_TFM_SP_CORE_TEST[] =
{
    TFM_PERIPHERAL_FPGA_IO,
    NULL
};
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
const struct tfm_spm_partition_platform_data_t *
    platform_data_list_TFM_SP_SECURE_TEST_PARTITION[] =
{
    TFM_PERIPHERAL_STD_UART,
    NULL
};
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_ENABLE_IRQ_TEST
const struct tfm_spm_partition_platform_data_t *
    platform_data_list_TFM_IRQ_TEST_1[] =
{
    TFM_PERIPHERAL_TIMER0,
    NULL
};
#endif /* TFM_ENABLE_IRQ_TEST */

const struct tfm_spm_partition_platform_data_t **platform_data_list_list[] =
{
    NULL,

#ifndef TFM_PSA_API
    NULL,
#endif

#ifdef TFM_PARTITION_SECURE_STORAGE
    NULL,
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    NULL,
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
    platform_data_list_TFM_SP_AUDIT_LOG,
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
    NULL,
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
    NULL,
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    NULL,
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
    platform_data_list_TFM_SP_CORE_TEST,
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
    NULL,
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    platform_data_list_TFM_SP_SECURE_TEST_PARTITION,
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    NULL,
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    NULL,
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
    platform_data_list_TFM_IRQ_TEST_1,
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
    NULL,
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    NULL,
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
    NULL,
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
    NULL,
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    NULL,
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
    NULL,
#endif /* MTK_CI_TEST */

};

/**************************************************************************/
/** The memory data of the partition list */
/**************************************************************************/
#ifdef TFM_PSA_API
const struct tfm_spm_partition_memory_data_t memory_data_list[] =
{
    {
        .stack_bottom         = PART_REGION_ADDR(ARM_LIB_STACK, $$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(ARM_LIB_STACK, $$ZI$$Limit),
        .rw_start             = PART_REGION_ADDR(ARM_LIB_STACK, $$ZI$$Base),
    },
#ifdef TFM_PARTITION_SECURE_STORAGE
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_STORAGE_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_SECURE_STORAGE */

#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_ITS_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_ITS_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_ITS_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_ITS_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_ITS_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_ITS_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_ITS_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_ITS_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_ITS_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_ITS_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_AUDIT_LOG
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_AUDIT_LOG_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_AUDIT_LOG */

#ifdef TFM_PARTITION_CRYPTO
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_CRYPTO_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_PLATFORM_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_INITIAL_ATTESTATION_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_CORE_TEST_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_CORE_TEST_2_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_SECURE_TEST_PARTITION_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_IPC_SERVICE_TEST_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_IPC_CLIENT_TEST_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
    {
        .code_start           = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_IRQ_TEST_1_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SST
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_SST_TEST_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_SST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_SECURE_CLIENT_2_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_MULTI_CORE_TEST
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_MULTI_CORE_TEST_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_MULTI_CORE_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
    {
        .code_start           = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_IRQ_MT7933_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* TFM_ENABLE_MT7933_IRQ */

#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    {
        .code_start           = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_SP_PLATFORM_MTK_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

#ifdef MTK_CI_TEST
    {
        .code_start           = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, $$Base),
        .code_limit           = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, $$Limit),
        .ro_start             = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, $$RO$$Base),
        .ro_limit             = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, $$RO$$Limit),
        .rw_start             = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, _DATA$$RW$$Base),
        .rw_limit             = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, _DATA$$RW$$Limit),
        .zi_start             = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, _DATA$$ZI$$Base),
        .zi_limit             = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, _DATA$$ZI$$Limit),
        .stack_bottom         = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, _STACK$$ZI$$Base),
        .stack_top            = PART_REGION_ADDR(TFM_MTK_CI_TEST_LINKER, _STACK$$ZI$$Limit),
    },
#endif /* MTK_CI_TEST */

};
#endif /* defined(TFM_PSA_API) */

/**************************************************************************/
/** The partition list for the DB */
/**************************************************************************/
static struct spm_partition_desc_t partition_list [] =
{
    {{0}}, /* placeholder for Non-secure internal partition */
#ifndef TFM_PSA_API
    {{0}},   /* placeholder for TF-M Core internal partition */
#endif

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_STORAGE */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_SECURE_STORAGE
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_SECURE_STORAGE */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_ITS */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_AUDIT_LOG */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_AUDIT_LOG
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_AUDIT_LOG */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_CRYPTO */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_CRYPTO
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_CRYPTO */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_PLATFORM */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_PLATFORM
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_PLATFORM */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_INITIAL_ATTESTATION */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_CORE_TEST */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_CORE
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_CORE */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_CORE_TEST_2 */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_CORE
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_CORE */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_SECURE_TEST_PARTITION */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_IPC_SERVICE_TEST */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_CORE_IPC
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_CORE_IPC */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_IPC_CLIENT_TEST */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_CORE_IPC
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_CORE_IPC */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_IRQ_TEST_1 */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_ENABLE_IRQ_TEST
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_ENABLE_IRQ_TEST */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_SST_TEST */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_SST
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_SST */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_SECURE_CLIENT_2 */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_MULTI_CORE_TEST */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_MULTI_CORE_TEST
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_MULTI_CORE_TEST */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_IRQ_MT7933 */
    /* -----------------------------------------------------------------------*/
#ifdef TFM_ENABLE_MT7933_IRQ
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* TFM_ENABLE_MT7933_IRQ */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_SP_PLATFORM_MTK */
    /* -----------------------------------------------------------------------*/
#ifdef MTK_HW_ACCELERATOR_WO_MBEDTLS
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* MTK_HW_ACCELERATOR_WO_MBEDTLS */

    /* -----------------------------------------------------------------------*/
    /* - Partition DB record for TFM_MTK_CI_TEST */
    /* -----------------------------------------------------------------------*/
#ifdef MTK_CI_TEST
    {
    /* Runtime data */
        .runtime_data             = {0},
        .static_data              = NULL,
        .platform_data_list       = NULL,
    },
#endif /* MTK_CI_TEST */

};

struct spm_partition_db_t g_spm_partition_db = {
    .is_init = 0,
    .partition_count = sizeof(partition_list) / sizeof(partition_list[0]),
#ifndef TFM_PSA_API
    .running_partition_idx = 0,
#endif
    .partitions = partition_list,
};

#endif /* __TFM_SPM_DB_INC__ */
