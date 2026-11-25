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

#include <app/AttributeValueDecoder.h>
#include <app/InteractionModelEngine.h>
#include <clusters/GeneralCommissioning/ClusterId.h>
#include <clusters/GeneralCommissioning/CommandIds.h>
#include <clusters/OperationalCredentials/ClusterId.h>
#include <clusters/OperationalCredentials/CommandIds.h>
#include <esp_matter_client_operation.h>
#include <app/data-model/DecodableList.h>
#include <app/server-cluster/testing/EmptyProvider.h>
#include <esp_matter_client.h>
#include <lib/support/ScopedMemoryBuffer.h>
#include <unity.h>
#include <array>
#include <iterator>
#include <cstdint>
#include <cstring>
#include <optional>

namespace {

namespace client = esp_matter::client;
namespace DataModel = chip::app::DataModel;
using chip::Protocols::InteractionModel::Status;

void CheckLocalSource(const client::target_t &source, const chip::ScopedNodeId &target)
{
    TEST_ASSERT_TRUE(source.type == client::target_type_t::node);
    TEST_ASSERT_TRUE(source.execution == client::execution_t::local);
    TEST_ASSERT_EQUAL(target.GetFabricIndex(), source.fabric_index);
    TEST_ASSERT_EQUAL_UINT64(target.GetNodeId(), source.node_id);
}

void RemoveFabric()
{
    TEST_ASSERT_TRUE(client::operation::GetFabricTable()->Delete(1) == CHIP_NO_ERROR);
}

void RestoreFabric(esp_matter::test::client::Fixture &runtime)
{
    TEST_ASSERT_TRUE(client::operation::GetFabricTable()->SetFabricIndexForNextAddition(1) == CHIP_NO_ERROR);
    runtime.AddFabric(1);
}

class LocalProvider final : public chip::Testing::EmptyProvider {
public:
    CHIP_ERROR Endpoints(chip::ReadOnlyBufferBuilder<DataModel::EndpointEntry> &builder) override
    {
        static const DataModel::EndpointEntry endpoints[] = {
            {
                .id = 1, .parentId = chip::kInvalidEndpointId,
                .compositionPattern = DataModel::EndpointCompositionPattern::kTree
            },
        };
        return builder.ReferenceExisting(endpoints);
    }

    CHIP_ERROR ServerClusters(chip::EndpointId endpoint,
                              chip::ReadOnlyBufferBuilder<DataModel::ServerClusterEntry> &builder) override
    {
        if (endpoint != 1) {
            return CHIP_NO_ERROR;
        }
        if (list_write_active && fail_version_on_list_begin) {
            return CHIP_ERROR_INTERNAL;
        }
        static const DataModel::ServerClusterEntry clusters[] = {
            { .clusterId = 6, .dataVersion = 10 },
        };
        return builder.ReferenceExisting(clusters);
    }

    CHIP_ERROR Attributes(const chip::app::ConcreteClusterPath &path,
                          chip::ReadOnlyBufferBuilder<DataModel::AttributeEntry> &builder) override
    {
        if (path.mEndpointId != 1 || path.mClusterId != 6) {
            return CHIP_NO_ERROR;
        }
        if (write_list) {
            static const DataModel::AttributeEntry writable(
                1, chip::BitMask<DataModel::AttributeQualityFlags>(
                    DataModel::AttributeQualityFlags::kListAttribute),
                chip::Access::Privilege::kView, chip::Access::Privilege::kOperate);
            static const DataModel::AttributeEntry read_only(
                1, chip::BitMask<DataModel::AttributeQualityFlags>(
                    DataModel::AttributeQualityFlags::kListAttribute),
                chip::Access::Privilege::kView, std::nullopt);
            const auto &attribute = read_only_on_list_begin && list_write_active ? read_only : writable;
            return builder.ReferenceExisting(chip::Span<const DataModel::AttributeEntry>(&attribute, 1));
        }
        static const DataModel::AttributeEntry attributes[] = {
            DataModel::AttributeEntry(1, {}, chip::Access::Privilege::kView, chip::Access::Privilege::kOperate),
            DataModel::AttributeEntry(
                3, chip::BitMask<DataModel::AttributeQualityFlags>(
                    DataModel::AttributeQualityFlags::kTimed),
                chip::Access::Privilege::kView, chip::Access::Privilege::kOperate),
            DataModel::AttributeEntry(6, {}, chip::Access::Privilege::kView, chip::Access::Privilege::kOperate),
            DataModel::AttributeEntry(7, {}, std::nullopt, chip::Access::Privilege::kOperate),
            DataModel::AttributeEntry(8, {}, chip::Access::Privilege::kView, std::nullopt),
            DataModel::AttributeEntry(9, {}, chip::Access::Privilege::kView, std::nullopt),
        };
        return builder.ReferenceExisting(attributes);
    }

    ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest &request,
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
            if (omit_read_value) {
                return Status::Success;
            }
            static const uint8_t large_value[UINT16_MAX - 2] = {};
            return encoder.Encode(chip::ByteSpan(large_value, read_bytes_size));
        }
        return Status::UnsupportedAttribute;
    }

    ActionReturnStatus WriteAttribute(const DataModel::WriteAttributeRequest &request,
                                      chip::app::AttributeValueDecoder &decoder) override
    {
        if (request.path.mAttributeId == 6) {
            return Status::ConstraintError;
        }
        if (write_list) {
            VerifyOrReturnError(request.path.mListOp == chip::app::ConcreteDataAttributePath::ListOperation::ReplaceAll,
                                CHIP_ERROR_INCORRECT_STATE);
            DataModel::DecodableList<chip::ByteSpan> values;
            ReturnErrorOnFailure(decoder.Decode(values));
            auto iterator = values.begin();
            VerifyOrReturnError(iterator.Next(), CHIP_ERROR_INVALID_ARGUMENT);
            chip::ByteSpan value = iterator.GetValue();
            VerifyOrReturnError(!iterator.Next(), CHIP_ERROR_INVALID_ARGUMENT);
            ReturnErrorOnFailure(iterator.GetStatus());
            VerifyOrReturnError(list_write_active, CHIP_ERROR_INCORRECT_STATE);
            VerifyOrReturnError(pending_bytes.Alloc(value.size()), CHIP_ERROR_NO_MEMORY);
            memcpy(pending_bytes.Get(), value.data(), value.size());
            return CHIP_NO_ERROR;
        }
        return decoder.Decode(scalar);
    }

    void ListAttributeWriteNotification(const chip::app::ConcreteAttributePath &path,
                                        DataModel::ListWriteOperation operation,
                                        chip::FabricIndex fabric_index) override
    {
        using DataModel::ListWriteOperation;
        TEST_ASSERT_EQUAL(1, path.mAttributeId);
        TEST_ASSERT_EQUAL(1, fabric_index);
        if (operation == ListWriteOperation::kListWriteBegin) {
            TEST_ASSERT_FALSE(list_write_active);
            list_write_active = true;
            ++list_begins;
            return;
        }
        TEST_ASSERT_TRUE(list_write_active);
        list_write_active = false;
        list_write_succeeded = operation == ListWriteOperation::kListWriteSuccess;
        ++list_ends;
        if (list_write_succeeded) {
            written_bytes.Free();
            written_bytes = std::move(pending_bytes);
        }
        pending_bytes.Free();
    }

    CHIP_ERROR AcceptedCommands(const chip::app::ConcreteClusterPath &,
                                chip::ReadOnlyBufferBuilder<DataModel::AcceptedCommandEntry> &builder) override
    {
        ReturnErrorOnFailure(builder.EnsureAppendCapacity(1));
        return builder.Append(DataModel::AcceptedCommandEntry(command_id));
    }

    std::optional<ActionReturnStatus> InvokeCommand(const DataModel::InvokeRequest &,
                                                    chip::TLV::TLVReader &payload,
                                                    chip::app::CommandHandler *handler) override
    {
        ++invocations;
        chip::TLV::TLVType type;
        CHIP_ERROR err = payload.EnterContainer(type);
        if (err == CHIP_NO_ERROR) {
            err = payload.Next();
            if (err == CHIP_NO_ERROR) {
                if (payload.GetType() == chip::TLV::kTLVType_ByteString) {
                    chip::ByteSpan bytes;
                    err = payload.Get(bytes);
                    TEST_ASSERT_TRUE(err == CHIP_NO_ERROR);
                    TEST_ASSERT_EACH_EQUAL_UINT8(0, bytes.data(), bytes.size());
                    received_bytes_size = bytes.size();
                } else {
                    err = payload.Get(received_value);
                }
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
        return Status::Success;
    }

    void RespondAndRelease()
    {
        if (held.Get() != nullptr) {
            client::json_encodable fields(R"({"0:OBJ":{"0:U16":42}})");
            held.Get()->AddResponse(chip::app::ConcreteCommandPath(1, 6, 1), 2, fields);
        }
        held.Release();
    }

    chip::app::CommandHandler::Handle held;
    uint16_t received_value = 0;
    size_t received_bytes_size = 0;
    unsigned invocations = 0;
    chip::CommandId command_id = 1;
    bool retain = true;

    uint16_t scalar = 0;
    size_t read_bytes_size = chip::kMaxAppMessageLen;
    bool omit_read_value = false;
    uint16_t list_count = 2;
    uint16_t list_error_index = UINT16_MAX;
    bool list_bytes = false;
    bool write_list = false;
    bool fail_version_on_list_begin = false;
    bool read_only_on_list_begin = false;
    bool list_write_active = false;
    bool list_write_succeeded = false;
    unsigned list_begins = 0;
    unsigned list_ends = 0;
    chip::Platform::ScopedMemoryBufferWithSize<uint8_t> written_bytes;
    chip::Platform::ScopedMemoryBufferWithSize<uint8_t> pending_bytes;
};

class LocalReceiver : public client::read_command::delegate,
    public client::write_command::delegate,
    public client::invoke_command::delegate {
public:
    void OnResponse(const client::target_t &source, const chip::app::ConcreteDataAttributePath &, const chip::app::StatusIB &status) override
    {
        CheckLocalSource(source, target);
        TEST_ASSERT_TRUE(attribute_count < statuses.size());
        statuses[attribute_count++] = status.mStatus;
        if (remove_fabric) {
            remove_fabric = false;
            RemoveFabric();
        }
    }

    void OnReportBegin(const client::target_t &source) override
    {
        CheckLocalSource(source, target);
        ++report_begins;
    }
    void OnResponse(const client::target_t &source, const chip::app::ConcreteCommandPath &path, const chip::app::StatusIB &status,
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

    void OnDone(const client::target_t &source, CHIP_ERROR err) override
    {
        CheckLocalSource(source, target);
        ++completions;
        result = err;
        if (stop_on_done != nullptr) {
            stop_on_done->stop();
        }
    }

    void OnAttribute(const client::target_t &source, const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value,
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
        if (remove_fabric) {
            remove_fabric = false;
            RemoveFabric();
        }
    }

    void OnReportEnd(const client::target_t &source, CHIP_ERROR err) override
    {
        CheckLocalSource(source, target);
        ++report_ends;
        report_error = err;
    }

    chip::ScopedNodeId target;
    client::invoke_command::interaction *stop_on_done = nullptr;
    uint32_t responses = 0;
    chip::CommandId response_id = chip::kInvalidCommandId;
    uint16_t response_value = 0;
    bool has_value = false;
    Status response_status = Status::Failure;
    bool remove_fabric = false;
    chip::app::ConcreteDataAttributePath first_path = { 0, 0, 0 };
    std::array<Status, 8> statuses = {};
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

class BlobFields final : public DataModel::EncodableToTLV {
public:
    explicit BlobFields(size_t size) : mSize(size) {}

    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const override
    {
        static const uint8_t data[chip::app::kMaxSecureSduLengthBytes + 1] = {};
        chip::TLV::TLVType type;
        ReturnErrorOnFailure(writer.StartContainer(tag, chip::TLV::kTLVType_Structure, type));
        ReturnErrorOnFailure(writer.PutBytes(chip::TLV::ContextTag(0), data, mSize));
        return writer.EndContainer(type);
    }

private:
    const size_t mSize;
};

class ByteListEncoder final : public DataModel::EncodableToTLV {
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

class LocalFixture {
public:
    LocalFixture() : runtime(chip::Platform::MakeUnique<esp_matter::test::client::Fixture>())
    {
        TEST_ASSERT_NOT_NULL(runtime.get());
        auto *engine = chip::app::InteractionModelEngine::GetInstance();
        previous = engine->GetDataModelProvider();
        engine->SetDataModelProvider(&provider);
        target = runtime->SelfNode();
        receiver.target = target;
    }

    ~LocalFixture()
    {
        read.stop();
        write.stop();
        invoke.stop();
        provider.RespondAndRelease();
        esp_matter::test::client::drain_timers();
        chip::app::InteractionModelEngine::GetInstance()->SetDataModelProvider(previous);
    }

    chip::DeviceLayer::StackLock lock;
    chip::Platform::UniquePtr<esp_matter::test::client::Fixture> runtime;
    LocalProvider provider;
    LocalReceiver receiver;
    chip::ScopedNodeId target;
    client::read_command::interaction read;
    client::write_command::interaction write;
    client::invoke_command::interaction invoke;
    DataModel::Provider *previous = nullptr;
};

} // namespace

CLIENT_TEST_CASE("local Read expands wildcards and continues after path failures", "[client][read][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    chip::app::AttributePathParams paths[] = {
        chip::app::AttributePathParams(chip::ClusterId{ 6 }, chip::AttributeId{ 1 }),
        chip::app::AttributePathParams(1, 6, 99),
        chip::app::AttributePathParams(1, 6, 7),
        chip::app::AttributePathParams(1, 6, 1),
    };

    client::read_command::request_t read_request({ paths, std::size(paths) });
    TEST_ASSERT_TRUE(fixture.read.start(fixture.target, read_request, fixture.receiver) == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(0, fixture.receiver.completions);
    esp_matter::test::client::drain_timers();
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
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(Status::UnsupportedAttribute, fixture.receiver.statuses[1]);
    TEST_ASSERT_EQUAL(Status::UnsupportedRead, fixture.receiver.statuses[2]);
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[3]);
    TEST_ASSERT_FALSE(fixture.receiver.has_values[1]);
    TEST_ASSERT_FALSE(fixture.receiver.has_values[2]);
    TEST_ASSERT_TRUE(fixture.receiver.has_values[3]);
    TEST_ASSERT_TRUE(fixture.receiver.decode_error == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(42, fixture.receiver.scalar);
    TEST_ASSERT_EQUAL(1, fixture.receiver.report_begins);
    TEST_ASSERT_EQUAL(1, fixture.receiver.report_ends);
    TEST_ASSERT_TRUE(fixture.receiver.report_error == CHIP_NO_ERROR);
}

CLIENT_TEST_CASE("local Read delivers complete lists with metadata up to the size limit", "[client][read][local]")
{
    esp_matter::test::client::init_test_stack();
    // 21,844 three-byte integers plus array delimiters fit just below the value-size limit
    for (uint16_t count : {
                0, 21844
            }) {
        LocalFixture fixture;
        fixture.provider.list_count = count;
        chip::app::AttributePathParams path(1, 6, 9);
        client::read_command::request_t read_request({ &path, 1 });
        TEST_ASSERT_TRUE(fixture.read.start(fixture.target, read_request, fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(1, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[0]);
        TEST_ASSERT_EQUAL(chip::app::ConcreteDataAttributePath::ListOperation::ReplaceAll, fixture.receiver.first_path.mListOp);
        TEST_ASSERT_TRUE(fixture.receiver.first_path.mDataVersion.HasValue());
        TEST_ASSERT_EQUAL(10, fixture.receiver.first_path.mDataVersion.Value());
        TEST_ASSERT_EQUAL(count, fixture.receiver.list_size);
    }
}

CLIENT_TEST_CASE("local Read can stop queued work and be destroyed in OnDone", "[client][read][local][lifetime]")
{
    esp_matter::test::client::init_test_stack();
    class ReadOwner final : public LocalReceiver {
    public:
        void OnDone(const client::target_t &source, CHIP_ERROR err) override
        {
            LocalReceiver::OnDone(source, err);
            operation.reset();
        }
        chip::Platform::UniquePtr<client::read_command::interaction> operation;
    } receiver;

    LocalFixture fixture;
    receiver.target = fixture.target;
    receiver.operation = chip::Platform::MakeUnique<client::read_command::interaction>();
    TEST_ASSERT_NOT_NULL(receiver.operation.get());
    chip::app::AttributePathParams path(1, 6, 1);
    client::read_command::request_t request({ &path, 1 });
    TEST_ASSERT_TRUE(receiver.operation->start(fixture.target, request, receiver) == CHIP_NO_ERROR);
    receiver.operation->stop();
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(0, receiver.attribute_count);
    TEST_ASSERT_EQUAL(0, receiver.completions);

    TEST_ASSERT_TRUE(receiver.operation->start(fixture.target, request, receiver) == CHIP_NO_ERROR);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(42, receiver.scalar);
    TEST_ASSERT_EQUAL(1, receiver.completions);
    TEST_ASSERT_TRUE(receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_NULL(receiver.operation.get());
}

CLIENT_TEST_CASE("local Read grows buffers for byte strings and indivisible list items", "[client][read][local]")
{
    esp_matter::test::client::init_test_stack();
    // A byte string needs three TLV header bytes; the report envelope is additional
    const size_t byte_sizes[] = {chip::kMaxAppMessageLen, UINT16_MAX - 3, UINT16_MAX - 2};
    for (size_t size : byte_sizes) {
        LocalFixture fixture;
        fixture.provider.read_bytes_size = size;
        fixture.provider.list_bytes = true;
        fixture.provider.list_count = 4;
        chip::app::AttributePathParams paths[] = {{1, 6, 8}, {1, 6, 9}};
        client::read_command::request_t read_request({ paths, std::size(paths) });
        TEST_ASSERT_TRUE(fixture.read.start(fixture.target, read_request, fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
        const bool oversized = size > UINT16_MAX - 3;
        TEST_ASSERT_EQUAL(oversized ? Status::ResourceExhausted :
                          Status::Success, fixture.receiver.statuses[0]);
        TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[1]);
        TEST_ASSERT_TRUE(fixture.receiver.decode_error == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(oversized ? 0 : size, fixture.receiver.value_size);
        TEST_ASSERT_EQUAL(4, fixture.receiver.list_size);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    }
}

CLIENT_TEST_CASE("local Read discards incomplete lists and continues after failures", "[client][read][local]")
{
    esp_matter::test::client::init_test_stack();
    for (bool exhaust : {
                false, true
            }) {
        LocalFixture fixture;
        fixture.provider.list_count = 25000;
        // fail near the value-size limit, after earlier attempts have exhausted their buffers
        fixture.provider.list_error_index = exhaust ? UINT16_MAX : 21844;
        chip::app::AttributePathParams paths[] = {{1, 6, 9}, {1, 6, 1}};
        client::read_command::request_t read_request({ paths, std::size(paths) });
        TEST_ASSERT_TRUE(fixture.read.start(fixture.target, read_request, fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(exhaust ? Status::ResourceExhausted :
                          Status::ConstraintError, fixture.receiver.statuses[0]);
        TEST_ASSERT_FALSE(fixture.receiver.has_values[0]);
        TEST_ASSERT_EQUAL(chip::app::ConcreteDataAttributePath::ListOperation::NotList, fixture.receiver.first_path.mListOp);
        TEST_ASSERT_FALSE(fixture.receiver.first_path.mDataVersion.HasValue());
        TEST_ASSERT_EQUAL(0, fixture.receiver.list_size);
        TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[1]);
        TEST_ASSERT_EQUAL(42, fixture.receiver.scalar);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    }
}

CLIENT_TEST_CASE("local Read closes a partial report after a provider omits its value", "[client][read][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    fixture.provider.omit_read_value = true;
    chip::app::AttributePathParams paths[] = { { 1, 6, 1 }, { 1, 6, 8 }, { 1, 6, 1 } };
    client::read_command::request_t request({ paths, std::size(paths) });
    TEST_ASSERT_TRUE(fixture.read.start(fixture.target, request, fixture.receiver) == CHIP_NO_ERROR);
    esp_matter::test::client::drain_timers();

    TEST_ASSERT_EQUAL(1, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(1, fixture.receiver.report_begins);
    TEST_ASSERT_EQUAL(1, fixture.receiver.report_ends);
    TEST_ASSERT_TRUE(fixture.receiver.report_error != CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == fixture.receiver.report_error);
}

CLIENT_TEST_CASE("local Write rechecks write access after list notifications", "[client][write][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    fixture.provider.write_list = true;
    fixture.provider.read_only_on_list_begin = true;
    TEST_ASSERT_TRUE(fixture.provider.written_bytes.Alloc(1));
    fixture.provider.written_bytes[0] = 9;
    const uint8_t bytes[] = { 7 };
    ByteListEncoder value{chip::ByteSpan(bytes)};
    client::write_command::attribute_write_t attribute = {
        chip::app::ConcreteDataAttributePath(1, 6, 1), &value
    };
    client::write_command::request_t write_request = { .attributes = { &attribute, 1 } };
    TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(1, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(Status::UnsupportedWrite, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(1, fixture.provider.written_bytes.AllocatedSize());
    TEST_ASSERT_EQUAL(9, fixture.provider.written_bytes[0]);
}

CLIENT_TEST_CASE("local Write captures child-writer values up to the encoded size limit", "[client][write][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    fixture.provider.write_list = true;
    static const uint8_t bytes[UINT16_MAX - 4] = { 1, 2, 3 };

    // the array delimiters and byte-string header take five bytes; closing a child
    // writer at the limit must use the tail reserved for the parent's end marker
    for (size_t size : {
                size_t(1024), size_t(UINT16_MAX - 5)
            }) {
        {
            chip::Platform::ScopedMemoryBuffer<uint8_t> input;
            // mutate the small input; avoid a second ceiling-sized allocation in the fixture
            const uint8_t *data = bytes;
            if (size == 1024) {
                TEST_ASSERT_TRUE(input.Alloc(size));
                memcpy(input.Get(), bytes, size);
                data = input.Get();
            }
            ByteListEncoder value{chip::ByteSpan(data, size)};
            client::write_command::attribute_write_t attribute = {
                chip::app::ConcreteDataAttributePath(1, 6, 1), &value
            };
            client::write_command::request_t write_request = { .attributes = { &attribute, 1 } };
            TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
            // dispatch must use its copy after the caller mutates and releases the input
            if (input) {
                memset(input.Get(), 0xff, size);
            }
        }
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(Status::Success,
                          fixture.receiver.statuses[fixture.receiver.attribute_count - 1]);
        TEST_ASSERT_EQUAL(size, fixture.provider.written_bytes.AllocatedSize());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, fixture.provider.written_bytes.Get(), size);
    }

    ByteListEncoder oversized{chip::ByteSpan(bytes)};
    client::write_command::attribute_write_t attribute = {
        chip::app::ConcreteDataAttributePath(1, 6, 1), &oversized
    };
    client::write_command::request_t write_request = { .attributes = { &attribute, 1 } };
    TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_ERROR_BUFFER_TOO_SMALL);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(2, fixture.receiver.completions);
    TEST_ASSERT_EQUAL(UINT16_MAX - 5, fixture.provider.written_bytes.AllocatedSize());
}

CLIENT_TEST_CASE("local Write snapshots batches and reports per-path failures without rollback", "[client][write][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    {
        client::json_encodable first(R"({"0:U16":10})");
        client::json_encodable missing(R"({"0:U16":30})");
        client::json_encodable last(R"({"0:U16":50})");
        client::json_encodable rejected(R"({"0:U16":40})");
        client::json_encodable invalid("{}");
        client::write_command::attribute_write_t writes[] = {
            { chip::app::ConcreteDataAttributePath(1, 6, 1), &first },
            { chip::app::ConcreteDataAttributePath(1, 6, 99), &missing },
            { chip::app::ConcreteDataAttributePath(1, 6, 1), &last },
            { chip::app::ConcreteDataAttributePath(1, 6, 6), &rejected },
            { chip::app::ConcreteDataAttributePath(1, 6, 1, chip::MakeOptional<chip::DataVersion>(9)), &first },
            { chip::app::ConcreteDataAttributePath(1, 6, 3, chip::MakeOptional<chip::DataVersion>(10)), &first },
        };
        writes[1].value = &invalid;
        client::write_command::request_t write_request = { .attributes = { writes, std::size(writes) } };
        TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) != CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(0, fixture.receiver.completions);
        TEST_ASSERT_EQUAL(0, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(0, fixture.provider.scalar);
        writes[1].value = &missing;
        TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
    }

    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(50, fixture.provider.scalar);
    TEST_ASSERT_EQUAL(6, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(Status::UnsupportedAttribute, fixture.receiver.statuses[1]);
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[2]);
    TEST_ASSERT_EQUAL(Status::ConstraintError, fixture.receiver.statuses[3]);
    TEST_ASSERT_EQUAL(Status::DataVersionMismatch, fixture.receiver.statuses[4]);
    TEST_ASSERT_EQUAL(Status::NeedsTimedInteraction, fixture.receiver.statuses[5]);
}

CLIENT_TEST_CASE("local Write cancellation and timeout do not mutate attributes", "[client][write][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    client::json_encodable first(R"({"0:U16":1})");
    client::json_encodable second(R"({"0:U16":2})");
    client::write_command::attribute_write_t writes[] = {
        { chip::app::ConcreteDataAttributePath(1, 6, 1), &first },
        { chip::app::ConcreteDataAttributePath(1, 6, 3), &second },
    };
    client::write_command::request_t write_request = { .attributes = { writes, std::size(writes) } };
    TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
    fixture.write.stop();
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(0, fixture.receiver.completions);
    TEST_ASSERT_EQUAL(0, fixture.provider.scalar);

    client::write_command::options_t options;
    options.timed_timeout_ms.SetValue(1);
    write_request.options = options;
    TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);

    vTaskDelay(2);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(0, fixture.provider.scalar);
    TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(Status::Timeout, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(Status::Timeout, fixture.receiver.statuses[1]);
}

CLIENT_TEST_CASE("local Invoke retained handlers survive stop and timeout", "[client][local][lifetime]")
{
    esp_matter::test::client::init_test_stack();
    for (bool timeout : {
                false, true
            }) {
        LocalFixture fixture;
        fixture.receiver.stop_on_done = &fixture.invoke;
        client::invoke_command::request_t invoke_request = {
            .endpoint_id = 1, .cluster_id = 6, .command_id = 1,
        };
        if (timeout) {
            invoke_request.options.response_timeout.SetValue(chip::System::Clock::Milliseconds32(100));
        }
        TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, invoke_request, &fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        if (timeout) {
            vTaskDelay(pdMS_TO_TICKS(120));
            esp_matter::test::client::drain_timers();
            TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_ERROR_TIMEOUT);
        }
        fixture.invoke.stop();
        TEST_ASSERT_NOT_NULL(fixture.provider.held.Get());
        fixture.provider.RespondAndRelease();
        TEST_ASSERT_EQUAL(0, fixture.receiver.responses);
        TEST_ASSERT_EQUAL(timeout ? 1 : 0, fixture.receiver.completions);
    }
}

CLIENT_TEST_CASE("local Invoke snapshots fields and completes status or retained data", "[client][local][lifetime]")
{
    esp_matter::test::client::init_test_stack();
    for (bool retain : {
                false, true
            }) {
        LocalFixture fixture;
        fixture.provider.retain = retain;
        fixture.receiver.stop_on_done = &fixture.invoke;
        char body[] = R"({"0:OBJ":{"0:U16":17}})";
        client::json_encodable fields(body);
        client::invoke_command::request_t invoke_request = {
            .endpoint_id = 1, .cluster_id = 6, .command_id = 1,
            .fields = &fields,
        };
        invoke_request.options.response_timeout.SetValue(chip::System::Clock::Milliseconds32(100));
        TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, invoke_request, &fixture.receiver) == CHIP_NO_ERROR);

        // dispatch must use the captured fields after the caller releases its input
        std::memset(body, 0, sizeof(body));
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(17, fixture.provider.received_value);
        fixture.provider.RespondAndRelease();
        vTaskDelay(pdMS_TO_TICKS(120));
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(1, fixture.receiver.responses);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_TRUE(fixture.receiver.response_status == Status::Success);
        TEST_ASSERT_EQUAL(retain, fixture.receiver.has_value);
        TEST_ASSERT_EQUAL(retain ? 2 : 1, fixture.receiver.response_id);
        if (retain) {
            TEST_ASSERT_EQUAL(42, fixture.receiver.response_value);
        }
    }
}

CLIENT_TEST_CASE("local Invoke can release queued and rejected work", "[client][local][lifetime]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    client::invoke_command::request_t invoke_request = {
        .endpoint_id = 1, .cluster_id = 6, .command_id = 1,
    };
    TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, invoke_request, &fixture.receiver) == CHIP_NO_ERROR);
    fixture.invoke.stop();
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_NULL(fixture.provider.held.Get());
    TEST_ASSERT_EQUAL(0, fixture.receiver.completions);

    fixture.receiver.stop_on_done = &fixture.invoke;
    client::invoke_command::request_t request = {
        .endpoint_id = 1, .cluster_id = 6, .command_id = 2,
    };
    TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, request, &fixture.receiver) == CHIP_NO_ERROR);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_EQUAL(0, fixture.receiver.responses);
    TEST_ASSERT_TRUE(fixture.receiver.result != CHIP_NO_ERROR);
    TEST_ASSERT_NULL(fixture.provider.held.Get());
}

CLIENT_TEST_CASE("local Invoke delivers large fields and rejects oversized requests", "[client][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    fixture.provider.retain = false;
    BlobFields fields(chip::app::kMaxSecureSduLengthBytes - 64);
    client::invoke_command::request_t request = {
        .endpoint_id = 1, .cluster_id = 6, .command_id = 1, .fields = &fields,
    };
    TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, request, &fixture.receiver) == CHIP_NO_ERROR);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(chip::app::kMaxSecureSduLengthBytes - 64, fixture.provider.received_bytes_size);
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.response_status);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);

    BlobFields oversized(chip::app::kMaxSecureSduLengthBytes + 1);
    request.fields = &oversized;
    TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, request, &fixture.receiver) == CHIP_ERROR_BUFFER_TOO_SMALL);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(1, fixture.provider.invocations);
    TEST_ASSERT_EQUAL(1, fixture.receiver.responses);
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
}

CLIENT_TEST_CASE("local Invoke rejects session-dependent commands before handlers", "[client][local]")
{
    using namespace chip::app::Clusters;
    esp_matter::test::client::init_test_stack();
    const struct {
        chip::app::ConcreteCommandPath path;
        Status status;
    } cases[] = {
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::AttestationRequest::Id }, Status::UnsupportedCommand },
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::CSRRequest::Id }, Status::UnsupportedCommand },
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::AddNOC::Id }, Status::UnsupportedCommand },
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::UpdateNOC::Id }, Status::UnsupportedCommand },
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::RemoveFabric::Id }, Status::UnsupportedCommand },
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::SignVIDVerificationRequest::Id }, Status::UnsupportedCommand },
        { { 1, GeneralCommissioning::Id, GeneralCommissioning::Commands::CommissioningComplete::Id }, Status::UnsupportedCommand },
        { { 1, OperationalCredentials::Id, OperationalCredentials::Commands::CertificateChainRequest::Id }, Status::Success },
        { { 1, GeneralCommissioning::Id, GeneralCommissioning::Commands::ArmFailSafe::Id }, Status::Success },
        { { 1, 6, OperationalCredentials::Commands::AttestationRequest::Id }, Status::Success },
    };
    LocalFixture fixture;
    fixture.provider.retain = false;
    unsigned responses = 0;
    unsigned invocations = 0;
    for (const auto &test : cases) {
        fixture.provider.command_id = test.path.mCommandId;
        client::invoke_command::request_t request = {
            .endpoint_id = test.path.mEndpointId, .cluster_id = test.path.mClusterId, .command_id = test.path.mCommandId,
        };
        TEST_ASSERT_TRUE(fixture.invoke.start(fixture.target, request, &fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        ++responses;
        invocations += test.status == Status::Success;
        TEST_ASSERT_EQUAL(invocations, fixture.provider.invocations);
        TEST_ASSERT_EQUAL(responses, fixture.receiver.responses);
        TEST_ASSERT_EQUAL(responses, fixture.receiver.completions);
        TEST_ASSERT_EQUAL(test.status, fixture.receiver.response_status);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    }
}

CLIENT_TEST_CASE("local requests reject queued work after fabric removal", "[client][local]")
{
    esp_matter::test::client::init_test_stack();
    enum class Operation { Read, Write, Invoke };
    for (auto operation : {
                Operation::Read, Operation::Write, Operation::Invoke
            }) {
        LocalFixture fixture;
        chip::app::AttributePathParams path(1, 6, 1);
        client::json_encodable value(R"({"0:U16":42})");
        client::write_command::attribute_write_t attribute = { { 1, 6, 1 }, &value };
        client::write_command::request_t write_request = { .attributes = { &attribute, 1 } };
        client::read_command::request_t read_request({ &path, 1 });
        client::invoke_command::request_t invoke_request = { .endpoint_id = 1, .cluster_id = 6, .command_id = 1 };
        CHIP_ERROR err;
        switch (operation) {
        case Operation::Read:
            err = fixture.read.start(fixture.target, read_request, fixture.receiver);
            break;
        case Operation::Write:
            err = fixture.write.start(fixture.target, write_request, &fixture.receiver);
            break;
        case Operation::Invoke:
            err = fixture.invoke.start(fixture.target, invoke_request, &fixture.receiver);
            break;
        }
        TEST_ASSERT_TRUE(err == CHIP_NO_ERROR);
        RemoveFabric();
        esp_matter::test::client::drain_timers();
        RestoreFabric(*fixture.runtime);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_ERROR_INVALID_FABRIC_INDEX);
        TEST_ASSERT_EQUAL(0, fixture.receiver.attribute_count);
        TEST_ASSERT_EQUAL(0, fixture.receiver.responses);
        TEST_ASSERT_EQUAL(0, fixture.provider.scalar);
        TEST_ASSERT_EQUAL(0, fixture.provider.invocations);
    }
}

CLIENT_TEST_CASE("local Read and Write stop remaining paths after fabric removal in callbacks", "[client][local]")
{
    esp_matter::test::client::init_test_stack();
    for (bool write : {
                false, true
            }) {
        LocalFixture fixture;
        fixture.receiver.remove_fabric = true;
        chip::app::AttributePathParams paths[] = { { 1, 6, 1 }, { 1, 6, 8 } };
        client::json_encodable value(R"({"0:U16":42})");
        client::write_command::attribute_write_t attributes[] = {
            { chip::app::ConcreteDataAttributePath(1, 6, 1), &value },
            { chip::app::ConcreteDataAttributePath(1, 6, 8), &value },
        };
        client::write_command::request_t write_request = { .attributes = { attributes, 2 } };
        client::read_command::request_t read_request({ paths, 2 });
        CHIP_ERROR err = write ? fixture.write.start(fixture.target, write_request, &fixture.receiver)
                         : fixture.read.start(fixture.target, read_request, fixture.receiver);
        TEST_ASSERT_TRUE(err == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        RestoreFabric(*fixture.runtime);
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_ERROR_INVALID_FABRIC_INDEX);
        TEST_ASSERT_EQUAL(1, fixture.receiver.attribute_count);
        if (!write) {
            TEST_ASSERT_EQUAL(1, fixture.receiver.report_begins);
            TEST_ASSERT_EQUAL(1, fixture.receiver.report_ends);
            TEST_ASSERT_TRUE(fixture.receiver.report_error == CHIP_ERROR_INVALID_FABRIC_INDEX);
        }
    }
}

CLIENT_TEST_CASE("local Write commits consecutive list replacements as one sequence", "[client][write][local]")
{
    esp_matter::test::client::init_test_stack();
    LocalFixture fixture;
    fixture.provider.write_list = true;
    const uint8_t first[] = { 1, 2 };
    const uint8_t second[] = { 3, 4, 5 };
    ByteListEncoder first_value{ chip::ByteSpan(first) };
    ByteListEncoder second_value{ chip::ByteSpan(second) };
    client::write_command::attribute_write_t attributes[] = {
        { chip::app::ConcreteDataAttributePath(1, 6, 1), &first_value },
        { chip::app::ConcreteDataAttributePath(1, 6, 1, chip::MakeOptional<chip::DataVersion>(10)), &second_value },
    };
    client::write_command::request_t write_request = { .attributes = { attributes, 2 } };
    TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
    esp_matter::test::client::drain_timers();
    TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
    TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
    TEST_ASSERT_EQUAL(2, fixture.receiver.attribute_count);
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[0]);
    TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[1]);
    TEST_ASSERT_EQUAL(1, fixture.provider.list_begins);
    TEST_ASSERT_EQUAL(1, fixture.provider.list_ends);
    TEST_ASSERT_TRUE(fixture.provider.list_write_succeeded);
    TEST_ASSERT_EQUAL(sizeof(second), fixture.provider.written_bytes.AllocatedSize());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(second, fixture.provider.written_bytes.Get(), sizeof(second));
}

CLIENT_TEST_CASE("local Write finalizes list failure on status and internal errors", "[client][write][local]")
{
    esp_matter::test::client::init_test_stack();
    for (bool internal_error : {
                false, true
            }) {
        LocalFixture fixture;
        fixture.provider.write_list = true;
        fixture.provider.fail_version_on_list_begin = internal_error;
        TEST_ASSERT_TRUE(fixture.provider.written_bytes.Alloc(1));
        fixture.provider.written_bytes[0] = 9;
        const uint8_t bytes[] = { 1, 2 };
        ByteListEncoder value{ chip::ByteSpan(bytes) };
        client::json_encodable invalid(R"({"0:U16":42})");
        client::write_command::attribute_write_t attributes[] = {
            { chip::app::ConcreteDataAttributePath(1, 6, 1, chip::MakeOptional<chip::DataVersion>(10)), &value },
            { chip::app::ConcreteDataAttributePath(1, 6, 1), &invalid },
        };
        client::write_command::request_t write_request = { .attributes = { attributes, 2 } };
        TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(1, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == (internal_error ? CHIP_ERROR_INTERNAL : CHIP_NO_ERROR));
        TEST_ASSERT_EQUAL(internal_error ? 0 : 2, fixture.receiver.attribute_count);
        if (!internal_error) {
            TEST_ASSERT_EQUAL(Status::Success, fixture.receiver.statuses[0]);
            TEST_ASSERT_NOT_EQUAL(Status::Success, fixture.receiver.statuses[1]);
        }
        TEST_ASSERT_EQUAL(1, fixture.provider.list_begins);
        TEST_ASSERT_EQUAL(1, fixture.provider.list_ends);
        TEST_ASSERT_FALSE(fixture.provider.list_write_active);
        TEST_ASSERT_FALSE(fixture.provider.list_write_succeeded);
        TEST_ASSERT_EQUAL(1, fixture.provider.written_bytes.AllocatedSize());
        TEST_ASSERT_EQUAL(9, fixture.provider.written_bytes[0]);

        fixture.provider.fail_version_on_list_begin = false;
        write_request.attributes = { attributes, 1 };
        TEST_ASSERT_TRUE(fixture.write.start(fixture.target, write_request, &fixture.receiver) == CHIP_NO_ERROR);
        esp_matter::test::client::drain_timers();
        TEST_ASSERT_EQUAL(2, fixture.receiver.completions);
        TEST_ASSERT_TRUE(fixture.receiver.result == CHIP_NO_ERROR);
        TEST_ASSERT_EQUAL(2, fixture.provider.list_begins);
        TEST_ASSERT_EQUAL(2, fixture.provider.list_ends);
        TEST_ASSERT_TRUE(fixture.provider.list_write_succeeded);
        TEST_ASSERT_EQUAL(sizeof(bytes), fixture.provider.written_bytes.AllocatedSize());
        TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, fixture.provider.written_bytes.Get(), sizeof(bytes));
    }
}
