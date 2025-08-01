/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_keypad_input_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace KeypadInput {

void MockKeypadInputDelegate::HandleSendKey(CommandResponseHelper<Commands::SendKeyResponse::Type>  &helper, const CECKeyCodeEnum  &keyCode)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::SendKeyResponse::Type response;
    response.status = KeypadInputStatusEnum::kSuccess;
    helper.Success(response);
}

uint32_t MockKeypadInputDelegate::GetFeatureMap(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace KeypadInput
} // namespace Clusters
} // namespace app
} // namespace chip