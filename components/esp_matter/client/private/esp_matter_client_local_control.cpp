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

#include "esp_matter_client_local_control.h"
#include "esp_matter_client_operation.h"

#include <access/SubjectDescriptor.h>
#include <app/AttributePathExpandIterator.h>
#include <app/AttributeValueDecoder.h>
#include <app/AttributeValueEncoder.h>
#include <app/CommandHandlerImpl.h>
#include <app/GlobalAttributes.h>
#include <app/InteractionModelEngine.h>
#include <app/MessageDef/AttributeReportIB.h>
#include <app/MessageDef/AttributeReportIBs.h>
#include <app/MessageDef/CommandStatusIB.h>
#include <app/MessageDef/InvokeRequestMessage.h>
#include <app/MessageDef/InvokeResponseMessage.h>
#include <app/data-model-provider/MetadataLookup.h>
#include <clusters/GeneralCommissioning/ClusterId.h>
#include <clusters/GeneralCommissioning/CommandIds.h>
#include <clusters/OperationalCredentials/ClusterId.h>
#include <clusters/OperationalCredentials/CommandIds.h>
#include <credentials/GroupDataProvider.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_provider.h>
#include <lib/core/ReferenceCounted.h>
#include <lib/support/ScopedMemoryBuffer.h>
#include <platform/CHIPDeviceLayer.h>
#include <system/TLVPacketBufferBackingStore.h>

#include <algorithm>
#include <optional>

namespace esp_matter {
namespace client {
namespace {

// local attribute values have a 16-bit encoded-size limit, excluding report envelopes
constexpr uint32_t kMaxLocalEncodedSize = UINT16_MAX;

// grow one owned value without replaying its encoder or staging a packet chain
class LocalEncodeBuffer final : public chip::TLV::TLVBackingStore {
public:
    explicit LocalEncodeBuffer(uint32_t limit = kMaxLocalEncodedSize) : mLimit(limit) {}
    LocalEncodeBuffer(const LocalEncodeBuffer &) = delete;
    LocalEncodeBuffer &operator=(const LocalEncodeBuffer &) = delete;

    ~LocalEncodeBuffer() override
    {
        Free();
    }

    const uint8_t *Get() const
    {
        return mData;
    }
    size_t Size() const
    {
        return mSize;
    }

    void Free()
    {
        chip::Platform::MemoryFree(mData);
        mData = nullptr;
        mSize = 0;
        mCapacity = 0;
    }

    CHIP_ERROR OnInit(chip::TLV::TLVReader &, const uint8_t * &, uint32_t &) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }

    CHIP_ERROR GetNextBuffer(chip::TLV::TLVReader &, const uint8_t * &, uint32_t &) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }

    CHIP_ERROR OnInit(chip::TLV::TLVWriter &writer, uint8_t * &start, uint32_t &size) override
    {
        return GetNewBuffer(writer, start, size);
    }

    CHIP_ERROR FinalizeBuffer(chip::TLV::TLVWriter &, uint8_t *start, uint32_t size) override
    {
        // child writers reset their length counter; use the buffer offset instead
        mSize = static_cast<uint32_t>(start - mData) + size;
        VerifyOrReturnError(mSize <= mCapacity, CHIP_ERROR_INCORRECT_STATE);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR GetNewBuffer(chip::TLV::TLVWriter &, uint8_t * &start, uint32_t &size) override
    {
        // child writer may have reserved an unused tail for its parent's end marker
        if (mSize == mCapacity) {
            VerifyOrReturnError(mCapacity < mLimit, CHIP_ERROR_BUFFER_TOO_SMALL);
            // limit spare capacity and moving-realloc peaks for larger values
            uint32_t step = std::min(mCapacity, uint32_t(1024));
            uint32_t capacity = std::min(mCapacity == 0 ? 64 : mCapacity + step, mLimit);
            auto *data = static_cast<uint8_t *>(chip::Platform::MemoryRealloc(mData, capacity));
            VerifyOrReturnError(data != nullptr, CHIP_ERROR_NO_MEMORY);
            mData = data;
            mCapacity = capacity;
        }
        start = mData + mSize;
        size = mCapacity - mSize;
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Finish(chip::TLV::TLVWriter &writer)
    {
        ReturnErrorOnFailure(writer.Finalize());
        chip::TLV::TLVReader reader;
        reader.Init(mData, mSize);
        ReturnErrorOnFailure(reader.Next());
        VerifyOrReturnError(reader.Next() == CHIP_END_OF_TLV, CHIP_ERROR_INVALID_ARGUMENT);

        // shrinking is optional, a failed realloc leaves the captured value intact
        if (auto *compact = static_cast<uint8_t *>(chip::Platform::MemoryRealloc(mData, mSize)); compact != nullptr) {
            mData = compact;
            mCapacity = mSize;
        }
        return CHIP_NO_ERROR;
    }

    void Release(uint8_t * &data, size_t &size)
    {
        data = mData;
        size = mSize;
        mData = nullptr;
        mSize = 0;
        mCapacity = 0;
    }

private:
    uint8_t *mData = nullptr;
    uint32_t mSize = 0;
    uint32_t mCapacity = 0;
    const uint32_t mLimit;
};

CHIP_ERROR GetLocalDataVersion(chip::app::DataModel::Provider *provider,
                               const chip::app::ConcreteClusterPath &path, chip::DataVersion &version)
{
    auto &native = esp_matter::data_model::provider::get_instance();
    if (provider == &native) {
        // match provider::ServerClusters without allocating every cluster entry
        auto *cluster = esp_matter::cluster::get(path.mEndpointId, path.mClusterId);
        VerifyOrReturnError(cluster != nullptr && (esp_matter::cluster::get_flags(cluster) & CLUSTER_FLAG_SERVER),
                            CHIP_ERROR_INTERNAL);
        if (auto *registered = native.registry().Get(path); registered != nullptr) {
            version = registered->GetDataVersion(path);
        } else {
            VerifyOrReturnError(esp_matter::cluster::get_data_version(cluster, version) == ESP_OK, CHIP_ERROR_INTERNAL);
        }
        return CHIP_NO_ERROR;
    }

    // custom providers retain their own metadata semantics
    auto cluster = chip::app::DataModel::ServerClusterFinder(provider).Find(path);
    VerifyOrReturnError(cluster.has_value(), CHIP_ERROR_INTERNAL);
    version = cluster->dataVersion;
    return CHIP_NO_ERROR;
}

} // namespace

namespace operation {

static CHIP_ERROR ValidateLocalTarget(const target_t &target)
{
    if (target.type == target_type_t::group) {
        return ValidateGroupTarget(target.group_id, target.fabric_index);
    }
    execution_t execution;
    ReturnErrorOnFailure(ResolveNodeTarget(chip::ScopedNodeId(target.node_id, target.fabric_index), execution));
    VerifyOrReturnError(execution == execution_t::local, CHIP_ERROR_INCORRECT_STATE);
    return CHIP_NO_ERROR;
}

// These handlers in the pinned SDK require an exchange or secure session. Local dispatch has neither.
static chip::Protocols::InteractionModel::Status ValidateLocalCommand(const chip::app::ConcreteCommandPath &path)
{
    using namespace chip::app::Clusters;
    using chip::Protocols::InteractionModel::Status;

    if (path.mClusterId == OperationalCredentials::Id) {
        using namespace OperationalCredentials::Commands;
        switch (path.mCommandId) {
        case AttestationRequest::Id:
        case CSRRequest::Id:
        case AddNOC::Id:
        case UpdateNOC::Id:
        case RemoveFabric::Id:
        case SignVIDVerificationRequest::Id:
            return Status::UnsupportedCommand;
        }
    } else if (path.mClusterId == GeneralCommissioning::Id &&
               path.mCommandId == GeneralCommissioning::Commands::CommissioningComplete::Id) {
        return Status::UnsupportedCommand;
    }
    return Status::Success;
}

static chip::Access::SubjectDescriptor LocalSubject(chip::FabricIndex fabric_index)
{
    // trusted device access bypasses ACL checks but retains fabric scoping
    return {
        .fabricIndex = fabric_index,
        .authMode = chip::Access::AuthMode::kInternalDeviceAccess,
        .subject = chip::kUndefinedNodeId,
    };
}

} // namespace operation

namespace read_command {

using chip::app::InteractionModelEngine;
using chip::app::ConcreteDataAttributePath;
using chip::app::StatusIB;

namespace {

struct read_context_t {
    const target_t &target;
    const options_t &options;
    delegate &receiver;
    chip::Optional<chip::System::Clock::Timestamp> deadline;
};

CHIP_ERROR ReadAttributeValue(const read_context_t &context,
                              chip::app::DataModel::Provider *provider,
                              const chip::Access::SubjectDescriptor &subject,
                              const chip::app::ConcreteAttributePath &path,
                              const std::optional<chip::app::DataModel::AttributeEntry> &attribute,
                              chip::app::ConcreteDataAttributePath &response_path)
{
    using namespace chip::app;
    using namespace chip::app::DataModel;
    using chip::Protocols::InteractionModel::Status;

    VerifyOrReturnError(!context.deadline.HasValue() ||
                        chip::System::SystemClock().GetMonotonicTimestamp() < context.deadline.Value(), CHIP_ERROR_TIMEOUT);
    if (!attribute.has_value()) {
        context.receiver.OnAttribute(context.target, response_path, nullptr,
                                     StatusIB(ValidateClusterPath(provider, path, Status::UnsupportedAttribute)));
        return CHIP_NO_ERROR;
    }
    if (!attribute->GetReadPrivilege().has_value()) {
        context.receiver.OnAttribute(context.target, response_path, nullptr, StatusIB(Status::UnsupportedRead));
        return CHIP_NO_ERROR;
    }

    chip::DataVersion data_version;
    ReturnErrorOnFailure(GetLocalDataVersion(provider, path, data_version));
    size_t buffer_size = attribute->HasFlags(AttributeQualityFlags::kListAttribute) ? 512 : 128;
    size_t buffer_limit = kMaxLocalEncodedSize;

    // local delivery needs one contiguous reader, not network chunks
    while (true) {
        VerifyOrReturnError(!context.deadline.HasValue() ||
                            chip::System::SystemClock().GetMonotonicTimestamp() < context.deadline.Value(), CHIP_ERROR_TIMEOUT);

        // CHIP's list encoder requires contiguous storage for reservations and rollback
        chip::Platform::ScopedMemoryBuffer<uint8_t> encoded;
        VerifyOrReturnError(encoded.Alloc(buffer_size), CHIP_ERROR_NO_MEMORY);
        chip::TLV::TLVWriter writer;
        writer.Init(encoded.Get(), buffer_size);
        AttributeReportIBs::Builder reports;
        ReturnErrorOnFailure(reports.Init(&writer));

        AttributeValueEncoder encoder(reports, subject, path, data_version, context.options.fabric_filtered);
        ReadAttributeRequest request(path, subject);
        request.readFlags.Set(ReadFlags::kFabricFiltered, context.options.fabric_filtered);

        // global attributes are derived from metadata, not the provider's ReadAttribute implementation
        ActionReturnStatus result = IsSupportedGlobalAttributeNotInMetadata(path.mAttributeId)
                                    ? ReadGlobalAttributeFromMetadata(provider, path, encoder)
                                    : provider->ReadAttribute(request, encoder);

        if (result.IsOutOfSpaceEncodingResponse()) {
            if (buffer_size == kMaxLocalEncodedSize) {
                // the value limit excludes its report envelope
                // measure that envelope with CHIP's encoder instead of duplicating its path/tag layout
                writer.Init(encoded.Get(), buffer_size);
                ReturnErrorOnFailure(reports.Init(&writer));
                AttributeValueEncoder envelope(reports, subject, path, data_version);
                ReturnErrorOnFailure(envelope.Encode(false));
                ReturnErrorOnFailure(reports.EndOfAttributeReportIBs());
                ReturnErrorOnFailure(writer.Finalize());
                buffer_limit += writer.GetLengthWritten() - 1; // anonymous TLV boolean is one byte
            }
            VerifyOrReturnError(buffer_size < buffer_limit, CHIP_ERROR_NO_MEMORY);
            buffer_size = std::min(std::max(buffer_size * 2, size_t(chip::kMaxAppMessageLen)), buffer_limit);
            continue;
        }

        // attribute status failures do not terminate the remaining requested paths
        if (result.IsError()) {
            context.receiver.OnAttribute(context.target, response_path, nullptr, StatusIB(result.GetStatusCode()));
            return CHIP_NO_ERROR;
        }
        ReturnErrorOnFailure(reports.EndOfAttributeReportIBs());
        ReturnErrorOnFailure(writer.Finalize());

        // decode exactly one complete value, retaining CHIP's path and data version
        chip::TLV::TLVReader reader;
        reader.Init(encoded.Get(), writer.GetLengthWritten());
        ReturnErrorOnFailure(reader.Next());
        chip::TLV::TLVType type;
        ReturnErrorOnFailure(reader.EnterContainer(type));
        ReturnErrorOnFailure(reader.Next());
        AttributeReportIB::Parser report;
        AttributeDataIB::Parser data;
        AttributePathIB::Parser reported_path;
        chip::TLV::TLVReader value;
        chip::DataVersion version;
        ReturnErrorOnFailure(report.Init(reader));
        ReturnErrorOnFailure(report.GetAttributeData(&data));
        ReturnErrorOnFailure(data.GetPath(&reported_path));
        ReturnErrorOnFailure(reported_path.GetConcreteAttributePath(response_path));
        ReturnErrorOnFailure(data.GetData(&value));
        ReturnErrorOnFailure(data.GetDataVersion(&version));
        response_path.mDataVersion.SetValue(version);
        CHIP_ERROR err = reader.Next();
        VerifyOrReturnError(err == CHIP_END_OF_TLV, err == CHIP_NO_ERROR ? CHIP_ERROR_INCORRECT_STATE : err);
        ReturnErrorOnFailure(reader.ExitContainer(type));

        if (value.GetType() == chip::TLV::kTLVType_Array) {
            response_path.mListOp = ConcreteDataAttributePath::ListOperation::ReplaceAll;
        }
        context.receiver.OnAttribute(context.target, response_path, &value, StatusIB(Status::Success));
        return CHIP_NO_ERROR;
    }
}

} // namespace

CHIP_ERROR read_local(const chip::ScopedNodeId &target, const request_t &request,
                      chip::Optional<chip::System::Clock::Timestamp> deadline, delegate &receiver)
{
    using namespace chip::app;
    using namespace chip::app::DataModel;
    using chip::Protocols::InteractionModel::Status;

    const chip::Span<const AttributePathParams> paths = request.attribute_paths;
    const target_t source(target, execution_t::local);
    const read_context_t context{ source, request.options, receiver, deadline };
    ReturnErrorOnFailure(operation::ValidateLocalTarget(context.target));
    CHIP_ERROR err = context.deadline.HasValue() && chip::System::SystemClock().GetMonotonicTimestamp() >= context.deadline.Value()
                     ? CHIP_ERROR_TIMEOUT : CHIP_NO_ERROR;
    receiver.OnReportBegin(context.target);

    {
        if (err != CHIP_NO_ERROR) {
            goto report_end;
        }
        auto *provider = InteractionModelEngine::GetInstance()->GetDataModelProvider();
        if (provider == nullptr) {
            err = CHIP_ERROR_INCORRECT_STATE;
            goto report_end;
        }
        const chip::Access::SubjectDescriptor subject = operation::LocalSubject(context.target.fabric_index);
        for (const auto &requested_path : paths) {
            if (context.deadline.HasValue() && chip::System::SystemClock().GetMonotonicTimestamp() >= context.deadline.Value()) {
                err = CHIP_ERROR_TIMEOUT;
                goto report_end;
            }

            chip::SingleLinkedListNode<AttributePathParams> path_node = { .mValue = requested_path };
            AttributePathExpandIterator::Position position = AttributePathExpandIterator::Position::StartIterating(&path_node);
            AttributePathExpandIterator iterator(provider, position);
            ConcreteAttributePath path;
            std::optional<AttributeEntry> attribute;
            while (true) {
                err = operation::ValidateLocalTarget(context.target);
                if (err != CHIP_NO_ERROR) {
                    goto report_end;
                }
                if (!iterator.Next(path, &attribute)) {
                    break;
                }
                ConcreteDataAttributePath response_path(path);
                err = ReadAttributeValue(context, provider, subject, path, attribute, response_path);
                if (err == CHIP_ERROR_NO_MEMORY || err == CHIP_ERROR_BUFFER_TOO_SMALL) {
                    receiver.OnAttribute(context.target, response_path, nullptr, StatusIB(Status::ResourceExhausted));
                    continue;
                }
                if (err != CHIP_NO_ERROR) {
                    goto report_end;
                }
            }
        }
    }

report_end:
    receiver.OnReportEnd(context.target, err);
    return err;
}

} // namespace read_command

namespace write_command {

using chip::app::InteractionModelEngine;
using chip::app::ConcreteDataAttributePath;
using chip::app::StatusIB;

class LocalWriteOperation final {
public:
    LocalWriteOperation(const chip::ScopedNodeId &target, const request_t &request,
                        chip::Optional<chip::System::Clock::Timestamp> deadline)
        : mTarget(target, execution_t::local), mOptions(request.options), mDeadline(deadline)
    {}

    ~LocalWriteOperation()
    {
        for (const auto &item : mItems.Span()) {
            chip::Platform::MemoryFree(item.encoded);
        }
    }

    CHIP_ERROR Prepare(chip::Span<const attribute_write_t> attributes)
    {
        VerifyOrReturnError(attributes.size() <= SIZE_MAX / sizeof(item_t), CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(mItems.Alloc(attributes.size()), CHIP_ERROR_NO_MEMORY);

        // capture every borrowed value before dispatch, each encoder runs once
        for (size_t index = 0; index < attributes.size(); ++index) {
            item_t &item = mItems[index];
            item.path = attributes[index].path;

            LocalEncodeBuffer value_buffer;
            chip::TLV::TLVWriter writer;
            ReturnErrorOnFailure(writer.Init(value_buffer, kMaxLocalEncodedSize));
            ReturnErrorOnFailure(attributes[index].value->EncodeTo(writer, chip::TLV::AnonymousTag()));
            ReturnErrorOnFailure(value_buffer.Finish(writer));
            value_buffer.Release(item.encoded, item.encoded_size);
        }
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Run(delegate &receiver)
    {
        using namespace chip::app::DataModel;
        using chip::Protocols::InteractionModel::Status;

        ReturnErrorOnFailure(operation::ValidateLocalTarget(mTarget));
        // a timed request can expire while waiting for dispatch; no path may be written then
        if (mDeadline.HasValue() && chip::System::SystemClock().GetMonotonicTimestamp() >= mDeadline.Value()) {
            for (auto &item : mItems.Span()) {
                chip::Platform::MemoryFree(item.encoded);
                item.encoded = nullptr;
            }
            for (const auto &item : mItems.Span()) {
                receiver.OnResponse(mTarget, item.path, StatusIB(Status::Timeout));
            }
            return CHIP_NO_ERROR;
        }

        auto *provider = InteractionModelEngine::GetInstance()->GetDataModelProvider();
        VerifyOrReturnError(provider != nullptr, CHIP_ERROR_INCORRECT_STATE);
        chip::Access::SubjectDescriptor subject = operation::LocalSubject(mTarget.fabric_index);

        for (size_t begin = 0; begin < mItems.AllocatedSize();) {
            ReturnErrorOnFailure(operation::ValidateLocalTarget(mTarget));
            auto attribute = AttributeFinder(provider).Find(mItems[begin].path);
            bool is_list = attribute.has_value() && attribute->HasFlags(AttributeQualityFlags::kListAttribute);
            size_t end = begin + 1;
            if (is_list) {
                while (end < mItems.AllocatedSize() && mItems[end].path.MatchesConcreteAttributePath(mItems[begin].path)) {
                    ++end;
                }
                provider->ListAttributeWriteNotification(mItems[begin].path, ListWriteOperation::kListWriteBegin, mTarget.fabric_index);
            }

            bool success = true;
            CHIP_ERROR err = CHIP_NO_ERROR;
            size_t index = begin;
            for (; index < end; ++index) {
                err = operation::ValidateLocalTarget(mTarget);
                if (err != CHIP_NO_ERROR) {
                    break;
                }
                auto &item = mItems[index];
                // list hooks may change metadata between writes
                const auto current_attribute = is_list ? AttributeFinder(provider).Find(item.path) : attribute;
                err = Write(item, provider, subject, current_attribute);
                chip::Platform::MemoryFree(item.encoded);
                item.encoded = nullptr;
                if (err != CHIP_NO_ERROR) {
                    break;
                }
                success = success && item.status.IsSuccess();
            }
            if (is_list) {
                provider->ListAttributeWriteNotification(mItems[begin].path,
                                                         success && err == CHIP_NO_ERROR ? ListWriteOperation::kListWriteSuccess : ListWriteOperation::kListWriteFailure,
                                                         mTarget.fabric_index);
            }
            // Complete list notifications before application callbacks can observe the result.
            for (size_t response = begin; response < index; ++response) {
                receiver.OnResponse(mTarget, mItems[response].path, mItems[response].status);
            }
            ReturnErrorOnFailure(err);
            begin = end;
        }
        return CHIP_NO_ERROR;
    }

private:
    struct item_t {
        ConcreteDataAttributePath path;
        uint8_t *encoded = nullptr;
        size_t encoded_size = 0;
        StatusIB status;
    };

    CHIP_ERROR Write(item_t &item, chip::app::DataModel::Provider *provider,
                     const chip::Access::SubjectDescriptor &subject,
                     const std::optional<chip::app::DataModel::AttributeEntry> &attribute)
    {
        using namespace chip::app::DataModel;
        using chip::Protocols::InteractionModel::Status;

        const ConcreteDataAttributePath &request_path = item.path;
        StatusIB &status = item.status;
        if (!attribute.has_value()) {
            status = StatusIB(ValidateClusterPath(provider, request_path, Status::UnsupportedAttribute));
            return CHIP_NO_ERROR;
        }
        if (!attribute->GetWritePrivilege().has_value()) {
            status = StatusIB(Status::UnsupportedWrite);
            return CHIP_NO_ERROR;
        }
        if (attribute->HasFlags(AttributeQualityFlags::kTimed) && !mOptions.timed_timeout_ms.HasValue()) {
            status = StatusIB(Status::NeedsTimedInteraction);
            return CHIP_NO_ERROR;
        }

        // only versioned writes need cluster metadata, read it at dispatch time
        if (request_path.mDataVersion.HasValue()) {
            chip::DataVersion version;
            ReturnErrorOnFailure(GetLocalDataVersion(provider, request_path, version));
            if (version != request_path.mDataVersion.Value()) {
                status = StatusIB(Status::DataVersionMismatch);
                return CHIP_NO_ERROR;
            }
        }

        chip::TLV::TLVReader reader;
        reader.Init(item.encoded, item.encoded_size);
        ReturnErrorOnFailure(reader.Next());

        // a snapshotted list replaces the whole attribute rather than appending an item
        ConcreteDataAttributePath provider_path = request_path;
        if (attribute->HasFlags(AttributeQualityFlags::kListAttribute)) {
            provider_path.mListOp = ConcreteDataAttributePath::ListOperation::ReplaceAll;
        }

        WriteAttributeRequest request(provider_path, subject);
        request.writeFlags.Set(WriteFlags::kTimed, mOptions.timed_timeout_ms.HasValue());
        chip::app::AttributeValueDecoder decoder(reader, subject);
        status = StatusIB(provider->WriteAttribute(request, decoder).GetStatusCode());
        return CHIP_NO_ERROR;
    }

    target_t mTarget;
    options_t mOptions;
    chip::Optional<chip::System::Clock::Timestamp> mDeadline;
    chip::Platform::ScopedMemoryBufferWithSize<item_t> mItems;
};

CHIP_ERROR prepare_local(const chip::ScopedNodeId &target, const request_t &request,
                         chip::Optional<chip::System::Clock::Timestamp> deadline,
                         LocalWriteOperation * &operation)
{
    operation = chip::Platform::New<LocalWriteOperation>(target, request, deadline);
    VerifyOrReturnError(operation != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR err = operation->Prepare(request.attributes);
    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(operation);
        operation = nullptr;
    }
    return err;
}

CHIP_ERROR run_local(LocalWriteOperation *operation, delegate &receiver)
{
    VerifyOrReturnError(operation != nullptr, CHIP_ERROR_INCORRECT_STATE);
    return operation->Run(receiver);
}

void stop_local(LocalWriteOperation *operation)
{
    chip::Platform::Delete(operation);
}

} // namespace write_command

namespace invoke_command {

using chip::app::InteractionModelEngine;
using chip::app::ConcreteDataAttributePath;
using chip::app::StatusIB;

class LocalInvokeOperation final : public chip::app::CommandHandlerImpl::Callback,
    public chip::app::CommandHandlerExchangeInterface, public chip::ReferenceCounted<LocalInvokeOperation> {

public:
    LocalInvokeOperation(const chip::ScopedNodeId &target, const request_t &request) :
        mTarget(target, execution_t::local), mPath(request.endpoint_id, request.cluster_id, request.command_id),
        mOptions(request.options), mHandler(this)
    {
        if (mOptions.timed_timeout_ms.HasValue()) {
            mDispatchDeadline.SetValue(chip::System::SystemClock().GetMonotonicTimestamp() +
                                       chip::System::Clock::Milliseconds32(mOptions.timed_timeout_ms.Value()));
        }
    }

    LocalInvokeOperation(chip::GroupId group_id, chip::FabricIndex fabric_index, const request_t &request) :
        mTarget(group_id, fabric_index), mPath(chip::kInvalidEndpointId, request.cluster_id, request.command_id),
        mOptions(request.options), mHandler(this)
    {}

    CHIP_ERROR Encode(const chip::app::DataModel::EncodableToTLV *fields)
    {
        // feed request framing to the normal command handler without a network exchange
        chip::TLV::TLVWriter writer;
        ReturnErrorOnFailure(writer.Init(mRequestBytes, chip::app::kMaxSecureSduLengthBytes));

        chip::app::InvokeRequestMessage::Builder request;
        ReturnErrorOnFailure(request.Init(&writer));
        request.SuppressResponse(IsGroup()).TimedRequest(mOptions.timed_timeout_ms.HasValue());

        chip::app::InvokeRequests::Builder &requests = request.CreateInvokeRequests();
        ReturnErrorOnFailure(requests.GetError());

        chip::app::CommandDataIB::Builder &command = requests.CreateCommandData();
        ReturnErrorOnFailure(command.GetError());

        // group requests omit the endpoint so the handler can expand group membership
        if (IsGroup()) {
            chip::app::CommandPathParams path(mTarget.group_id, mPath.mClusterId, mPath.mCommandId,
                                              chip::app::CommandPathFlags::kGroupIdValid);
            ReturnErrorOnFailure(command.CreatePath().Encode(path));
        } else {
            ReturnErrorOnFailure(command.CreatePath().Encode(mPath));
        }

        // copy borrowed fields, omitted fields become an empty structure
        auto *command_writer = command.GetWriter();
        auto fields_tag = chip::TLV::ContextTag(chip::app::CommandDataIB::Tag::kFields);
        if (fields != nullptr) {
            ReturnErrorOnFailure(fields->EncodeTo(*command_writer, fields_tag));
        } else {
            chip::TLV::TLVType type;
            ReturnErrorOnFailure(command_writer->StartContainer(fields_tag, chip::TLV::kTLVType_Structure, type));
            ReturnErrorOnFailure(command_writer->EndContainer(type));
        }

        ReturnErrorOnFailure(command.EndOfCommandDataIB());
        ReturnErrorOnFailure(requests.EndOfInvokeRequests());
        ReturnErrorOnFailure(request.EndOfInvokeRequestMessage());

        ReturnErrorOnFailure(mRequestBytes.Finish(writer));
        if (IsGroup()) {
            // reserve the group packet before multicast can be sent
            mRequestPacket = chip::System::PacketBufferHandle::NewWithData(mRequestBytes.Get(), mRequestBytes.Size());
            VerifyOrReturnError(!mRequestPacket.IsNull(), CHIP_ERROR_NO_MEMORY);
            mRequestBytes.Free();
        }
        return CHIP_NO_ERROR;
    }

    void Detach()
    {
        mDelegate = nullptr;
        chip::DeviceLayer::SystemLayer().CancelTimer(OnTimeout, this);
        DiscardMessageBuffers();
        Release();
    }

    void OnDone(chip::app::CommandHandlerImpl &) override
    {
        if (mDelegate != nullptr) {
            Complete(DecodeResponse());
        }
        Release();
    }

    chip::Protocols::InteractionModel::Status ValidateCommandCanBeDispatched(
        const chip::app::DataModel::InvokeRequest &request) override
    {
        auto status = operation::ValidateLocalCommand(request.path);
        if (status != chip::Protocols::InteractionModel::Status::Success) {
            return status;
        }
        if (operation::ValidateLocalTarget(mTarget) != CHIP_NO_ERROR) {
            return chip::Protocols::InteractionModel::Status::Failure;
        }
        if (IsGroup()) {
            // group commands keep IM ACL checks, trusted local unicast uses ValidateDispatch()
            auto * callback = static_cast<chip::app::CommandHandlerImpl::Callback *>(InteractionModelEngine::GetInstance());
            return callback->ValidateCommandCanBeDispatched(request);
        }
        return chip::Protocols::InteractionModel::Status::Success;
    }

    void DispatchCommand(chip::app::CommandHandlerImpl &handler, const chip::app::ConcreteCommandPath &path,
                         chip::TLV::TLVReader &payload) override
    {
        auto *engine = InteractionModelEngine::GetInstance();
        if (engine->GetDataModelProvider() == nullptr) {
            handler.AddStatus(path, chip::Protocols::InteractionModel::Status::Failure);
            return;
        }

        auto *callback = static_cast<chip::app::CommandHandlerImpl::Callback *>(engine);
        callback->DispatchCommand(handler, path, payload);
    }

    chip::Messaging::ExchangeContext * GetExchangeContext() const override
    {
        return nullptr;
    }

    chip::Access::SubjectDescriptor GetSubjectDescriptor() const override
    {
        if (IsGroup()) {
            return {
                .fabricIndex = mTarget.fabric_index,
                .authMode = chip::Access::AuthMode::kGroup,
                .subject = chip::NodeIdFromGroupId(mTarget.group_id),
            };
        }

        return operation::LocalSubject(mTarget.fabric_index);
    }

    chip::FabricIndex GetAccessingFabricIndex() const override
    {
        return mTarget.fabric_index;
    }

    chip::Optional<chip::GroupId> GetGroupId() const override
    {
        return IsGroup() ? chip::MakeOptional(mTarget.group_id) : chip::NullOptional;
    }

    void HandlingSlowCommand() override {}

    void AddInvokeResponseToSend(chip::System::PacketBufferHandle && packet) override
    {
        // only one response packet is supported
        if (!mResponsePacket.IsNull()) {
            mResponseError = CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
            return;
        }

        mResponsePacket = std::move(packet);
    }

    void ResponseDropped() override
    {
        mResponseError = CHIP_ERROR_NO_MEMORY;
    }

    size_t GetCommandResponseMaxBufferSize() override
    {
        return chip::app::kMaxSecureSduLengthBytes;
    }

    void Run(delegate *receiver)
    {
        mDelegate = receiver;

        // recheck metadata and the timed deadline when queued dispatch actually runs
        CHIP_ERROR err = ValidateDispatch();

        // restore SDK packet storage only when dispatch is ready to consume it
        if (err == CHIP_NO_ERROR && !IsGroup()) {
            mRequestPacket = chip::System::PacketBufferHandle::NewWithData(mRequestBytes.Get(), mRequestBytes.Size());
            mRequestBytes.Free();
            if (mRequestPacket.IsNull()) {
                err = CHIP_ERROR_NO_MEMORY;
            }
        }
        if (err != CHIP_NO_ERROR) {
            Complete(err);
            return;
        }

        // response timeout ends observation, not provider execution
        if (mDelegate != nullptr && mOptions.response_timeout.HasValue()) {
            err = chip::DeviceLayer::SystemLayer().StartTimer(mOptions.response_timeout.Value(), OnTimeout, this);
            if (err != CHIP_NO_ERROR) {
                Complete(err);
                return;
            }
        }

        // caller ownership ends in Detach, handler ownership ends in OnDone
        Retain();

        // defer handler completion until dispatch errors have been processed
        chip::app::CommandHandler::Handle handle(&mHandler);
        chip::Protocols::InteractionModel::Status status =
            mHandler.OnInvokeCommandRequest(*this, std::move(mRequestPacket), mOptions.timed_timeout_ms.HasValue());

        if (mResponseError != CHIP_NO_ERROR) {
            Complete(mResponseError);
        } else if (status != chip::Protocols::InteractionModel::Status::Success) {
            Complete(CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
        }
    }

private:
    bool IsGroup() const
    {
        return mTarget.type == target_type_t::group;
    }

    CHIP_ERROR ValidateDispatch()
    {
        using namespace chip::app::DataModel;

        ReturnErrorOnFailure(operation::ValidateLocalTarget(mTarget));
        if (mDispatchDeadline.HasValue() && chip::System::SystemClock().GetMonotonicTimestamp() >= mDispatchDeadline.Value()) {
            return CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(chip::Protocols::InteractionModel::Status::Timeout);
        }

        auto * provider = InteractionModelEngine::GetInstance()->GetDataModelProvider();
        VerifyOrReturnError(provider != nullptr, CHIP_ERROR_INCORRECT_STATE);

        if (IsGroup()) {
            return CHIP_NO_ERROR;
        }

        // internal access still requires an accepted command and any required timed interaction
        chip::ReadOnlyBufferBuilder<AcceptedCommandEntry> accepted_commands;
        ReturnErrorOnFailure(provider->AcceptedCommands(mPath, accepted_commands));

        for (const auto &entry : accepted_commands.TakeBuffer()) {
            if (entry.commandId == mPath.mCommandId) {
                VerifyOrReturnError(
                    !entry.HasFlags(CommandQualityFlags::kTimed) || mOptions.timed_timeout_ms.HasValue(),
                    CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(chip::Protocols::InteractionModel::Status::NeedsTimedInteraction));
                return CHIP_NO_ERROR;
            }
        }

        return CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(
                   ValidateClusterPath(provider, mPath, chip::Protocols::InteractionModel::Status::UnsupportedCommand));
    }

    CHIP_ERROR DecodeResponse()
    {
        ReturnErrorOnFailure(mResponseError);
        VerifyOrReturnError(!mResponsePacket.IsNull(), CHIP_ERROR_INCORRECT_STATE);

        chip::System::PacketBufferTLVReader reader;
        reader.Init(std::move(mResponsePacket));

        chip::app::InvokeResponseMessage::Parser message;
        ReturnErrorOnFailure(message.Init(reader));

        chip::app::InvokeResponseIBs::Parser responses;
        ReturnErrorOnFailure(message.GetInvokeResponses(&responses));

        // a single-command operation accepts exactly one response entry
        chip::TLV::TLVReader response_reader;
        responses.GetReader(&response_reader);
        ReturnErrorOnFailure(response_reader.Next());

        chip::app::InvokeResponseIB::Parser response;
        ReturnErrorOnFailure(response.Init(response_reader));
        VerifyOrReturnError(response_reader.Next() == CHIP_END_OF_TLV, CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);

        // both status-only and data responses use the same client callback
        chip::app::CommandPathIB::Parser path;
        StatusIB result(chip::Protocols::InteractionModel::Status::Success);
        chip::TLV::TLVReader data;
        chip::TLV::TLVReader *value = nullptr;
        chip::app::CommandStatusIB::Parser status;
        if (response.GetStatus(&status) == CHIP_NO_ERROR) {
            StatusIB::Parser status_ib;
            ReturnErrorOnFailure(status.GetPath(&path));
            ReturnErrorOnFailure(status.GetErrorStatus(&status_ib));
            ReturnErrorOnFailure(status_ib.DecodeStatusIB(result));
        } else {
            chip::app::CommandDataIB::Parser command;
            ReturnErrorOnFailure(response.GetCommand(&command));
            ReturnErrorOnFailure(command.GetPath(&path));
            ReturnErrorOnFailure(command.GetFields(&data));
            value = &data;
        }

        chip::app::ConcreteCommandPath concrete_path(0, 0, 0);
        ReturnErrorOnFailure(path.GetConcreteCommandPath(concrete_path));

        // packet storage remains alive while the callback borrows the decoded fields
        if (mDelegate != nullptr) {
            mDelegate->OnResponse(mTarget, concrete_path, result, value);
        }

        return CHIP_NO_ERROR;
    }

    void DiscardMessageBuffers()
    {
        // SDK handles may outlive observation, but our finalized messages need not
        mRequestPacket = nullptr;
        mRequestBytes.Free();
        mResponsePacket = nullptr;
    }

    void Complete(CHIP_ERROR err)
    {
        chip::DeviceLayer::SystemLayer().CancelTimer(OnTimeout, this);
        DiscardMessageBuffers();

        // clear the observer before OnDone, which may destroy this operation
        delegate *receiver = mDelegate;
        mDelegate = nullptr;
        if (receiver != nullptr) {
            const target_t target = mTarget;
            receiver->OnDone(target, err);
        }
    }

    static void OnTimeout(chip::System::Layer *, void * context)
    {
        static_cast<LocalInvokeOperation *>(context)->Complete(CHIP_ERROR_TIMEOUT);
    }

    target_t mTarget;
    chip::app::ConcreteCommandPath mPath;
    options_t mOptions;
    delegate *mDelegate = nullptr;
    chip::app::CommandHandlerImpl mHandler;
    chip::System::PacketBufferHandle mRequestPacket;
    LocalEncodeBuffer mRequestBytes{chip::app::kMaxSecureSduLengthBytes};
    chip::System::PacketBufferHandle mResponsePacket;
    chip::Optional<chip::System::Clock::Timestamp> mDispatchDeadline;
    CHIP_ERROR mResponseError = CHIP_NO_ERROR;
};

CHIP_ERROR prepare_local(const chip::ScopedNodeId &target, const request_t &request, LocalInvokeOperation * &operation)
{
    operation = chip::Platform::New<LocalInvokeOperation>(target, request);
    VerifyOrReturnError(operation != nullptr, CHIP_ERROR_NO_MEMORY);
    return operation->Encode(request.fields);
}

CHIP_ERROR prepare_local(chip::GroupId group_id, chip::FabricIndex fabric_index, const request_t &request,
                         const operation::EncodedElement &fields, LocalInvokeOperation * &operation)
{
    auto *provider = chip::Credentials::GetGroupDataProvider();
    VerifyOrReturnError(provider != nullptr, CHIP_ERROR_INCORRECT_STATE);

    // skip local dispatch when this fabric has no endpoints in the group
    auto *iterator = provider->IterateEndpoints(fabric_index, group_id);
    VerifyOrReturnError(iterator != nullptr, CHIP_ERROR_NO_MEMORY);

    chip::Credentials::GroupDataProvider::GroupEndpoint endpoint;
    bool has_local_endpoint = iterator->Next(endpoint);
    iterator->Release();
    if (!has_local_endpoint) {
        return CHIP_NO_ERROR;
    }

    operation = chip::Platform::New<LocalInvokeOperation>(group_id, fabric_index, request);
    VerifyOrReturnError(operation != nullptr, CHIP_ERROR_NO_MEMORY);
    return operation->Encode(&fields);
}

void run_local(LocalInvokeOperation *operation, delegate *receiver)
{
    if (operation != nullptr) {
        operation->Run(receiver);
    }
}

void stop_local(LocalInvokeOperation *operation)
{
    if (operation != nullptr) {
        operation->Detach();
    }
}

} // namespace invoke_command
} // namespace client
} // namespace esp_matter
