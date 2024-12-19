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

#include <esp_err.h>
#include <esp_http_client.h>
#include <protocols/bdx/BdxTransferSession.h>
#include <protocols/bdx/TransferFacilitator.h>

#define OTA_URL_MAX_LEN 256

namespace esp_matter {
namespace ota_provider {

class OtaBdxSender : public chip::bdx::Responder {
public:
    enum BdxSenderErr {
        kErrBdxSenderNone = 0,
        kErrBdxSenderStatusReceived,
        kErrBdxSenderInternal,
        kErrBdxSenderTimeout,
    };

    OtaBdxSender()
    {
        memset(mOtaImageUrl, 0, sizeof(mOtaImageUrl));
        mOtaImageSize = 0;
    }

    // Initializes BDX transfer-related metadata. Should always be called first.
    esp_err_t InitializeTransfer(chip::FabricIndex fabricIndex, chip::NodeId nodeId);

    uint16_t GetTransferBlockSize(void);

    uint64_t GetTransferLength(void);

    void SetOtaImageUrl(const char *otaImageUrl)
    {
        strncpy(mOtaImageUrl, otaImageUrl, strnlen(otaImageUrl, OTA_URL_MAX_LEN));
    }

    const char *GetOtaImageUrl() const { return mOtaImageUrl; }

private:
    void HandleTransferSessionOutput(chip::bdx::TransferSession::OutputEvent &event) override;

    esp_err_t ParseOtaImageHeader(const uint8_t *header_buf, size_t header_buf_size);

    void Reset();

    uint64_t mNumBytesSent = 0;

    bool mInitialized = false;

    chip::Optional<chip::FabricIndex> mFabricIndex;
    chip::Optional<chip::NodeId> mNodeId;

    char mOtaImageUrl[OTA_URL_MAX_LEN];
    uint64_t mOtaImageSize;
    esp_http_client_handle_t mHttpDownloader = nullptr;
};

} // namespace ota_provider
} // namespace esp_matter
