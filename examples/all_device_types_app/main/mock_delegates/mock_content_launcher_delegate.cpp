/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_content_launcher_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ContentLauncher {

void MockContentLauncherDelegate::HandleLaunchContent(CommandResponseHelper<Commands::LauncherResponse::Type>  &helper,
                                                      const DataModel::DecodableList<Parameter>  &parameterList, bool autoplay,
                                                      const CharSpan  &data, const Optional<PlaybackPreferences> playbackPreferences,
                                                      bool useCurrentContext)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::LauncherResponse::Type response;
    response.status = StatusEnum::kSuccess;
    helper.Success(response);
}

void MockContentLauncherDelegate::HandleLaunchUrl(CommandResponseHelper<Commands::LauncherResponse::Type>  &helper, const CharSpan  &contentUrl,
                                                  const CharSpan  &displayString, const BrandingInformation  &brandingInformation)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::LauncherResponse::Type response;
    response.status = StatusEnum::kSuccess;
    helper.Success(response);
}

CHIP_ERROR MockContentLauncherDelegate::HandleGetAcceptHeaderList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

uint32_t MockContentLauncherDelegate::HandleGetSupportedStreamingProtocols()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint32_t MockContentLauncherDelegate::GetFeatureMap(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint16_t MockContentLauncherDelegate::GetClusterRevision(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace ContentLauncher
} // namespace Clusters
} // namespace app
} // namespace chip