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
#include <crypto/RandUtils.h>
#include <lib/core/CHIPError.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/ReadOnlyBuffer.h>
#include <optional>
#include <app/ConcreteEventPath.h>

#ifdef CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED
#include <app/clusters/ota-provider/ota-provider-cluster.h>
#endif

using chip::ClusterId;
using chip::CommandId;
using chip::EndpointId;
using chip::FabricIndex;
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

class provider : public chip::app::DataModel::Provider {
public:
    static provider &get_instance()
    {
        static provider instance;
        return instance;
    }

    CHIP_ERROR Shutdown() override
    {
        return CHIP_NO_ERROR;
    }

    ActionReturnStatus ReadAttribute(const ReadAttributeRequest &request, AttributeValueEncoder &encoder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (request.path.mEndpointId != chip::kRootEndpointId) {
            return chip::Protocols::InteractionModel::Status::UnsupportedEndpoint;
        }
        if (request.path.mClusterId != chip::app::Clusters::OtaSoftwareUpdateProvider::Id) {
            return chip::Protocols::InteractionModel::Status::UnsupportedCluster;
        }
        return mOtaProviderServer.ReadAttribute(request, encoder);
#else
        return chip::Protocols::InteractionModel::Status::InvalidAction;
#endif
    }

    ActionReturnStatus WriteAttribute(const WriteAttributeRequest &request, AttributeValueDecoder &decoder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (request.path.mEndpointId != chip::kRootEndpointId) {
            return chip::Protocols::InteractionModel::Status::UnsupportedEndpoint;
        }
        if (request.path.mClusterId != chip::app::Clusters::OtaSoftwareUpdateProvider::Id) {
            return chip::Protocols::InteractionModel::Status::UnsupportedCluster;
        }
        return chip::Protocols::InteractionModel::Status::UnsupportedWrite;
#else
        return chip::Protocols::InteractionModel::Status::InvalidAction;
#endif
    }

    void ListAttributeWriteNotification(const ConcreteAttributePath &aPath, ListWriteOperation opType,
                                        FabricIndex accessFabric) override {}

    std::optional<ActionReturnStatus> InvokeCommand(const InvokeRequest &request, TLVReader &input_arguments,
                                                    CommandHandler *handler) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (request.path.mEndpointId != chip::kRootEndpointId) {
            return chip::Protocols::InteractionModel::Status::UnsupportedEndpoint;
        }
        if (request.path.mClusterId != chip::app::Clusters::OtaSoftwareUpdateProvider::Id) {
            return chip::Protocols::InteractionModel::Status::UnsupportedCluster;
        }
        return mOtaProviderServer.InvokeCommand(request, input_arguments, handler);
#else
        return chip::Protocols::InteractionModel::Status::InvalidAction;
#endif
    }

    // ProviderMetadataTree overrides
    CHIP_ERROR Endpoints(ReadOnlyBufferBuilder<EndpointEntry> &builder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        ReturnErrorOnFailure(builder.EnsureAppendCapacity(1));
        EndpointEntry entry;
        entry.id = chip::kRootEndpointId;
        entry.compositionPattern = chip::app::DataModel::EndpointCompositionPattern::kFullFamily;
        entry.parentId = chip::kInvalidEndpointId;
        return builder.Append(entry);
#else
        return CHIP_NO_ERROR;
#endif
    }

    CHIP_ERROR DeviceTypes(EndpointId endpointId, ReadOnlyBufferBuilder<DeviceTypeEntry> &builder) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR ClientClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ClusterId> &builder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (endpointId != chip::kRootEndpointId) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
        }

        ReturnErrorOnFailure(builder.EnsureAppendCapacity(1));
        return builder.Append(chip::app::Clusters::OtaSoftwareUpdateRequestor::Id);
#else
        return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
#endif
    }

    CHIP_ERROR ServerClusters(EndpointId endpointId, ReadOnlyBufferBuilder<ServerClusterEntry> &builder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (endpointId != chip::kRootEndpointId) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
        }

        ReturnErrorOnFailure(builder.EnsureAppendCapacity(1));
        ServerClusterEntry entry;
        entry.clusterId = chip::app::Clusters::OtaSoftwareUpdateProvider::Id;
        entry.dataVersion = mDataVersion;
        entry.flags.ClearAll();
        return builder.Append(entry);
#else
        return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
#endif
    }

    CHIP_ERROR EventInfo(const ConcreteEventPath  &path, EventEntry  &eventInfo) override
    {
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Attributes(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<AttributeEntry> &builder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (path.mEndpointId != chip::kRootEndpointId) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
        }
        if (path.mClusterId != chip::app::Clusters::OtaSoftwareUpdateProvider::Id) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedCluster);
        }
        return mOtaProviderServer.Attributes(path, builder);
#else
        return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
#endif
    }

    CHIP_ERROR GeneratedCommands(const ConcreteClusterPath &path, ReadOnlyBufferBuilder<CommandId> &builder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (path.mEndpointId != chip::kRootEndpointId) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
        }
        if (path.mClusterId != chip::app::Clusters::OtaSoftwareUpdateProvider::Id) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedCluster);
        }
        return mOtaProviderServer.GeneratedCommands(path, builder);
#else
        return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
#endif
    }

    CHIP_ERROR AcceptedCommands(const ConcreteClusterPath &path,
                                ReadOnlyBufferBuilder<AcceptedCommandEntry> &builder) override
    {
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
        if (path.mEndpointId != chip::kRootEndpointId) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
        }
        if (path.mClusterId != chip::app::Clusters::OtaSoftwareUpdateProvider::Id) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedCluster);
        }
        return mOtaProviderServer.AcceptedCommands(path, builder);
#else
        return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
#endif
    }

    void Temporary_ReportAttributeChanged(const AttributePathParams &path) override {}
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
    void set_ota_provider_delegate(chip::app::Clusters::OTAProviderDelegate * delegate)
    {
        mOtaProviderServer.SetDelegate(delegate);
    }
#endif

private:
#if defined(CONFIG_ESP_MATTER_OTA_PROVIDER_ENABLED) && CHIP_DEVICE_CONFIG_DYNAMIC_SERVER
    chip::app::Clusters::OtaProviderServer mOtaProviderServer;
    chip::DataVersion mDataVersion;
    provider()
        : mOtaProviderServer(chip::kRootEndpointId)
    {
        mDataVersion = chip::Crypto::GetRandU32();
    }
#else
    provider() = default;
#endif
    ~provider() = default;
};

} // namespace data_model
} // namespace controller
} // namespace esp_matter
