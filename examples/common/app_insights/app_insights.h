/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#if __cplusplus
extern "C" {
#endif

#if CONFIG_ESP_MATTER_ENABLE_INSIGHTS

/**
 * @brief Initialize the app insights in matter example
 *
 * @param[in] auth_key The app insights auth key
 *
 * @return ESP_OK on success, appropriate error code otherwise
 */
esp_err_t app_insights_enable(const char *auth_key);

#endif // CONFIG_ESP_MATTER_ENABLE_INSIGHTS

#if __cplusplus
}
#endif
