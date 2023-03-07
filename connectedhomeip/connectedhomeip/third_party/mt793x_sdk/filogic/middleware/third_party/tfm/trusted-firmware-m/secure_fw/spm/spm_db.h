/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_DB_H__
#define __SPM_DB_H__

#include <stdint.h>
#include "spm_api.h"
#include "target_cfg.h"
#ifdef TFM_PSA_API
#include "tfm_spm_hal.h"
#endif

struct spm_partition_desc_t;
struct spm_partition_db_t;

typedef void(*sp_entry_point)(void);

#define TFM_PARTITION_TYPE_APP   "APPLICATION-ROT"
#define TFM_PARTITION_TYPE_PSA   "PSA-ROT"

#ifdef TFM_PSA_API
#define TFM_PRIORITY_LOW    THRD_PRIOR_LOWEST
#define TFM_PRIORITY_NORMAL THRD_PRIOR_MEDIUM
#define TFM_PRIORITY_HIGH   THRD_PRIOR_HIGHEST
#else
#define TFM_PRIORITY_LOW    0xFF
#define TFM_PRIORITY_NORMAL 0x7F
#define TFM_PRIORITY_HIGH   0
#endif

#define TFM_PRIORITY(LEVEL)      TFM_PRIORITY_##LEVEL

/**
 * Holds the fields of the partition DB used by the SPM code. The values of
 * these fields are calculated at compile time, and set during initialisation
 * phase.
 */
struct spm_partition_static_data_t {
#ifdef TFM_PSA_API
    uint32_t psa_framework_version;
#endif /* defined(TFM_PSA_API) */
    uint32_t partition_id;
    uint32_t partition_flags;
    uint32_t partition_priority;
    sp_entry_point partition_init;
    uint32_t dependencies_num;
    int32_t *p_dependencies;
};

/**
 * Holds the fields that define a partition for SPM. The fields are further
 * divided to structures, to keep the related fields close to each other.
 */
struct spm_partition_desc_t {
    struct spm_partition_runtime_data_t runtime_data;
    const struct spm_partition_static_data_t *static_data;
    /** A list of platform_data pointers */
    const struct tfm_spm_partition_platform_data_t **platform_data_list;
#ifdef TFM_PSA_API
    const struct tfm_spm_partition_memory_data_t *memory_data;
#endif
};

struct spm_partition_db_t {
    uint32_t is_init;
    uint32_t partition_count;
#ifndef TFM_PSA_API
    uint32_t running_partition_idx;
#endif /* !defined(TFM_PSA_API) */
    struct spm_partition_desc_t *partitions;
};

#ifdef TFM_PSA_API
#define PART_REGION_ADDR(partition, region) \
    (uint32_t)&REGION_NAME(Image$$, partition, region)
#endif

#endif /* __SPM_DB_H__ */
