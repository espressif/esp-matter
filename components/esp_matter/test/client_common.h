// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <esp_event.h>
#include <nvs_flash.h>
#include <platform/CHIPDeviceLayer.h>
#include <unity.h>

namespace esp_matter::test {

inline void init_client_test_stack()
{
    static bool initialized = false;
    if (!initialized) {
        TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_init());
        esp_err_t err = esp_event_loop_create_default();
        TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_INVALID_STATE);
        TEST_ASSERT_TRUE(chip::Platform::MemoryInit() == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL_HEX32(CHIP_NO_ERROR.AsInteger(), chip::DeviceLayer::PlatformMgr().InitChipStack().AsInteger());
        initialized = true;
    }
}

inline void drain_client_test_timers()
{
    // Use DeviceLayer::StackLock in callers: the ESP-Matter guard can skip
    // locking before the event loop starts, unbalancing StackUnlock below.
    // Process platform work as well as timers to avoid filling the FreeRTOS queue.
    auto &platform = chip::DeviceLayer::PlatformMgr();
    TEST_ASSERT_TRUE(platform.ScheduleWork([](intptr_t) {
        TEST_ASSERT_TRUE(chip::DeviceLayer::PlatformMgr().StopEventLoopTask() == CHIP_NO_ERROR);
    }) == CHIP_NO_ERROR);
    chip::DeviceLayer::StackUnlock unlock;
    platform.RunEventLoop();
}

} // namespace esp_matter::test
