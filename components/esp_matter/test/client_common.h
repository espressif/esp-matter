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

#include <app/CASESessionManager.h>
#include <app/InteractionModelEngine.h>
#include <credentials/GroupDataProviderImpl.h>
#include <app/reporting/ReportSchedulerImpl.h>
#include <app/server-cluster/testing/FabricTestFixture.h>
#include <crypto/DefaultSessionKeystore.h>
#include <platform/DefaultTimerDelegate.h>
#include <transport/TransportMgr.h>
#include <transport/raw/tests/NetworkTestHelpers.h>
#include <lib/support/TestPersistentStorageDelegate.h>
#include <esp_event.h>
#include <esp_heap_caps.h>
#include <nvs_flash.h>
#include <platform/CHIPDeviceLayer.h>
#include <unity.h>

namespace esp_matter::test::client {

inline void init_test_stack()
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

inline void check_mem_leak(void (*exercise)())
{
    exercise(); // Run once to initialize SDK allocations before measuring later runs.
    multi_heap_info_t before;
    heap_caps_get_info(&before, MALLOC_CAP_8BIT);
    for (unsigned iteration = 0; iteration < 3; ++iteration) {
        exercise();
        multi_heap_info_t after;
        heap_caps_get_info(&after, MALLOC_CAP_8BIT);
        // byte totals fluctuate slightly; unreleased allocations must not accumulate
        TEST_ASSERT_EQUAL(before.allocated_blocks, after.allocated_blocks);
    }
}

inline void drain_timers()
{
    // use DeviceLayer::StackLock in callers: the ESP-Matter guard can skip
    // locking before the event loop starts, unbalancing StackUnlock below
    // process platform work as well as timers to avoid filling the FreeRTOS queue
    auto &platform = chip::DeviceLayer::PlatformMgr();
    TEST_ASSERT_TRUE(platform.ScheduleWork([](intptr_t) {
        TEST_ASSERT_TRUE(chip::DeviceLayer::PlatformMgr().StopEventLoopTask() == CHIP_NO_ERROR);
    }) == CHIP_NO_ERROR);
    chip::DeviceLayer::StackUnlock unlock;
    platform.RunEventLoop();
}

class Fixture {
public:
    Fixture() : mFabric(&mStorage), mScheduler(&mTimerDelegate)
    {
        TEST_ASSERT_TRUE(mFabric.SetUpTestFabric(mFabricIndex) == CHIP_NO_ERROR);
        auto &transport = mTransport.GetTransport().GetImplAtIndex<0>();
        transport.InitLoopbackTransport(&chip::DeviceLayer::SystemLayer());
        // exercise native send-error completion without a network peer or dropped packets
        transport.mMessageSendError = CHIP_ERROR_WRITE_FAILED;
        TEST_ASSERT_TRUE(mTransport.Init("client-test") == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(mSessions.Init(&chip::DeviceLayer::SystemLayer(), &mTransport, nullptr,
                                        &mStorage, &mFabric.GetFabricTable(), mKeystore) == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(mExchanges.Init(&mSessions) == CHIP_NO_ERROR);
        mGroups.SetStorageDelegate(&mStorage);
        mGroups.SetSessionKeystore(&mKeystore);
        TEST_ASSERT_TRUE(mGroups.Init() == CHIP_NO_ERROR);
        chip::CASESessionManagerConfig config;
        config.sessionInitParams.sessionManager = &mSessions;
        config.sessionInitParams.exchangeMgr = &mExchanges;
        config.sessionInitParams.fabricTable = &mFabric.GetFabricTable();
        config.sessionInitParams.groupDataProvider = &mGroups;
        config.clientPool = &mCaseClients;
        config.sessionSetupPool = &mSetups;
        TEST_ASSERT_TRUE(mCase.Init(&chip::DeviceLayer::SystemLayer(), config) == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(chip::app::InteractionModelEngine::GetInstance()->Init(
                             &mExchanges, &mFabric.GetFabricTable(), &mScheduler, &mCase) == CHIP_NO_ERROR);
    }

    ~Fixture()
    {
        auto *engine = chip::app::InteractionModelEngine::GetInstance();
        engine->Shutdown();
        // SDK Shutdown leaves runtime pointers installed.
        // Reset them to their pre-test values.
        engine->~InteractionModelEngine();
        new (engine) chip::app::InteractionModelEngine();
        mCase.Shutdown();
        mGroups.Finish();
        mSessions.ExpireAllSecureSessions();
        mExchanges.Shutdown();
        mSessions.Shutdown();
        mTransport.Close();
        mTransport.GetTransport().GetImplAtIndex<0>().ShutdownLoopbackTransport();
        TEST_ASSERT_TRUE(mFabric.TearDownTestFabric(mFabricIndex) == CHIP_NO_ERROR);
    }

    void Connect(chip::NodeId node, chip::FabricIndex fabric = 1)
    {
        const auto *info = mFabric.GetFabricTable().FindFabricWithIndex(fabric);
        TEST_ASSERT_NOT_NULL(info);
        TEST_ASSERT_LESS_THAN(2, mConnected);
        TEST_ASSERT_TRUE(mSessions.InjectCaseSessionWithTestKey(
                             mInjected[mConnected], mConnected + 1, mConnected + 10, info->GetNodeId(), node, fabric,
                             chip::Transport::PeerAddress::UDP(chip::Inet::IPAddress::Any, 5540),
                             chip::CryptoContext::SessionRole::kInitiator) == CHIP_NO_ERROR);
        ++mConnected;
    }

    chip::ScopedNodeId SelfNode()
    {
        const auto *info = mFabric.GetFabricTable().FindFabricWithIndex(mFabricIndex);
        TEST_ASSERT_NOT_NULL(info);
        return chip::ScopedNodeId(info->GetNodeId(), mFabricIndex);
    }

    void AddFabric(chip::FabricIndex fabric)
    {
        TEST_ASSERT_TRUE(mFabric.AddAdditionalTestFabric(fabric) == CHIP_NO_ERROR);
    }

protected:
    chip::TestPersistentStorageDelegate mStorage;
    chip::Testing::FabricTestFixture mFabric;
    chip::FabricIndex mFabricIndex = 1;
    chip::TransportMgr<chip::Testing::LoopbackTransport> mTransport;
    chip::Crypto::DefaultSessionKeystore mKeystore;
    chip::SessionManager mSessions;
    chip::SessionHolder mInjected[2];
    size_t mConnected = 0;
    chip::Messaging::ExchangeManager mExchanges;
    chip::app::DefaultTimerDelegate mTimerDelegate;
    chip::app::reporting::ReportSchedulerImpl mScheduler;
    chip::Credentials::GroupDataProviderImpl mGroups;
    chip::CASEClientPool<2> mCaseClients;
    chip::OperationalSessionSetupPool<2> mSetups;
    chip::CASESessionManager mCase;
};

} // namespace esp_matter::test::client

// keep test bodies flat; check_mem_leak repeats them, including their teardown
#define CLIENT_TEST_CASE(name, tags) \
    static void UNITY_TEST_UID(client_test_body_)(); \
    TEST_CASE(name, tags) \
    { \
        esp_matter::test::client::check_mem_leak(UNITY_TEST_UID(client_test_body_)); \
    } \
    static void UNITY_TEST_UID(client_test_body_)()
