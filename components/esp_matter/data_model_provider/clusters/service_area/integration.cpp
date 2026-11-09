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

#include "integration.h"

#include <app/ClusterCallbacks.h>
#include <clusters/ServiceArea/Attributes.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model/esp_matter_endpoint.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ServiceArea;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, StorageDelegate *> gStorageDelegates;

ServiceAreaCluster::OptionalAttributeSet BuildOptionalAttributes(EndpointId endpointId)
{
    ServiceAreaCluster::OptionalAttributeSet optionalAttributes;
    if (endpoint::is_attribute_enabled(endpointId, ServiceArea::Id, Attributes::SupportedMaps::Id)) {
        optionalAttributes.template ForceSet<Attributes::SupportedMaps::Id>();
    }
    if (endpoint::is_attribute_enabled(endpointId, ServiceArea::Id, Attributes::CurrentArea::Id)) {
        optionalAttributes.template ForceSet<Attributes::CurrentArea::Id>();
    }
    if (endpoint::is_attribute_enabled(endpointId, ServiceArea::Id, Attributes::EstimatedEndTime::Id)) {
        optionalAttributes.template ForceSet<Attributes::EstimatedEndTime::Id>();
    }
    if (endpoint::is_attribute_enabled(endpointId, ServiceArea::Id, Attributes::Progress::Id)) {
        optionalAttributes.template ForceSet<Attributes::Progress::Id>();
    }
    return optionalAttributes;
}

} // namespace

namespace chip {
namespace app {
namespace Clusters {
namespace ServiceArea {

void SetStorageDelegate(EndpointId endpointId, StorageDelegate * storageDelegate)
{
    gStorageDelegates[endpointId] = storageDelegate;
}

StorageDelegate * GetStorageDelegate(EndpointId endpointId)
{
    auto it = gStorageDelegates.find(endpointId);
    return it == gStorageDelegates.end() ? nullptr : it->second;
}

// Same as upstream CodegenIntegration.cpp, but registered with the esp-matter data model provider.
Instance::Instance(StorageDelegate * storageDelegate, Delegate * aDelegate, EndpointId aEndpointId,
                   BitMask<ServiceArea::Feature> aFeature) :
    mStorageDelegate(storageDelegate),
    mDelegate(aDelegate), mCluster(aEndpointId, *storageDelegate, *aDelegate, aFeature, BuildOptionalAttributes(aEndpointId))
{
    aDelegate->SetInstance(this);
}

Instance::~Instance()
{
    if (mRegistered) {
        LogErrorOnFailure(
            esp_matter::data_model::provider::get_instance().registry().Unregister(&mCluster.Cluster()));
        mRegistered = false;
    }
}

CHIP_ERROR Instance::Init()
{
    EndpointId endpointId = mCluster.Cluster().GetPaths()[0].mEndpointId;
    VerifyOrReturnError(cluster::get(endpointId, ServiceArea::Id) != nullptr, CHIP_ERROR_INVALID_ARGUMENT,
                        ChipLogError(Zcl, "Service Area: cluster not present on endpoint %u", endpointId));

    VerifyOrReturnError(!mRegistered, CHIP_NO_ERROR);
    ReturnErrorOnFailure(mStorageDelegate->Init());
    ReturnErrorOnFailure(mDelegate->Init());
    ReturnErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration()));
    mRegistered = true;
    return CHIP_NO_ERROR;
}

uint32_t Instance::GetNumberOfSupportedAreas()
{
    return mCluster.Cluster().GetNumberOfSupportedAreas();
}

bool Instance::GetSupportedAreaByIndex(uint32_t listIndex, AreaStructureWrapper  &aSupportedArea)
{
    return mCluster.Cluster().GetSupportedAreaByIndex(listIndex, aSupportedArea);
}

bool Instance::GetSupportedAreaById(uint32_t aAreaId, uint32_t  &listIndex, AreaStructureWrapper  &aSupportedArea)
{
    return mCluster.Cluster().GetSupportedAreaById(aAreaId, listIndex, aSupportedArea);
}

bool Instance::AddSupportedArea(AreaStructureWrapper  &aNewArea)
{
    return mCluster.Cluster().AddSupportedArea(aNewArea);
}

bool Instance::ModifySupportedArea(AreaStructureWrapper  &aNewArea)
{
    return mCluster.Cluster().ModifySupportedArea(aNewArea);
}

bool Instance::ClearSupportedAreas()
{
    return mCluster.Cluster().ClearSupportedAreas();
}

bool Instance::RemoveSupportedArea(uint32_t areaId)
{
    return mCluster.Cluster().RemoveSupportedArea(areaId);
}

uint32_t Instance::GetNumberOfSupportedMaps()
{
    return mCluster.Cluster().GetNumberOfSupportedMaps();
}

bool Instance::GetSupportedMapByIndex(uint32_t listIndex, MapStructureWrapper  &aSupportedMap)
{
    return mCluster.Cluster().GetSupportedMapByIndex(listIndex, aSupportedMap);
}

bool Instance::GetSupportedMapById(uint32_t aMapId, uint32_t  &listIndex, MapStructureWrapper  &aSupportedMap)
{
    return mCluster.Cluster().GetSupportedMapById(aMapId, listIndex, aSupportedMap);
}

bool Instance::AddSupportedMap(uint32_t aMapId, const CharSpan  &aMapName)
{
    return mCluster.Cluster().AddSupportedMap(aMapId, aMapName);
}

bool Instance::RenameSupportedMap(uint32_t aMapId, const CharSpan  &newMapName)
{
    return mCluster.Cluster().RenameSupportedMap(aMapId, newMapName);
}

bool Instance::ClearSupportedMaps()
{
    return mCluster.Cluster().ClearSupportedMaps();
}

bool Instance::RemoveSupportedMap(uint32_t mapId)
{
    return mCluster.Cluster().RemoveSupportedMap(mapId);
}

uint32_t Instance::GetNumberOfSelectedAreas()
{
    return mCluster.Cluster().GetNumberOfSelectedAreas();
}

bool Instance::GetSelectedAreaByIndex(uint32_t listIndex, uint32_t  &selectedArea)
{
    return mCluster.Cluster().GetSelectedAreaByIndex(listIndex, selectedArea);
}

bool Instance::AddSelectedArea(uint32_t  &aSelectedArea)
{
    return mCluster.Cluster().AddSelectedArea(aSelectedArea);
}

bool Instance::ClearSelectedAreas()
{
    return mCluster.Cluster().ClearSelectedAreas();
}

bool Instance::RemoveSelectedAreas(uint32_t areaId)
{
    return mCluster.Cluster().RemoveSelectedAreas(areaId);
}

DataModel::Nullable<uint32_t> Instance::GetCurrentArea()
{
    return mCluster.Cluster().GetCurrentArea();
}

bool Instance::SetCurrentArea(const DataModel::Nullable<uint32_t>  &aCurrentArea)
{
    return mCluster.Cluster().SetCurrentArea(aCurrentArea);
}

DataModel::Nullable<uint32_t> Instance::GetEstimatedEndTime()
{
    return mCluster.Cluster().GetEstimatedEndTime();
}

bool Instance::SetEstimatedEndTime(const DataModel::Nullable<uint32_t>  &aEstimatedEndTime)
{
    return mCluster.Cluster().SetEstimatedEndTime(aEstimatedEndTime);
}

uint32_t Instance::GetNumberOfProgressElements()
{
    return mCluster.Cluster().GetNumberOfProgressElements();
}

bool Instance::GetProgressElementByIndex(uint32_t listIndex, Structs::ProgressStruct::Type  &aProgressElement)
{
    return mCluster.Cluster().GetProgressElementByIndex(listIndex, aProgressElement);
}

bool Instance::GetProgressElementById(uint32_t aAreaId, uint32_t  &listIndex, Structs::ProgressStruct::Type  &aProgressElement)
{
    return mCluster.Cluster().GetProgressElementById(aAreaId, listIndex, aProgressElement);
}

bool Instance::AddPendingProgressElement(uint32_t aAreaId)
{
    return mCluster.Cluster().AddPendingProgressElement(aAreaId);
}

bool Instance::SetProgressStatus(uint32_t aAreaId, OperationalStatusEnum opStatus)
{
    return mCluster.Cluster().SetProgressStatus(aAreaId, opStatus);
}

bool Instance::SetProgressTotalOperationalTime(uint32_t aAreaId, const DataModel::Nullable<uint32_t>  &aTotalOperationalTime)
{
    return mCluster.Cluster().SetProgressTotalOperationalTime(aAreaId, aTotalOperationalTime);
}

bool Instance::SetProgressEstimatedTime(uint32_t aAreaId, const DataModel::Nullable<uint32_t>  &aEstimatedTime)
{
    return mCluster.Cluster().SetProgressEstimatedTime(aAreaId, aEstimatedTime);
}

bool Instance::ClearProgress()
{
    return mCluster.Cluster().ClearProgress();
}

bool Instance::RemoveProgressElement(uint32_t areaId)
{
    return mCluster.Cluster().RemoveProgressElement(areaId);
}

bool Instance::HasFeature(ServiceArea::Feature feature) const
{
    return mCluster.Cluster().HasFeature(feature);
}

} // namespace ServiceArea
} // namespace Clusters
} // namespace app
} // namespace chip

// The Instance is created by ServiceAreaDelegateInitCB, which owns its lifetime.
void ESPMatterServiceAreaClusterServerInitCallback(EndpointId endpointId) {}

void ESPMatterServiceAreaClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gStorageDelegates.erase(endpointId);
    }
}
