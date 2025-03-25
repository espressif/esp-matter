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

#include <esp_log.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_icd_client.h>

#include <crypto/CHIPCryptoPAL.h>

static constexpr char *TAG = "icd_client";

using namespace ::chip;

namespace esp_matter {
namespace controller {
esp_err_t list_registered_icd()
{
    auto &icd_client_storage = matter_controller_client::get_instance().get_icd_client_storage();
    auto iter = icd_client_storage.IterateICDClientInfo();
    if (iter == nullptr) {
        return ESP_ERR_NO_MEM;
    }
    char icd_aes_key_hex[Crypto::kAES_CCM128_Key_Length * 2 + 1];
    char icd_hmac_key_hex[Crypto::kHMAC_CCM128_Key_Length * 2 + 1];
    app::DefaultICDClientStorage::ICDClientInfoIteratorWrapper wrapper(iter);
    ESP_LOGI(TAG, "  +------------------------------------------------------------------------------------------------------+");
    ESP_LOGI(TAG, "  | %-100s |", "Known ICDs:");
    ESP_LOGI(TAG, "  +------------------------------------------------------------------------------------------------------+");
    ESP_LOGI(TAG, "  | %-16s | %16s | %13s | %14s | %16s | %10s |", "Peer Node ID", "CheckIn Node ID", "Start Counter",
             "Counter Offset", "MonitoredSubject", "ClientType");
    ESP_LOGI(TAG, "  +------------------------------------------------------------------------------------------------------+");
    app::ICDClientInfo info;
    while (iter->Next(info)) {
        ESP_LOGI(TAG, "  | " ChipLogFormatX64 " | " ChipLogFormatX64 " | %13" PRIu32 " | %14" PRIu32
                 " | " ChipLogFormatX64 " | %10u |", ChipLogValueX64(info.peer_node.GetNodeId()),
                 ChipLogValueX64(info.check_in_node.GetNodeId()),info.start_icd_counter, info.offset,
                 ChipLogValueX64(info.monitored_subject), static_cast<uint8_t>(info.client_type));
        Encoding::BytesToHex(info.aes_key_handle.As<Crypto::Symmetric128BitsKeyByteArray>(), Crypto::kAES_CCM128_Key_Length,
                             icd_aes_key_hex, sizeof(icd_aes_key_hex), Encoding::HexFlags::kNullTerminate);
        ESP_LOGI(TAG,"  | aes key:  %60s                               |", icd_aes_key_hex);
        Encoding::BytesToHex(info.hmac_key_handle.As<Crypto::Symmetric128BitsKeyByteArray>(), Crypto::kHMAC_CCM128_Key_Length,
                             icd_hmac_key_hex, sizeof(icd_hmac_key_hex), Encoding::HexFlags::kNullTerminate);
        ESP_LOGI(TAG,"  | hmac key: %60s                               |", icd_hmac_key_hex);
        ESP_LOGI(TAG, "  +------------------------------------------------------------------------------------------------------+");
    }
    return ESP_OK;
}
} // namespace controller
} // namespace esp_matter
