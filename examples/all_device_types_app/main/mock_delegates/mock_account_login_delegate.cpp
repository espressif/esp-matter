/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_account_login_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace AccountLogin {

void MockAccountLoginDelegate::SetSetupPin(char * setupPin)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
}

bool MockAccountLoginDelegate::HandleLogin(const chip::CharSpan  &tempAccountIdentifierString, const chip::CharSpan  &setupPinString,
                                           const chip::Optional<NodeId>  &nodeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockAccountLoginDelegate::HandleLogout(const chip::Optional<NodeId>  &nodeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

void MockAccountLoginDelegate::HandleGetSetupPin(CommandResponseHelper<Commands::GetSetupPINResponse::Type>  &helper,
                                                 const chip::CharSpan  &tempAccountIdentifierString)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
}

void MockAccountLoginDelegate::GetSetupPin(char * setupPin, size_t setupPinSize, const chip::CharSpan  &tempAccountIdentifierString)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
}

uint16_t MockAccountLoginDelegate::GetClusterRevision(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace AccountLogin
} // namespace Clusters
} // namespace app
} // namespace chip
