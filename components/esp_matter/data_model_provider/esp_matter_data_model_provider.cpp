// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter_attr_data_buffer.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_cluster.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>
#include <esp_matter_data_model_provider.h>

#include <access/Privilege.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/AttributeAccessInterface.h>
#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterface.h>
#include <app/CommandHandlerInterfaceRegistry.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <clusters/shared/GlobalIds.h>
#include <lib/core/CHIPError.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/CodeUtils.h>
#include <zap-generated/access.h>
#include <app/persistence/AttributePersistenceProviderInstance.h>
#include <app/persistence/DefaultAttributePersistenceProvider.h>

using namespace chip;
using namespace chip::app;

constexpr char TAG[] = "DataModelProvider";

namespace {
/// Attempts to read via an attribute access interface (AAI)
///
/// If it returns a CHIP_ERROR, then this is a FINAL result (i.e. either failure or success).
///
/// If it returns std::nullopt, then there is no AAI to handle the given path
/// and processing should figure out the value otherwise (generally from esp_matter data)
std::optional<CHIP_ERROR> TryReadViaAccessInterface(const ConcreteAttributePath &path, AttributeAccessInterface *aai,
                                                    AttributeValueEncoder &encoder)
{
    // Processing can happen only if an attribute access interface actually exists..
    if (aai == nullptr) {
        return std::nullopt;
    }
    CHIP_ERROR err = aai->Read(path, encoder);
    if (err != CHIP_NO_ERROR) {
        // Implementation of 8.4.3.2 of the spec for path expansion
        if (path.mExpanded && (err == CHIP_IM_GLOBAL_STATUS(UnsupportedRead))) {
            return CHIP_NO_ERROR;
        }
        return err;
    }

    // If the encoder tried to encode, then a value should have been written.
    //   - if encode, assume DONE (i.e. FINAL CHIP_NO_ERROR)
    //   - if no encode, say that processing must continue
    return encoder.TriedEncode() ? std::make_optional(CHIP_NO_ERROR) : std::nullopt;
}

/// Attempts to write via an attribute access interface (AAI)
///
/// If it returns a CHIP_ERROR, then this is a FINAL result (i.e. either failure or success)
///
/// If it returns std::nullopt, then there is no AAI to handle the given path
/// and processing should figure out the value otherwise (generally from other esp_matter data)
std::optional<CHIP_ERROR> TryWriteViaAccessInterface(const ConcreteDataAttributePath &path,
                                                     AttributeAccessInterface *aai, AttributeValueDecoder &decoder)
{
    // Processing can happen only if an attribute access interface actually exists..
    if (aai == nullptr) {
        return std::nullopt;
    }

    CHIP_ERROR err = aai->Write(path, decoder);

    if (err != CHIP_NO_ERROR) {
        return std::make_optional(err);
    }

    // If the decoder tried to decode, then a value should have been read for processing.
    //   - if decoding was done, assume DONE (i.e. final CHIP_NO_ERROR)
    //   -  otherwise, if no decoding done, return that processing must continue via nullopt
    return decoder.TriedDecode() ? std::make_optional(CHIP_NO_ERROR) : std::nullopt;
}
} // namespace

namespace {

#ifdef GENERATED_ACCESS_READ_ATTRIBUTE__CLUSTER
namespace GeneratedAccessReadAttribute {
constexpr ClusterId kCluster[] = GENERATED_ACCESS_READ_ATTRIBUTE__CLUSTER;
constexpr AttributeId kAttribute[] = GENERATED_ACCESS_READ_ATTRIBUTE__ATTRIBUTE;
constexpr chip::Access::Privilege kPrivilege[] = GENERATED_ACCESS_READ_ATTRIBUTE__PRIVILEGE;
static_assert(MATTER_ARRAY_SIZE(kCluster) == MATTER_ARRAY_SIZE(kAttribute) &&
                  MATTER_ARRAY_SIZE(kAttribute) == MATTER_ARRAY_SIZE(kPrivilege),
              "Generated parallel arrays must be same size");
} // namespace GeneratedAccessReadAttribute
#endif

#ifdef GENERATED_ACCESS_WRITE_ATTRIBUTE__CLUSTER
namespace GeneratedAccessWriteAttribute {
constexpr ClusterId kCluster[] = GENERATED_ACCESS_WRITE_ATTRIBUTE__CLUSTER;
constexpr AttributeId kAttribute[] = GENERATED_ACCESS_WRITE_ATTRIBUTE__ATTRIBUTE;
constexpr chip::Access::Privilege kPrivilege[] = GENERATED_ACCESS_WRITE_ATTRIBUTE__PRIVILEGE;
static_assert(MATTER_ARRAY_SIZE(kCluster) == MATTER_ARRAY_SIZE(kAttribute) &&
                  MATTER_ARRAY_SIZE(kAttribute) == MATTER_ARRAY_SIZE(kPrivilege),
              "Generated parallel arrays must be same size");
} // namespace GeneratedAccessWriteAttribute
#endif

#ifdef GENERATED_ACCESS_INVOKE_COMMAND__CLUSTER
namespace GeneratedAccessInvokeCommand {
constexpr ClusterId kCluster[] = GENERATED_ACCESS_INVOKE_COMMAND__CLUSTER;
constexpr CommandId kCommand[] = GENERATED_ACCESS_INVOKE_COMMAND__COMMAND;
constexpr chip::Access::Privilege kPrivilege[] = GENERATED_ACCESS_INVOKE_COMMAND__PRIVILEGE;
static_assert(MATTER_ARRAY_SIZE(kCluster) == MATTER_ARRAY_SIZE(kCommand) &&
                  MATTER_ARRAY_SIZE(kCommand) == MATTER_ARRAY_SIZE(kPrivilege),
              "Generated parallel arrays must be same size");
} // namespace GeneratedAccessInvokeCommand
#endif

#ifdef GENERATED_ACCESS_READ_EVENT__CLUSTER
namespace GeneratedAccessReadEvent {
constexpr ClusterId kCluster[] = GENERATED_ACCESS_READ_EVENT__CLUSTER;
constexpr EventId kEvent[] = GENERATED_ACCESS_READ_EVENT__EVENT;
constexpr chip::Access::Privilege kPrivilege[] = GENERATED_ACCESS_READ_EVENT__PRIVILEGE;
static_assert(MATTER_ARRAY_SIZE(kCluster) == MATTER_ARRAY_SIZE(kEvent) &&
                  MATTER_ARRAY_SIZE(kEvent) == MATTER_ARRAY_SIZE(kPrivilege),
              "Generated parallel arrays must be same size");
} // namespace GeneratedAccessReadEvent
#endif

chip::Access::Privilege MatterGetAccessPrivilegeForReadAttribute(ClusterId cluster, AttributeId attribute)
{
#ifdef GENERATED_ACCESS_READ_ATTRIBUTE__CLUSTER
    using namespace GeneratedAccessReadAttribute;
    for (size_t i = 0; i < MATTER_ARRAY_SIZE(kCluster); ++i) {
        if (kCluster[i] == cluster && kAttribute[i] == attribute) {
            return kPrivilege[i];
        }
    }
#endif
    return chip::Access::Privilege::kView;
}

chip::Access::Privilege MatterGetAccessPrivilegeForWriteAttribute(ClusterId cluster, AttributeId attribute)
{
#ifdef GENERATED_ACCESS_WRITE_ATTRIBUTE__CLUSTER
    using namespace GeneratedAccessWriteAttribute;
    for (size_t i = 0; i < MATTER_ARRAY_SIZE(kCluster); ++i) {
        if (kCluster[i] == cluster && kAttribute[i] == attribute) {
            return kPrivilege[i];
        }
    }
#endif
    return chip::Access::Privilege::kOperate;
}

chip::Access::Privilege MatterGetAccessPrivilegeForInvokeCommand(ClusterId cluster, CommandId command)
{
#ifdef GENERATED_ACCESS_INVOKE_COMMAND__CLUSTER
    using namespace GeneratedAccessInvokeCommand;
    for (size_t i = 0; i < MATTER_ARRAY_SIZE(kCluster); ++i) {
        if (kCluster[i] == cluster && kCommand[i] == command) {
            return kPrivilege[i];
        }
    }
#endif
    return chip::Access::Privilege::kOperate;
}

chip::Access::Privilege MatterGetAccessPrivilegeForReadEvent(ClusterId cluster, EventId event)
{
#ifdef GENERATED_ACCESS_READ_EVENT__CLUSTER
    using namespace GeneratedAccessReadEvent;
    for (size_t i = 0; i < MATTER_ARRAY_SIZE(kCluster); ++i) {
        if (kCluster[i] == cluster && kEvent[i] == event) {
            return kPrivilege[i];
        }
    }
#endif
    return chip::Access::Privilege::kView;
}

size_t get_command_count(esp_matter::cluster_t *cluster, uint8_t flag)
{
    size_t ret = 0;
    esp_matter::command_t *command = esp_matter::command::get_first(cluster);
    while (command) {
        if (esp_matter::command::get_flags(command) & flag) {
            ret++;
        }
        command = esp_matter::command::get_next(command);
    }
    return ret;
}

size_t get_attribute_count(esp_matter::cluster_t *cluster)
{
    size_t ret = 0;
    esp_matter::attribute_t *attribute = esp_matter::attribute::get_first(cluster);
    while (attribute) {
        ret++;
        attribute = esp_matter::attribute::get_next(attribute);
    }
    return ret;
}

DefaultAttributePersistenceProvider gDefaultAttributePersistence;
} // anonymous namespace

namespace esp_matter {
namespace data_model {

provider &provider::get_instance()
{
    static provider instance;
    return instance;
}

/// Generic model implementations
CHIP_ERROR provider::Startup(InteractionModelContext context)
{
    ReturnErrorOnFailure(DataModel::Provider::Startup(context));
    mContext.emplace(context);
    esp_matter::cluster::add_bounds_callback_common();
    esp_matter::cluster::plugin_init_callback_common();
    endpoint_t *ep = endpoint::get_first(node::get());
    while (ep) {
        cluster_t *cluster = cluster::get_first(ep);
        while (cluster) {
            uint8_t flags = cluster::get_flags(cluster);
            cluster::initialization_callback_t init_callback = cluster::get_init_callback(cluster);
            if (init_callback) {
                init_callback(endpoint::get_id(ep));
            }
            if ((flags & CLUSTER_FLAG_SERVER) && (flags & CLUSTER_FLAG_INIT_FUNCTION)) {
                cluster::function_cluster_init_t init_function =
                    (cluster::function_cluster_init_t)cluster::get_function(cluster, CLUSTER_FLAG_INIT_FUNCTION);
                if (init_function) {
                    init_function(endpoint::get_id(ep));
                }
            }
            cluster = cluster::get_next(cluster);
        }
        ep = endpoint::get_next(ep);
    }
    if (GetAttributePersistenceProvider() == nullptr) {
        gDefaultAttributePersistence.Init(&Server::GetInstance().GetPersistentStorage());
        SetAttributePersistenceProvider(&gDefaultAttributePersistence);
    }
    return mRegistry.SetContext(ServerClusterContext{
        .provider = *this,
        .storage = Server::GetInstance().GetPersistentStorage(),
        .attributeStorage = *GetAttributePersistenceProvider(),
        .interactionContext = *mContext,
    });
}

CHIP_ERROR provider::Shutdown()
{
    mRegistry.ClearContext();
    return CHIP_NO_ERROR;
}

ActionReturnStatus provider::ReadAttribute(const ReadAttributeRequest &request, AttributeValueEncoder &encoder)
{
    if (auto *cluster = mRegistry.Get(request.path); cluster != nullptr) {
        return cluster->ReadAttribute(request, encoder);
    }
    Status status = CheckDataModelPath(request.path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    attribute_t *attribute =
        attribute::get(request.path.mEndpointId, request.path.mClusterId, request.path.mAttributeId);

    std::optional<CHIP_ERROR> aai_result = TryReadViaAccessInterface(
        request.path,
        AttributeAccessInterfaceRegistry::Instance().Get(request.path.mEndpointId, request.path.mClusterId), encoder);
    VerifyOrReturnError(!aai_result.has_value(), *aai_result);

    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    attribute::get_val(attribute, &val);
    attribute_data_encode_buffer data_buffer(val);
    return encoder.Encode(data_buffer);
}

ActionReturnStatus provider::WriteAttribute(const WriteAttributeRequest &request, AttributeValueDecoder &decoder)
{
    if (auto *cluster = mRegistry.Get(request.path); cluster != nullptr) {
        return cluster->WriteAttribute(request, decoder);
    }
    Status status = CheckDataModelPath(request.path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    cluster_t *cluster = cluster::get(request.path.mEndpointId, request.path.mClusterId);
    attribute_t *attribute = attribute::get(cluster, request.path.mAttributeId);
    if (request.path.mDataVersion.HasValue()) {
        chip::DataVersion data_version;
        VerifyOrReturnValue(cluster::get_data_version(cluster, data_version) == ESP_OK,
                            Protocols::InteractionModel::Status::DataVersionMismatch);
        VerifyOrReturnValue(data_version == request.path.mDataVersion.Value(),
                            Protocols::InteractionModel::Status::DataVersionMismatch);
    }

    AttributeAccessInterface *aai =
        AttributeAccessInterfaceRegistry::Instance().Get(request.path.mEndpointId, request.path.mClusterId);
    std::optional<CHIP_ERROR> aai_result = TryWriteViaAccessInterface(request.path, aai, decoder);
    if (aai_result.has_value()) {
        if (*aai_result == CHIP_NO_ERROR) {
            cluster::increase_data_version(cluster);
            AttributePathParams path(request.path.mEndpointId, request.path.mClusterId, request.path.mAttributeId);
            mContext->dataModelChangeListener.MarkDirty(path);
        }
        return *aai_result;
    }

    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    attribute::get_val(attribute, &val);
    attribute_data_decode_buffer data_buffer(val.type);
    ReturnErrorOnFailure(decoder.Decode(data_buffer));
    esp_err_t err =
        attribute::update(request.path.mEndpointId, request.path.mClusterId, request.path.mAttributeId, &data_buffer.get_attr_val());
    if (err == ESP_ERR_NO_MEM) {
        return Protocols::InteractionModel::Status::ResourceExhausted;
    } else if (err != ESP_OK) {
        return Protocols::InteractionModel::Status::Failure;
    }
    return Protocols::InteractionModel::Status::Success;
}

void provider::ListAttributeWriteNotification(const ConcreteAttributePath &aPath, ListWriteOperation opType,
                                              FabricIndex accessFabric)
{
    if (auto *cluster = mRegistry.Get(aPath); cluster != nullptr) {
        return cluster->ListAttributeWriteNotification(aPath, opType, accessFabric);
    }
    AttributeAccessInterface *aai =
        AttributeAccessInterfaceRegistry::Instance().Get(aPath.mEndpointId, aPath.mClusterId);
    if (aai != nullptr) {
        switch (opType) {
        case DataModel::ListWriteOperation::kListWriteBegin:
            aai->OnListWriteBegin(aPath);
            break;
        case DataModel::ListWriteOperation::kListWriteFailure:
            aai->OnListWriteEnd(aPath, false);
            break;
        case DataModel::ListWriteOperation::kListWriteSuccess:
            aai->OnListWriteEnd(aPath, true);
            break;
        }
    }
    return;
}

std::optional<ActionReturnStatus> provider::InvokeCommand(const InvokeRequest &request,
                                                          chip::TLV::TLVReader &input_arguments,
                                                          CommandHandler *handler)
{
    if (auto *cluster = mRegistry.Get(request.path); cluster != nullptr) {
        return cluster->InvokeCommand(request, input_arguments, handler);
    }
    Status status = CheckDataModelPath(request.path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    CommandHandlerInterface *handler_interface = CommandHandlerInterfaceRegistry::Instance().GetCommandHandler(
        request.path.mEndpointId, request.path.mClusterId);

    if (handler_interface) {
        CommandHandlerInterface::HandlerContext context(*handler, request.path, input_arguments);
        handler_interface->InvokeCommand(context);

        // If the command was handled, don't proceed any further and return successfully.
        if (context.mCommandHandled) {
            return std::nullopt;
        }
    }

    command::dispatch_single_cluster_command(request.path, input_arguments, handler);
    return std::nullopt;
}

/// attribute tree iteration
CHIP_ERROR provider::Endpoints(ReadOnlyBufferBuilder<EndpointEntry> &builder)
{
    const uint16_t ep_count = endpoint::get_count(node::get());
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(ep_count));
    endpoint_t *ep = endpoint::get_first(node::get());
    while (ep) {
        if (endpoint::is_enabled(ep)) {
            DataModel::EndpointEntry entry;
            entry.id = endpoint::get_id(ep);
            entry.parentId = endpoint::get_parent_endpoint_id(ep);
            entry.compositionPattern = endpoint::get_composition_pattern(ep);
            ReturnErrorOnFailure(builder.Append(entry));
        }
        ep = endpoint::get_next(ep);
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR provider::DeviceTypes(EndpointId endpointId, ReadOnlyBufferBuilder<DeviceTypeEntry> &builder)
{
    Status status = CheckDataModelPath(endpointId);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    endpoint_t *ep = endpoint::get(endpointId);
    size_t count = endpoint::get_device_type_count(ep);
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(count));
    for (size_t idx = 0; idx < count; idx++) {
        DeviceTypeEntry entry;
        VerifyOrReturnError(endpoint::get_device_type_at_index(ep, idx, entry.deviceTypeId, entry.deviceTypeRevision) ==
                                ESP_OK,
                            CHIP_ERROR_INTERNAL, ESP_LOGE(TAG, "Failed to get device type at %d", idx));
        ReturnErrorOnFailure(builder.Append(entry));
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR provider::ClientClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ClusterId> &builder)
{
    Status status = CheckDataModelPath(endpointId);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    endpoint_t *ep = endpoint::get(endpointId);
    size_t count = endpoint::get_cluster_count(endpointId, chip::kInvalidClusterId, CLUSTER_FLAG_CLIENT);
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(count));
    cluster_t *cluster = cluster::get_first(ep);
    while (cluster) {
        if (cluster::get_flags(cluster) & CLUSTER_FLAG_CLIENT) {
            ReturnErrorOnFailure(builder.Append(cluster::get_id(cluster)));
        }
        cluster = cluster::get_next(cluster);
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR provider::ServerClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ServerClusterEntry> &builder)
{
    Status status = CheckDataModelPath(endpointId);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    endpoint_t *ep = endpoint::get(endpointId);
    size_t count = endpoint::get_cluster_count(endpointId, chip::kInvalidClusterId, CLUSTER_FLAG_SERVER);
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(count));
    cluster_t *cluster = cluster::get_first(ep);
    while (cluster) {
        if (cluster::get_flags(cluster) & CLUSTER_FLAG_SERVER) {
            ServerClusterEntry entry;
            entry.clusterId = cluster::get_id(cluster);
            if (auto *server_cluster = mRegistry.Get(ConcreteClusterPath(endpointId, entry.clusterId)); server_cluster != nullptr) {
                entry.flags = server_cluster->GetClusterFlags(ConcreteClusterPath(endpointId, entry.clusterId));
                entry.dataVersion = server_cluster->GetDataVersion(ConcreteClusterPath(endpointId, entry.clusterId));
            } else {
                entry.flags.ClearAll();
                VerifyOrReturnError(cluster::get_data_version(cluster, entry.dataVersion) == ESP_OK, CHIP_ERROR_INTERNAL);
            }
            ReturnErrorOnFailure(builder.Append(entry));
        }
        cluster = cluster::get_next(cluster);
    }

    return CHIP_NO_ERROR;
}

#if CHIP_CONFIG_USE_ENDPOINT_UNIQUE_ID
CHIP_ERROR provider::EndpointUniqueID(EndpointId endpointId, MutableCharSpan &epUniqueId)
{
    // TODO: Support Endpoint Unique ID
    return CHIP_NO_ERROR;
}
#endif

CHIP_ERROR provider::EventInfo(const ConcreteEventPath &path, EventEntry &eventInfo)
{
    if (auto *cluster = mRegistry.Get(path); cluster != nullptr) {
        return cluster->EventInfo(path, eventInfo);
    }
    Status status = CheckDataModelPath(path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    eventInfo.readPrivilege = MatterGetAccessPrivilegeForReadEvent(path.mClusterId, path.mEventId);
    return CHIP_NO_ERROR;
}

CHIP_ERROR provider::GeneratedCommands(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<CommandId> &builder)
{
    if (auto *cluster = mRegistry.Get(path); cluster != nullptr) {
        return cluster->GeneratedCommands(path, builder);
    }
    Status status = CheckDataModelPath(path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    cluster_t *cluster = cluster::get(path.mEndpointId, path.mClusterId);
    size_t count = get_command_count(cluster, COMMAND_FLAG_GENERATED);
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(count));
    command_t *command = command::get_first(cluster);
    while (command) {
        if (command::get_flags(command) & COMMAND_FLAG_GENERATED) {
            ReturnErrorOnFailure(builder.Append(command::get_id(command)));
        }
        command = command::get_next(command);
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR provider::AcceptedCommands(const ConcreteClusterPath &path,
                                      ReadOnlyBufferBuilder<AcceptedCommandEntry> &builder)
{
    if (auto *cluster = mRegistry.Get(path); cluster != nullptr) {
        return cluster->AcceptedCommands(path, builder);
    }
    Status status = CheckDataModelPath(path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    CommandHandlerInterface *interface =
        CommandHandlerInterfaceRegistry::Instance().GetCommandHandler(path.mEndpointId, path.mClusterId);
    if (interface != nullptr) {
        CHIP_ERROR err = interface->RetrieveAcceptedCommands(path, builder);
        // If retrieving the accepted commands returns CHIP_ERROR_NOT_IMPLEMENTED then continue with normal processing.
        // Otherwise we finished.
        VerifyOrReturnError(err == CHIP_ERROR_NOT_IMPLEMENTED, err);
    }
    // If we cannot get AcceptedCommands array from CommandHandlerinterface, get it from esp_matter data model.
    cluster_t *cluster = cluster::get(path.mEndpointId, path.mClusterId);
    size_t count = get_command_count(cluster, COMMAND_FLAG_ACCEPTED);
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(count));
    command_t *command = command::get_first(cluster);
    while (command) {
        if (command::get_flags(command) & COMMAND_FLAG_ACCEPTED) {
            uint32_t command_id = command::get_id(command);
            BitMask<DataModel::CommandQualityFlags> quality_flags;
            quality_flags
                .Set(DataModel::CommandQualityFlags::kFabricScoped, CommandIsFabricScoped(path.mClusterId, command_id))
                .Set(DataModel::CommandQualityFlags::kTimed, CommandNeedsTimedInvoke(path.mClusterId, command_id))
                .Set(DataModel::CommandQualityFlags::kLargeMessage,
                     CommandHasLargePayload(path.mClusterId, command_id));
            AcceptedCommandEntry entry(command_id, quality_flags,
                                       MatterGetAccessPrivilegeForInvokeCommand(path.mClusterId, command_id));
            ReturnErrorOnFailure(builder.Append(entry));
        }
        command = command::get_next(command);
    }
    return CHIP_NO_ERROR;
}

static constexpr AttributeId k_global_attributes_not_in_metadata[] = {
    Clusters::Globals::Attributes::AttributeList::Id, Clusters::Globals::Attributes::AcceptedCommandList::Id,
    Clusters::Globals::Attributes::GeneratedCommandList::Id};

static constexpr size_t k_global_attributes_count =
    sizeof(k_global_attributes_not_in_metadata) / sizeof(k_global_attributes_not_in_metadata[0]);

CHIP_ERROR provider::Attributes(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<AttributeEntry> &builder)
{
    if (auto *cluster = mRegistry.Get(path); cluster != nullptr) {
        return cluster->Attributes(path, builder);
    }
    Status status = CheckDataModelPath(path);
    VerifyOrReturnValue(status == Protocols::InteractionModel::Status::Success,
                        CHIP_ERROR_IM_GLOBAL_STATUS_VALUE(status));
    cluster_t *cluster = cluster::get(path.mEndpointId, path.mClusterId);
    size_t count = get_attribute_count(cluster);
    // There are three attributes(Attributes, AcceptedCommands, and GeneratedCommands) which are not
    // in esp_matter data model metadata;
    ReturnErrorOnFailure(builder.EnsureAppendCapacity(count + k_global_attributes_count));
    attribute_t *attribute = attribute::get_first(cluster);
    while (attribute) {
        uint32_t id = attribute::get_id(attribute);
        uint16_t flags = attribute::get_flags(attribute);
        chip::BitFlags<DataModel::AttributeQualityFlags> attr_quality_flags;
        // TODO Array
        attr_quality_flags.Set(DataModel::AttributeQualityFlags::kTimed, flags & ATTRIBUTE_FLAG_MUST_USE_TIMED_WRITE);
        chip::Access::Privilege read_privilege = MatterGetAccessPrivilegeForReadAttribute(path.mClusterId, id);
        auto write_privilege = (flags & ATTRIBUTE_FLAG_WRITABLE)
            ? std::make_optional(MatterGetAccessPrivilegeForWriteAttribute(path.mClusterId, id))
            : std::nullopt;
        AttributeEntry entry(id, attr_quality_flags, read_privilege, write_privilege);
        ReturnErrorOnFailure(builder.Append(entry));
        attribute = attribute::get_next(attribute);
    }
    // Append the three Global attributes
    for (size_t index = 0; index < k_global_attributes_count; ++index)
    {
        AttributeEntry entry(k_global_attributes_not_in_metadata[index],
                         chip::BitFlags<DataModel::AttributeQualityFlags>(), chip::Access::Privilege::kView,
                         std::nullopt);
        ReturnErrorOnFailure(builder.Append(entry));
    }

    return CHIP_NO_ERROR;
}

void provider::Temporary_ReportAttributeChanged(const AttributePathParams &path)
{
    cluster_t *cluster = cluster::get(path.mEndpointId, path.mClusterId);
    VerifyOrReturn(cluster != nullptr);
    VerifyOrReturn(cluster::increase_data_version(cluster) == ESP_OK);
    mContext->dataModelChangeListener.MarkDirty(path);
}

Status provider::CheckDataModelPath(EndpointId endpointId)
{
    endpoint_t *endpoint = endpoint::get(endpointId);
    VerifyOrReturnValue(endpoint != nullptr, Protocols::InteractionModel::Status::UnsupportedEndpoint);
    return Protocols::InteractionModel::Status::Success;
}

Status provider::CheckDataModelPath(const ConcreteClusterPath &path)
{
    endpoint_t *endpoint = endpoint::get(path.mEndpointId);
    VerifyOrReturnValue(endpoint, Protocols::InteractionModel::Status::UnsupportedEndpoint);
    cluster_t *cluster = cluster::get(path.mEndpointId, path.mClusterId);
    VerifyOrReturnValue(cluster != nullptr, Protocols::InteractionModel::Status::UnsupportedCluster);
    return Protocols::InteractionModel::Status::Success;
}

Status provider::CheckDataModelPath(const ConcreteAttributePath &path)
{
    endpoint_t *endpoint = endpoint::get(path.mEndpointId);
    VerifyOrReturnValue(endpoint, Protocols::InteractionModel::Status::UnsupportedEndpoint);
    cluster_t *cluster = cluster::get(endpoint, path.mClusterId);
    VerifyOrReturnValue(cluster != nullptr, Protocols::InteractionModel::Status::UnsupportedCluster);
    attribute_t *attribute = attribute::get(cluster, path.mAttributeId);
    VerifyOrReturnValue(attribute != nullptr, Protocols::InteractionModel::Status::UnsupportedAttribute);
    return Protocols::InteractionModel::Status::Success;
}

Status provider::CheckDataModelPath(const ConcreteEventPath &path)
{
    endpoint_t *endpoint = endpoint::get(path.mEndpointId);
    VerifyOrReturnValue(endpoint, Protocols::InteractionModel::Status::UnsupportedEndpoint);
    cluster_t *cluster = cluster::get(endpoint, path.mClusterId);
    VerifyOrReturnValue(cluster != nullptr, Protocols::InteractionModel::Status::UnsupportedCluster);
    event_t *event = event::get(cluster, path.mEventId);
    VerifyOrReturnValue(event != nullptr, Protocols::InteractionModel::Status::UnsupportedEvent);
    return Protocols::InteractionModel::Status::Success;
}

Status provider::CheckDataModelPath(const chip::app::ConcreteCommandPath path)
{
    endpoint_t *endpoint = endpoint::get(path.mEndpointId);
    VerifyOrReturnValue(endpoint, Protocols::InteractionModel::Status::UnsupportedEndpoint);
    cluster_t *cluster = cluster::get(endpoint, path.mClusterId);
    VerifyOrReturnValue(cluster != nullptr, Protocols::InteractionModel::Status::UnsupportedCluster);
    command_t *command = command::get(cluster, path.mCommandId, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_GENERATED);
    VerifyOrReturnValue(command != nullptr, Protocols::InteractionModel::Status::UnsupportedCommand);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace data_model
} // namespace esp_matter
