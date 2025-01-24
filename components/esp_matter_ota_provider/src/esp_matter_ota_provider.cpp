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

#include <algorithm>
#include <cstring>
#include <esp_check.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_matter_mem.h>
#include <esp_matter_ota_candidates.h>
#include <esp_matter_ota_provider.h>
#include <json_parser.h>

#include <app/server/Server.h>
#include <platform/PlatformManager.h>
#include <protocols/bdx/BdxUri.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app::Clusters::OtaSoftwareUpdateProvider::Commands;
using chip::BitFlags;
using chip::ByteSpan;
using chip::app::CommandHandler;
using chip::app::ConcreteCommandPath;
using chip::bdx::TransferControlFlags;
using chip::Protocols::InteractionModel::Status;

static constexpr char TAG[] = "ota_provider";

namespace esp_matter {
namespace ota_provider {

// Arbitrary BDX Transfer Params
constexpr uint32_t kMaxBdxBlockSize = 1024;
constexpr chip::System::Clock::Timeout kBdxTimeout =
    chip::System::Clock::Seconds16(5 * 60); // OTA Spec mandates >= 5 minutes
constexpr uint32_t kBdxServerPollIntervalMillis = 50;

static void GenerateUpdateToken(uint8_t *buf, size_t bufSize)
{
    for (size_t i = 0; i < bufSize; ++i) {
        buf[i] = chip::Crypto::GetRandU8();
    }
}

static void GetUpdateTokenString(const ByteSpan &token, char *buf, size_t bufSize)
{
    const uint8_t *tokenData = static_cast<const uint8_t *>(token.data());
    size_t minLength = std::min(token.size(), bufSize);
    for (size_t i = 0; i < (minLength / 2) - 1; ++i) {
        snprintf(&buf[i * 2], bufSize, "%02X", tokenData[i]);
    }
}

void EspOtaProvider::Init(bool otaAllowedDefault)
{
    mDelayedQueryActionTimeSec = 0;
    mUpdateAction = OTAApplyUpdateAction::kProceed;
    mDelayedApplyActionTimeSec = 0;
    mPollInterval = kBdxServerPollIntervalMillis;
    mOtaRequestorList = nullptr;
    mOtaAllowedDefault = otaAllowedDefault;
    init_ota_candidates();
    chip::Server::GetInstance().GetExchangeManager().RegisterUnsolicitedMessageHandlerForProtocol(
        chip::Protocols::BDX::Id, &mOtaBdxSender);
}

void EspOtaProvider::SendQueryImageResponse(OTAQueryStatus status)
{
    auto commandHandleRef = std::move(mAsyncCommandHandle);
    auto commandHandle = commandHandleRef.Get();
    if (commandHandle == nullptr || commandHandle->GetExchangeContext()->GetSessionHandle()->GetPeer() != mPeerNodeId) {
        ESP_LOGE(TAG, "Invalid commandHandle, cannot send QueryImageResponse");
        return;
    }
    EspOtaRequestorEntry *requestor = FindOtaRequestorEntry(mPeerNodeId);
    if (requestor) {
        if ((!requestor->mOtaAllowed) && (!requestor->mOtaAllowedOnce)) {
            if (status == OTAQueryStatus::kUpdateAvailable) {
                requestor->mHasNewVersion = true;
            }
            status = OTAQueryStatus::kNotAvailable;
        }
    } else {
        status = OTAQueryStatus::kNotAvailable;
    }

    QueryImageResponse::Type response;
    char strBuf[kUpdateTokenStrLen] = {0};

    // Set fields specific for an available status response
    if (status == OTAQueryStatus::kUpdateAvailable) {
        FabricIndex fabricIndex = mSubjectDescriptor.fabricIndex;
        const FabricInfo *fabricInfo = Server::GetInstance().GetFabricTable().FindFabricWithIndex(fabricIndex);
        NodeId providerNodeId = fabricInfo->GetPeerId().GetNodeId();

        // Generate the ImageURI
        MutableCharSpan uri(requestor->mImageUri);
        char otaFileName[128] = {0};
        char *ptr = strrchr(requestor->mOtaImageUrl, '/');
        strncpy(otaFileName, ptr + 1, strnlen(ptr + 1, 255));
        CHIP_ERROR error = chip::bdx::MakeURI(providerNodeId, CharSpan::fromCharString(otaFileName), uri);
        if (error != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Cannot generate URI");
            memset(requestor->mImageUri, 0, sizeof(requestor->mImageUri));
        } else {
            ESP_LOGD(TAG, "Generated URI: %s", requestor->mImageUri);
        }

        // Initialize the transfer session in prepartion for a BDX transfer
        BitFlags<TransferControlFlags> bdxFlags;
        bdxFlags.Set(TransferControlFlags::kReceiverDrive);
        if (mOtaBdxSender.InitializeTransfer(mSubjectDescriptor.fabricIndex, mSubjectDescriptor.subject) == ESP_OK) {
            mOtaBdxSender.SetOtaImageUrl(requestor->mOtaImageUrl);
            ESP_LOGI(TAG, "Bdx Sender will query the OTA image from %s", requestor->mOtaImageUrl);
            CHIP_ERROR error = mOtaBdxSender.PrepareForTransfer(
                &chip::DeviceLayer::SystemLayer(), chip::bdx::TransferRole::kSender, bdxFlags, kMaxBdxBlockSize,
                kBdxTimeout, chip::System::Clock::Milliseconds32(mPollInterval));
            if (error != CHIP_NO_ERROR) {
                ESP_LOGE(TAG, "Cannot prepare for transfer: %" CHIP_ERROR_FORMAT, error.Format());
                commandHandle->AddStatus(mPath, Status::Failure);
                return;
            }
            GenerateUpdateToken(requestor->mUpdateToken, kUpdateTokenLen);
            GetUpdateTokenString(ByteSpan(requestor->mUpdateToken), strBuf, kUpdateTokenStrLen);
            ESP_LOGD(TAG, "Generated updateToken: %s", strBuf);

            response.imageURI.Emplace(chip::CharSpan::fromCharString(requestor->mImageUri));
            response.softwareVersion.Emplace(requestor->mSoftwareVersion);
            response.softwareVersionString.Emplace(chip::CharSpan::fromCharString(requestor->mSoftwareVersionString));
            response.updateToken.Emplace(chip::ByteSpan(requestor->mUpdateToken));
        } else {
            // Another BDX transfer in progress
            status = OTAQueryStatus::kBusy;
        }
    }

    // Delay action time is only applicable when the provider is busy
    if (status == OTAQueryStatus::kBusy) {
        if (mDelayedApplyActionTimeSec == 0) {
            mDelayedQueryActionTimeSec = 120;
        }
        response.delayedActionTime.Emplace(mDelayedQueryActionTimeSec);
    }

    // Set remaining fields common to all status types
    response.status = status;
    // Either sends the response or an error status
    commandHandle->AddResponse(mPath, response);
}

void EspOtaProvider::FetchImageDoneCallback(OTAQueryStatus status, const char *imageUrl, size_t imageSize,
                                            uint32_t softwareVersion, const char *softwareVersionStr, void *arg)
{
    EspOtaProvider *provider = (EspOtaProvider *)arg;
    assert(provider);
    EspOtaRequestorEntry *requestor = provider->FindOtaRequestorEntry(provider->mPeerNodeId);
    if (requestor && status == OTAQueryStatus::kUpdateAvailable) {
        strncpy(requestor->mOtaImageUrl, imageUrl, sizeof(requestor->mOtaImageUrl) - 1);
        requestor->mOtaImageSize = imageSize;
        requestor->mSoftwareVersion = softwareVersion;
        strncpy(requestor->mSoftwareVersionString, softwareVersionStr, sizeof(requestor->mSoftwareVersionString) - 1);
    }
    DeviceLayer::PlatformMgr().LockChipStack();
    provider->SendQueryImageResponse(status);
    DeviceLayer::PlatformMgr().UnlockChipStack();
}

void EspOtaProvider::HandleQueryImage(CommandHandler *commandObj, const ConcreteCommandPath &commandPath,
                                      const QueryImage::DecodableType &commandData)
{
    uint16_t vendor_id = commandData.vendorID;
    uint16_t product_id = commandData.productID;
    uint32_t software_version = commandData.softwareVersion;
    if (CreateOtaRequestorEntry(commandObj->GetExchangeContext()->GetSessionHandle()->GetPeer()) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create Ota Pending Entry");
        commandObj->AddStatus(commandPath, Status::ResourceExhausted);
        return;
    }

    if (mAsyncCommandHandle.Get() != nullptr) {
        // We have a command processing in the backend, reject query image command.
        QueryImageResponse::Type response;
        response.status = OTAQueryStatus::kBusy;
        if (mDelayedApplyActionTimeSec == 0) {
            mDelayedQueryActionTimeSec = 120;
        }
        response.delayedActionTime.Emplace(mDelayedQueryActionTimeSec);
        commandObj->AddResponse(commandPath, response);
        return;
    }
    // The OTA provider might need some time to query the image information from DCL.
    commandObj->FlushAcksRightAwayOnSlowCommand();
    // Use a command handle to hold the CommandHandler so that it will not be released.
    mSubjectDescriptor = commandObj->GetSubjectDescriptor();
    mPeerNodeId = commandObj->GetExchangeContext()->GetSessionHandle()->GetPeer();
    mAsyncCommandHandle = chip::app::CommandHandler::Handle(commandObj);
    mPath = commandPath;
    if (fetch_ota_candidate(vendor_id, product_id, software_version, FetchImageDoneCallback, this) != ESP_OK) {
        SendQueryImageResponse(OTAQueryStatus::kNotAvailable);
    }
}

void EspOtaProvider::HandleApplyUpdateRequest(app::CommandHandler *commandObj,
                                              const app::ConcreteCommandPath &commandPath,
                                              const ApplyUpdateRequest::DecodableType &commandData)
{
    if (commandObj == nullptr) {
        ESP_LOGE(TAG, "Invalid commandObj, cannot handle ApplyUpdateRequest");
        return;
    }
    EspOtaRequestorEntry *requestor =
        FindOtaRequestorEntry(commandObj->GetExchangeContext()->GetSessionHandle()->GetPeer());
    char tokenBuf[kUpdateTokenStrLen] = {0};
    GetUpdateTokenString(commandData.updateToken, tokenBuf, kUpdateTokenStrLen);
    ESP_LOGD(TAG, "%s: token: %s, version: %" PRIu32, __FUNCTION__, tokenBuf, commandData.newVersion);
    if (requestor && commandData.updateToken.data_equal(ByteSpan(requestor->mUpdateToken)) &&
        commandData.newVersion == requestor->mSoftwareVersion) {
        ApplyUpdateResponse::Type response;
        response.action = mUpdateAction;
        response.delayedActionTime = mDelayedApplyActionTimeSec;

        // Reset delay back to 0 for subsequent uses
        mDelayedApplyActionTimeSec = 0;
        // Reset back to success case for subsequent uses
        mUpdateAction = OTAApplyUpdateAction::kProceed;

        // Either sends the response or an error status
        commandObj->AddResponse(commandPath, response);
    } else {
        commandObj->AddStatus(commandPath, Status::InvalidCommand);
    }
}

void EspOtaProvider::HandleNotifyUpdateApplied(app::CommandHandler *commandObj,
                                               const app::ConcreteCommandPath &commandPath,
                                               const NotifyUpdateApplied::DecodableType &commandData)
{
    if (commandObj == nullptr) {
        ESP_LOGE(TAG, "Invalid commandObj, cannot handle ApplyUpdateRequest");
        return;
    }
    EspOtaRequestorEntry *requestor =
        FindOtaRequestorEntry(commandObj->GetExchangeContext()->GetSessionHandle()->GetPeer());
    char tokenBuf[kUpdateTokenStrLen] = {0};
    GetUpdateTokenString(commandData.updateToken, tokenBuf, kUpdateTokenStrLen);
    ESP_LOGD(TAG, "%s: token: %s, version: %" PRIu32, __FUNCTION__, tokenBuf, commandData.softwareVersion);
    if (requestor && commandData.updateToken.data_equal(ByteSpan(requestor->mUpdateToken)) &&
        commandData.softwareVersion == requestor->mSoftwareVersion) {
        commandObj->AddStatus(commandPath, Status::Success);
        // Finish OTA, set the set OtaAllowedOnce to false.
        requestor->mOtaAllowedOnce = false;
    } else {
        commandObj->AddStatus(commandPath, Status::InvalidCommand);
    }
}

esp_err_t EspOtaProvider::EnableOtaForNode(const chip::ScopedNodeId &nodeId, bool forOnlyOnce)
{
    EspOtaRequestorEntry *iter = mOtaRequestorList;
    bool found = false;
    while (iter) {
        if (iter->mNodeId == nodeId ||
            (nodeId.GetNodeId() == chip::kUndefinedNodeId &&
             (nodeId.GetFabricIndex() == iter->mNodeId.GetFabricIndex() ||
              nodeId.GetFabricIndex() == chip::kUndefinedFabricIndex))) {
            if (!forOnlyOnce) {
                iter->mOtaAllowed = true;
            } else {
                iter->mOtaAllowedOnce = true;
            }
            found = true;
        }
        iter = iter->mNext;
    }
    return found ? ESP_OK : ESP_ERR_NOT_FOUND;
}
esp_err_t EspOtaProvider::DisableOtaForNode(const chip::ScopedNodeId &nodeId)
{
    EspOtaRequestorEntry *iter = mOtaRequestorList;
    bool found = false;
    while (iter) {
        if (iter->mNodeId == nodeId ||
            (nodeId.GetNodeId() == chip::kUndefinedNodeId &&
             (nodeId.GetFabricIndex() == iter->mNodeId.GetFabricIndex() ||
              nodeId.GetFabricIndex() == chip::kUndefinedFabricIndex))) {
            iter->mOtaAllowed = false;
            iter->mOtaAllowedOnce = false;
            found = true;
        }
        iter = iter->mNext;
    }
    return found ? ESP_OK : ESP_ERR_NOT_FOUND;
}

EspOtaProvider::EspOtaRequestorEntry *EspOtaProvider::FindOtaRequestorEntry(const chip::ScopedNodeId &nodeId)
{
    EspOtaRequestorEntry *iter = mOtaRequestorList;
    while (iter) {
        if (iter->mNodeId == nodeId) {
            return iter;
        }
        iter = iter->mNext;
    }
    return nullptr;
}

esp_err_t EspOtaProvider::CreateOtaRequestorEntry(const chip::ScopedNodeId &nodeId)
{
    EspOtaRequestorEntry *entry = FindOtaRequestorEntry(nodeId);
    if (!entry) {
        entry = chip::Platform::New<EspOtaRequestorEntry>();
        if (!entry) {
            return ESP_ERR_NO_MEM;
        }
        entry->mNodeId = nodeId;
        entry->mOtaAllowed = mOtaAllowedDefault;
        entry->mNext = mOtaRequestorList;
        mOtaRequestorList = entry;
    }
    return ESP_OK;
}

esp_err_t EspOtaProvider::RemoveOtaRequestorEntry(const chip::ScopedNodeId &nodeId)
{
    EspOtaRequestorEntry *prev = nullptr;
    EspOtaRequestorEntry *iter = mOtaRequestorList;
    while (iter && iter->mNodeId != nodeId) {
        prev = iter;
        iter = iter->mNext;
    }
    if (iter) {
        if (!prev) {
            mOtaRequestorList = iter->mNext;
        } else {
            prev->mNext = iter->mNext;
        }
        chip::Platform::Delete(iter);
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

} // namespace ota_provider
} // namespace esp_matter
