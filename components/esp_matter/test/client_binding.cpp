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
#include <esp_matter_client.h>
#include <app/clusters/bindings/BindingManager.h>

namespace {

namespace binding = esp_matter::client::binding;
using namespace esp_matter::client;
using namespace esp_matter::test::client;
using namespace chip::app::Clusters::Binding;

class BindingFixture : public Fixture {
public:
    BindingFixture()
    {
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

CLIENT_TEST_CASE("server self-targets never fall back to CASE", "[client-binding][target]")
{
    init_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    const chip::ScopedNodeId self = fixture->SelfNode();
    execution_t execution;
    TEST_ASSERT_TRUE(resolve_target(self, execution) == CHIP_NO_ERROR);
    TEST_ASSERT_TRUE(execution == execution_t::local);

    const CHIP_ERROR expected = CHIP_ERROR_INCORRECT_STATE;
    FiniteDelegate delegate;
    invoke_command::interaction invoke;
    invoke_command::request_t request = {.endpoint_id = 1, .cluster_id = 6, .command_id = 1};
    TEST_ASSERT_TRUE(invoke.start(self, request, &delegate) == expected);
    TEST_ASSERT_FALSE(invoke.active());

    chip::app::AttributePathParams path(1, 6, 0);
    chip::Span<const chip::app::AttributePathParams> paths(&path, 1);
    read_command::interaction read;
    TEST_ASSERT_TRUE(read.start(self, read_command::request_t(paths), delegate) == expected);

    json_encodable value(R"({"0:U8":1})");
    write_command::attribute_write_t attribute{chip::app::ConcreteDataAttributePath(1, 6, 0), &value};
    write_command::interaction write;
    TEST_ASSERT_TRUE(write.start(self, {{&attribute, 1}, {}}, &delegate) == expected);

    subscribe_command::subscription subscription;
    SubscriptionDelegate subscription_delegate;
    TEST_ASSERT_TRUE(subscription.start(self, subscribe_command::request_t(paths), subscription_delegate) ==
                     CHIP_ERROR_INVALID_ARGUMENT);
    drain_timers();
    TEST_ASSERT_EQUAL(0, delegate.count);
    TEST_ASSERT_EQUAL(0, subscription_delegate.states);
}

CLIENT_TEST_CASE("binding dispatcher selects matching unicast destinations", "[client-binding]")
{
    init_test_stack();
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
    drain_timers();
    TEST_ASSERT_EQUAL(2, delegate.count);
    TEST_ASSERT_TRUE((delegate.nodes[0] == 0x1234 && delegate.nodes[1] == 0x5678) ||
                     (delegate.nodes[1] == 0x1234 && delegate.nodes[0] == 0x5678));
}

CLIENT_TEST_CASE("binding stop suppresses deferred invoke completion and permits reuse", "[client-binding]")
{
    init_test_stack();
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
    drain_timers();
    TEST_ASSERT_EQUAL(0, delegate.count);
    TEST_ASSERT_TRUE(bindings.dispatch(1, request, &delegate) == CHIP_NO_ERROR);
    drain_timers();
    TEST_ASSERT_EQUAL(1, delegate.count);
}

CLIENT_TEST_CASE("binding completion may stop and reuse dispatcher without invalidating target", "[client-binding]")
{
    init_test_stack();
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
    drain_timers();
    drain_timers();
    TEST_ASSERT_EQUAL(2, delegate.count);
}

CLIENT_TEST_CASE("binding rejects nonterminal callback dispatch without cancelling other endpoints", "[client-binding]")
{
    init_test_stack();
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
            TEST_ASSERT_TRUE(bindings->dispatch(1, invoke, finite) == CHIP_ERROR_INCORRECT_STATE);
        }
    } delegate;
    delegate.bindings = &bindings;
    delegate.finite = &finite;
    invoke_command::request_t invoke = {.cluster_id = 6, .command_id = 2};
    TEST_ASSERT_TRUE(bindings.dispatch(2, invoke, &finite) == CHIP_NO_ERROR);
    chip::app::AttributePathParams path(1, 6, 0);
    subscribe_command::request_t subscription(chip::Span<const chip::app::AttributePathParams>(&path, 1));
    TEST_ASSERT_TRUE(bindings.dispatch(1, subscription, delegate) == CHIP_NO_ERROR);
    drain_timers();
    TEST_ASSERT_TRUE(delegate.states > 0);
    TEST_ASSERT_EQUAL(1, finite.count);
    TEST_ASSERT_EQUAL_UINT64(0x5678, finite.nodes[0]);
    bindings.stop();
}

CLIENT_TEST_CASE("binding late SDK callbacks cannot reach a destroyed dispatcher", "[client-binding]")
{
    init_test_stack();
    chip::DeviceLayer::StackLock lock;
    auto fixture = chip::Platform::MakeUnique<BindingFixture>();
    TEST_ASSERT_NOT_NULL(fixture.get());
    fixture->Add(TableEntry::ForNode(2, 0x1234, 1, 2, 6));
    fixture->Add(TableEntry::ForNode(2, 0x1234, 2, 3, 6));
    FiniteDelegate first, second;
    invoke_command::request_t request = {.cluster_id = 6, .command_id = 2};
    {
        binding::dispatcher bindings;
        // missing fabric: SDK retains its notification after a synchronous CASE error
        TEST_ASSERT_TRUE(bindings.dispatch(1, request, &first) != CHIP_NO_ERROR);
    }
    fixture->AddFabric(2);
    fixture->Connect(0x1234, 2);
    binding::dispatcher bindings;
    TEST_ASSERT_TRUE(bindings.dispatch(2, request, &second) == CHIP_NO_ERROR);
    drain_timers();
    TEST_ASSERT_EQUAL(0, first.count);
    TEST_ASSERT_EQUAL(1, second.count);
}

CLIENT_TEST_CASE("binding rejects mixed clusters and skips multicast reads and subscriptions", "[client-binding]")
{
    init_test_stack();
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
    drain_timers();
    TEST_ASSERT_EQUAL(0, delegate.count);
    TEST_ASSERT_EQUAL(0, subscription_delegate.states);
}
