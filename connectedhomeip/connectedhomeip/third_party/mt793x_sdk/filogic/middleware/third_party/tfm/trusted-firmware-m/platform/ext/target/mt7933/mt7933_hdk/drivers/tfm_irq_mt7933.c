/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <mt7933.h>
#include "tfm_api.h"
#include "hal_devapc.h"
#include "hal_asic_mpu.h"

void SPM_CORE_DEVAPC_INFRA_AON_SECURE_VIO_IRQn_isr(void)
{
    devapc_irq_handler(DEVAPC_INFRA_AON_SECURE_VIO_IRQn);
}

void SPM_CORE_DEVAPC_AUD_BUS_SECURE_VIO_IRQn_isr(void)
{
    devapc_irq_handler(DEVAPC_AUD_BUS_SECURE_VIO_IRQn);
}

void SPM_CORE_ASIC_MPU_SECURE_VIO_IRQn_isr(void)
{
    asic_mpu_irq_handler(ASIC_MPU_SECURE_VIO_IRQn);
}

void SPM_CORE_ASIC_MPU_PSRAM_SECURE_VIO_IRQn_isr(void)
{
    asic_mpu_irq_handler(ASIC_MPU_PSRAM_SECURE_VIO_IRQn);
}

int32_t tfm_irq_mt7933_init(void)
{
#ifdef TFM_PSA_API
  #error "TFM_PSA_API not supported yet in mt7933."
#else
    return TFM_SUCCESS;
#endif /* TFM_PSA_API */
}
