/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#include "secure_fw/services/secure_storage/psa_manifest/tfm_secure_storage.h"
#include "secure_fw/services/internal_trusted_storage/psa_manifest/tfm_internal_trusted_storage.h"
#include "secure_fw/services/audit_logging/psa_manifest/tfm_audit_logging.h"
#include "secure_fw/services/crypto/psa_manifest/tfm_crypto.h"
#include "secure_fw/services/platform/psa_manifest/tfm_platform.h"
#include "secure_fw/services/initial_attestation/psa_manifest/tfm_initial_attestation.h"
#include "test/test_services/tfm_core_test/psa_manifest/tfm_test_core.h"
#include "test/test_services/tfm_core_test_2/psa_manifest/tfm_test_core_2.h"
#include "test/test_services/tfm_secure_client_service/psa_manifest/tfm_test_client_service.h"
#include "test/test_services/tfm_ipc_service/psa_manifest/tfm_ipc_service_partition.h"
#include "test/test_services/tfm_ipc_client/psa_manifest/tfm_ipc_client_partition.h"
#include "test/test_services/tfm_irq_test_service_1/psa_manifest/tfm_irq_test_service_1.h"
#include "test/test_services/tfm_sst_test_service/psa_manifest/tfm_sst_test_service.h"
#include "test/test_services/tfm_secure_client_2/psa_manifest/tfm_secure_client_2.h"
#include "test/test_services/tfm_multi_core_test/psa_manifest/tfm_multi_core_test.h"
#include "platform/ext/target/mt7933/mt7933_hdk/drivers/psa_manifest/tfm_irq_mt7933.h"
#include "platform/ext/target/mt7933/mt7933_hdk/services/psa_manifest/tfm_platform_MTK.h"
#include "test/test_services/tfm_MTK_CI_test/psa_manifest/tfm_MTK_CI_test.h"
#include "cmsis_compiler.h"

/* Definitions of the signals of the IRQs (if any) */
const struct tfm_core_irq_signal_data_t tfm_core_irq_signals[] = {
#ifdef TFM_ENABLE_IRQ_TEST
    { TFM_IRQ_TEST_1, SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ, TFM_TIMER0_IRQ, 64 },
#endif /* TFM_ENABLE_IRQ_TEST */
#ifdef TFM_ENABLE_MT7933_IRQ
    { TFM_IRQ_MT7933, SPM_CORE_DEVAPC_INFRA_AON_SECURE_VIO_IRQn, DEVAPC_INFRA_AON_SECURE_VIO_IRQn, 64 },
    { TFM_IRQ_MT7933, SPM_CORE_DEVAPC_AUD_BUS_SECURE_VIO_IRQn, DEVAPC_AUD_BUS_SECURE_VIO_IRQn, 64 },
    { TFM_IRQ_MT7933, SPM_CORE_ASIC_MPU_SECURE_VIO_IRQn, ASIC_MPU_SECURE_VIO_IRQn, 64 },
    { TFM_IRQ_MT7933, SPM_CORE_ASIC_MPU_PSRAM_SECURE_VIO_IRQn, ASIC_MPU_PSRAM_SECURE_VIO_IRQn, 64 },
#endif /* TFM_ENABLE_MT7933_IRQ */
   {0, 0, (IRQn_Type) 0, 0}                         /* add dummy element to avoid non-standard empty array */
};

const size_t tfm_core_irq_signals_count = (sizeof(tfm_core_irq_signals) /
                                           sizeof(*tfm_core_irq_signals)) - 1; /* adjust for the dummy element */

/* Definitions of privileged IRQ handlers (if any) */
#ifdef TFM_ENABLE_IRQ_TEST
void TFM_TIMER0_IRQ_Handler(void)
{
    __disable_irq();
    /* It is OK to call tfm_irq_handler directly from here, as we are already
     * in handler mode, and we will not be pre-empted as we disabled interrupts
     */
    tfm_irq_handler(TFM_IRQ_TEST_1, SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ, TFM_TIMER0_IRQ);
    __enable_irq();
}

#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_ENABLE_MT7933_IRQ
void DEVAPC_INFRA_AON_SECURE_VIO_IRQn_Handler(void)
{
    __disable_irq();
    /* It is OK to call tfm_irq_handler directly from here, as we are already
     * in handler mode, and we will not be pre-empted as we disabled interrupts
     */
    tfm_irq_handler(TFM_IRQ_MT7933, SPM_CORE_DEVAPC_INFRA_AON_SECURE_VIO_IRQn, DEVAPC_INFRA_AON_SECURE_VIO_IRQn);
    __enable_irq();
}

void DEVAPC_AUD_BUS_SECURE_VIO_IRQn_Handler(void)
{
    __disable_irq();
    /* It is OK to call tfm_irq_handler directly from here, as we are already
     * in handler mode, and we will not be pre-empted as we disabled interrupts
     */
    tfm_irq_handler(TFM_IRQ_MT7933, SPM_CORE_DEVAPC_AUD_BUS_SECURE_VIO_IRQn, DEVAPC_AUD_BUS_SECURE_VIO_IRQn);
    __enable_irq();
}

void ASIC_MPU_SECURE_VIO_IRQn_Handler(void)
{
    __disable_irq();
    /* It is OK to call tfm_irq_handler directly from here, as we are already
     * in handler mode, and we will not be pre-empted as we disabled interrupts
     */
    tfm_irq_handler(TFM_IRQ_MT7933, SPM_CORE_ASIC_MPU_SECURE_VIO_IRQn, ASIC_MPU_SECURE_VIO_IRQn);
    __enable_irq();
}

void ASIC_MPU_PSRAM_SECURE_VIO_IRQn_Handler(void)
{
    __disable_irq();
    /* It is OK to call tfm_irq_handler directly from here, as we are already
     * in handler mode, and we will not be pre-empted as we disabled interrupts
     */
    tfm_irq_handler(TFM_IRQ_MT7933, SPM_CORE_ASIC_MPU_PSRAM_SECURE_VIO_IRQn, ASIC_MPU_PSRAM_SECURE_VIO_IRQn);
    __enable_irq();
}

#endif /* TFM_ENABLE_MT7933_IRQ */

