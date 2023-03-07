/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_IRQ_LIST_H__
#define __TFM_IRQ_LIST_H__

#include "psa/service.h"

#define TFM_DEFAULT_SECURE_IRQ_PRIOTITY 128

struct tfm_core_irq_signal_data_t {
    int32_t partition_id;
    psa_signal_t signal_value;
    IRQn_Type irq_line;
    uint32_t irq_priority;
};

extern const struct tfm_core_irq_signal_data_t tfm_core_irq_signals[];
extern const size_t tfm_core_irq_signals_count;

#endif /* __TFM_IRQ_LIST_H__ */
