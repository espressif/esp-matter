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

#include "client_common.h"

#include <esp_matter_client_binding.h>
#include <esp_matter_client_invoke_command.h>
#include <app/clusters/bindings/BindingManager.h>
#include <app/InteractionModelEngine.h>
#include <credentials/GroupDataProviderImpl.h>
#include <app/reporting/ReportSchedulerImpl.h>
#include <app/server-cluster/testing/FabricTestFixture.h>
#include <crypto/DefaultSessionKeystore.h>
#include <platform/DefaultTimerDelegate.h>
#include <transport/TransportMgr.h>
#include <transport/raw/tests/NetworkTestHelpers.h>
#include <lib/support/TestPersistentStorageDelegate.h>
#include <unity.h>

namespace {

namespace binding = esp_matter::client::binding;
using namespace esp_matter::client;
using namespace esp_matter::test;
using namespace chip::app::Clusters::Binding;

class BindingFixture {
public:
    BindingFixture() : mFabric(&mStorage), mScheduler(&mTimerDelegate)
    {
        TEST_ASSERT_TRUE(mFabric.SetUpTestFabric(mFabricIndex) == CHIP_NO_ERROR);
        auto &transport = mTransport.GetTransport().GetImplAtIndex<0>();
        transport.InitLoopbackTransport(&chip::DeviceLayer::SystemLayer());
        // Exercise native send-error completion without a network peer or dropped packets.
        transport.mMessageSendError = CHIP_ERROR_WRITE_FAILED;
        TEST_ASSERT_TRUE(mTransport.Init("binding-test") == CHIP_NO_ERROR);
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
        binding::initialize();
        ManagerInitParams params = {
            .mFabricTable = &mFabric.GetFabricTable(),
                                    .mCASESessionManager = &mCase,
                                    .mStorage = &mStorage,
                                    .mEstablishConnectionOnInit = false,
        };
        TEST_ASSERT_TRUE(Manager::GetInstance().Init(params) == CHIP_NO_ERROR);
    }

    ~BindingFixture()
    {
        ClearBindings();
        Table::GetInstance().SetPersistentStorage(nullptr);
        mFabric.GetFabricTable().RemoveFabricDelegate(&Manager::GetInstance());
        auto *engine = chip::app::InteractionModelEngine::GetInstance();
        engine->Shutdown();
        // SDK Shutdown leaves runtime pointers installed; restore the pristine unit-test runtime.
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

    void ClearBindings()
    {
        auto &table = Table::GetInstance();
        while (table.begin() != table.end()) {
            auto entry = table.begin();
            TEST_ASSERT_TRUE(Manager::GetInstance().UnicastBindingRemoved(entry.GetIndex()) == CHIP_NO_ERROR);
            TEST_ASSERT_TRUE(table.RemoveAt(entry) == CHIP_NO_ERROR);
        }
    }

    void Add(const TableEntry &entry)
    {
        TEST_ASSERT_TRUE(Table::GetInstance().Add(entry) == CHIP_NO_ERROR);
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

    void AddFabric(chip::FabricIndex fabric)
    {
        TEST_ASSERT_TRUE(mFabric.AddAdditionalTestFabric(fabric) == CHIP_NO_ERROR);
    }

private:
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

class FiniteDelegate final : public invoke_command::delegate, public read_command::delegate,
    public write_command::delegate {
public:
    void OnDone(const target_t &target, CHIP_ERROR err) override
    {
        TEST_ASSERT_LESS_THAN(8, count);
        TEST_ASSERT_TRUE(target.type == target_type_t::node);
        TEST_ASSERT_TRUE(err == CHIP_ERROR_WRITE_FAILED);
        nodes[count++] = target.node_id;
    }

    chip::NodeId nodes[8] = {};
    size_t count = 0;
};

class SubscriptionDelegate final : public subscribe_command::delegate {
public:
    void OnStateChanged(const target_t &target, subscribe_command::state_t, CHIP_ERROR) override
    {
        TEST_ASSERT_TRUE(target.type == target_type_t::node);
        TEST_ASSERT_EQUAL_UINT64(0x1234, target.node_id);
        ++states;
    }

    size_t states = 0;
};

} // namespace

TEST_CASE("binding dispatcher selects matching unicast destinations", "[client-binding]")
{
    init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Connect(0x1234);
    fixture->Connect(0x5678);
    fixture->Add(TableEntry::ForNode(1, 0x1234, 1, 2, 6));
    fixture->Add(TableEntry::ForNode(1, 0x5678, 1, 4, std::nullopt));
    fixture->Add(TableEntry::ForNode(1, 0x1234, 2, 3, 6));
    fixture->Add(TableEntry::ForNode(1, 0x5678, 1, 5, 3));
    FiniteDelegate delegate;
    binding::dispatcher bindings;
    invoke_command::request_t request = {.cluster_id = 6, .command_id = 2};
    TEST_ASSERT_TRUE(bindings.dispatch(1, request, &delegate) == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(0, delegate.count);
    drain_client_test_timers();
    TEST_ASSERT_EQUAL(2, delegate.count);
    TEST_ASSERT_TRUE((delegate.nodes[0] == 0x1234 && delegate.nodes[1] == 0x5678) ||
                     (delegate.nodes[1] == 0x1234 && delegate.nodes[0] == 0x5678));
}

TEST_CASE("binding stop suppresses deferred invoke completion and permits reuse", "[client-binding]")
{
    init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Connect(0x1234);
    fixture->Add(TableEntry::ForNode(1, 0x1234, 1, 2, 6));
    FiniteDelegate delegate;
    binding::dispatcher bindings;
    invoke_command::request_t request = {.cluster_id = 6, .command_id = 2};
    TEST_ASSERT_TRUE(bindings.dispatch(1, request, &delegate) == CHIP_NO_ERROR);
    bindings.stop();
    drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, delegate.count);
    TEST_ASSERT_TRUE(bindings.dispatch(1, request, &delegate) == CHIP_NO_ERROR);
    drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, delegate.count);
}

TEST_CASE("binding completion may stop and reuse dispatcher without invalidating target", "[client-binding]")
{
    init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Connect(0x1234);
    fixture->Connect(0x5678);
    fixture->Add(TableEntry::ForNode(1, 0x1234, 1, 2, 6));
    fixture->Add(TableEntry::ForNode(1, 0x5678, 2, 3, 6));
    struct Delegate final : invoke_command::delegate {
        binding::dispatcher *bindings = nullptr;
        size_t count = 0;
        void OnDone(const target_t &target, CHIP_ERROR err) override
        {
            TEST_ASSERT_TRUE(err == CHIP_ERROR_WRITE_FAILED);
            bindings->stop();
            if (++count == 1) {
                invoke_command::request_t request = {.cluster_id = 6, .command_id = 2};
                TEST_ASSERT_TRUE(bindings->dispatch(2, request, this) == CHIP_NO_ERROR);
                TEST_ASSERT_EQUAL_UINT64(0x1234, target.node_id);
            } else {
                TEST_ASSERT_EQUAL_UINT64(0x5678, target.node_id);
            }
        }
    } delegate;
    binding::dispatcher bindings;
    delegate.bindings = &bindings;
    invoke_command::request_t request = {.cluster_id = 6, .command_id = 2};
    TEST_ASSERT_TRUE(bindings.dispatch(1, request, &delegate) == CHIP_NO_ERROR);
    drain_client_test_timers();
    drain_client_test_timers();
    TEST_ASSERT_EQUAL(2, delegate.count);
}

TEST_CASE("binding rejects nonterminal callback dispatch without cancelling other endpoints", "[client-binding]")
{
    init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Connect(0x1234);
    fixture->Connect(0x5678);
    fixture->Add(TableEntry::ForNode(1, 0x1234, 1, 2, 6));
    fixture->Add(TableEntry::ForNode(1, 0x5678, 2, 3, 6));
    binding::dispatcher bindings;
    FiniteDelegate finite;
    struct Delegate final : subscribe_command::delegate {
        binding::dispatcher *bindings = nullptr;
        FiniteDelegate *finite = nullptr;
        size_t states = 0;
        void OnStateChanged(const target_t &, subscribe_command::state_t, CHIP_ERROR) override
        {
            ++states;
            invoke_command::request_t invoke = {.cluster_id = 6, .command_id = 2};
            chip::app::AttributePathParams path(1, 6, 0);
            read_command::request_t read(chip::Span<const chip::app::AttributePathParams>(&path, 1));
            write_command::request_t write = {};
            subscribe_command::request_t subscription(chip::Span<const chip::app::AttributePathParams>(&path, 1));
            TEST_ASSERT_TRUE(bindings->dispatch(1, invoke, finite) == CHIP_ERROR_INCORRECT_STATE);
            TEST_ASSERT_TRUE(bindings->dispatch(1, read, *finite) == CHIP_ERROR_INCORRECT_STATE);
            TEST_ASSERT_TRUE(bindings->dispatch(1, write, finite) == CHIP_ERROR_INCORRECT_STATE);
            TEST_ASSERT_TRUE(bindings->dispatch(1, subscription, *this) == CHIP_ERROR_INCORRECT_STATE);
        }
    } delegate;
    delegate.bindings = &bindings;
    delegate.finite = &finite;
    invoke_command::request_t invoke = {.cluster_id = 6, .command_id = 2};
    TEST_ASSERT_TRUE(bindings.dispatch(2, invoke, &finite) == CHIP_NO_ERROR);
    chip::app::AttributePathParams path(1, 6, 0);
    subscribe_command::request_t subscription(chip::Span<const chip::app::AttributePathParams>(&path, 1));
    TEST_ASSERT_TRUE(bindings.dispatch(1, subscription, delegate) == CHIP_NO_ERROR);
    drain_client_test_timers();
    TEST_ASSERT_TRUE(delegate.states > 0);
    TEST_ASSERT_EQUAL(1, finite.count);
    TEST_ASSERT_EQUAL_UINT64(0x5678, finite.nodes[0]);
    bindings.stop();
}

TEST_CASE("binding late SDK callbacks cannot reach a destroyed dispatcher", "[client-binding]")
{
    init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Add(TableEntry::ForNode(2, 0x1234, 1, 2, 6));
    fixture->Add(TableEntry::ForNode(2, 0x1234, 2, 3, 6));
    FiniteDelegate first, second;
    invoke_command::request_t request = {.cluster_id = 6, .command_id = 2};
    {
        binding::dispatcher bindings;
        // Missing fabric: SDK retains its notification after a synchronous CASE error.
        TEST_ASSERT_TRUE(bindings.dispatch(1, request, &first) != CHIP_NO_ERROR);
    }
    fixture->AddFabric(2);
    fixture->Connect(0x1234, 2);
    binding::dispatcher bindings;
    TEST_ASSERT_TRUE(bindings.dispatch(2, request, &second) == CHIP_NO_ERROR);
    drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, first.count);
    TEST_ASSERT_EQUAL(1, second.count);
}

TEST_CASE("binding rejects mixed clusters and skips multicast reads and subscriptions", "[client-binding]")
{
    init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Add(TableEntry::ForGroup(1, 7, 1, 6));
    FiniteDelegate delegate;
    SubscriptionDelegate subscription_delegate;
    binding::dispatcher bindings;
    chip::app::AttributePathParams paths[] = {{1, 6, 0}, {1, 3, 0}};
    read_command::request_t mixed{chip::Span<const chip::app::AttributePathParams>(paths)};
    TEST_ASSERT_TRUE(bindings.dispatch(1, mixed, delegate) == CHIP_ERROR_INVALID_ARGUMENT);
    read_command::request_t attribute(chip::Span<const chip::app::AttributePathParams>(paths, 1));
    TEST_ASSERT_TRUE(bindings.dispatch(1, attribute, delegate) == CHIP_NO_ERROR);
    chip::app::EventPathParams path(1, 6, 1);
    read_command::request_t event(chip::Span<const chip::app::EventPathParams>(&path, 1));
    TEST_ASSERT_TRUE(bindings.dispatch(1, event, delegate) == CHIP_NO_ERROR);
    subscribe_command::options_t options;
    options.max_interval = 10;
    subscribe_command::request_t subscription(chip::Span<const chip::app::EventPathParams>(&path, 1), options);
    TEST_ASSERT_TRUE(bindings.dispatch(1, subscription, subscription_delegate) == CHIP_NO_ERROR);
    drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, delegate.count);
    TEST_ASSERT_EQUAL(0, subscription_delegate.states);
}
