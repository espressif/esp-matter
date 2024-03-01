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

#include <esp_check.h>
#include <esp_matter_thread_br_launcher.h>
#include <esp_netif.h>
#include <esp_openthread_border_router.h>
#include <esp_openthread_cli.h>
#include <esp_openthread_lock.h>
#include <esp_openthread_netif_glue.h>
#include <esp_openthread_types.h>
#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
#include <esp_spiffs.h>
#endif
#include <esp_vfs_dev.h>
#include <esp_vfs_eventfd.h>
#include <memory>
#include <string.h>

#include <openthread/cli.h>
#include <openthread/logging.h>

#define TAG "thread_br_launcher"

namespace esp_matter {

class scoped_thread_lock {
public:
    scoped_thread_lock() { esp_openthread_lock_acquire(portMAX_DELAY); }
    ~scoped_thread_lock() { esp_openthread_lock_release(); }
};

#if CONFIG_OPENTHREAD_CLI
static TaskHandle_t cli_transmit_task = NULL;
static QueueHandle_t cli_transmit_task_queue = NULL;

static int cli_output_callback(void *context, const char *format, va_list args)
{
    int ret = 0;
    char prompt_check[3];
    vsnprintf(prompt_check, sizeof(prompt_check), format, args);
    if (!strncmp(prompt_check, "> ", sizeof(prompt_check))) {
        if (cli_transmit_task) {
            xTaskNotifyGive(cli_transmit_task);
        }
    } else {
        ret = vprintf(format, args);
    }
    return ret;
}

static void cli_transmit_worker(void *context)
{
    cli_transmit_task_queue = xQueueCreate(4, sizeof(ot_cli_buffer_t));
    if (!cli_transmit_task_queue) {
        vTaskDelete(NULL);
        return;
    }
    while (true) {
        ot_cli_buffer_t buffer;
        if (xQueueReceive(cli_transmit_task_queue, &buffer, portMAX_DELAY) == pdTRUE) {
            esp_openthread_cli_input(buffer.buf);
            xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        }
    }
}

esp_err_t cli_transmit_task_post(ot_cli_buffer_t &cli_buf)
{
    if (!cli_transmit_task_queue || xQueueSend(cli_transmit_task_queue, &cli_buf, portMAX_DELAY) != pdTRUE) {
        return ESP_FAIL;
    }
    return ESP_OK;
}
#endif // CONFIG_OPENTHREAD_CLI

#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
#define RCP_VERSION_MAX_SIZE 100

static void update_rcp(void)
{
    // Deinit uart to transfer UART to the serial loader
    esp_openthread_rcp_deinit();
    if (esp_rcp_update() == ESP_OK) {
        esp_rcp_mark_image_verified(true);
    } else {
        esp_rcp_mark_image_verified(false);
    }
    esp_restart();
}

static void try_update_ot_rcp(void)
{
    ESP_LOGW(TAG, "Try to update openthread rcp");
    char internal_rcp_version[RCP_VERSION_MAX_SIZE];
    const char *running_rcp_version = otPlatRadioGetVersionString(esp_openthread_get_instance());

    if (esp_rcp_load_version_in_storage(internal_rcp_version, sizeof(internal_rcp_version)) == ESP_OK) {
        ESP_LOGI(TAG, "Internal RCP Version: %s", internal_rcp_version);
        ESP_LOGI(TAG, "Running  RCP Version: %s", running_rcp_version);
        if (strcmp(internal_rcp_version, running_rcp_version) == 0) {
            esp_rcp_mark_image_verified(true);
        } else {
            update_rcp();
        }
    } else {
        ESP_LOGI(TAG, "RCP firmware not found in storage, will reboot to try next image");
        esp_rcp_mark_image_verified(false);
        esp_restart();
    }
}

static void rcp_failure_handler(void)
{
    esp_rcp_mark_image_unusable();
    try_update_ot_rcp();
    esp_rcp_reset();
}

esp_err_t thread_rcp_update_init(const esp_rcp_update_config_t *update_config)
{
    return esp_rcp_update_init(update_config);
}

#endif // CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t *config = static_cast<esp_openthread_platform_config_t *>(aContext);
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *openthread_netif = esp_netif_new(&cfg);
    assert(openthread_netif != NULL);

    // Initialize the OpenThread stack
#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
    esp_openthread_register_rcp_failure_handler(rcp_failure_handler);
#endif
    ESP_ERROR_CHECK(esp_openthread_init(config));
#if CONFIG_OPENTHREAD_CLI
    otCliInit(esp_openthread_get_instance(), cli_output_callback, NULL);
#endif
    // Initialize border routing features
    esp_openthread_lock_acquire(portMAX_DELAY);
#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
    try_update_ot_rcp();
#endif
    ESP_ERROR_CHECK(esp_netif_attach(openthread_netif, esp_openthread_netif_glue_init(config)));
    ESP_ERROR_CHECK(esp_openthread_border_router_init());
#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
#endif
    otInstance *instance = esp_openthread_get_instance();
    if (otDatasetIsCommissioned(instance)) {
        (void)otIp6SetEnabled(instance, true);
        (void)otThreadSetEnabled(instance, true);
    }
    esp_openthread_lock_release();
    free(config);

#if CONFIG_OPENTHREAD_CLI
    xTaskCreate(cli_transmit_worker, "ot_cli_task", 3072, NULL, 5, &cli_transmit_task);
#endif
    esp_openthread_launch_mainloop();
    // Clean up
    esp_netif_destroy(openthread_netif);
    esp_openthread_netif_glue_deinit();
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

static esp_err_t init_spiffs(void)
{
#if CONFIG_OPENTHREAD_BR_AUTO_UPDATE_RCP
    esp_vfs_spiffs_conf_t rcp_fw_conf = {
        .base_path = "/rcp_fw", .partition_label = "rcp_fw", .max_files = 10, .format_if_mount_failed = false};
    ESP_RETURN_ON_ERROR(esp_vfs_spiffs_register(&rcp_fw_conf), TAG, "Failed to mount rcp firmware storage");
#endif
    return ESP_OK;
}

esp_err_t thread_br_init(esp_openthread_platform_config_t *config)
{
    static bool thread_br_initialized = false;
    if (thread_br_initialized) {
        return ESP_OK;
    }
    // Used eventfds:
    // * netif
    // * task_queue
    // * border router
    esp_vfs_eventfd_config_t eventfd_config = {
#if CONFIG_OPENTHREAD_RADIO_NATIVE
        // * radio driver
        .max_fds = 4,
#else
        .max_fds = 3,
#endif
    };
    ESP_RETURN_ON_ERROR(esp_vfs_eventfd_register(&eventfd_config), TAG, "Failed to register eventfd");
    ESP_ERROR_CHECK(init_spiffs());
    esp_openthread_set_backbone_netif(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));

    esp_openthread_platform_config_t *config_copy =
        (esp_openthread_platform_config_t *)malloc(sizeof(esp_openthread_platform_config_t));
    if (!config_copy) {
        ESP_LOGE(TAG, "Failed to allocate memory for openthread_platform_config");
        return ESP_ERR_NO_MEM;
    }
    memcpy(config_copy, config, sizeof(esp_openthread_platform_config_t));
    if (xTaskCreate(ot_task_worker, "ot_br", 8192, config_copy, 5, NULL) != pdTRUE) {
        free(config_copy);
        return ESP_FAIL;
    }

    thread_br_initialized = true;
    return ESP_OK;
}

esp_err_t set_thread_enabled(bool enabled)
{
    scoped_thread_lock lock;
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    bool is_enabled = (otThreadGetDeviceRole(instance) != OT_DEVICE_ROLE_DISABLED);
    bool is_ip6_enabled = otIp6IsEnabled(instance);

    if (enabled && !is_ip6_enabled) {
        ESP_RETURN_ON_FALSE(otIp6SetEnabled(instance, enabled) == OT_ERROR_NONE, ESP_FAIL, TAG, "Failed to %s netif",
                            enabled ? "enable" : "disable");
    }
    if (enabled != is_enabled) {
        ESP_RETURN_ON_FALSE(otThreadSetEnabled(instance, enabled) == OT_ERROR_NONE, ESP_FAIL, TAG,
                            "Failed to %s thread", enabled ? "enable" : "disable");
    }
    if (!enabled && is_ip6_enabled) {
        ESP_RETURN_ON_FALSE(otIp6SetEnabled(instance, enabled) == OT_ERROR_NONE, ESP_FAIL, TAG, "Failed to %s netif",
                            enabled ? "enable" : "disable");
    }
    return ESP_OK;
}

esp_err_t set_thread_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs)
{
    if (!dataset_tlvs) {
        return ESP_ERR_INVALID_ARG;
    }
    scoped_thread_lock lock;
    ESP_RETURN_ON_FALSE(otDatasetSetActiveTlvs(esp_openthread_get_instance(), dataset_tlvs) == OT_ERROR_NONE, ESP_FAIL,
                        TAG, "Failed to set Thread DatasetTlvs");
    return ESP_OK;
}

esp_err_t get_thread_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs)
{
    if (!dataset_tlvs) {
        return ESP_ERR_INVALID_ARG;
    }
    scoped_thread_lock lock;
    ESP_RETURN_ON_FALSE(otDatasetGetActiveTlvs(esp_openthread_get_instance(), dataset_tlvs) == OT_ERROR_NONE, ESP_FAIL,
                        TAG, "Failed to get Thread DatasetTlvs");
    return ESP_OK;
}

uint8_t get_thread_role()
{
    scoped_thread_lock lock;
    otDeviceRole role = otThreadGetDeviceRole(esp_openthread_get_instance());
    return role;
}

} // namespace esp_matter
