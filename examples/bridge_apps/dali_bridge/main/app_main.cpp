/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_matter.h>
#include <esp_matter_bridge.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <common_macros.h>
#include <app_priv.h>
#include <app_bridged_device.h>
#include <app_dali_bridged_device.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

static const char *TAG = "app_main";

/* Array of per-ballast context objects, indexed by DALI short address */
#define DALI_MAX_BALLASTS 64
static dali_light_ctx_t *s_ballast_ctx[DALI_MAX_BALLASTS] = {nullptr};

/* Interval between consecutive reachability polls (ms) */
#define DALI_POLL_INTERVAL_MS 5000

/* Set to true by app_event_cb when the Matter server is fully ready to
 * accept attribute::update() calls (kServerReady event received). */
static volatile bool s_matter_server_ready = false;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

constexpr auto k_timeout_seconds = 300;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
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
    case chip::DeviceLayer::DeviceEventType::kServerReady:
        ESP_LOGI(TAG, "Matter server ready — attribute updates now accepted");
        s_matter_server_ready = true;
        break;
    case chip::DeviceLayer::DeviceEventType::kFabricRemoved: {
        ESP_LOGI(TAG, "Fabric removed successfully");
        if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0) {
            chip::CommissioningWindowManager &commissionMgr =
                chip::Server::GetInstance().GetCommissioningWindowManager();
            constexpr auto kTimeoutSeconds = chip::System::Clock::Seconds16(k_timeout_seconds);
            if (!commissionMgr.IsCommissioningWindowOpen()) {
                CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(
                                     kTimeoutSeconds,
                                     chip::CommissioningWindowAdvertisement::kDnssdOnly);
                if (err != CHIP_NO_ERROR) {
                    ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT,
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

static esp_err_t app_identification_cb(identification::callback_type_t type,
                                       uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type=%u effect=%u variant=%u",
             type, effect_id, effect_variant);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(attribute::callback_type_t type,
                                         uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id,
                                         esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    if (type == PRE_UPDATE) {
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        err = app_driver_attribute_update(driver_handle, endpoint_id,
                                          cluster_id, attribute_id, val);
    }
    return err;
}

/* Reachability polling task */
static void dali_poll_task(void *arg)
{
    /* Wait for the Matter server to be fully ready before touching attributes */
    ESP_LOGI(TAG, "dali_poll_task: waiting for Matter server ready ...");
    while (!s_matter_server_ready) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ESP_LOGI(TAG, "dali_poll_task: Matter server ready, starting initial polls");

    const int WARMUP_POLLS = 3; /* REACH_CONFIRM_NEEDED(2) + 1 extra */
    for (int i = 0; i < WARMUP_POLLS; i++) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "dali_poll_task: warm-up poll %d/%d", i + 1, WARMUP_POLLS);
        app_driver_dali_poll_reachability(s_ballast_ctx, DALI_MAX_BALLASTS);
    }

    ESP_LOGI(TAG, "dali_poll_task: pushing final state to all endpoints");
    app_driver_dali_push_all_states(s_ballast_ctx, DALI_MAX_BALLASTS);

    ESP_LOGI(TAG, "dali_poll_task: entering steady-state polling every %d ms", DALI_POLL_INTERVAL_MS);
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(DALI_POLL_INTERVAL_MS));
        app_driver_dali_poll_reachability(s_ballast_ctx, DALI_MAX_BALLASTS);
    }
}

/* app_bridge passes the bridged device before set_dev_addr(). Extended Color Light
 * is selected only when the gear reports Tc, so CT is the default until an address
 * is already attached. */
static bool dali_extended_color_prefers_ct(void *priv_data)
{
    app_bridged_device_t *bridged_device = (app_bridged_device_t *)priv_data;
    if (!bridged_device) {
        return true;
    }
    dali_device_addr_t *addr = (dali_device_addr_t *)bridged_device->get_dev_addr();
    if (addr) {
        return (addr->color_caps & (1U << 1)) != 0;
    }
    dali_light_ctx_t *ctx = (dali_light_ctx_t *)bridged_device->get_priv_data();
    if (ctx) {
        return (ctx->color_caps & (1U << 1)) != 0;
    }
    return true;
}

static dali_light_ctx_t *dali_light_ctx_alloc(uint8_t addr, uint8_t color_caps, dali_device_type_t dev_type)
{
    dali_light_ctx_t *ctx = (dali_light_ctx_t *)calloc(1, sizeof(dali_light_ctx_t));
    if (!ctx) {
        return nullptr;
    }
    ctx->dali_addr = addr;
    ctx->power = DEFAULT_POWER;
    ctx->dali_level = 0;
    ctx->reachable = false;
    ctx->color_caps = color_caps;
    ctx->dev_type = dev_type;
    /* Match Matter's default ColorControl values so an untouched lamp is not
     * replayed. 0x616B/0x607D are the data-model defaults for CurrentX/CurrentY. */
    ctx->color_temp_mirek = DEFAULT_COLOR_TEMP_MIREK;
    ctx->color_x = 0x616B;
    ctx->color_y = 0x607D;
    return ctx;
}

static dali_device_type_t dali_dev_type_from_matter(uint32_t device_type_id)
{
    if (device_type_id == ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID ||
            device_type_id == ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID) {
        return DALI_DEV_TYPE_DT8;
    }
    return DALI_DEV_TYPE_DT6;
}

static const char *dali_matter_type_name(uint32_t device_type_id)
{
    if (device_type_id == ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID) {
        return "DT8 ExtColorLight";
    }
    if (device_type_id == ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID) {
        return "DT8 ColorTempLight";
    }
    return "DT6 DimmableLight";
}

static esp_err_t dali_bridge_device_type_callback(esp_matter::endpoint_t *ep,
                                                  uint32_t device_type_id,
                                                  void *priv_data)
{
    esp_err_t err = ESP_OK;

    switch (device_type_id) {
    case ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID: {
        esp_matter::endpoint::dimmable_light::config_t dimmable_light_cfg;
        dimmable_light_cfg.on_off.on_off = DEFAULT_POWER;
        dimmable_light_cfg.on_off_lighting.start_up_on_off = nullptr;
        dimmable_light_cfg.level_control.current_level = DEFAULT_BRIGHTNESS;
        dimmable_light_cfg.level_control.on_level = UINT8_MAX;
        dimmable_light_cfg.level_control_lighting.start_up_current_level = UINT8_MAX;
        err = dimmable_light::add(ep, &dimmable_light_cfg);
        break;
    }
    case ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID: {
        esp_matter::endpoint::extended_color_light::config_t extended_color_light_cfg;
        extended_color_light_cfg.on_off.on_off = DEFAULT_POWER;
        extended_color_light_cfg.on_off_lighting.start_up_on_off = nullptr;
        extended_color_light_cfg.level_control.current_level = DEFAULT_BRIGHTNESS;
        extended_color_light_cfg.level_control.on_level = UINT8_MAX;
        extended_color_light_cfg.level_control_lighting.start_up_current_level = UINT8_MAX;
        extended_color_light_cfg.color_control_color_temperature.color_temperature_mireds = DEFAULT_COLOR_TEMP_MIREK;
        extended_color_light_cfg.color_control_color_temperature.color_temp_physical_min_mireds = COLOR_TEMP_MIN_MIREK;
        extended_color_light_cfg.color_control_color_temperature.color_temp_physical_max_mireds = COLOR_TEMP_MAX_MIREK;

        if (dali_extended_color_prefers_ct(priv_data)) {
            extended_color_light_cfg.color_control.color_mode = 2; /* CT */
            extended_color_light_cfg.color_control.enhanced_color_mode = 2;
        } else {
            extended_color_light_cfg.color_control.color_mode = 1; /* XY */
            extended_color_light_cfg.color_control.enhanced_color_mode = 1;
        }

        err = extended_color_light::add(ep, &extended_color_light_cfg);
        if (err == ESP_OK) {
            cluster_t *cc_cluster = esp_matter::cluster::get(endpoint::get_id(ep), ColorControl::Id);
            if (cc_cluster) {
                esp_matter::cluster::color_control::feature::hue_saturation::config_t hs_cfg;
                hs_cfg.current_hue = DEFAULT_HUE;
                hs_cfg.current_saturation = DEFAULT_SATURATION;
                err = esp_matter::cluster::color_control::feature::hue_saturation::add(cc_cluster, &hs_cfg);
            }
        }
        break;
    }
    case ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID: {
        esp_matter::endpoint::color_temperature_light::config_t color_temperature_light_cfg;
        color_temperature_light_cfg.on_off.on_off = DEFAULT_POWER;
        color_temperature_light_cfg.on_off_lighting.start_up_on_off = nullptr;
        color_temperature_light_cfg.level_control.current_level = DEFAULT_BRIGHTNESS;
        color_temperature_light_cfg.level_control.on_level = UINT8_MAX;
        color_temperature_light_cfg.level_control_lighting.start_up_current_level = UINT8_MAX;
        color_temperature_light_cfg.color_control_color_temperature.color_temperature_mireds = DEFAULT_COLOR_TEMP_MIREK;
        color_temperature_light_cfg.color_control_color_temperature.color_temp_physical_min_mireds =
            COLOR_TEMP_MIN_MIREK;
        color_temperature_light_cfg.color_control_color_temperature.color_temp_physical_max_mireds =
            COLOR_TEMP_MAX_MIREK;
        color_temperature_light_cfg.color_control.color_mode = 2; /* CT */
        color_temperature_light_cfg.color_control.enhanced_color_mode = 2;
        err = color_temperature_light::add(ep, &color_temperature_light_cfg);
        break;
    }
    default:
        ESP_LOGE(TAG, "Unsupported bridged Matter device type 0x%lX", (unsigned long)device_type_id);
        return ESP_ERR_INVALID_ARG;
    }

    if (err != ESP_OK) {
        return err;
    }

    attribute_t *level_attr = attribute::get(endpoint::get_id(ep),
                                             LevelControl::Id,
                                             LevelControl::Attributes::CurrentLevel::Id);
    if (level_attr) {
        attribute::set_deferred_persistence(level_attr);
    }
    return ESP_OK;
}

static int dali_bridge_bind_resumed_devices(void)
{
    uint16_t endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    if (esp_matter_bridge::get_bridged_endpoint_ids(endpoint_id_array) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get bridged endpoint id array");
        return 0;
    }

    int resumed = 0;
    for (size_t i = 0; i < MAX_BRIDGED_DEVICE_COUNT; i++) {
        uint16_t endpoint_id = endpoint_id_array[i];
        if (endpoint_id == chip::kInvalidEndpointId) {
            continue;
        }

        app_bridged_device_t *bridged_device = app_bridge_get_device(endpoint_id);
        dali_device_addr_t *addr = bridged_device ? (dali_device_addr_t *)bridged_device->get_dev_addr() : nullptr;
        esp_matter_bridge::device_t *matter_device = bridged_device ? bridged_device->get_matter_device() : nullptr;
        if (!addr || !matter_device || addr->short_addr >= DALI_MAX_BALLASTS) {
            ESP_LOGW(TAG, "No DALI address for bridged endpoint %u", endpoint_id);
            continue;
        }
        if (s_ballast_ctx[addr->short_addr]) {
            continue;
        }

        uint32_t device_type_id = matter_device->persistent_info.device_type_id;
        dali_light_ctx_t *ctx = dali_light_ctx_alloc(addr->short_addr, addr->color_caps,
                                                     dali_dev_type_from_matter(device_type_id));
        if (!ctx) {
            ESP_LOGE(TAG, "OOM allocating ctx for bridged endpoint %u", endpoint_id);
            continue;
        }
        ctx->endpoint_id = endpoint_id;
        bridged_device->set_priv_data(ctx);
        s_ballast_ctx[addr->short_addr] = ctx;
        resumed++;

        ESP_LOGI(TAG, "Resumed bridged endpoint %u <-> DALI addr=%u (%s caps=0x%02X)",
                 ctx->endpoint_id, addr->short_addr, dali_matter_type_name(device_type_id), ctx->color_caps);
    }
    return resumed;
}

static esp_err_t dali_bridge_create_devices(node_t *node, uint16_t aggregator_endpoint_id,
                                            uint64_t present_mask,
                                            const dali_device_type_t dev_types[64],
                                            const uint8_t color_caps[64],
                                            int *created_out)
{
    int created = 0;

    for (uint8_t addr = 0; addr < 64; addr++) {
        if (!(present_mask & (1ULL << addr))) {
            continue;
        }

        dali_device_addr_t lookup = {
            .short_addr = addr,
            .color_caps = 0,
        };
        if (s_ballast_ctx[addr] || app_bridge_get_device(&lookup)) {
            continue;
        }

        uint8_t caps = (color_caps && dev_types[addr] == DALI_DEV_TYPE_DT8) ? color_caps[addr] : 0;
        uint32_t device_type_id = ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
        /* Extended Color Light always exposes CT and XY. Require Tc plus RGBWAF
         * so those features match the gear. Primary-N is not RGB. */
        if (dev_types[addr] == DALI_DEV_TYPE_DT8 &&
                (((caps >> 5) & 0x07) != 0) &&
                ((caps & (1U << 1)) != 0)) {
            device_type_id = ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID;
        } else if (dev_types[addr] == DALI_DEV_TYPE_DT8 && (caps & (1U << 1)) != 0) {
            device_type_id = ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID;
        }

        dali_light_ctx_t *ctx = dali_light_ctx_alloc(addr, caps, dali_dev_type_from_matter(device_type_id));
        if (!ctx) {
            ESP_LOGE(TAG, "OOM allocating ctx for DALI addr %u", addr);
            return ESP_ERR_NO_MEM;
        }

        dali_device_addr_t dev_addr = {
            .short_addr = addr,
            .color_caps = caps,
        };
        esp_err_t err = app_bridge_create_new_device(node, aggregator_endpoint_id, device_type_id, &dev_addr, ctx);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create bridged device for DALI addr %u: %s", addr, esp_err_to_name(err));
            free(ctx);
            continue;
        }

        app_bridged_device_t *bridged_device = app_bridge_get_device(&dev_addr);
        if (!bridged_device || !bridged_device->get_matter_device()) {
            ESP_LOGE(TAG, "Created DALI addr %u but the bridged device is missing", addr);
            continue;
        }
        /* app_bridge_create_new_device() only warns when the address blob cannot be
         * stored. A missing blob makes the next boot fail inside restore_dev_addr(). */
        if (bridged_device->store_dev_addr() != ESP_OK) {
            ESP_LOGW(TAG, "Failed to store DALI mapping for addr %u", addr);
            app_bridge_remove_device(bridged_device);
            continue;
        }
        ctx->endpoint_id = endpoint::get_id(bridged_device->get_matter_device()->endpoint);
        s_ballast_ctx[addr] = ctx;

        ESP_LOGI(TAG, "Created bridged endpoint %u <-> DALI addr=%u (%s caps=0x%02X)",
                 ctx->endpoint_id, addr, dali_matter_type_name(device_type_id), ctx->color_caps);
        created++;
    }

    if (created_out) {
        *created_out = created;
    }
    return ESP_OK;
}

static app_bridged_device_t *create_dali_bridged_device(node_t *node, uint16_t endpoint)
{
    return chip::Platform::New<app_dali_bridged_device_t>();
}

static void free_dali_bridged_device(app_bridged_device_t *device)
{
    if (device) {
        dali_light_ctx_t *ctx = (dali_light_ctx_t *)device->get_priv_data();
        if (ctx) {
            if (ctx->dali_addr < DALI_MAX_BALLASTS && s_ballast_ctx[ctx->dali_addr] == ctx) {
                s_ballast_ctx[ctx->dali_addr] = nullptr;
            }
            free(ctx);
            device->set_priv_data(nullptr);
        }
    }
    chip::Platform::Delete((app_dali_bridged_device_t *)device);
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize NVS (required by Matter) */
    nvs_flash_init();

    /* Initialise DALI RMT driver */
    app_driver_handle_t dali_handle = app_driver_dali_init();
    ABORT_APP_ON_FAILURE(dali_handle != nullptr,
                         ESP_LOGE(TAG, "Failed to initialize DALI driver"));

    /* Scan DALI bus for present ballasts (with device-type detection) */
    uint64_t present_mask = 0;
    dali_device_type_t dali_dev_types[64];
    uint8_t dali_color_caps[64];
    err = app_driver_dali_scan_with_type(&present_mask, dali_dev_types, dali_color_caps);
    ABORT_APP_ON_FAILURE(err == ESP_OK,
                         ESP_LOGE(TAG, "DALI bus scan failed: %d", err));

    if (present_mask == 0) {
        ESP_LOGW(TAG, "No DALI control gear found — resuming persisted bridged endpoints only");
    }

    /* Create Matter node */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb,
                                app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr,
                         ESP_LOGE(TAG, "Failed to create Matter node"));

    /* Create the Aggregator endpoint (parent of all bridged devices) */
    aggregator::config_t aggregator_config;
    endpoint_t *aggregator = aggregator::create(node, &aggregator_config, ENDPOINT_FLAG_NONE, nullptr);
    ABORT_APP_ON_FAILURE(aggregator != nullptr,
                         ESP_LOGE(TAG, "Failed to create aggregator endpoint"));
    uint16_t aggregator_endpoint_id = endpoint::get_id(aggregator);
    ESP_LOGI(TAG, "Aggregator created with endpoint id %u", aggregator_endpoint_id);

    {
        nvs_handle_t handle;
        esp_err_t err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, "esp_matter_kvs",
                                                NVS_READWRITE, &handle);
        if (err == ESP_OK) {
            uint16_t stored_ep_id = 0;
            err = nvs_get_u16(handle, "min_uu_ep_id", &stored_ep_id);
            if (err == ESP_OK && stored_ep_id <= 1) {
                nvs_erase_key(handle, "min_uu_ep_id");
                nvs_commit(handle);
                ESP_LOGW(TAG, "Removed stale min_uu_ep_id=%u from NVS (would collide with endpoint id 1)",
                         stored_ep_id);
            }
            nvs_close(handle);
        }
    }

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && CHIP_DEVICE_CONFIG_ENABLE_WIFI_STATION
    secondary_network_interface::config_t secondary_net_cfg;
    endpoint_t *sec_ep = endpoint::secondary_network_interface::create(
                             node, &secondary_net_cfg, ENDPOINT_FLAG_NONE, nullptr);
    ABORT_APP_ON_FAILURE(sec_ep != nullptr,
                         ESP_LOGE(TAG, "Failed to create secondary network interface endpoint"));
#endif

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    esp_openthread_platform_config_t ot_config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&ot_config);
#endif

    /* Start Matter stack */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK,
                         ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

    err = app_bridge_initialize(node, dali_bridge_device_type_callback, create_dali_bridged_device,
                                free_dali_bridged_device);
    ABORT_APP_ON_FAILURE(err == ESP_OK,
                         ESP_LOGE(TAG, "Failed to resume the bridged endpoints: %d", err));

    int resumed = dali_bridge_bind_resumed_devices();

    int created = 0;
    if (present_mask != 0) {
        err = dali_bridge_create_devices(node, aggregator_endpoint_id, present_mask,
                                         dali_dev_types, dali_color_caps, &created);
        ABORT_APP_ON_FAILURE(err == ESP_OK,
                             ESP_LOGE(TAG, "Failed to create bridged DALI light endpoints: %d", err));
    }
    ESP_LOGI(TAG, "Bridged DALI lights: %d resumed from NVS, %d new created", resumed, created);

    /* Start DALI reachability polling task */
    BaseType_t task_rc = xTaskCreate(dali_poll_task, "dali_poll", 6144, nullptr, 5, nullptr);
    ABORT_APP_ON_FAILURE(task_rc == pdPASS,
                         ESP_LOGE(TAG, "Failed to create dali_poll task"));

    /* Initial DALI reachability state is applied by dali_poll_task after the
     * Matter server reports ready.  Each context already caches its endpoint ID. */

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::attribute_register_commands();
#if CONFIG_OPENTHREAD_CLI
    esp_matter::console::otcli_register_commands();
#endif
    esp_matter::console::init();
#endif

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
