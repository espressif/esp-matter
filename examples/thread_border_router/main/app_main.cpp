/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_openthread_lock.h>
#include <esp_openthread_border_router.h>
#include <esp_spiffs.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_feature.h>

#include <app_reset.h>
#include <esp_ot_config.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_OPENTHREAD_BORDER_ROUTER) && defined(CONFIG_AUTO_UPDATE_RCP)
#include <esp_ot_rcp_update.h>
#include <esp_rcp_update.h>
#include <platform/ThreadStackManager.h>
#endif

#include <platform/ESP32/OpenthreadLauncher.h>
#include <platform/OpenThread/GenericThreadBorderRouterDelegate.h>
#include <app/server/Server.h>
#include <credentials/FabricTable.h>
#include <platform/KvsPersistentStorageDelegate.h>

static const char *TAG = "app_main";

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;
using chip::app::Clusters::ThreadBorderRouterManagement::GenericOpenThreadBorderRouterDelegate;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::PublicEventTypes::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;
    case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
        if (event->Platform.ESPSystemEvent.Base == IP_EVENT &&
                event->Platform.ESPSystemEvent.Id == IP_EVENT_STA_GOT_IP) {
#if CONFIG_OPENTHREAD_BORDER_ROUTER
            static bool sThreadBRInitialized = false;
            if (!sThreadBRInitialized) {
                esp_openthread_set_backbone_netif(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"));
                esp_openthread_lock_acquire(portMAX_DELAY);
                esp_openthread_border_router_init();
                esp_openthread_lock_release();
                sThreadBRInitialized = true;
            }
#endif
        }
        break;
    default:
        break;
    }
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();
    // If there is no commissioner in the controller, we need a default node so that the controller can be commissioned
    // to a specific fabric.
    node::config_t node_config;
    node_t *node = node::create(&node_config, NULL, NULL);
    static chip::KvsPersistentStorageDelegate tbr_storage_delegate;
    chip::DeviceLayer::PersistedStorage::KeyValueStoreManager  &kvsManager = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr();
    LogErrorOnFailure(tbr_storage_delegate.Init(&kvsManager));
    GenericOpenThreadBorderRouterDelegate *delegate = chip::Platform::New<GenericOpenThreadBorderRouterDelegate>(&tbr_storage_delegate);
    char threadBRName[] = "Espressif-ThreadBR";
    delegate->SetThreadBorderRouterName(chip::CharSpan(threadBRName));
    if (!delegate) {
        ESP_LOGE(TAG, "Failed to create thread_border_router delegate");
        return;
    }
    thread_border_router::config_t tbr_config;
    tbr_config.thread_border_router_management.delegate = delegate;
    endpoint_t *tbr_endpoint = thread_border_router::create(node, &tbr_config, ENDPOINT_FLAG_NONE, NULL);
    if (!node || !tbr_endpoint) {
        ESP_LOGE(TAG, "Failed to create data model");
        return;
    }
    cluster_t *tbr_cluster = cluster::get(tbr_endpoint, ThreadBorderRouterManagement::Id);
    cluster::thread_border_router_management::feature::pan_change::add(tbr_cluster);
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#if defined(CONFIG_OPENTHREAD_BORDER_ROUTER) && defined(CONFIG_AUTO_UPDATE_RCP)
    esp_vfs_spiffs_conf_t rcp_fw_conf = {
        .base_path = "/rcp_fw", .partition_label = "rcp_fw", .max_files = 10, .format_if_mount_failed = false
    };
    if (ESP_OK != esp_vfs_spiffs_register(&rcp_fw_conf)) {
        ESP_LOGE(TAG, "Failed to mount rcp firmware storage");
        return;
    }
    esp_rcp_update_config_t rcp_update_config = ESP_OPENTHREAD_RCP_UPDATE_CONFIG();
    esp_rcp_update_init(&rcp_update_config);
    esp_ot_register_rcp_handler();
#endif // CONFIG_OPENTHREAD_BORDER_ROUTER && CONFIG_AUTO_UPDATE_RCP
    /* Set OpenThread platform config */
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_OPENTHREAD_BORDER_ROUTER) && defined(CONFIG_AUTO_UPDATE_RCP)
    if (err == ESP_OK) {
        esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
        using namespace chip::DeviceLayer;
        bool thread_was_enabled = ThreadStackMgr().IsThreadEnabled();
        if (thread_was_enabled) {
            if (ThreadStackMgr().SetThreadEnabled(false) != CHIP_NO_ERROR) {
                ESP_LOGE(TAG, "Failed to disable Thread before updating RCP");
                return;
            }
        }
        esp_ot_update_rcp_if_different();
        if (thread_was_enabled) {
            if (ThreadStackMgr().SetThreadEnabled(true) != CHIP_NO_ERROR) {
                ESP_LOGE(TAG, "Failed to enable Thread after updating RCP");
                return;
            }
        }
    }
#endif
#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif // CONFIG_ENABLE_CHIP_SHELL
}
