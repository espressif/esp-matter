// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#include <access/SubjectDescriptor.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/CommandHandler.h>
#include <app/clusters/ota-provider/OTAProviderUserConsentDelegate.h>
#include <app/clusters/ota-provider/ota-provider-delegate.h>
#include <cstdint>
#include <esp_matter_ota_bdx_sender.h>
#include <freertos/FreeRTOS.h>
#include <lib/core/OTAImageHeader.h>

#define SOFTWARE_VERSION_STR_MAX_LEN 64

namespace esp_matter {
namespace ota_provider {

class EspOtaProvider : public chip::app::Clusters::OTAProviderDelegate {
public:
    using OTAQueryStatus = chip::app::Clusters::OtaSoftwareUpdateProvider::OTAQueryStatus;
    using OTAApplyUpdateAction = chip::app::Clusters::OtaSoftwareUpdateProvider::OTAApplyUpdateAction;

    static constexpr size_t kUriMaxLen = 256;
    static constexpr uint8_t kUpdateTokenLen = 32;
    static constexpr uint8_t kUpdateTokenStrLen = kUpdateTokenLen * 2 + 1;
    struct EspOtaRequestorEntry {
        chip::ScopedNodeId mNodeId;
        bool mOtaAllowed;
        bool mOtaAllowedOnce;
        bool mHasNewVersion;
        uint8_t mUpdateToken[kUpdateTokenLen];
        char mImageUri[kUriMaxLen];
        char mOtaImageUrl[OTA_URL_MAX_LEN];
        size_t mOtaImageSize;
        uint32_t mSoftwareVersion;
        char mSoftwareVersionString[SOFTWARE_VERSION_STR_MAX_LEN];
        EspOtaRequestorEntry *mNext;
    };

    // OTAProviderDelegate Implementation
    void HandleQueryImage(chip::app::CommandHandler *commandObj, const chip::app::ConcreteCommandPath &commandPath,
                          const chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::QueryImage::DecodableType
                              &commandData) override;

    void HandleApplyUpdateRequest(
        chip::app::CommandHandler *commandObj, const chip::app::ConcreteCommandPath &commandPath,
        const chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::ApplyUpdateRequest::DecodableType &commandData)
        override;

    void HandleNotifyUpdateApplied(
        chip::app::CommandHandler *commandObj, const chip::app::ConcreteCommandPath &commandPath,
        const chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::NotifyUpdateApplied::DecodableType &commandData)
        override;

    // OTAProviderImpl public APIs
    static EspOtaProvider &GetInstance()
    {
        static EspOtaProvider instance;
        return instance;
    }
    void Init(bool otaAllowedDefault);
    void SetApplyUpdateAction(OTAApplyUpdateAction action) { mUpdateAction = action; }
    void SetDelayedQueryActionTimeSec(uint32_t time) { mDelayedQueryActionTimeSec = time; }
    void SetDelayedApplyActionTimeSec(uint32_t time) { mDelayedApplyActionTimeSec = time; }
    void SetPollInterval(uint32_t interval) { mPollInterval = (interval != 0) ? interval : mPollInterval; }

    static void FetchImageDoneCallback(OTAQueryStatus status, const char *imageUrl, size_t imageSize,
                                       uint32_t softwareVersion, const char *softwareVersionStr, void *arg);

    // When the OTA Provider receives a QueryImage command from an OTA Requestor and there is no existing entry for the
    // Requestor node, the Provider will create an OTA Requestor Entry for the requestor, and set the entry's
    // mOtaAllowed to mOtaAllowedDefault.
    void SetOtaAllowedDefault(bool otaAllowed) { mOtaAllowedDefault = otaAllowed; }
    // When there is a Requestor entry for the nodeId, we can call the EnableOtaForNode/DisableOtaForNode to make the
    // provider allow whether the requestor proceed the OTA process.
    esp_err_t EnableOtaForNode(const chip::ScopedNodeId &nodeId, bool forOnlyOnce);
    esp_err_t DisableOtaForNode(const chip::ScopedNodeId &nodeId);
    // This should be called when the OTA Provider is notified that one node is removed from the Fabric.
    esp_err_t RemoveOtaRequestorEntry(const chip::ScopedNodeId &nodeId);
    EspOtaRequestorEntry *FindOtaRequestorEntry(const chip::ScopedNodeId &nodeId);

private:
    EspOtaProvider() {}
    ~EspOtaProvider() {}

    void SendQueryImageResponse(OTAQueryStatus status);

    esp_err_t CreateOtaRequestorEntry(const chip::ScopedNodeId &nodeId);

    OtaBdxSender mOtaBdxSender;
    uint32_t mDelayedQueryActionTimeSec;
    OTAApplyUpdateAction mUpdateAction;
    uint32_t mDelayedApplyActionTimeSec;
    uint32_t mPollInterval;
    bool mOtaAllowedDefault;
    EspOtaRequestorEntry *mOtaRequestorList;

    // Use async command handler for QueryImage command
    chip::app::CommandHandler::Handle mAsyncCommandHandle;
    chip::app::ConcreteCommandPath mPath = chip::app::ConcreteCommandPath(0, 0, 0);
    chip::Access::SubjectDescriptor mSubjectDescriptor;
    chip::ScopedNodeId mPeerNodeId;
};
} // namespace ota_provider
} // namespace esp_matter
