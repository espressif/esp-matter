/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_insights.h>
#include <esp_rmaker_utils.h>

esp_err_t app_insights_enable(const char *auth_key)
{
    // Sync the time with the NTP server
    esp_rmaker_time_sync_init(NULL);

    esp_insights_config_t config = {
        .log_type = ESP_DIAG_LOG_TYPE_ERROR | ESP_DIAG_LOG_TYPE_WARNING,
        .auth_key = auth_key,
    };
    return esp_insights_init(&config);
}
