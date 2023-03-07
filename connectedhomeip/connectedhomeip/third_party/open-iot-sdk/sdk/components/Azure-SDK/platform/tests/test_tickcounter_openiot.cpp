/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/xlogging.h"
}

#include "cmsis_os2.h"
#include "fff.h"
#include "iotsdk_alloc.h"

#include "gtest/gtest.h"
#include <stdint.h>

#define MILLISECONDS_IN_SECOND 1000

DEFINE_FFF_GLOBALS

class TestTickcounterOpeniot : public ::testing::Test {
public:
    TestTickcounterOpeniot()
    {
        RESET_FAKE(iotsdk_alloc_malloc);
        RESET_FAKE(iotsdk_alloc_free);
        RESET_FAKE(osKernelGetTickCount);
        RESET_FAKE(osKernelGetTickFreq);
    }
};

TEST_F(TestTickcounterOpeniot, tickcounter_create_returns_valid_ptr)
{
    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    iotsdk_alloc_malloc_fake.return_val = fake_heap_tick_counter_memory;
    TICK_COUNTER_HANDLE tick_counter = tickcounter_create();
    EXPECT_EQ((TICK_COUNTER_HANDLE)fake_heap_tick_counter_memory, tick_counter);
}

TEST_F(TestTickcounterOpeniot, tickcounter_create_returns_null_if_malloc_fails)
{
    iotsdk_alloc_malloc_fake.return_val = nullptr;
    EXPECT_EQ(nullptr, tickcounter_create());
}

TEST_F(TestTickcounterOpeniot, tickcounter_destroy_does_not_call_free_if_tickcounter_is_null)
{
    tickcounter_destroy(nullptr);
    EXPECT_EQ(iotsdk_alloc_free_fake.call_count, 0);
}

TEST_F(TestTickcounterOpeniot, tickcounter_destroy_calls_free_if_tickcounter_is_not_null)
{
    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    tickcounter_destroy((TICK_COUNTER_HANDLE)fake_heap_tick_counter_memory);
    EXPECT_EQ(iotsdk_alloc_free_fake.call_count, 1);
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_returns_failure_when_tick_counter_is_null)
{
    tickcounter_ms_t current_ms = 1;
    EXPECT_EQ(MU_FAILURE, tickcounter_get_current_ms(nullptr, &current_ms));
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_returns_failure_when_current_ms_is_null)
{
    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    EXPECT_EQ(MU_FAILURE, tickcounter_get_current_ms((TICK_COUNTER_HANDLE)fake_heap_tick_counter_memory, nullptr));
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_returns_incremented_ms)
{
    osKernelGetTickCount_fake.return_val = 1;
    const uint32_t fake_os_tick_frequency = 1000;
    osKernelGetTickFreq_fake.return_val = fake_os_tick_frequency;

    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    // As the array is initialized to 0, we expect the previous ms to be 0
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    tickcounter_ms_t current_ms = 0xFF;
    EXPECT_NE(MU_FAILURE, tickcounter_get_current_ms((TICK_COUNTER_HANDLE)fake_heap_tick_counter_memory, &current_ms));

    // The current ms should be greater than 0 as we have incremented the tick count by faking it
    // with osKernelGetTickCount
    const uint32_t fake_ticks_per_millisecond = fake_os_tick_frequency / MILLISECONDS_IN_SECOND;
    EXPECT_EQ(fake_ticks_per_millisecond, current_ms);
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_returns_one_after_overflow)
{
    // The first value is what is used when tick_counter is created to set previous count
    // The second value is returned when tickcounter_get_current_ms() is called
    uint32_t tick_counts[2] = {UINT32_MAX, 0};
    SET_RETURN_SEQ(osKernelGetTickCount, tick_counts, 2);

    const uint32_t fake_os_tick_frequency = 1000;
    osKernelGetTickFreq_fake.return_val = fake_os_tick_frequency;

    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    iotsdk_alloc_malloc_fake.return_val = fake_heap_tick_counter_memory;

    // Assume that tick_counter_create() sets the previous counter with osKernelGetTickCount
    TICK_COUNTER_HANDLE tick_counter = tickcounter_create();
    tickcounter_ms_t current_ms = 0;
    EXPECT_NE(MU_FAILURE, tickcounter_get_current_ms(tick_counter, &current_ms));
    const uint32_t fake_ticks_per_millisecond = fake_os_tick_frequency / MILLISECONDS_IN_SECOND;
    EXPECT_EQ(fake_ticks_per_millisecond, current_ms);
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_returns_max_count_value_if_count_is_max)
{
    osKernelGetTickCount_fake.return_val = UINT32_MAX;
    osKernelGetTickFreq_fake.return_val = 1000;

    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    tickcounter_ms_t current_ms = 0;
    EXPECT_NE(MU_FAILURE, tickcounter_get_current_ms((TICK_COUNTER_HANDLE)fake_heap_tick_counter_memory, &current_ms));
    EXPECT_EQ(UINT32_MAX, current_ms);
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_returns_unchanged_value_if_ticks_suspended)
{
    // The first value is what is used when tick_counter is created to set previous count
    // The second value is returned when tickcounter_get_current_ms() is called
    uint32_t tick_counts[2] = {1, 1};
    SET_RETURN_SEQ(osKernelGetTickCount, tick_counts, 2);
    const uint32_t fake_os_tick_frequency = 1000;
    osKernelGetTickFreq_fake.return_val = fake_os_tick_frequency;

    // As size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    iotsdk_alloc_malloc_fake.return_val = fake_heap_tick_counter_memory;

    // Assume that tick_counter_create() sets the previous counter with osKernelGetTickCount
    TICK_COUNTER_HANDLE tick_counter = tickcounter_create();
    tickcounter_ms_t current_ms = 0;
    EXPECT_NE(MU_FAILURE, tickcounter_get_current_ms(tick_counter, &current_ms));
    EXPECT_EQ(0, current_ms);
}

TEST_F(TestTickcounterOpeniot, tickcounter_get_current_ms_wraps_around_if_previous_tick_count_is_greater_than_current)
{
    // The first value is what is used when tick_counter is created to set previous count
    // The second value is returned when tickcounter_get_current_ms() is called
    uint32_t tick_counts[2] = {1, 0};
    SET_RETURN_SEQ(osKernelGetTickCount, tick_counts, 2);
    osKernelGetTickFreq_fake.return_val = 1000;

    // as size of tickcounter instance is not known; allocate enough memory
    // so that dereferences do not result in a crash
    uint8_t fake_heap_tick_counter_memory[100] = {0};
    iotsdk_alloc_malloc_fake.return_val = fake_heap_tick_counter_memory;

    // Assume that tick_counter_create() sets the previous counter with osKernelGetTickCount
    TICK_COUNTER_HANDLE tick_counter = tickcounter_create();
    tickcounter_ms_t current_ms = 0;
    EXPECT_NE(MU_FAILURE, tickcounter_get_current_ms(tick_counter, &current_ms));
    EXPECT_EQ(UINT32_MAX, current_ms);
}
