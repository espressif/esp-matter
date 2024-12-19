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

#include <esp_crt_bundle.h>
#include <esp_log.h>
#include <esp_matter_ota_bdx_sender.h>
#include <esp_matter_ota_http_downloader.h>

#include <lib/core/CHIPError.h>
#include <lib/support/BitFlags.h>
#include <lib/support/CHIPMemString.h>
#include <messaging/ExchangeContext.h>
#include <messaging/Flags.h>
#include <protocols/bdx/BdxTransferSession.h>

static constexpr char TAG[] = "ota_provider";

using chip::bdx::StatusCode;
using chip::bdx::TransferControlFlags;
using chip::bdx::TransferSession;

namespace esp_matter {
namespace ota_provider {

esp_err_t OtaBdxSender::InitializeTransfer(chip::FabricIndex fabricIndex, chip::NodeId nodeId)
{
    if (mInitialized) {
        if ((mFabricIndex.HasValue() && mFabricIndex.Value() == fabricIndex) &&
            (mNodeId.HasValue() && mNodeId.Value() == nodeId)) {
            Reset();
        } else if ((mFabricIndex.HasValue() && mFabricIndex.Value() != fabricIndex) ||
                   (mNodeId.HasValue() && mNodeId.Value() != nodeId)) {
            return ESP_ERR_INVALID_STATE;
        } else {
            return ESP_FAIL;
        }
    }
    mFabricIndex.SetValue(fabricIndex);
    mNodeId.SetValue(nodeId);
    mInitialized = true;
    return ESP_OK;
}

esp_err_t OtaBdxSender::ParseOtaImageHeader(const uint8_t *header_buf, size_t header_buf_size)
{
    if (header_buf_size < sizeof(ota_image_header_prefix_t)) {
        ESP_LOGE(TAG, "Invalid header buffer size");
        return ESP_ERR_INVALID_ARG;
    }
    ota_image_header_prefix_t *prefix = (ota_image_header_prefix_t *)header_buf;
    if (prefix->file_identifier != k_ota_image_file_identifier) {
        ESP_LOGE(TAG, "Invalid OTA image file identifier");
        return ESP_ERR_INVALID_ARG;
    }
    if (prefix->total_size <= prefix->header_size + sizeof(ota_image_header_prefix_t)) {
        ESP_LOGE(TAG, "Invalid payload size");
        return ESP_ERR_INVALID_ARG;
    }
    mOtaImageSize = prefix->total_size;
    return ESP_OK;
}

void OtaBdxSender::HandleTransferSessionOutput(TransferSession::OutputEvent &event)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    if (event.EventType != TransferSession::OutputEventType::kNone) {
        ESP_LOGD(TAG, "OutputEvent type: %s", event.ToString(event.EventType));
    }
    switch (event.EventType) {
    case TransferSession::OutputEventType::kNone:
        break;
    case TransferSession::OutputEventType::kMsgToSend: {
        chip::Messaging::SendFlags sendFlags;
        if (!event.msgTypeData.HasMessageType(chip::Protocols::SecureChannel::MsgType::StatusReport)) {
            sendFlags.Set(chip::Messaging::SendMessageFlags::kExpectResponse);
        }
        if (mExchangeCtx == nullptr) {
            ESP_LOGE(TAG, "mExchangeCtx cannot be NULL");
            return;
        }
        err = mExchangeCtx->SendMessage(event.msgTypeData.ProtocolId, event.msgTypeData.MessageType,
                                        std::move(event.MsgData), sendFlags);
        if (err == CHIP_NO_ERROR) {
            if (!sendFlags.Has(chip::Messaging::SendMessageFlags::kExpectResponse)) {
                // After sending the StatusReport, exchange context gets closed so, set mExchangeCtx to null
                mExchangeCtx = nullptr;
            }
        } else {
            ESP_LOGE(TAG, "SendMessage failed: %" CHIP_ERROR_FORMAT, err.Format());
            Reset();
        }
        break;
    }
    case TransferSession::OutputEventType::kInitReceived: {
        // TransferSession will automatically reject a transfer if there are no
        // common supported control modes. It will also default to the smaller
        // block size.
        TransferSession::TransferAcceptData acceptData;
        acceptData.ControlMode = TransferControlFlags::kReceiverDrive; // OTA must use receiver drive
        acceptData.MaxBlockSize = mTransfer.GetTransferBlockSize();
        acceptData.StartOffset = mTransfer.GetStartOffset();
        acceptData.Length = mTransfer.GetTransferLength();
        if (mTransfer.AcceptTransfer(acceptData) != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "AcceptTransfter failed error:%" CHIP_ERROR_FORMAT, err.Format());
            return;
        }
        // Establish http connection
        esp_http_client_config_t config = {
            .url = mOtaImageUrl,
            .event_handler = NULL,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .skip_cert_common_name_check = false,
            .crt_bundle_attach = esp_crt_bundle_attach,
            .keep_alive_enable = true,
        };
        if (mHttpDownloader) {
            http_downloader_abort(mHttpDownloader);
        }
        if (http_downloader_start(&config, &mHttpDownloader) != ESP_OK) {
            mTransfer.AbortTransfer(StatusCode::kUnknown);
        }
        break;
    }
    case TransferSession::OutputEventType::kQueryReceived: {
        TransferSession::BlockData blockData;
        uint16_t bytesToRead = mTransfer.GetTransferBlockSize();

        chip::System::PacketBufferHandle blockBuf = chip::System::PacketBufferHandle::New(bytesToRead);
        if (blockBuf.IsNull()) {
            mTransfer.AbortTransfer(StatusCode::kUnknown);
            return;
        }
        // Read http response
        int bytes_read =
            http_downloader_read(mHttpDownloader, reinterpret_cast<char *>(blockBuf->Start()), bytesToRead);
        if (bytes_read < 0) {
            ESP_LOGE(TAG, "http_downloader_read failed");
            mTransfer.AbortTransfer(StatusCode::kUnknown);
            break;
        }
        if (mOtaImageSize == 0 && mNumBytesSent == 0) {
            if (ParseOtaImageHeader(blockBuf->Start(), static_cast<size_t>(bytes_read)) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to Parse OTA image header");
                mTransfer.AbortTransfer(StatusCode::kUnknown);
                break;
            }
        }
        blockData.Data = blockBuf->Start();
        blockData.Length =
            static_cast<size_t>(std::min(static_cast<uint64_t>(bytes_read), (mOtaImageSize - mNumBytesSent)));
        blockData.IsEof = (blockData.Length < bytesToRead) ||
            (mNumBytesSent + static_cast<uint64_t>(blockData.Length) == mOtaImageSize);
        mNumBytesSent = static_cast<uint64_t>(mNumBytesSent + blockData.Length);

        if (CHIP_NO_ERROR != mTransfer.PrepareBlock(blockData)) {
            ESP_LOGE(TAG, "PrepareBlock failed: %" CHIP_ERROR_FORMAT, err.Format());
            mTransfer.AbortTransfer(StatusCode::kUnknown);
        }
        break;
    }
    case TransferSession::OutputEventType::kAckReceived:
        break;
    case TransferSession::OutputEventType::kAckEOFReceived: {
        ESP_LOGI(TAG, "Transfer completed, got AckEOF");
        Reset();
        break;
    }
    case TransferSession::OutputEventType::kStatusReceived: {
        ESP_LOGE(TAG, "Got StatusReport %x", static_cast<uint16_t>(event.statusData.statusCode));
        Reset();
        break;
    }
    case TransferSession::OutputEventType::kInternalError: {
        ESP_LOGE(TAG, "InternalError");
        Reset();
        break;
    }
    case TransferSession::OutputEventType::kTransferTimeout: {
        ESP_LOGE(TAG, "TransferTimeout");
        Reset();
        break;
    }
    case TransferSession::OutputEventType::kAcceptReceived:
    case TransferSession::OutputEventType::kBlockReceived:
    default:
        ESP_LOGE(TAG, "unsupported event type");
        break;
    }
    return;
}

void OtaBdxSender::Reset()
{
    mFabricIndex.ClearValue();
    mNodeId.ClearValue();
    ResetTransfer();
    if (mExchangeCtx != nullptr) {
        mExchangeCtx->Close();
        mExchangeCtx = nullptr;
    }

    mInitialized = false;
    mNumBytesSent = 0;
    mOtaImageSize = 0;
    if (mHttpDownloader) {
        // Release current http client
        http_downloader_abort(mHttpDownloader);
    }
    mHttpDownloader = nullptr;
    memset(mOtaImageUrl, 0, sizeof(mOtaImageUrl));
}

uint16_t OtaBdxSender::GetTransferBlockSize(void)
{
    return mTransfer.GetTransferBlockSize();
}

uint64_t OtaBdxSender::GetTransferLength()
{
    return mTransfer.GetTransferLength();
}

} // namespace ota_provider
} // namespace esp_matter
