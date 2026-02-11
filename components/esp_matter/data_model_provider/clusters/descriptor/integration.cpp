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

#include <app/clusters/descriptor/DescriptorCluster.h>
#include <esp_matter_data_model.h>

#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/core/CHIPConfig.h>
#include <lib/core/CHIPError.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/ScopedBuffer.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Descriptor;

namespace {
/**
 * This is a DescriptorCluster class made specifically to fetch the tag list once through ember before one of either the
 * Attributes() or ReadAttribute() functions are called. This can NOT be called before endpoint init and passed to the constructor
 * of the regular DescriptorCluster class. This is because for fixed endpoints, we define endpoints in emberAfEndpointConfigure()
 * and init them in emberAfInit() with back to back calls in InitDataModelHandler(). For dynamic endpoints, we init endpoints in
 * emberAfSetDynamicEndpointWithEpUniqueId() by calling emberAfEndpointEnableDisable(), which calls initializeEndpoint(). The tag
 * list is a fixed attribute, but to maintain backwards compatibility we get that information within the functions here.
 */
class ESPMatterDescriptorCluster : public DescriptorCluster {
public:
    ESPMatterDescriptorCluster(EndpointId endpointId, DescriptorCluster::OptionalAttributesSet optionalAttributeSet) :
        DescriptorCluster(endpointId, optionalAttributeSet, Span<const SemanticTag>())
    {}

    CHIP_ERROR Attributes(const ConcreteClusterPath  &path, ReadOnlyBufferBuilder<DataModel::AttributeEntry>  &builder) override
    {
        if (!mFetchedSemanticTags) {
            ReturnErrorOnFailure(GetSemanticTag(path.mEndpointId, mSemanticTags));
            mFetchedSemanticTags = true;
        }
        return DescriptorCluster::Attributes(path, builder);
    }

    DataModel::ActionReturnStatus ReadAttribute(const DataModel::ReadAttributeRequest  &request,
                                                AttributeValueEncoder  &encoder) override
    {
        if (!mFetchedSemanticTags) {
            ReturnErrorOnFailure(GetSemanticTag(request.path.mEndpointId, mSemanticTags));
            mFetchedSemanticTags = true;
        }
        return DescriptorCluster::ReadAttribute(request, encoder);
    }

private:

    CHIP_ERROR GetSemanticTag(EndpointId endpoint, Span<const SemanticTag> &tags)
    {
        esp_matter::endpoint_t *ep = esp_matter::endpoint::get(endpoint);
        if (!ep) {
            return CHIP_IM_GLOBAL_STATUS(UnsupportedEndpoint);
        }
        int tagCount = esp_matter::endpoint::get_semantic_tag_count(ep);
        if (tagCount > 0) {
            mTagBuffer.Alloc(tagCount);
            if (!mTagBuffer.Get()) {
                return CHIP_ERROR_NO_MEMORY;
            }
            for (int index = 0; index < tagCount; ++index) {
                esp_matter::endpoint::get_semantic_tag_at_index(ep, index, mTagBuffer[index]);
            }
            tags = Span<const SemanticTag>(mTagBuffer.Get(), tagCount);
        }
        return CHIP_NO_ERROR;
    }
    Platform::ScopedMemoryBuffer<SemanticTag> mTagBuffer;
    bool mFetchedSemanticTags = false;
};

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ESPMatterDescriptorCluster>> gServers;

} // namespace

void ESPMatterDescriptorClusterServerInitCallback(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return;
    }
    DescriptorCluster::OptionalAttributesSet optionalAttrSet;
    if (esp_matter::endpoint::is_attribute_enabled(endpointId, Descriptor::Id, Descriptor::Attributes::EndpointUniqueID::Id)) {
        optionalAttrSet.Set<Descriptor::Attributes::EndpointUniqueID::Id>();
    }

    gServers[endpointId].Create(endpointId, optionalAttrSet);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Descriptor on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterDescriptorClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[endpointId].Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister UserLabel on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    gServers[endpointId].Destroy();
}

void MatterDescriptorPluginServerInitCallback() {}

void MatterDescriptorPluginServerShutdownCallback() {}
