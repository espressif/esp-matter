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

#include <app/InteractionModelEngine.h>
#include <esp_matter_client.h>
#include <cJSON.h>
#include <tlv_to_json.h>
#include <unity.h>
#include <cstdint>

namespace {

class ValidationDelegate final : public esp_matter::client::read_command::delegate,
    public esp_matter::client::subscribe_command::delegate,
    public esp_matter::client::write_command::delegate {
public:
    void OnReportBegin(const esp_matter::client::target_t &) override
    {
        ++callbacks;
    }
    void OnReportEnd(const esp_matter::client::target_t &, CHIP_ERROR) override
    {
        ++callbacks;
    }
    void OnDone(const esp_matter::client::target_t &, CHIP_ERROR) override
    {
        ++callbacks;
    }
    void OnStateChanged(const esp_matter::client::target_t &, esp_matter::client::subscribe_command::state_t, CHIP_ERROR) override
    {
        ++callbacks;
    }
    void OnResponse(const esp_matter::client::target_t &, const chip::app::ConcreteDataAttributePath &, const chip::app::StatusIB &) override
    {
        ++callbacks;
    }

    uint32_t callbacks = 0;
};

} // namespace

TEST_CASE("client interactions reject unavailable CHIP runtime without callbacks", "[client][validation]")
{
    esp_matter::test::init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    TEST_ASSERT_NULL(chip::app::InteractionModelEngine::GetInstance()->GetExchangeManager());

    chip::app::AttributePathParams path(1, 6, 0);
    const chip::Span<const chip::app::AttributePathParams> paths(&path, 1);
    const chip::ScopedNodeId peer(42, 1);
    const chip::ScopedNodeId target = peer;
    ValidationDelegate delegate;
    esp_matter::client::read_command::interaction read;
    esp_matter::client::subscribe_command::subscription subscription;

    TEST_ASSERT_TRUE(read.start(target, esp_matter::client::read_command::request_t(paths), delegate) ==
                     CHIP_ERROR_INCORRECT_STATE);
    TEST_ASSERT_TRUE(subscription.start(peer, esp_matter::client::subscribe_command::request_t(paths), delegate) ==
                     CHIP_ERROR_INCORRECT_STATE);
    TEST_ASSERT_TRUE(subscription.state() == esp_matter::client::subscribe_command::state_t::terminated);
    TEST_ASSERT_FALSE(subscription.id().HasValue());

    esp_matter::client::invoke_command::request_t request = {
        .endpoint_id = 1,
        .cluster_id = 6,
        .command_id = 1,
    };
    esp_matter::client::invoke_command::interaction invoke;
    TEST_ASSERT_TRUE(invoke.start(target, request, nullptr) == CHIP_ERROR_INCORRECT_STATE);
    TEST_ASSERT_FALSE(invoke.active());
    TEST_ASSERT_TRUE(invoke.start(1, 1, request, nullptr) == CHIP_ERROR_INCORRECT_STATE);
    esp_matter::client::json_encodable value(R"({"0:U16":1})");
    const esp_matter::client::write_command::attribute_write_t attribute = {
        chip::app::ConcreteDataAttributePath(chip::kInvalidEndpointId, 6, 1), &value
    };
    esp_matter::client::write_command::interaction write;
    TEST_ASSERT_TRUE(write.start(1, 1, { { &attribute, 1 }, {} }, &delegate) == CHIP_ERROR_INCORRECT_STATE);

    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, delegate.callbacks);
    read.stop();
    subscription.stop();
}

TEST_CASE("group Write rejects timed and version-conditional requests without callbacks", "[client][write][validation]")
{
    esp_matter::test::init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    esp_matter::client::json_encodable value(R"({"0:U16":1})");
    esp_matter::client::write_command::attribute_write_t attribute = {
        chip::app::ConcreteDataAttributePath(chip::kInvalidEndpointId, 6, 1), &value
    };
    esp_matter::client::write_command::request_t request = { { &attribute, 1 }, {} };
    ValidationDelegate delegate;
    esp_matter::client::write_command::interaction write;

    request.options.timed_timeout_ms.SetValue(100);
    TEST_ASSERT_TRUE(write.start(1, 1, request, &delegate) == CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
    request.options.timed_timeout_ms.ClearValue();
    attribute.path.mDataVersion.SetValue(10);
    TEST_ASSERT_TRUE(write.start(1, 1, request, &delegate) == CHIP_ERROR_INVALID_ARGUMENT);

    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, delegate.callbacks);
}

TEST_CASE("client Write rejects malformed spans and list-item writes without callbacks", "[client][write][validation]")
{
    esp_matter::test::init_client_test_stack();
    chip::DeviceLayer::StackLock lock;
    const chip::ScopedNodeId target(42, 1);
    esp_matter::client::json_encodable value(R"({"0:U16":1})");
    esp_matter::client::write_command::attribute_write_t valid = {
        chip::app::ConcreteDataAttributePath(1, 6, 1), &value
    };
    auto missing_value = valid;
    missing_value.value = nullptr;
    auto list_item = valid;
    list_item.path.mListOp = chip::app::ConcreteDataAttributePath::ListOperation::AppendItem;

    const chip::Span<const esp_matter::client::write_command::attribute_write_t> invalid[] = {
        {},
        { &valid, SIZE_MAX },
        { chip::Unchecked, static_cast<const esp_matter::client::write_command::attribute_write_t *>(nullptr), 1 },
        { &missing_value, 1 },
        { &list_item, 1 },
    };
    ValidationDelegate delegate;
    esp_matter::client::write_command::interaction write;
    for (const auto &attributes : invalid) {
        TEST_ASSERT_TRUE(write.start(target, { attributes, {} }, &delegate) == CHIP_ERROR_INVALID_ARGUMENT);
    }
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, delegate.callbacks);
}

TEST_CASE("client JSON encodes one complete value", "[client][json]")
{
    const struct {
        const char *input;
        const char *expected;
    } cases[] = {
        { R"({"0:U16":500})", R"({"7:U16":500})" },
        { R"({"0:OBJ":{}})", R"({"7:OBJ":{}})" },
        { R"({"0:ARR-U8":[]})", R"({"7:ARR-?":[]})" },
        { R"({"0:ARR-U8":[1,2]})", R"({"7:ARR-U8":[1,2]})" },
        { R"({"0:OBJ":{"1:U8":7,"2:BOOL":true}})", R"({"7:OBJ":{"1:U8":7,"2:BOOL":true}})" },
    };
    for (const auto &test : cases) {
        uint8_t buffer[256];
        chip::TLV::TLVWriter writer;
        writer.Init(buffer);
        chip::TLV::TLVType type;
        TEST_ASSERT_TRUE(writer.StartContainer(chip::TLV::AnonymousTag(), chip::TLV::kTLVType_Structure, type) == CHIP_NO_ERROR);
        esp_matter::client::json_encodable value(test.input);
        TEST_ASSERT_TRUE(value.EncodeTo(writer, chip::TLV::ContextTag(7)) == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(writer.EndContainer(type) == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(writer.Finalize() == CHIP_NO_ERROR);
        chip::TLV::TLVReader reader;
        reader.Init(buffer, writer.GetLengthWritten());
        TEST_ASSERT_TRUE(reader.Next() == CHIP_NO_ERROR);
        cJSON *actual = nullptr;
        TEST_ASSERT_EQUAL(ESP_OK, esp_matter::tlv_to_json(reader, &actual));
        cJSON *expected = cJSON_Parse(test.expected);
        TEST_ASSERT_NOT_NULL(expected);
        TEST_ASSERT_TRUE(cJSON_Compare(expected, actual, true));
        cJSON_Delete(expected);
        cJSON_Delete(actual);
    }
    const char *invalid[] = { nullptr, "{}", "[]", R"({"0:U16":1,"1:U16":2})" };
    for (const char *json : invalid) {
        uint8_t buffer[64];
        chip::TLV::TLVWriter writer;
        writer.Init(buffer);
        esp_matter::client::json_encodable value(json);
        TEST_ASSERT_TRUE(value.EncodeTo(writer, chip::TLV::AnonymousTag()) != CHIP_NO_ERROR);
    }
}
