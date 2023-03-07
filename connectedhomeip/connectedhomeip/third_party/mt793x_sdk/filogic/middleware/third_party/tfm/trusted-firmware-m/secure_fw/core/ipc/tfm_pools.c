/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tfm_thread.h"
#include "tfm_wait.h"
#include "psa/client.h"
#include "psa/service.h"
#include "tfm_internal_defines.h"
#include "cmsis_compiler.h"
#include "tfm_utils.h"
#include "tfm_list.h"
#include "tfm_pools.h"
#include "tfm_memory_utils.h"
#include "tfm_core_utils.h"

int32_t tfm_pool_init(struct tfm_pool_instance_t *pool, size_t poolsz,
                      size_t chunksz, size_t num)
{
    struct tfm_pool_chunk_t *pchunk;
    size_t i;

    if (!pool || num == 0) {
        return IPC_ERROR_BAD_PARAMETERS;
    }

    /* Ensure buffer is large enough */
    if (poolsz != ((chunksz + sizeof(struct tfm_pool_chunk_t)) * num +
        sizeof(struct tfm_pool_instance_t))) {
        return IPC_ERROR_BAD_PARAMETERS;
    }

    /* Buffer should be BSS cleared but clear it again */
    tfm_core_util_memset(pool, 0, poolsz);

    /* Chain pool chunks */
    tfm_list_init(&pool->chunks_list);

    pchunk = (struct tfm_pool_chunk_t *)pool->chunks;
    for (i = 0; i < num; i++) {
        pchunk->pool = pool;
        tfm_list_add_tail(&pool->chunks_list, &pchunk->list);
        pchunk = (struct tfm_pool_chunk_t *)&pchunk->data[chunksz];
    }

    /* Prepare instance and insert to pool list */
    pool->chunksz = chunksz;
    pool->chunk_count = num;

    return IPC_SUCCESS;
}

void *tfm_pool_alloc(struct tfm_pool_instance_t *pool)
{
    struct tfm_list_node_t *node;
    struct tfm_pool_chunk_t *pchunk;

    if (!pool) {
        return NULL;
    }

    if (tfm_list_is_empty(&pool->chunks_list)) {
        return NULL;
    }

    node = tfm_list_first_node(&pool->chunks_list);
    pchunk = TFM_GET_CONTAINER_PTR(node, struct tfm_pool_chunk_t, list);

    /* Remove node from list node, it will be added when pool free */
    tfm_list_del_node(node);

    return &pchunk->data;
}

void tfm_pool_free(void *ptr)
{
    struct tfm_pool_chunk_t *pchunk;
    struct tfm_pool_instance_t *pool;

    pchunk = TFM_GET_CONTAINER_PTR(ptr, struct tfm_pool_chunk_t, data);
    pool = (struct tfm_pool_instance_t *)pchunk->pool;
    tfm_list_add_tail(&pool->chunks_list, &pchunk->list);
}

bool is_valid_chunk_data_in_pool(struct tfm_pool_instance_t *pool,
                                 uint8_t *data)
{
    const uintptr_t chunks_start = (uintptr_t)(pool->chunks);
    const size_t chunks_size = pool->chunksz + sizeof(struct tfm_pool_chunk_t);
    const size_t chunk_count = pool->chunk_count;
    const uintptr_t chunks_end = chunks_start + chunks_size * chunk_count;
    uintptr_t pool_chunk_address = 0;

    /* Check that the message was allocated from the pool. */
    if ((uintptr_t)data < chunks_start || (uintptr_t)data >= chunks_end) {
        return false;
    }

    pool_chunk_address =
    (uint32_t)TFM_GET_CONTAINER_PTR(data, struct tfm_pool_chunk_t, data);

    /* Make sure that the chunk containing the message is aligned on */
    /* chunk boundary in the pool. */
    if ((pool_chunk_address - chunks_start) % chunks_size != 0) {
        return false;
    }
    return true;
}
