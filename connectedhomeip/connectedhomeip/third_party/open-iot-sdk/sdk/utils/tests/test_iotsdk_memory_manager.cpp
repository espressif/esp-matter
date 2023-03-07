/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {
#include "hal/network_stack_memory_manager.h"
}

#include "fff.h"
#include "iotsdk_alloc.h"
#include "iotsdk_memory_manager.h"

#include "gtest/gtest.h"

#define FAKE_HEAP_SIZE 500U

DEFINE_FFF_GLOBALS

static void *iotsdk_alloc_malloc_from_fake_heap_byte_aligned(size_t size);
static void *iotsdk_alloc_malloc_3_bytes_aligned(size_t size);
static void iotsdk_alloc_free_return_all_memory_to_heap(void *ptr);

static uint8_t gs_fake_heap[FAKE_HEAP_SIZE] = {0};
static size_t gs_fake_heap_allocated_size = 0U;

class TestIotSdkEmacMemoryManager : public ::testing::Test {
public:
    TestIotSdkEmacMemoryManager()
    {
        RESET_FAKE(iotsdk_alloc_malloc);
        RESET_FAKE(iotsdk_alloc_free);

        gs_fake_heap_allocated_size = 0U;
        memset(gs_fake_heap, 0xFF, FAKE_HEAP_SIZE);
    }
};

TEST_F(TestIotSdkEmacMemoryManager, alloc_from_heap_with_size_zero_fails)
{
    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();
    EXPECT_EQ(nullptr, mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, 0, 4U));
}

TEST_F(TestIotSdkEmacMemoryManager, alloc_from_heap_with_no_space_for_buffer_fails)
{
    iotsdk_alloc_malloc_fake.return_val = nullptr;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();
    EXPECT_EQ(nullptr, mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, 100U, 4U));
}

TEST_F(TestIotSdkEmacMemoryManager, alloc_from_heap_with_no_space_for_buffer_payload_fails)
{
    const size_t max_metadata_size = 128U;
    // Leave just enough space for the buffer metadata
    gs_fake_heap_allocated_size = FAKE_HEAP_SIZE - max_metadata_size;

    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    // Expect more bytes than are available from the fake heap
    EXPECT_EQ(nullptr,
              mdh_network_stack_memory_manager_alloc_from_heap(
                  iotsdk_memory_manager_get_instance(), max_metadata_size + 1U, 4U));
}

TEST_F(TestIotSdkEmacMemoryManager, alloc_from_heap_with_align_and_already_aligned_succeeds)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    EXPECT_NE(nullptr,
              mdh_network_stack_memory_manager_alloc_from_heap(iotsdk_memory_manager_get_instance(), 100U, 4U));
}

TEST_F(TestIotSdkEmacMemoryManager, alloc_from_heap_with_align_and_misaligned_succeeds)
{
    const size_t payload_size = 1U;
    const size_t max_metadata_size = 128U;
    // Leave just enough space for the buffer metadata, payload and the extra bytes for alignment
    gs_fake_heap_allocated_size = FAKE_HEAP_SIZE - max_metadata_size - payload_size - 3U;

    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_3_bytes_aligned;

    const size_t align = 4U;
    // Expect more bytes than are available from the fake heap
    EXPECT_NE(nullptr,
              mdh_network_stack_memory_manager_alloc_from_heap(
                  iotsdk_memory_manager_get_instance(), max_metadata_size + payload_size, align));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_with_nullptr_as_dst_fails)
{
    const size_t max_metadata_size = 128U;
    const size_t buf_payload_size = 1U;
    uint8_t src[max_metadata_size + buf_payload_size] = {0};
    src[max_metadata_size] = 0xDE;
    // No possible assertion so just check it runs to completion
    mdh_network_stack_memory_manager_copy(
        iotsdk_memory_manager_get_instance(), (mdh_network_stack_buffer_t *)src, nullptr);
}

TEST_F(TestIotSdkEmacMemoryManager, copy_with_nullptr_as_src_leaves_dst_unmodified)
{
    const size_t max_metadata_size = 128U;
    const size_t payload_size = 1U;
    uint8_t dst[max_metadata_size + payload_size] = {0};
    dst[max_metadata_size] = 0xDE;

    uint8_t unmodified_dst[max_metadata_size + payload_size] = {0};
    unmodified_dst[max_metadata_size] = 0xDE;
    mdh_network_stack_memory_manager_copy(
        iotsdk_memory_manager_get_instance(), nullptr, (mdh_network_stack_buffer_t *)dst);
    EXPECT_EQ(0, memcmp(unmodified_dst, dst, max_metadata_size + payload_size));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_with_not_enough_space_in_dst_leaves_dst_unmodified)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    mdh_network_stack_buffer_t *src = mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, 4U, 4U);
    EXPECT_NE(nullptr, src);

    uint32_t data = 0xDEADBEEF;
    EXPECT_EQ(4U, mdh_network_stack_memory_manager_copy_to_buf(memory_manager, &data, sizeof(data), src));
    size_t src_payload_len = mdh_network_stack_memory_manager_get_payload_len(memory_manager, src);

    const size_t dst_data_size = src_payload_len - 1;
    mdh_network_stack_buffer_t *dst =
        mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, dst_data_size, 4U);
    EXPECT_NE(nullptr, dst);
    uint8_t dst_data[5] = {0};
    EXPECT_EQ(dst_data_size,
              mdh_network_stack_memory_manager_copy_to_buf(memory_manager, dst_data, dst_data_size, dst));

    mdh_network_stack_memory_manager_copy(memory_manager, src, dst);
    void *dst_payload = mdh_network_stack_memory_manager_get_payload(memory_manager, dst);

    EXPECT_EQ(0, memcmp(dst_data, dst_payload, dst_data_size));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_with_enough_space_in_dst_succeeds)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    mdh_network_stack_buffer_t *src = mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, 4U, 4U);
    uint32_t data = 0xDEADBEEF;
    mdh_network_stack_memory_manager_copy_to_buf(memory_manager, &data, sizeof(data), src);
    size_t src_payload_len = mdh_network_stack_memory_manager_get_payload_len(memory_manager, src);

    mdh_network_stack_buffer_t *dst =
        mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, src_payload_len, 4U);

    mdh_network_stack_memory_manager_copy(memory_manager, src, dst);
    void *src_payload = mdh_network_stack_memory_manager_get_payload(memory_manager, src);
    void *dst_payload = mdh_network_stack_memory_manager_get_payload(memory_manager, dst);

    EXPECT_EQ(0, memcmp(src_payload, dst_payload, src_payload_len));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_from_buf_with_buf_as_nullptr_leaves_ptr_unmodified)
{
    uint8_t ptr[4U] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t unmodified_ptr[4U] = {0xDE, 0xAD, 0xBE, 0xEF};
    EXPECT_EQ(0,
              mdh_network_stack_memory_manager_copy_from_buf(iotsdk_memory_manager_get_instance(), nullptr, ptr, 4U));
    EXPECT_EQ(0, memcmp(unmodified_ptr, ptr, 4U));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_from_buf_with_ptr_as_nullptr_fails)
{
    EXPECT_EQ(0,
              mdh_network_stack_memory_manager_copy_from_buf(
                  iotsdk_memory_manager_get_instance(), gs_fake_heap, nullptr, 4U));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_from_buf_with_ptr_size_greater_than_buf_copies_only_buf)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    const size_t buf_payload_size = 4U;
    mdh_network_stack_buffer_t *buffer =
        mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, buf_payload_size, 4U);
    uint8_t buf_payload[buf_payload_size] = {0xDE, 0xAD, 0xBE, 0xEF};
    mdh_network_stack_memory_manager_copy_to_buf(memory_manager, buf_payload, buf_payload_size, buffer);

    const size_t num_extra_bytes = 2U;
    const size_t ptr_size = buf_payload_size + num_extra_bytes;
    uint8_t ptr[ptr_size] = {0};
    const uint8_t extra_bytes_content[num_extra_bytes] = {0xAA, 0xBB};
    memcpy(ptr + ptr_size - num_extra_bytes, extra_bytes_content, num_extra_bytes);

    EXPECT_EQ(buf_payload_size, mdh_network_stack_memory_manager_copy_from_buf(memory_manager, buffer, ptr, ptr_size));

    void *buffer_payload = mdh_network_stack_memory_manager_get_payload(memory_manager, buffer);

    // Check that all the buffer payload was copied to ptr
    EXPECT_EQ(0, memcmp(buffer_payload, ptr, buf_payload_size));
    // Check that the extra bytes were not modified
    EXPECT_EQ(0, memcmp(ptr + ptr_size - num_extra_bytes, extra_bytes_content, num_extra_bytes));
}

TEST_F(TestIotSdkEmacMemoryManager, get_payload_with_buf_as_nullptr_fails)
{
    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    EXPECT_EQ(nullptr, mdh_network_stack_memory_manager_get_payload(memory_manager, nullptr));
}

TEST_F(TestIotSdkEmacMemoryManager, get_payload_len_with_buf_as_nullptr_fails)
{
    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    EXPECT_EQ(0, mdh_network_stack_memory_manager_get_payload_len(memory_manager, nullptr));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_to_buf_with_buf_as_nullptr_fails)
{
    EXPECT_EQ(
        0,
        mdh_network_stack_memory_manager_copy_to_buf(iotsdk_memory_manager_get_instance(), gs_fake_heap, 4U, nullptr));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_to_buf_with_ptr_as_nullptr_leaves_buf_unmodified)
{
    const size_t max_metadata_size = 128U;
    const size_t buf_payload_size = 1U;
    uint8_t buf[max_metadata_size + buf_payload_size] = {0};
    buf[max_metadata_size] = 0xDE;
    uint8_t unmodified_buf[max_metadata_size + buf_payload_size] = {0};
    unmodified_buf[max_metadata_size] = 0xDE;
    EXPECT_EQ(0,
              mdh_network_stack_memory_manager_copy_to_buf(
                  iotsdk_memory_manager_get_instance(), nullptr, buf_payload_size, (mdh_network_stack_buffer_t *)buf));
    EXPECT_EQ(0, memcmp(unmodified_buf, buf, max_metadata_size + buf_payload_size));
}

TEST_F(TestIotSdkEmacMemoryManager, copy_to_buf_with_ptr_size_greater_than_buf_leaves_buf_unmodified)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    const size_t max_metadata_size = 128U;
    const size_t buf_payload_size = 1U;
    uint8_t buf[max_metadata_size + buf_payload_size] = {0};
    buf[max_metadata_size] = 0xDE;
    uint8_t unmodified_buf[max_metadata_size + buf_payload_size] = {0};
    unmodified_buf[max_metadata_size] = 0xDE;

    const size_t ptr_size = buf_payload_size + 1;
    uint8_t ptr[ptr_size] = {0xAA, 0xBB};

    EXPECT_EQ(
        0,
        mdh_network_stack_memory_manager_copy_to_buf(memory_manager, ptr, ptr_size, (mdh_network_stack_buffer_t *)buf));
    EXPECT_EQ(0, memcmp(unmodified_buf, buf, max_metadata_size + buf_payload_size));
}

TEST_F(TestIotSdkEmacMemoryManager, free_with_buf_as_nullptr_return_heap_memory)
{
    iotsdk_alloc_free_fake.custom_fake = iotsdk_alloc_free_return_all_memory_to_heap;

    const size_t max_metadata_size = 128U;
    const size_t payload_size = 1U;

    gs_fake_heap_allocated_size += (max_metadata_size + payload_size);

    const size_t prev_allocated_memory = gs_fake_heap_allocated_size;

    mdh_network_stack_memory_manager_free(iotsdk_memory_manager_get_instance(), nullptr);
    EXPECT_EQ(prev_allocated_memory, gs_fake_heap_allocated_size);
}

TEST_F(TestIotSdkEmacMemoryManager, free_valid_buf_returns_all_mem_to_heap)
{
    iotsdk_alloc_free_fake.custom_fake = iotsdk_alloc_free_return_all_memory_to_heap;
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    const size_t max_metadata_size = 128U;
    const size_t payload_size = 1U;

    uint8_t *buf[max_metadata_size + payload_size] = {0};
    gs_fake_heap_allocated_size += (max_metadata_size + payload_size);

    mdh_network_stack_memory_manager_free(iotsdk_memory_manager_get_instance(), (mdh_network_stack_buffer_t *)buf);
    EXPECT_EQ(0, gs_fake_heap_allocated_size);
}

TEST_F(TestIotSdkEmacMemoryManager, set_payload_len_with_buf_as_nullptr_does_not_crash)
{
    // No possible assertion so just check it runs to completion
    mdh_network_stack_memory_manager_set_payload_len(iotsdk_memory_manager_get_instance(), nullptr, 1);
}

TEST_F(TestIotSdkEmacMemoryManager,
       set_payload_len_with_size_greater_than_buf_capacity_leaves_payload_length_unmmodified)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    size_t buf_payload_size = 4U;
    mdh_network_stack_buffer_t *buffer =
        mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, buf_payload_size, 4U);
    const size_t prev_payload_len = mdh_network_stack_memory_manager_get_payload_len(memory_manager, buffer);

    mdh_network_stack_memory_manager_set_payload_len(memory_manager, buffer, buf_payload_size + 1);

    EXPECT_EQ(prev_payload_len, mdh_network_stack_memory_manager_get_payload_len(memory_manager, buffer));
}

TEST_F(TestIotSdkEmacMemoryManager, set_payload_len_with_size_within_buf_capacity_succeeds)
{
    iotsdk_alloc_malloc_fake.custom_fake = iotsdk_alloc_malloc_from_fake_heap_byte_aligned;

    mdh_network_stack_memory_manager_t *memory_manager = iotsdk_memory_manager_get_instance();

    const size_t buf_payload_size = 4U;
    mdh_network_stack_buffer_t *buffer =
        mdh_network_stack_memory_manager_alloc_from_heap(memory_manager, buf_payload_size, 4U);

    const size_t new_payload_size_config = buf_payload_size - 3;
    mdh_network_stack_memory_manager_set_payload_len(memory_manager, buffer, new_payload_size_config);
    const size_t new_payload_len = mdh_network_stack_memory_manager_get_payload_len(memory_manager, buffer);
    EXPECT_NE(buf_payload_size, new_payload_len);
    EXPECT_EQ(new_payload_size_config, new_payload_len);
}

static void *iotsdk_alloc_malloc_from_fake_heap_byte_aligned(size_t size)
{
    if (gs_fake_heap_allocated_size + size >= FAKE_HEAP_SIZE) {
        return nullptr;
    }

    uint8_t *allocated_mem_ptr = gs_fake_heap + gs_fake_heap_allocated_size;
    gs_fake_heap_allocated_size += size;

    return allocated_mem_ptr;
}

static void *iotsdk_alloc_malloc_3_bytes_aligned(size_t size)
{
    uint8_t *allocated_mem_ptr = gs_fake_heap + gs_fake_heap_allocated_size;
    gs_fake_heap_allocated_size += size;

    size_t extra_misalignment_byte = 0;
    uintptr_t allocated_mem_ptr_addr = (uintptr_t)allocated_mem_ptr;
    if ((allocated_mem_ptr_addr % 4) == 0) {
        extra_misalignment_byte = 3;
    }

    allocated_mem_ptr += extra_misalignment_byte;

    return allocated_mem_ptr;
}

static void iotsdk_alloc_free_return_all_memory_to_heap(void *ptr)
{
    (void)ptr; /* Avoid cppcheck warning that ptr could be const */

    if (ptr) {
        gs_fake_heap_allocated_size = 0;
    }
}
