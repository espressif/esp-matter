/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip_memory_manager.h"

#include "hal/emac_api.h"
#include "lwip/pbuf.h"
#include "lwipopts.h"

#include <stdbool.h>
#include <string.h>

static mdh_network_stack_buffer_t *
lwip_mm_alloc_from_heap(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align);
static mdh_network_stack_buffer_t *
lwip_mm_alloc_from_static_pool(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align);
static size_t lwip_mm_get_static_pool_alloc_unit(mdh_network_stack_memory_manager_t *const self, size_t align);
static void lwip_mm_free(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf);
static void lwip_mm_copy(mdh_network_stack_memory_manager_t *const self,
                         const mdh_network_stack_buffer_t *const source,
                         const mdh_network_stack_buffer_t *destination);
static size_t lwip_mm_copy_to_buf(mdh_network_stack_memory_manager_t *const self,
                                  const void *const ptr,
                                  size_t size,
                                  const mdh_network_stack_buffer_t *buf);
static size_t lwip_mm_copy_from_buf(mdh_network_stack_memory_manager_t *const self,
                                    const mdh_network_stack_buffer_t *const buf,
                                    void *const ptr,
                                    size_t size);
static void *lwip_mm_get_payload(mdh_network_stack_memory_manager_t *const self,
                                 const mdh_network_stack_buffer_t *const buf);
static size_t lwip_mm_get_payload_len(mdh_network_stack_memory_manager_t *const self,
                                      const mdh_network_stack_buffer_t *const buf);
static void lwip_mm_set_payload_len(mdh_network_stack_memory_manager_t *const self,
                                    const mdh_network_stack_buffer_t *buf,
                                    size_t size);

typedef struct pbuf pbuf_t;

typedef struct lwip_mm_instance {
    mdh_network_stack_memory_manager_t memory_manager;
} lwip_mm_instance;

static const mdh_network_stack_memory_manager_vtable_t memory_manager_vtable = {
    .alloc_from_heap = lwip_mm_alloc_from_heap,
    .alloc_from_static_pool = lwip_mm_alloc_from_static_pool,
    .get_static_pool_alloc_unit = lwip_mm_get_static_pool_alloc_unit,
    .free = lwip_mm_free,
    .copy = lwip_mm_copy,
    .copy_to_buf = lwip_mm_copy_to_buf,
    .copy_from_buf = lwip_mm_copy_from_buf,
    .get_payload = lwip_mm_get_payload,
    .get_payload_len = lwip_mm_get_payload_len,
    .set_payload_len = lwip_mm_set_payload_len,
};

static lwip_mm_instance instance = {.memory_manager.vtable = &memory_manager_vtable};

static uint32_t count_total_align(uint32_t size, uint32_t align)
{
    uint32_t buffers = size / (LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE) - align);
    if (size % (LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE) - align) != 0) {
        buffers++;
    }
    return buffers * align;
}

static void set_total_len(pbuf_t *pbuf)
{
    if (!pbuf->next) {
        pbuf->tot_len = pbuf->len;
        return;
    }

    pbuf_t *pbuf_tailing;

    while (pbuf) {
        uint32_t total_len = pbuf->len;

        pbuf_tailing = pbuf->next;
        while (pbuf_tailing) {
            total_len += pbuf_tailing->len;
            pbuf_tailing = pbuf_tailing->next;
        }

        pbuf->tot_len = total_len;
        pbuf = pbuf->next;
    }
}

static void align_memory(pbuf_t *pbuf, uint32_t align)
{
    if (!align) {
        return;
    }

    pbuf_t *pbuf_start = pbuf;

    while (pbuf) {
        uint32_t remainder = (uint32_t)(pbuf->payload) % align;
        if (remainder) {
            uint32_t offset = align - remainder;
            if (offset >= align) {
                offset = align;
            }
            pbuf->payload = (char *)(pbuf->payload) + offset;
        }
        pbuf->len -= align;
        pbuf = pbuf->next;
    }

    // Correct total lengths
    set_total_len(pbuf_start);
}

mdh_network_stack_buffer_t *
lwip_mm_alloc_from_heap(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align)
{
    pbuf_t *pbuf = pbuf_alloc(PBUF_RAW, size + align, PBUF_RAM);
    if (pbuf == NULL) {
        return NULL;
    }

    align_memory(pbuf, align);

    return (mdh_network_stack_buffer_t *)(pbuf);
}

mdh_network_stack_buffer_t *
lwip_mm_alloc_from_static_pool(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align)
{
    uint32_t total_align = count_total_align(size, align);

    pbuf_t *pbuf = pbuf_alloc(PBUF_RAW, size + total_align, PBUF_POOL);
    if (pbuf == NULL) {
        return NULL;
    }

    align_memory(pbuf, align);

    return (mdh_network_stack_buffer_t *)(pbuf);
}

size_t lwip_mm_get_static_pool_alloc_unit(mdh_network_stack_memory_manager_t *const self, size_t align)
{
    uint32_t alloc_unit = LWIP_MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE) - align;
    return alloc_unit;
}

void lwip_mm_free(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf)
{
    pbuf_free((pbuf_t *)(buf));
}

void lwip_mm_copy(mdh_network_stack_memory_manager_t *const self,
                  const mdh_network_stack_buffer_t *const source,
                  const mdh_network_stack_buffer_t *destination)
{
    pbuf_copy((pbuf_t *)(destination), (const pbuf_t *)(source));
}

size_t lwip_mm_copy_to_buf(mdh_network_stack_memory_manager_t *const self,
                           const void *const ptr,
                           size_t size,
                           const mdh_network_stack_buffer_t *buf)
{
    if (pbuf_take((pbuf_t *)(buf), ptr, size) == ERR_OK) {
        return size;
    }
    return 0;
}

size_t lwip_mm_copy_from_buf(mdh_network_stack_memory_manager_t *const self,
                             const mdh_network_stack_buffer_t *const buf,
                             void *const ptr,
                             size_t size)
{
    return pbuf_copy_partial((const pbuf_t *)(buf), ptr, size, 0);
}

void *lwip_mm_get_payload(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *const buf)
{
    return ((const pbuf_t *)(buf))->payload;
}

size_t lwip_mm_get_payload_len(mdh_network_stack_memory_manager_t *const self,
                               const mdh_network_stack_buffer_t *const buf)
{
    return ((const pbuf_t *)(buf))->tot_len;
}

void lwip_mm_set_payload_len(mdh_network_stack_memory_manager_t *const self,
                             const mdh_network_stack_buffer_t *buf,
                             size_t size)
{
    pbuf_t *pbuf = (pbuf_t *)(buf);
    pbuf->len = size;
    set_total_len(pbuf);
};

mdh_network_stack_memory_manager_t *lwip_mm_get_instance(void)
{
    return &(instance.memory_manager);
}
