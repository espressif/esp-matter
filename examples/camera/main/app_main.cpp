/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <app_priv.h>
#include <app_reset.h>
#include <common_macros.h>
#include <esp_wifi.h>

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

#if CONFIG_ENABLE_SNTP_TIME_SYNC
#include <app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h>
#endif

#include "camera-app.h"
#include "camera-device.h"

#include "esp_webrtc_time.h"
#include "esp_work_queue.h"
#include "signaling_serializer.h"
#include "webrtc_bridge.h"

extern "C" void network_coprocessor_init();

static const char *TAG = "app_main";
uint16_t camera_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;
using namespace Camera;

CameraDevice gCameraDevice;

constexpr auto k_timeout_seconds = 300;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        esp_webrtc_time_sntp_time_sync_no_wait();
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricRemoved: {
        ESP_LOGI(TAG, "Fabric removed successfully");
        if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0) {
            chip::CommissioningWindowManager &commissionMgr =
                chip::Server::GetInstance().GetCommissioningWindowManager();
            constexpr auto kTimeoutSeconds =
                chip::System::Clock::Seconds16(k_timeout_seconds);
            if (!commissionMgr.IsCommissioningWindowOpen()) {
                /* After removing last fabric, this example does not remove the Wi-Fi
                 * credentials and still has IP connectivity so, only advertising on
                 * DNS-SD.
                 */
                CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(
                                     kTimeoutSeconds,
                                     chip::CommissioningWindowAdvertisement::kDnssdOnly);
                if (err != CHIP_NO_ERROR) {
                    ESP_LOGE(
                        TAG,
                        "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT,
                        err.Format());
                }
            }
        }
        break;
    }

    case chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved:
        ESP_LOGI(TAG, "Fabric will be removed");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricUpdated:
        ESP_LOGI(TAG, "Fabric is updated");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricCommitted:
        ESP_LOGI(TAG, "Fabric is committed");
        break;

    case chip::DeviceLayer::DeviceEventType::kBLEDeinitialized:
        ESP_LOGI(TAG, "BLE deinitialized and memory reclaimed");
        break;

    default:
        break;
    }
}

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by
// flashing an LED or light).
static esp_err_t app_identification_cb(identification::callback_type_t type,
                                       uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant,
                                       void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u",
             type, effect_id, effect_variant);
    return ESP_OK;
}

// This callback is called for every attribute update. The callback
// implementation shall handle the desired attributes and return an appropriate
// error code. If the attribute is not of your interest, please do not return an
// error code and strictly return ESP_OK.
static esp_err_t
app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id,
                        uint32_t cluster_id, uint32_t attribute_id,
                        esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Driver update */
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
    }

    return err;
}

#ifdef CONFIG_SLAVE_LWIP_ENABLED
static void create_slave_sta_netif(uint8_t dhcp_at_slave)
{
    /* Create "almost" default station, but with un-flagged DHCP client */
    esp_netif_inherent_config_t netif_cfg;
    memcpy(&netif_cfg, ESP_NETIF_BASE_DEFAULT_WIFI_STA, sizeof(netif_cfg));

    if (!dhcp_at_slave) {
        netif_cfg.flags =
            (esp_netif_flags_t)(netif_cfg.flags & ~ESP_NETIF_DHCP_CLIENT);
    }

    esp_netif_config_t cfg_sta = {
        .base = &netif_cfg,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA,
    };
    esp_netif_t *netif_sta = esp_netif_new(&cfg_sta);
    assert(netif_sta);

    ESP_ERROR_CHECK(esp_netif_attach_wifi_station(netif_sta));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_sta_handlers());

    if (!dhcp_at_slave) {
        ESP_ERROR_CHECK(esp_netif_dhcpc_stop(netif_sta));
    }

    // slave_sta_netif = netif_sta;
}
#endif

void sdp_mem_dump()
{
    printf("\tDescription\tInternal\tSPIRAM\n");
    printf("Total Memory\t\t%d\t\t%d\n",
           heap_caps_get_total_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_total_size(MALLOC_CAP_SPIRAM));
    printf("Current Free Memory\t%d\t\t%d\n",
           heap_caps_get_free_size(MALLOC_CAP_8BIT) -
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf(
        "Largest Free Block\t%d\t\t%d\n",
        heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
        heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    printf("Min. Ever Free Size\t%d\t\t%d\n",
           heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef CONFIG_SLAVE_LWIP_ENABLED
    create_slave_sta_netif(true);
#endif

    signaling_serializer_init();
    network_coprocessor_init();

    esp_work_queue_init();
    esp_work_queue_start();

    webrtc_bridge_start();

    /* Initialize driver */
    app_driver_handle_t button_handle = app_driver_button_init();
    app_reset_button_register(button_handle);

    /* Create a Matter node and add the mandatory Root Node device type on
     * endpoint 0 */
    node::config_t node_config;

    // node handle can be used to add/modify other endpoints.
    node_t *node = node::create(&node_config, app_attribute_update_cb,
                                app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr,
                         ESP_LOGE(TAG, "Failed to create Matter node"));

    camera::config_t cam_config;

#if CONFIG_ENABLE_SNTP_TIME_SYNC
    static chip::app::Clusters::TimeSynchronization::DefaultTimeSyncDelegate
    time_sync_delegate;
    // cam_config.time_synchronization.delegate = &time_sync_delegate;
#endif

    // endpoint handles can be used to add/modify clusters.

    cam_config.camera_av_stream_management.feature_flags = cluster::camera_av_stream_management::feature::audio::get_id() | cluster::camera_av_stream_management::feature::video::get_id() | cluster::camera_av_stream_management::feature::snapshot::get_id();

    endpoint_t *endpoint =
        camera::create(node, &cam_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(endpoint != nullptr,
                         ESP_LOGE(TAG, "Failed to create camera endpoint"));

#if CONFIG_ENABLE_SNTP_TIME_SYNC

    cluster::time_synchronization::feature::time_zone::config_t tz_cfg;
    cluster_t *time_sync_cluster =
        cluster::get(endpoint, TimeSynchronization::Id);
    cluster::time_synchronization::feature::time_zone::add(time_sync_cluster,
                                                           &tz_cfg);

#endif

    camera_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Camera created with endpoint_id %d", camera_endpoint_id);

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK,
                         ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

    ESP_LOGW("Camera", "ESP Matter Camera App: ApplicationInit()");

    lock::ScopedChipStackLock lock(portMAX_DELAY);
    gCameraDevice.Init();
    CameraAppInit(&gCameraDevice);

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif
}
