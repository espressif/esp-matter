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
#include <app/DefaultTimerDelegate.h>
#include <app/InteractionModelEngine.h>
#include <app/clusters/identify-server/IdentifyCluster.h>
#include <app/clusters/identify-server/identify-server.h>
#include <app/server-cluster/DefaultServerCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/TimerDelegate.h>
#include <tracing/macros.h>
#include "support/logging/TextOnlyLogging.h"

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
    explicit IdentifyLegacyDelegate(EndpointId endpoint)
        : mEndpoint(endpoint)
    {
    }

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
        return esp_matter::command::get(mEndpoint, Id, Commands::TriggerEffect::Id) != nullptr;
    }

    EndpointId mEndpoint;
    IdentifyLegacyDelegate *mNext = nullptr;
};

IdentifyLegacyDelegate *sIdentifyDelegateList = nullptr;

IdentifyLegacyDelegate *GetOrCreateIdentifyDelegate(EndpointId endpoint)
{
    for (IdentifyLegacyDelegate *delegate = sIdentifyDelegateList; delegate != nullptr; delegate = delegate->mNext) {
        if (delegate->mEndpoint == endpoint) {
            return delegate;
        }
    }
    IdentifyLegacyDelegate *delegate = chip::Platform::New<IdentifyLegacyDelegate>(endpoint);
    if (delegate != nullptr) {
        delegate->mNext = sIdentifyDelegateList;
        sIdentifyDelegateList = delegate;
    }
    return delegate;
}

void FreeIdentifyDelegate(EndpointId endpoint)
{
    IdentifyLegacyDelegate *previous = nullptr;
    for (IdentifyLegacyDelegate *delegate = sIdentifyDelegateList; delegate != nullptr;
         previous = delegate, delegate = delegate->mNext) {
        if (delegate->mEndpoint == endpoint) {
            if (previous == nullptr) {
                sIdentifyDelegateList = delegate->mNext;
            } else {
                previous->mNext = delegate->mNext;
            }
            chip::Platform::Delete(delegate);
            return;
        }
    }
}

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
          chip::app::Clusters::IdentifyCluster::Config(endpoint, timerDelegate ? *timerDelegate : sDefaultTimerDelegate)
              .WithIdentifyType(identifyType)
              .WithDelegate(GetOrCreateIdentifyDelegate(endpoint))
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
    FreeIdentifyDelegate(mCluster.Cluster().GetPaths()[0].mEndpointId);
    UnregisterLegacyIdentify(this);
}

void ESPMatterIdentifyClusterServerInitCallback(EndpointId endpointId)
{
    // Intentionally make this function empty as the identify cluster will be registered when enabling endpoint.
}

void ESPMatterIdentifyClusterServerShutdownCallback(EndpointId endpointId)
{
    // Intentionally make this function empty as the identify cluster will be unregistered when disabling endpoint.
}

// Legacy PluginServer callback stubs
void MatterIdentifyPluginServerInitCallback()
{
}
void MatterIdentifyPluginServerShutdownCallback()
{
}
