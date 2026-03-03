/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_content_app_observer_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ContentAppObserver {

void MockContentAppObserverDelegate::HandleContentAppMessage(CommandResponseHelper<Commands::ContentAppMessageResponse::Type>  &helper,
                                                             const chip::Optional<chip::CharSpan>  &data, const chip::CharSpan  &encodingHint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);

    Commands::ContentAppMessageResponse::Type response;
    helper.Success(response);
    return;
}

} // namespace ContentAppObserver
} // namespace Clusters
} // namespace app
} // namespace chip