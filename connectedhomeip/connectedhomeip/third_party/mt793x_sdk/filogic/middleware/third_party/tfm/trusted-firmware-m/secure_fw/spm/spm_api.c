/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* All the APIs defined in this file are common for library and IPC model. */

#include <stdio.h>
#include <string.h>
#include "spm_api.h"
#include "tfm_spm_hal.h"
#include "tfm_memory_utils.h"
#include "spm_db.h"
#include "tfm_internal.h"
#include "tfm_api.h"
#include "tfm_nspm.h"
#include "tfm_core.h"
#include "tfm_peripherals_def.h"
#include "spm_partition_defs.h"
#include "region.h"

#define NON_SECURE_INTERNAL_PARTITION_DB_IDX 0
#define TFM_CORE_INTERNAL_PARTITION_DB_IDX   1

/* Define SPM DB structure */
#include "tfm_spm_db.inc"

uint32_t get_partition_idx(uint32_t partition_id)
{
    uint32_t i;

    if (partition_id == INVALID_PARTITION_ID) {
        return SPM_INVALID_PARTITION_IDX;
    }

    for (i = 0; i < g_spm_partition_db.partition_count; ++i) {
        if (g_spm_partition_db.partitions[i].static_data->partition_id ==
                partition_id) {
            return i;
        }
    }
    return SPM_INVALID_PARTITION_IDX;
}

enum spm_err_t tfm_spm_db_init(void)
{
    uint32_t i;

    /* This function initialises partition db */

    /* For the non secure Execution environment */
#ifndef TFM_PSA_API
    tfm_nspm_configure_clients();
#endif

    for (i = 0; i < g_spm_partition_db.partition_count; i++) {
#ifndef TFM_PSA_API
        g_spm_partition_db.partitions[i].runtime_data.partition_state =
            SPM_PARTITION_STATE_UNINIT;
        g_spm_partition_db.partitions[i].runtime_data.caller_partition_idx =
            SPM_INVALID_PARTITION_IDX;
        g_spm_partition_db.partitions[i].runtime_data.caller_client_id =
            TFM_INVALID_CLIENT_ID;
        g_spm_partition_db.partitions[i].runtime_data.ctx_stack_ptr =
            ctx_stack_list[i];
#endif /* !defined(TFM_PSA_API) */
        g_spm_partition_db.partitions[i].static_data = &static_data_list[i];
        g_spm_partition_db.partitions[i].platform_data_list =
                                                     platform_data_list_list[i];
#ifdef TFM_PSA_API
        g_spm_partition_db.partitions[i].memory_data = &memory_data_list[i];
#endif
    }
    g_spm_partition_db.is_init = 1;

    return SPM_ERR_OK;
}

uint32_t tfm_spm_partition_get_partition_id(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].static_data->
            partition_id;
}

uint32_t tfm_spm_partition_get_flags(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].static_data->
            partition_flags;
}

uint32_t tfm_spm_partition_get_privileged_mode(uint32_t partition_flags)
{
    if (partition_flags & SPM_PART_FLAG_PSA_ROT) {
        return TFM_PARTITION_PRIVILEGED_MODE;
    } else {
        return TFM_PARTITION_UNPRIVILEGED_MODE;
    }
}

bool tfm_is_partition_privileged(uint32_t partition_idx)
{
    uint32_t flags = tfm_spm_partition_get_flags(partition_idx);

    return tfm_spm_partition_get_privileged_mode(flags) ==
        TFM_PARTITION_PRIVILEGED_MODE;
}

__attribute__((section("SFN")))
void tfm_spm_partition_change_privilege(uint32_t privileged)
{
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();

    if (privileged == TFM_PARTITION_PRIVILEGED_MODE) {
        ctrl.b.nPRIV = 0;
    } else {
        ctrl.b.nPRIV = 1;
    }

    __set_CONTROL(ctrl.w);
}
