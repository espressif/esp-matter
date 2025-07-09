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

#pragma once

#include <app/AttributePathParams.h>
#include <app/AttributeValueDecoder.h>
#include <app/CommandHandler.h>
#include <app/ConcreteAttributePath.h>
#include <app/ConcreteClusterPath.h>
#include <app/ConcreteCommandPath.h>
#include <app/ConcreteEventPath.h>
#include <app/data-model-provider/ActionReturnStatus.h>
#include <app/data-model-provider/Context.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/data-model-provider/OperationTypes.h>
#include <app/data-model-provider/Provider.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <app/server/Server.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <lib/support/Span.h>
#include <protocols/interaction_model/StatusCode.h>

namespace esp_matter {
namespace data_model {

using chip::ClusterId;
using chip::CommandId;
using chip::EndpointId;
using chip::MutableCharSpan;
using chip::ReadOnlyBufferBuilder;
using chip::app::AttributePathParams;
using chip::app::AttributeValueDecoder;
using chip::app::AttributeValueEncoder;
using chip::app::CommandHandler;
using chip::app::ConcreteAttributePath;
using chip::app::ConcreteClusterPath;
using chip::app::ConcreteEventPath;
using chip::app::DataModel::AcceptedCommandEntry;
using chip::app::DataModel::ActionReturnStatus;
using chip::app::DataModel::AttributeEntry;
using chip::app::DataModel::DeviceTypeEntry;
using chip::app::DataModel::EndpointEntry;
using chip::app::DataModel::EventEntry;
using chip::app::DataModel::InteractionModelContext;
using chip::app::DataModel::InvokeRequest;
using chip::app::DataModel::ListWriteOperation;
using chip::app::DataModel::ReadAttributeRequest;
using chip::app::DataModel::ServerClusterEntry;
using chip::app::DataModel::WriteAttributeRequest;
using chip::Protocols::InteractionModel::Status;

class provider : public chip::app::DataModel::Provider {
public:
    // access to the typed global singleton of this class.
    static provider &get_instance();

    chip::app::ServerClusterInterfaceRegistry &registry() { return m_registry; }

    /// Generic model implementations
    CHIP_ERROR Startup(InteractionModelContext context) override;
    CHIP_ERROR Shutdown() override;

    ActionReturnStatus ReadAttribute(const ReadAttributeRequest &request, AttributeValueEncoder &encoder) override;
    ActionReturnStatus WriteAttribute(const WriteAttributeRequest &request, AttributeValueDecoder &decoder) override;

    void ListAttributeWriteNotification(const ConcreteAttributePath &aPath, ListWriteOperation opType) override;
    std::optional<ActionReturnStatus> InvokeCommand(const InvokeRequest &request, chip::TLV::TLVReader &input_arguments,
                                                    CommandHandler *handler) override;

    /// attribute tree iteration
    CHIP_ERROR Endpoints(ReadOnlyBufferBuilder<EndpointEntry> &out) override;
    CHIP_ERROR SemanticTags(EndpointId endpointId, ReadOnlyBufferBuilder<SemanticTag> &builder) override;
    CHIP_ERROR DeviceTypes(EndpointId endpointId, ReadOnlyBufferBuilder<DeviceTypeEntry> &builder) override;
    CHIP_ERROR ClientClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ClusterId> &builder) override;
    CHIP_ERROR ServerClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ServerClusterEntry> &builder) override;
#if CHIP_CONFIG_USE_ENDPOINT_UNIQUE_ID
    CHIP_ERROR EndpointUniqueID(EndpointId endpointId, MutableCharSpan &epUniqueId) override;
#endif
    CHIP_ERROR EventInfo(const ConcreteEventPath &path, EventEntry &eventInfo) override;
    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<CommandId> &builder) override;
    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath &path,
                                ReadOnlyBufferBuilder<AcceptedCommandEntry> &builder) override;
    CHIP_ERROR Attributes(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<AttributeEntry> &builder) override;

    void Temporary_ReportAttributeChanged(const AttributePathParams &path) override;

private:
    Status CheckDataModelPath(EndpointId endpointId);
    Status CheckDataModelPath(const ConcreteClusterPath &path);
    Status CheckDataModelPath(const ConcreteAttributePath &path);
    Status CheckDataModelPath(const ConcreteEventPath &path);
    Status CheckDataModelPath(const chip::app::ConcreteCommandPath path);

    chip::app::ServerClusterInterfaceRegistry m_registry;
};

} // namespace data_model
} // namespace esp_matter
