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

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>
#include <app/CommandHandler.h>
#include <app/ConcreteCommandPath.h>
#include <app/InteractionModelEngine.h>
#include <app/clusters/identify-server/IdentifyCluster.h>
#include <app/clusters/identify-server/identify-server.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model_priv.h>
#include <esp_matter_identify.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/TextOnlyLogging.h>
#include <lib/support/TimerDelegate.h>
#include <platform/DefaultTimerDelegate.h>
#include <tracing/macros.h>
#include "integration.h"

namespace {

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::Identify;

using chip::app::Clusters::IdentifyCluster;
using chip::app::Clusters::IdentifyDelegate;
using chip::Protocols::InteractionModel::Status;

Identify *firstLegacyIdentify = nullptr;
DefaultTimerDelegate sDefaultTimerDelegate;

Identify *GetLegacyIdentifyInstance(EndpointId endpoint)
{
    Identify *current = firstLegacyIdentify;
    while (current != nullptr && current->mCluster.Cluster().GetPaths()[0].mEndpointId != endpoint) {
        current = current->nextIdentify;
    }
    return current;
}

class IdentifyLegacyDelegate : public IdentifyDelegate {
public:
    void OnIdentifyStart(IdentifyCluster &cluster) override
    {
        Identify *identify = GetLegacyIdentifyInstance(cluster.GetPaths()[0].mEndpointId);
        if (identify != nullptr) {
            identify->mActive = true;
            if (identify->mOnIdentifyStart) {
                identify->mOnIdentifyStart(identify);
            }
        }
    }
    void OnIdentifyStop(IdentifyCluster &cluster) override
    {
        Identify *identify = GetLegacyIdentifyInstance(cluster.GetPaths()[0].mEndpointId);
        if (identify != nullptr) {
            identify->mActive = false;
            if (identify->mOnIdentifyStop) {
                identify->mOnIdentifyStop(identify);
            }
        }
    }
    void OnTriggerEffect(IdentifyCluster &cluster) override
    {
        Identify *identify = GetLegacyIdentifyInstance(cluster.GetPaths()[0].mEndpointId);
        if (identify != nullptr) {
            identify->mCurrentEffectIdentifier = cluster.GetEffectIdentifier();
            identify->mEffectVariant = cluster.GetEffectVariant();
            if (identify->mOnEffectIdentifier) {
                identify->mOnEffectIdentifier(identify);
            }
        }
    }
    bool IsTriggerEffectEnabled() const override
    {
        return true;
    }
};

IdentifyLegacyDelegate gLegacyDelegate;

inline void RegisterLegacyIdentify(Identify *inst)
{
    inst->nextIdentify = firstLegacyIdentify;
    firstLegacyIdentify = inst;
}

inline void UnregisterLegacyIdentify(Identify *inst)
{
    if (firstLegacyIdentify == inst) {
        firstLegacyIdentify = firstLegacyIdentify->nextIdentify;
    } else if (firstLegacyIdentify != nullptr) {
        Identify *previous = firstLegacyIdentify;
        Identify *current = firstLegacyIdentify->nextIdentify;

        while (current != nullptr && current != inst) {
            previous = current;
            current = current->nextIdentify;
        }

        if (current != nullptr) {
            previous->nextIdentify = current->nextIdentify;
        }
    }
}

} // namespace

IdentifyCluster *FindIdentifyClusterOnEndpoint(EndpointId endpointId)
{
    Identify *legacyInstance = GetLegacyIdentifyInstance(endpointId);
    if (legacyInstance != nullptr) {
        return &legacyInstance->mCluster.Cluster();
    }
    return nullptr;
}

Identify::Identify(EndpointId endpoint, onIdentifyStartCb onIdentifyStart, onIdentifyStopCb onIdentifyStop,
                   IdentifyTypeEnum identifyType, onEffectIdentifierCb onEffectIdentifier,
                   EffectIdentifierEnum effectIdentifier, EffectVariantEnum effectVariant,
                   chip::TimerDelegate *timerDelegate)
    :

    mOnIdentifyStart(onIdentifyStart)
    , mOnIdentifyStop(onIdentifyStop)
    , mIdentifyType(identifyType)
    , mOnEffectIdentifier(onEffectIdentifier)
    , mCurrentEffectIdentifier(effectIdentifier)
    , mEffectVariant(effectVariant)
    , mCluster(
        chip::app::Clusters::IdentifyCluster::Config(endpoint, timerDelegate ? * timerDelegate : sDefaultTimerDelegate)
        .WithIdentifyType(identifyType)
        .WithDelegate(&gLegacyDelegate)
        .WithEffectIdentifier(effectIdentifier)
        .WithEffectVariant(effectVariant))
{
    RegisterLegacyIdentify(this);

    if (esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration()) !=
            CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to registry Identify cluster registration for endpoint %u", endpoint);
    }
};

Identify::~Identify()
{
    if (esp_matter::data_model::provider::get_instance().registry().Unregister(&(mCluster.Cluster())) !=
            CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregistry Identify cluster registration");
    }
    UnregisterLegacyIdentify(this);
}

void ESPMatterIdentifyClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(GetLegacyIdentifyInstance(endpointId) == nullptr);

    esp_matter::attribute_t *identifyTypeAttr = esp_matter::attribute::get(
                                                    endpointId, Id, Attributes::IdentifyType::Id);
    VerifyOrReturn(identifyTypeAttr != nullptr,
                   ChipLogError(AppServer, "Failed to get IdentifyType attribute for endpoint %u", endpointId));

    esp_matter_attr_val_t identifyType = esp_matter_invalid(nullptr);
    VerifyOrReturn(esp_matter::attribute::get_val_internal(identifyTypeAttr, &identifyType) == ESP_OK,
                   ChipLogError(AppServer, "Failed to get IdentifyType value for endpoint %u", endpointId));
    VerifyOrReturn(esp_matter::identification::init(endpointId, identifyType.val.u8) == ESP_OK,
                   ChipLogError(AppServer, "Failed to initialize Identify cluster for endpoint %u", endpointId));
}

void ESPMatterIdentifyClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(shutdownType == ClusterShutdownType::kClusterShutdown);

    Identify *identify = GetLegacyIdentifyInstance(endpointId);
    if (identify != nullptr) {
        chip::Platform::Delete(identify);
        esp_matter::endpoint::set_identify(endpointId, nullptr);
    }
}

// Legacy PluginServer callback stubs
void MatterIdentifyPluginServerInitCallback() {}
void MatterIdentifyPluginServerShutdownCallback() {}

namespace chip::app::Clusters::Identify {
IdentifyCluster * FindClusterOnEndpoint(EndpointId endpointId)
{
    return FindIdentifyClusterOnEndpoint(endpointId);
}
}
