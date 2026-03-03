/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_application_launcher_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ApplicationLauncher {

CHIP_ERROR MockApplicationLauncherDelegate::HandleGetCatalogList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

void MockApplicationLauncherDelegate::HandleLaunchApp(CommandResponseHelper<LauncherResponseType>  &helper, const ByteSpan  &data,
                                                      const Application  &application)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    LauncherResponseType response;
    response.status = ApplicationLauncherStatusEnum::kSuccess;
    helper.Success(response);
}

void MockApplicationLauncherDelegate::HandleStopApp(CommandResponseHelper<LauncherResponseType>  &helper, const Application  &application)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    LauncherResponseType response;
    response.status = ApplicationLauncherStatusEnum::kSuccess;
    helper.Success(response);
}

void MockApplicationLauncherDelegate::HandleHideApp(CommandResponseHelper<LauncherResponseType>  &helper, const Application  &application)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    LauncherResponseType response;
    response.status = ApplicationLauncherStatusEnum::kSuccess;
    helper.Success(response);
}

} // namespace ApplicationLauncher
} // namespace Clusters
} // namespace app
} // namespace chip