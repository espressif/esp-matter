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

#include "client_fixture.h"

#include <app/AttributeValueDecoder.h>
#include <app/InteractionModelEngine.h>
#include <app/data-model/DecodableList.h>
#include <app/server-cluster/testing/EmptyProvider.h>
#include <esp_matter_client.h>
#include <lib/support/ScopedMemoryBuffer.h>
#include <unity.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <optional>

namespace {

void CheckLocalSource(const esp_matter::client::target_t &source, const chip::ScopedNodeId &target)
{
    TEST_ASSERT_TRUE(source.type == esp_matter::client::target_type_t::node);
    TEST_ASSERT_TRUE(source.execution == esp_matter::client::execution_t::local);
    TEST_ASSERT_EQUAL(target.GetFabricIndex(), source.fabric_index);
    TEST_ASSERT_EQUAL_UINT64(target.GetNodeId(), source.node_id);
}

class LocalProvider final : public chip::Testing::EmptyProvider {
public:
    CHIP_ERROR Endpoints(chip::ReadOnlyBufferBuilder<chip::app::DataModel::EndpointEntry> &builder) override
    {
        static const chip::app::DataModel::EndpointEntry endpoints[] = {
            {
                .id = 1, .parentId = chip::kInvalidEndpointId,
                .compositionPattern = chip::app::DataModel::EndpointCompositionPattern::kTree
            },
        };
        return builder.ReferenceExisting(chip::Span<const chip::app::DataModel::EndpointEntry>(endpoints));
    }

    CHIP_ERROR ServerClusters(chip::EndpointId endpoint,
                              chip::ReadOnlyBufferBuilder<chip::app::DataModel::ServerClusterEntry> &builder) override
    {
        if (endpoint != 1) {
            return CHIP_NO_ERROR;
        }
        static const chip::app::DataModel::ServerClusterEntry clusters[] = {
            { .clusterId = 6, .dataVersion = 10 },
        };
        return builder.ReferenceExisting(chip::Span<const chip::app::DataModel::ServerClusterEntry>(clusters));
    }

    CHIP_ERROR Attributes(const chip::app::ConcreteClusterPath &path,
                          chip::ReadOnlyBufferBuilder<chip::app::DataModel::AttributeEntry> &builder) override
    {
        if (path.mEndpointId != 1 || path.mClusterId != 6) {
            return CHIP_NO_ERROR;
        }
        if (write_list) {
            static const chip::app::DataModel::AttributeEntry attribute(
                1, chip::BitMask<chip::app::DataModel::AttributeQualityFlags>(
                    chip::app::DataModel::AttributeQualityFlags::kListAttribute),
                chip::Access::Privilege::kView, chip::Access::Privilege::kOperate);
            return builder.ReferenceExisting(chip::Span<const chip::app::DataModel::AttributeEntry>(&attribute, 1));
        }
        static const chip::app::DataModel::AttributeEntry attributes[] = {
            chip::app::DataModel::AttributeEntry(1, {}, chip::Access::Privilege::kView,
            chip::Access::Privilege::kOperate),
            chip::app::DataModel::AttributeEntry(
                3, chip::BitMask<chip::app::DataModel::AttributeQualityFlags>(
                    chip::app::DataModel::AttributeQualityFlags::kTimed),
                chip::Access::Privilege::kView, chip::Access::Privilege::kOperate),
            chip::app::DataModel::AttributeEntry(6, {}, chip::Access::Privilege::kView,
            chip::Access::Privilege::kOperate),
            chip::app::DataModel::AttributeEntry(7, {}, std::nullopt, chip::Access::Privilege::kOperate),
            chip::app::DataModel::AttributeEntry(8, {}, chip::Access::Privilege::kView, std::nullopt),
            chip::app::DataModel::AttributeEntry(9, {}, chip::Access::Privilege::kView, std::nullopt),
        };
        return builder.ReferenceExisting(
                   chip::Span<const chip::app::DataModel::AttributeEntry>(attributes));
    }

    ActionReturnStatus ReadAttribute(const chip::app::DataModel::ReadAttributeRequest &request,
                                     chip::app::AttributeValueEncoder &encoder) override
    {
        if (request.path.mAttributeId == 1) {
            return encoder.Encode(static_cast<uint16_t>(42));
        }
        if (request.path.mAttributeId == 9) {
            return encoder.EncodeList([this](const auto & list) -> CHIP_ERROR {
                static const uint8_t large_value[chip::kMaxAppMessageLen + 64] = {};
                for (uint16_t index = 0; index < list_count; ++index)
                {
                    VerifyOrReturnError(index != list_error_index, CHIP_IM_GLOBAL_STATUS(ConstraintError));
                    if (list_bytes) {
                        ReturnErrorOnFailure(list.Encode(chip::ByteSpan(large_value)));
                    } else {
                        ReturnErrorOnFailure(list.Encode(static_cast<uint16_t>(index + 256)));
                    }
                }
                return CHIP_NO_ERROR;
            });
        }
        if (request.path.mAttributeId == 8) {
            static const uint8_t large_value[UINT16_MAX - 2] = {};
            return encoder.Encode(chip::ByteSpan(large_value, read_bytes_size));
        }
        return chip::Protocols::InteractionModel::Status::UnsupportedAttribute;
    }

    ActionReturnStatus WriteAttribute(const chip::app::DataModel::WriteAttributeRequest &request,
                                      chip::app::AttributeValueDecoder &decoder) override
    {
        if (request.path.mAttributeId == 6) {
            return chip::Protocols::InteractionModel::Status::ConstraintError;
        }
        if (write_bytes || write_list) {
            chip::ByteSpan value;
            if (write_list) {
                VerifyOrReturnError(request.path.mListOp == chip::app::ConcreteDataAttributePath::ListOperation::ReplaceAll,
                                    CHIP_ERROR_INCORRECT_STATE);
                chip::app::DataModel::DecodableList<chip::ByteSpan> values;
                ReturnErrorOnFailure(decoder.Decode(values));
                auto iterator = values.begin();
                VerifyOrReturnError(iterator.Next(), CHIP_ERROR_INVALID_ARGUMENT);
                value = iterator.GetValue();
                VerifyOrReturnError(!iterator.Next(), CHIP_ERROR_INVALID_ARGUMENT);
                ReturnErrorOnFailure(iterator.GetStatus());
            } else {
                ReturnErrorOnFailure(decoder.Decode(value));
            }
            VerifyOrReturnError(written_bytes.Alloc(value.size()), CHIP_ERROR_NO_MEMORY);
            memcpy(written_bytes.Get(), value.data(), value.size());
            return CHIP_NO_ERROR;
        }
        return decoder.Decode(scalar);
    }

    uint16_t scalar = 0;
    size_t read_bytes_size = chip::kMaxAppMessageLen;
    uint16_t list_count = 2;
    uint16_t list_error_index = UINT16_MAX;
    bool list_bytes = false;
    bool write_bytes = false;
    bool write_list = false;
    chip::Platform::ScopedMemoryBufferWithSize<uint8_t> written_bytes;
};

class AttributeReceiver : public esp_matter::client::read_command::delegate,
    public esp_matter::client::write_command::delegate {
public:
    void OnResponse(const esp_matter::client::target_t &source, const chip::app::ConcreteDataAttributePath &, const chip::app::StatusIB &status) override
    {
        CheckLocalSource(source, target);
        TEST_ASSERT_TRUE(attribute_count < statuses.size());
        statuses[attribute_count++] = status.mStatus;
    }

    void OnReportBegin(const esp_matter::client::target_t &source) override
    {
        CheckLocalSource(source, target);
        ++report_begins;
    }
    void OnDone(const esp_matter::client::target_t &source, CHIP_ERROR err) override
    {
        CheckLocalSource(source, target);
        ++completions;
        result = err;
    }

    void OnAttribute(const esp_matter::client::target_t &source, const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value,
                     const chip::app::StatusIB &status) override
    {
        CheckLocalSource(source, target);
        TEST_ASSERT_TRUE(attribute_count < statuses.size());
        if (attribute_count == 0) {
            first_path = path;
        }
        statuses[attribute_count] = status.mStatus;
        has_values[attribute_count++] = value != nullptr;
        if (value != nullptr && path.mAttributeId == 1) {
            decode_error = value->Get(scalar);
        } else if (value != nullptr && path.mAttributeId == 9) {
            chip::TLV::TLVType type;
            TEST_ASSERT_TRUE(value->EnterContainer(type) == CHIP_NO_ERROR);
            CHIP_ERROR err;
            while ((err = value->Next()) == CHIP_NO_ERROR) {
                if (value->GetType() == chip::TLV::kTLVType_ByteString) {
                    chip::ByteSpan bytes;
                    TEST_ASSERT_TRUE(value->Get(bytes) == CHIP_NO_ERROR);
                    TEST_ASSERT_EQUAL(chip::kMaxAppMessageLen + 64, bytes.size());
                    TEST_ASSERT_EACH_EQUAL_UINT8(0, bytes.data(), bytes.size());
                } else {
                    uint16_t item;
                    TEST_ASSERT_TRUE(value->Get(item) == CHIP_NO_ERROR);
                    TEST_ASSERT_EQUAL(static_cast<uint16_t>(list_size + 256), item);
                }
                ++list_size;
            }
            TEST_ASSERT_TRUE(err == CHIP_END_OF_TLV);
            TEST_ASSERT_TRUE(value->ExitContainer(type) == CHIP_NO_ERROR);
        } else if (value != nullptr && path.mAttributeId == 8) {
            if (value->GetType() == chip::TLV::kTLVType_ByteString) {
                chip::ByteSpan bytes;
                decode_error = value->Get(bytes);
                value_size = bytes.size();
                TEST_ASSERT_EACH_EQUAL_UINT8(0, bytes.data(), bytes.size());
            } else {
                decode_error = CHIP_ERROR_WRONG_TLV_TYPE;
            }
        }
    }

    void OnReportEnd(const esp_matter::client::target_t &source, CHIP_ERROR err) override
    {
        CheckLocalSource(source, target);
        ++report_ends;
        report_error = err;
    }

    chip::ScopedNodeId target;
    chip::app::ConcreteDataAttributePath first_path = { 0, 0, 0 };
    std::array<chip::Protocols::InteractionModel::Status, 8> statuses = {};
    std::array<bool, 8> has_values = {};
    size_t attribute_count = 0;
    size_t value_size = 0;
    size_t list_size = 0;
    uint32_t completions = 0;
    CHIP_ERROR result = CHIP_ERROR_INTERNAL;
    uint16_t scalar = 0;
    uint32_t report_begins = 0;
    uint32_t report_ends = 0;
    CHIP_ERROR decode_error = CHIP_NO_ERROR;
    CHIP_ERROR report_error = CHIP_ERROR_INTERNAL;
};

class LocalAttributeFixture {
public:
    LocalAttributeFixture()
    {
        runtime = chip::Platform::MakeUnique<esp_matter::test::ClientFixture>();
        TEST_ASSERT_NOT_NULL(runtime.get());
        target = runtime->SelfNode();
        receiver.target = target;
        previous = chip::app::InteractionModelEngine::GetInstance()->SetDataModelProvider(&provider);
    }

    ~LocalAttributeFixture()
    {
        read.stop();
        write.stop();
        esp_matter::test::drain_client_test_timers();
        chip::app::InteractionModelEngine::GetInstance()->SetDataModelProvider(previous);
    }

    CHIP_ERROR Read(chip::Span<const chip::app::AttributePathParams> paths,
                    const esp_matter::client::read_command::options_t &options = {})
    {
        esp_matter::client::read_command::request_t request(paths, options);
        return read.start(target, request, receiver);
    }

    CHIP_ERROR Write(chip::Span<const esp_matter::client::write_command::attribute_write_t> attributes,
                     const esp_matter::client::write_command::options_t &options = {})
    {
        esp_matter::client::write_command::request_t request = { attributes, options };
        return write.start(target, request, &receiver);
    }

    chip::DeviceLayer::StackLock lock;
    chip::Platform::UniquePtr<esp_matter::test::ClientFixture> runtime;
    chip::ScopedNodeId target;
    LocalProvider provider;
    AttributeReceiver receiver;
    esp_matter::client::read_command::interaction read;
    esp_matter::client::write_command::interaction write;
    chip::app::DataModel::Provider *previous = nullptr;
};

class ByteListEncoder final : public chip::app::DataModel::EncodableToTLV {
public:
    explicit ByteListEncoder(chip::ByteSpan bytes) : mBytes(bytes) {}

    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const override
    {
        chip::TLV::TLVWriter list;
        ReturnErrorOnFailure(writer.OpenContainer(tag, chip::TLV::kTLVType_Array, list));
        ReturnErrorOnFailure(list.Put(chip::TLV::AnonymousTag(), mBytes));
        return writer.CloseContainer(list);
    }

private:
    chip::ByteSpan mBytes;
};

class RetainingProvider final : public chip::Testing::EmptyProvider {
public:
    CHIP_ERROR AcceptedCommands(const chip::app::ConcreteClusterPath &,
                                chip::ReadOnlyBufferBuilder<chip::app::DataModel::AcceptedCommandEntry> &builder) override
    {
        ReturnErrorOnFailure(builder.EnsureAppendCapacity(1));
        return builder.Append(chip::app::DataModel::AcceptedCommandEntry(1));
    }

    std::optional<ActionReturnStatus> InvokeCommand(const chip::app::DataModel::InvokeRequest &,
                                                    chip::TLV::TLVReader &payload,
                                                    chip::app::CommandHandler *handler) override
    {
        chip::TLV::TLVType type;
        CHIP_ERROR err = payload.EnterContainer(type);
        if (err == CHIP_NO_ERROR) {
            err = payload.Next();
            if (err == CHIP_NO_ERROR) {
                err = payload.Get(received_value);
            } else if (err == CHIP_END_OF_TLV) {
                err = CHIP_NO_ERROR;
            }
            if (err == CHIP_NO_ERROR) {
                err = payload.ExitContainer(type);
            }
        }
        if (err != CHIP_NO_ERROR) {
            return ActionReturnStatus(err);
        }

        if (retain) {
            held = chip::app::CommandHandler::Handle(handler);
            return std::nullopt;
        }
        return chip::Protocols::InteractionModel::Status::Success;
    }

    void RespondAndRelease()
    {
        if (held.Get() != nullptr) {
            esp_matter::client::json_encodable fields(R"({"0:OBJ":{"0:U16":42}})");
            held.Get()->AddResponse(chip::app::ConcreteCommandPath(1, 6, 1), 2, fields);
        }
        held.Release();
    }

    chip::app::CommandHandler::Handle held;
    uint16_t received_value = 0;
    bool retain = true;
};

class InvokeReceiver final : public esp_matter::client::invoke_command::delegate {
public:
    void OnResponse(const esp_matter::client::target_t &source, const chip::app::ConcreteCommandPath &path, const chip::app::StatusIB &status,
                    chip::TLV::TLVReader *value) override
    {
        CheckLocalSource(source, target);
        ++responses;
        response_status = status.mStatus;
        response_id = path.mCommandId;
        has_value = value != nullptr;
        if (value != nullptr) {
            chip::TLV::TLVType type;
            TEST_ASSERT_TRUE(value->EnterContainer(type) == CHIP_NO_ERROR);
            TEST_ASSERT_TRUE(value->Next() == CHIP_NO_ERROR);
            TEST_ASSERT_TRUE(value->Get(response_value) == CHIP_NO_ERROR);
            TEST_ASSERT_TRUE(value->Next() == CHIP_END_OF_TLV);
            TEST_ASSERT_TRUE(value->ExitContainer(type) == CHIP_NO_ERROR);
        }
    }

    void OnDone(const esp_matter::client::target_t &source, CHIP_ERROR err) override
    {
        CheckLocalSource(source, target);
        ++completions;
        result = err;
        if (stop_on_done != nullptr) {
            stop_on_done->stop();
        }
    }

    chip::ScopedNodeId target;
    esp_matter::client::invoke_command::interaction *stop_on_done = nullptr;
    uint32_t responses = 0;
    chip::CommandId response_id = chip::kInvalidCommandId;
    uint16_t response_value = 0;
    bool has_value = false;
    uint32_t completions = 0;
    chip::Protocols::InteractionModel::Status response_status = chip::Protocols::InteractionModel::Status::Failure;
    CHIP_ERROR result = CHIP_ERROR_INTERNAL;
};

class LocalInvokeFixture {
public:
    LocalInvokeFixture()
    {
        runtime = chip::Platform::MakeUnique<esp_matter::test::ClientFixture>();
        TEST_ASSERT_NOT_NULL(runtime.get());
        target = runtime->SelfNode();
        receiver.target = target;
        previous = chip::app::InteractionModelEngine::GetInstance()->SetDataModelProvider(&provider);
    }

    ~LocalInvokeFixture()
    {
        invoke.stop();
        provider.RespondAndRelease();
        esp_matter::test::drain_client_test_timers();
        chip::app::InteractionModelEngine::GetInstance()->SetDataModelProvider(previous);
    }

    CHIP_ERROR Start(bool with_response_timeout = false, const chip::app::DataModel::EncodableToTLV *fields = nullptr)
    {
        esp_matter::client::invoke_command::request_t request;
        request.endpoint_id = 1;
        request.cluster_id = 6;
        request.command_id = 1;
        request.fields = fields;
        if (with_response_timeout) {
            request.options.response_timeout.SetValue(chip::System::Clock::Milliseconds32(100));
        }
        return invoke.start(target, request, &receiver);
    }

    chip::DeviceLayer::StackLock lock;
    chip::Platform::UniquePtr<esp_matter::test::ClientFixture> runtime;
    chip::ScopedNodeId target;
    RetainingProvider provider;
    InvokeReceiver receiver;
    esp_matter::client::invoke_command::interaction invoke;
    chip::app::DataModel::Provider *previous = nullptr;
};

} // namespace

TEST_CASE("local Read expands wildcards and continues after path failures", "[client][read][local]")
{
    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    chip::app::AttributePathParams paths[] = {
        chip::app::AttributePathParams(chip::ClusterId{ 6 }, chip::AttributeId{ 1 }),
        chip::app::AttributePathParams(1, 6, 99),
        chip::app::AttributePathParams(1, 6, 7),
        chip::app::AttributePathParams(1, 6, 1),
    };

    TEST_ASSERT_TRUE(fixture.Read(chip::Span<const chip::app::AttributePathParams>(paths)) == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(0, fixture.receiver.completions);
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(4, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(1, fixture.receiver.first_path.mEndpointId);
    TEST_ASSERT_EQUAL(6, fixture.receiver.first_path.mClusterId);
    TEST_ASSERT_EQUAL(1, fixture.receiver.first_path.mAttributeId);
    TEST_ASSERT_TRUE(fixture.receiver.first_path.mExpanded);
    TEST_ASSERT_TRUE(fixture.receiver.first_path.mDataVersion.HasValue());
    TEST_ASSERT_EQUAL(10, fixture.receiver.first_path.mDataVersion.Value());
    TEST_ASSERT_TRUE(fixture.receiver.first_path.mListOp == chip::app::ConcreteDataAttributePath::ListOperation::NotList);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::UnsupportedAttribute, fixture.receiver.statuses[1]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::UnsupportedRead, fixture.receiver.statuses[2]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[3]);
    TEST_ASSERT_FALSE(fixture.receiver.has_values[1]);
    TEST_ASSERT_FALSE(fixture.receiver.has_values[2]);
    TEST_ASSERT_TRUE(fixture.receiver.has_values[3]);
    TEST_ASSERT_TRUE(fixture.receiver.decode_error == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(42, fixture.receiver.scalar);
    TEST_ASSERT_EQUAL(1, fixture.receiver.report_begins);
    TEST_ASSERT_EQUAL(1, fixture.receiver.report_ends);
    TEST_ASSERT_TRUE(fixture.receiver.report_error == CHIP_NO_ERROR);
}

TEST_CASE("local Read delivers complete lists with metadata up to the size limit", "[client][read][local]")
{
    esp_matter::test::init_client_test_stack();
    // 21,844 three-byte integers plus array delimiters fit just below the value-size limit
    for (uint16_t count : {
                0, 3, 21844
            }) {
        LocalAttributeFixture fixture;
        fixture.provider.list_count = count;
        chip::app::AttributePathParams path(1, 6, 9);
        TEST_ASSERT_TRUE(fixture.Read(chip::Span<const chip::app::AttributePathParams>(&path, 1)) == CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(1, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[0]);
        TEST_ASSERT_EQUAL(chip::app::ConcreteDataAttributePath::ListOperation::ReplaceAll, fixture.receiver.first_path.mListOp);
        TEST_ASSERT_TRUE(fixture.receiver.first_path.mDataVersion.HasValue());
        TEST_ASSERT_EQUAL(10, fixture.receiver.first_path.mDataVersion.Value());
        TEST_ASSERT_EQUAL(count, fixture.receiver.list_size);
    }
}

TEST_CASE("local Read can stop queued work and be destroyed in OnDone", "[client][read][local][lifetime]")
{
    class ReadOwner final : public AttributeReceiver {
    public:
        void OnDone(const esp_matter::client::target_t &source, CHIP_ERROR err) override
        {
            AttributeReceiver::OnDone(source, err);
            operation.reset();
        }
        chip::Platform::UniquePtr<esp_matter::client::read_command::interaction> operation;
    } receiver;

    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    receiver.target = fixture.target;
    receiver.operation = chip::Platform::MakeUnique<esp_matter::client::read_command::interaction>();
    TEST_ASSERT_NOT_NULL(receiver.operation.get());
    chip::app::AttributePathParams path(1, 6, 1);
    esp_matter::client::read_command::request_t request({ &path, 1 });
    TEST_ASSERT_TRUE(receiver.operation->start(fixture.target, request, receiver) == CHIP_NO_ERROR);
    receiver.operation->stop();
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(0, receiver.attribute_count);
    TEST_ASSERT_EQUAL(0, receiver.completions);

    TEST_ASSERT_TRUE(receiver.operation->start(fixture.target, request, receiver) == CHIP_NO_ERROR);
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(42, receiver.scalar);
    TEST_ASSERT_EQUAL(1, receiver.completions);
    TEST_ASSERT_TRUE(receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_NULL(receiver.operation.get());
}

TEST_CASE("local Read grows buffers for byte strings and indivisible list items", "[client][read][local]")
{
    esp_matter::test::init_client_test_stack();
    // A byte string needs three TLV header bytes; the report envelope is additional
    const size_t byte_sizes[] = {chip::kMaxAppMessageLen, UINT16_MAX - 3, UINT16_MAX - 2};
    for (size_t size : byte_sizes) {
        LocalAttributeFixture fixture;
        fixture.provider.read_bytes_size = size;
        fixture.provider.list_bytes = true;
        fixture.provider.list_count = 4;
        chip::app::AttributePathParams paths[] = {{1, 6, 8}, {1, 6, 9}};
        TEST_ASSERT_TRUE(fixture.Read(chip::Span<const chip::app::AttributePathParams>(paths)) == CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
        const bool oversized = size > UINT16_MAX - 3;
        TEST_ASSERT_EQUAL(oversized ? chip::Protocols::InteractionModel::Status::ResourceExhausted :
                          chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[0]);
        TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[1]);
        TEST_ASSERT_TRUE(fixture.receiver.decode_error == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(oversized ? 0 : size, fixture.receiver.value_size);
        TEST_ASSERT_EQUAL(4, fixture.receiver.list_size);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    }
}

TEST_CASE("local Read discards incomplete lists and continues after failures", "[client][read][local]")
{
    esp_matter::test::init_client_test_stack();
    for (bool exhaust : {
                false, true
            }) {
        LocalAttributeFixture fixture;
        fixture.provider.list_count = 25000;
        // fail near the value-size limit, after earlier attempts have exhausted their buffers
        fixture.provider.list_error_index = exhaust ? UINT16_MAX : 21844;
        chip::app::AttributePathParams paths[] = {{1, 6, 9}, {1, 6, 1}};
        TEST_ASSERT_TRUE(fixture.Read(chip::Span<const chip::app::AttributePathParams>(paths)) == CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(exhaust ? chip::Protocols::InteractionModel::Status::ResourceExhausted :
                          chip::Protocols::InteractionModel::Status::ConstraintError, fixture.receiver.statuses[0]);
        TEST_ASSERT_FALSE(fixture.receiver.has_values[0]);
        TEST_ASSERT_EQUAL(chip::app::ConcreteDataAttributePath::ListOperation::NotList, fixture.receiver.first_path.mListOp);
        TEST_ASSERT_FALSE(fixture.receiver.first_path.mDataVersion.HasValue());
        TEST_ASSERT_EQUAL(0, fixture.receiver.list_size);
        TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[1]);
        TEST_ASSERT_EQUAL(42, fixture.receiver.scalar);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    }
}

TEST_CASE("local Write captures large byte strings before dispatch", "[client][write][local]")
{
    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    fixture.provider.write_bytes = true;
    {
        static const uint8_t oversized[UINT16_MAX] = {};
        chip::ByteSpan bytes(oversized);
        chip::app::DataModel::EncodableType<chip::ByteSpan> value(bytes);
        esp_matter::client::write_command::attribute_write_t attribute = {chip::app::ConcreteDataAttributePath(1, 6, 1), &value};
        TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(&attribute, 1)) != CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(0, fixture.receiver.completions);
        TEST_ASSERT_EQUAL(0, fixture.provider.written_bytes.AllocatedSize());
    }
    const size_t size = chip::kMaxAppMessageLen * 3;
    {
        chip::Platform::ScopedMemoryBuffer<uint8_t> input;
        TEST_ASSERT_TRUE(bool(input.Alloc(size)));
        for (size_t index = 0; index < size; ++index) {
            input[index] = static_cast<uint8_t>(index % 251);
        }
        chip::ByteSpan bytes(input.Get(), size);
        chip::app::DataModel::EncodableType<chip::ByteSpan> value(bytes);
        esp_matter::client::write_command::attribute_write_t attribute = {chip::app::ConcreteDataAttributePath(1, 6, 1), &value};
        TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(&attribute, 1)) == CHIP_NO_ERROR);
        memset(input.Get(), 0, size);
    }
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(size, fixture.provider.written_bytes.AllocatedSize());
    for (size_t index = 0; index < size; ++index) {
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(index % 251), fixture.provider.written_bytes[index]);
    }

}

TEST_CASE("local Write grows child writers up to the encoded value limit", "[client][write][local]")
{
    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    fixture.provider.write_list = true;
    static const uint8_t bytes[UINT16_MAX - 4] = { 1, 2, 3 };

    // the array delimiters and byte-string header take five bytes; closing a child
    // writer at the limit must use the tail reserved for the parent's end marker
    for (size_t size : {
                size_t(1024), size_t(UINT16_MAX - 5)
            }) {
        ByteListEncoder value{chip::ByteSpan(bytes, size)};
        esp_matter::client::write_command::attribute_write_t attribute = {
            chip::app::ConcreteDataAttributePath(1, 6, 1), &value
        };
        TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(&attribute, 1)) == CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success,
                          fixture.receiver.statuses[fixture.receiver.attribute_count - 1]);
        TEST_ASSERT_EQUAL(size, fixture.provider.written_bytes.AllocatedSize());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, fixture.provider.written_bytes.Get(), size);
    }

    ByteListEncoder oversized{chip::ByteSpan(bytes)};
    esp_matter::client::write_command::attribute_write_t attribute = {
        chip::app::ConcreteDataAttributePath(1, 6, 1), &oversized
    };
    TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(&attribute, 1)) == CHIP_ERROR_BUFFER_TOO_SMALL);
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(2, fixture.receiver.completions);
    TEST_ASSERT_EQUAL(UINT16_MAX - 5, fixture.provider.written_bytes.AllocatedSize());
}

TEST_CASE("local Write retains values and continues after failures without rollback", "[client][write][local]")
{
    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    {
        esp_matter::client::json_encodable first(R"({"0:U16":10})");
        esp_matter::client::json_encodable missing(R"({"0:U16":30})");
        esp_matter::client::json_encodable last(R"({"0:U16":50})");
        esp_matter::client::json_encodable rejected(R"({"0:U16":40})");
        esp_matter::client::json_encodable invalid("{}");
        esp_matter::client::write_command::attribute_write_t writes[] = {
            { chip::app::ConcreteDataAttributePath(1, 6, 1), &first },
            { chip::app::ConcreteDataAttributePath(1, 6, 99), &missing },
            { chip::app::ConcreteDataAttributePath(1, 6, 1), &last },
            { chip::app::ConcreteDataAttributePath(1, 6, 6), &rejected },
        };
        writes[1].value = &invalid;
        TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(writes)) != CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(0, fixture.receiver.completions);
        TEST_ASSERT_EQUAL(0, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(0, fixture.provider.scalar);
        writes[1].value = &missing;
        TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(writes)) == CHIP_NO_ERROR);
    }

    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(50, fixture.provider.scalar);
    TEST_ASSERT_EQUAL(4, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::UnsupportedAttribute, fixture.receiver.statuses[1]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Success, fixture.receiver.statuses[2]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::ConstraintError, fixture.receiver.statuses[3]);
}

TEST_CASE("local Write enforces data versions and timed attributes per path", "[client][write][local]")
{
    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    esp_matter::client::json_encodable mismatch(R"({"0:U16":1})");
    esp_matter::client::json_encodable timed(R"({"0:U16":2})");
    esp_matter::client::write_command::attribute_write_t writes[] = {
        { chip::app::ConcreteDataAttributePath(1, 6, 1, chip::MakeOptional<chip::DataVersion>(9)), &mismatch },
        { chip::app::ConcreteDataAttributePath(1, 6, 3, chip::MakeOptional<chip::DataVersion>(10)), &timed },
    };
    TEST_ASSERT_TRUE(fixture.Write(chip::Span<const esp_matter::client::write_command::attribute_write_t>(writes)) == CHIP_NO_ERROR);

    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(0, fixture.provider.scalar);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::DataVersionMismatch, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::NeedsTimedInteraction, fixture.receiver.statuses[1]);
}

TEST_CASE("expired local Write deadline reports every path without mutation", "[client][write][local]")
{
    esp_matter::test::init_client_test_stack();
    LocalAttributeFixture fixture;
    esp_matter::client::json_encodable first(R"({"0:U16":1})");
    esp_matter::client::json_encodable second(R"({"0:U16":2})");
    esp_matter::client::write_command::attribute_write_t writes[] = {
        { chip::app::ConcreteDataAttributePath(1, 6, 1), &first },
        { chip::app::ConcreteDataAttributePath(1, 6, 3), &second },
    };
    esp_matter::client::write_command::options_t options;
    options.timed_timeout_ms.SetValue(1);
    TEST_ASSERT_TRUE(fixture.Write(
                         chip::Span<const esp_matter::client::write_command::attribute_write_t>(writes), options) == CHIP_NO_ERROR);

    vTaskDelay(2);
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(0, fixture.provider.scalar);
    TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Timeout, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(chip::Protocols::InteractionModel::Status::Timeout, fixture.receiver.statuses[1]);
}

TEST_CASE("local Invoke retained handlers survive stop and timeout", "[client][local][lifetime]")
{
    esp_matter::test::init_client_test_stack();
    for (bool timeout : {
                false, true
            }) {
        LocalInvokeFixture fixture;
        fixture.receiver.stop_on_done = &fixture.invoke;
        TEST_ASSERT_TRUE(fixture.Start(timeout) == CHIP_NO_ERROR);
        esp_matter::test::drain_client_test_timers();
        if (timeout) {
            vTaskDelay(pdMS_TO_TICKS(120));
            esp_matter::test::drain_client_test_timers();
            TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_ERROR_TIMEOUT);
        }
        fixture.invoke.stop();
        TEST_ASSERT_NOT_NULL(fixture.provider.held.Get());
        fixture.provider.RespondAndRelease();
        TEST_ASSERT_EQUAL(0, fixture.receiver.responses);
        TEST_ASSERT_EQUAL(timeout ? 1 : 0, fixture.receiver.completions);
    }
}

TEST_CASE("local Invoke snapshots fields and completes status or retained data", "[client][local][lifetime]")
{
    esp_matter::test::init_client_test_stack();
    for (bool retain : {
                false, true
            }) {
        LocalInvokeFixture fixture;
        fixture.provider.retain = retain;
        fixture.receiver.stop_on_done = &fixture.invoke;
        char body[] = R"({"0:OBJ":{"0:U16":17}})";
        esp_matter::client::json_encodable fields(body);
        TEST_ASSERT_TRUE(fixture.Start(true, &fields) == CHIP_NO_ERROR);

        // dispatch must use the captured fields after the caller releases its input
        std::memset(body, 0, sizeof(body));
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(17, fixture.provider.received_value);
        fixture.provider.RespondAndRelease();
        vTaskDelay(pdMS_TO_TICKS(120));
        esp_matter::test::drain_client_test_timers();
        TEST_ASSERT_EQUAL(1, fixture.receiver.responses);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(fixture.receiver.response_status == chip::Protocols::InteractionModel::Status::Success);
        TEST_ASSERT_EQUAL(retain, fixture.receiver.has_value);
        TEST_ASSERT_EQUAL(retain ? 2 : 1, fixture.receiver.response_id);
        if (retain) {
            TEST_ASSERT_EQUAL(42, fixture.receiver.response_value);
        }
    }
}

TEST_CASE("local Invoke can release queued and rejected work", "[client][local][lifetime]")
{
    esp_matter::test::init_client_test_stack();
    LocalInvokeFixture fixture;
    TEST_ASSERT_TRUE(fixture.Start() == CHIP_NO_ERROR);
    fixture.invoke.stop();
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_NULL(fixture.provider.held.Get());
    TEST_ASSERT_EQUAL(0, fixture.receiver.completions);

    fixture.receiver.stop_on_done = &fixture.invoke;
    esp_matter::client::invoke_command::request_t request = {
        .endpoint_id = 1,
        .cluster_id = 6,
        .command_id = 2,
    };
    TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, request, &fixture.receiver) == CHIP_NO_ERROR);
    esp_matter::test::drain_client_test_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_EQUAL(0, fixture.receiver.responses);
    TEST_ASSERT_TRUE(fixture.receiver.result != CHIP_NO_ERROR);
    TEST_ASSERT_NULL(fixture.provider.held.Get());
}
