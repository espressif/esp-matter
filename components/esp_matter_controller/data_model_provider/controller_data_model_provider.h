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
#include <app/AttributeValueEncoder.h>
#include <app/CommandHandler.h>
#include <app/ConcreteAttributePath.h>
#include <app/ConcreteClusterPath.h>
#include <app/ConcreteCommandPath.h>
#include <app/ConcreteEventPath.h>
#include <app/data-model-provider/ActionReturnStatus.h>
#include <app/data-model-provider/MetadataTypes.h>
#include <app/data-model-provider/OperationTypes.h>
#include <app/data-model-provider/Provider.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <optional>

using chip::ClusterId;
using chip::CommandId;
using chip::EndpointId;
using chip::ReadOnlyBufferBuilder;
using chip::app::AttributePathParams;
using chip::app::AttributeValueDecoder;
using chip::app::AttributeValueEncoder;
using chip::app::CommandHandler;
using chip::app::ConcreteAttributePath;
using chip::app::ConcreteClusterPath;
using chip::app::ConcreteCommandPath;
using chip::app::ConcreteEventPath;
using chip::app::DataModel::AcceptedCommandEntry;
using chip::app::DataModel::ActionReturnStatus;
using chip::app::DataModel::AttributeEntry;
using chip::app::DataModel::ClusterInfo;
using chip::app::DataModel::DeviceTypeEntry;
using chip::app::DataModel::EndpointEntry;
using chip::app::DataModel::EventEntry;
using chip::app::DataModel::InvokeRequest;
using chip::app::DataModel::ListWriteOperation;
using chip::app::DataModel::ReadAttributeRequest;
using chip::app::DataModel::ServerClusterEntry;
using chip::app::DataModel::WriteAttributeRequest;
using chip::TLV::TLVReader;

namespace esp_matter {
namespace controller {
namespace data_model {

// TODO: The client-only controller has no data model, use an empty data model provider for it.
//       We should finish the functions after enabling dynamic server for the controller.
class provider : public chip::app::DataModel::Provider {
public:
    static provider &get_instance()
    {
        static provider instance;
        return instance;
    }

    CHIP_ERROR Shutdown() override { return CHIP_NO_ERROR; }

    ActionReturnStatus ReadAttribute(const ReadAttributeRequest &request, AttributeValueEncoder &encoder) override
    {
        return CHIP_NO_ERROR;
    }

    ActionReturnStatus WriteAttribute(const WriteAttributeRequest &request, AttributeValueDecoder &decoder) override
    {
        return CHIP_NO_ERROR;
    }

    void ListAttributeWriteNotification(const ConcreteAttributePath &aPath, ListWriteOperation opType) override {}

    std::optional<ActionReturnStatus> InvokeCommand(const InvokeRequest &request, TLVReader &input_arguments,
                                                    CommandHandler *handler) override
    {
        return std::nullopt;
    }

    // ProviderMetadataTree overrides
    CHIP_ERROR Endpoints(ReadOnlyBufferBuilder<EndpointEntry> &builder) override { return CHIP_NO_ERROR; }
    CHIP_ERROR SemanticTags(EndpointId endpointId, ReadOnlyBufferBuilder<SemanticTag> &builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR DeviceTypes(EndpointId endpointId, ReadOnlyBufferBuilder<DeviceTypeEntry> &builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR ClientClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ClusterId> &builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR ServerClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ServerClusterEntry> &builder) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Attributes(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<AttributeEntry> &builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<CommandId> &builder) override
    {
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath &path,
                                ReadOnlyBufferBuilder<AcceptedCommandEntry> &builder) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR EventInfo(const ConcreteEventPath & path, EventEntry & eventInfo) override
    {
        return CHIP_NO_ERROR;
    }
    void Temporary_ReportAttributeChanged(const AttributePathParams &path) override {}

private:
    provider() = default;
    ~provider() = default;
};

} // namespace data_model
} // namespace controller
} // namespace esp_matter
