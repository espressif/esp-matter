/* Copyright (c) 2021-2022 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal/network_stack_memory_manager.h"
#include "iotsdk_alloc.h"

#include <stdlib.h>
#include <string.h>

typedef struct priv_memory_buf_s {
    void *allocated_memory; /**< Pointer to allocated buffer */
    void *data;             /**< Pointer to data (starting at aligned memory address) */
    uint32_t capacity;      /**< Allocated buffer length. */
    uint32_t length;        /**< Consumed buffer length. */
} priv_memory_buf_t;

static mdh_network_stack_buffer_t *
alloc_from_heap(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align);
static void free_buf(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf);
static void copy(mdh_network_stack_memory_manager_t *const self,
                 const mdh_network_stack_buffer_t *const source,
                 const mdh_network_stack_buffer_t *destination);
static size_t copy_to_buf(mdh_network_stack_memory_manager_t *const self,
                          const void *const ptr,
                          size_t size,
                          const mdh_network_stack_buffer_t *buf);
static size_t copy_from_buf(mdh_network_stack_memory_manager_t *const self,
                            const mdh_network_stack_buffer_t *const buf,
                            void *const ptr,
                            size_t size);
static void *get_payload(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *const buf);
static size_t get_payload_len(mdh_network_stack_memory_manager_t *const self,
                              const mdh_network_stack_buffer_t *const buf);
static void
set_payload_len(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf, size_t size);

static const mdh_network_stack_memory_manager_vtable_t gsc_vtable = {
    .alloc_from_heap = alloc_from_heap,
    .alloc_from_static_pool = NULL,
    .get_static_pool_alloc_unit = NULL,
    .free = free_buf,
    .copy = copy,
    .copy_to_buf = copy_to_buf,
    .copy_from_buf = copy_from_buf,
    .get_payload = get_payload,
    .get_payload_len = get_payload_len,
    .set_payload_len = set_payload_len,
};

static mdh_network_stack_memory_manager_t gs_memory_manager = {
    .vtable = &gsc_vtable,
};

mdh_network_stack_memory_manager_t *iotsdk_memory_manager_get_instance(void)
{
    return &gs_memory_manager;
}

static mdh_network_stack_buffer_t *
alloc_from_heap(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align)
{
    (void)self;

    if (size == 0) {
        return NULL;
    }

    priv_memory_buf_t *buf = (priv_memory_buf_t *)iotsdk_alloc_malloc(sizeof(priv_memory_buf_t));
    if (buf == NULL) {
        return NULL;
    }

    buf->allocated_memory = iotsdk_alloc_malloc(size + align);
    if (buf->allocated_memory == NULL) {
        iotsdk_alloc_free((void *)buf);
        return NULL;
    }

    buf->data = buf->allocated_memory;
    buf->capacity = buf->length = size;

    if (align) {
        uintptr_t remainder = (uintptr_t)buf->data % align;
        if (remainder) {
            uint32_t offset = align - remainder;
            if (offset >= align) {
                offset = align;
            }
            buf->data = (char *)buf->data + offset;
        }
    }

    return (mdh_network_stack_buffer_t *)buf;
}

static void free_buf(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf)
{
    (void)self;

    if (buf == NULL) {
        return;
    }

    const priv_memory_buf_t *buffer = (const priv_memory_buf_t *)buf;

    iotsdk_alloc_free(buffer->allocated_memory);
    iotsdk_alloc_free((void *)buffer);
}

static void copy(mdh_network_stack_memory_manager_t *const self,
                 const mdh_network_stack_buffer_t *const source,
                 const mdh_network_stack_buffer_t *destination)
{
    (void)self;

    if ((source == NULL) || (destination == NULL)) {
        return;
    }

    const priv_memory_buf_t *const src_buf = (const priv_memory_buf_t *const)source;
    priv_memory_buf_t *const dst_buf = (priv_memory_buf_t *const)destination;

    if (dst_buf->capacity < src_buf->capacity) {
        return;
    }

    memcpy(dst_buf->data, src_buf->data, src_buf->length);

    dst_buf->length = src_buf->length;
}

static size_t copy_to_buf(mdh_network_stack_memory_manager_t *const self,
                          const void *const ptr,
                          size_t size,
                          const mdh_network_stack_buffer_t *buf)
{
    (void)self;

    if ((ptr == NULL) || (buf == NULL)) {
        return 0;
    }

    priv_memory_buf_t *dst_buf = (priv_memory_buf_t *)buf;

    if (size > dst_buf->capacity) {
        return 0;
    }

    memcpy(dst_buf->data, (void *)ptr, size);
    dst_buf->length = size;
    return size;
}

static size_t copy_from_buf(mdh_network_stack_memory_manager_t *const self,
                            const mdh_network_stack_buffer_t *const buf,
                            void *const ptr,
                            size_t size)
{
    (void)self;

    if ((ptr == NULL) || (buf == NULL)) {
        return 0;
    }

    const priv_memory_buf_t *const src_buf = (priv_memory_buf_t *const)buf;

    if (size > src_buf->length) {
        size = src_buf->length;
    }

    memcpy((void *)ptr, src_buf->data, size);
    return size;
}

static void *get_payload(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *const buf)
{
    (void)self;

    if (buf == NULL) {
        return NULL;
    }

    return ((priv_memory_buf_t *)buf)->data;
}

static size_t get_payload_len(mdh_network_stack_memory_manager_t *const self,
                              const mdh_network_stack_buffer_t *const buf)
{
    (void)self;

    if (buf == NULL) {
        return 0;
    }

    return ((priv_memory_buf_t *)buf)->length;
}

static void
set_payload_len(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf, size_t size)
{
    (void)self;

    if (buf == NULL) {
        return;
    }

    priv_memory_buf_t *buffer = (priv_memory_buf_t *)buf;

    if (size > buffer->capacity) {
        return;
    }

    buffer->length = size;
}
