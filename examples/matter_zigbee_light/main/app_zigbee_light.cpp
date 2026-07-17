/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <app_priv.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_zigbee.h>
#include <ezbee/zha.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "app_zigbee_light";

#define ESP_ZIGBEE_PRIMARY_CHANNEL_MASK (1U << 13)
#define ESP_ZIGBEE_SECONDARY_CHANNEL_MASK 0x07FFF800
#define ESP_ZIGBEE_HA_COLOR_DIMMABLE_LIGHT_EP_ID (10)
#define ESP_MANUFACTURER_NAME "\x09""ESPRESSIF"
#define ESP_MODEL_IDENTIFIER "\x07" CONFIG_IDF_TARGET

#define ESP_ZIGBEE_DEFAULT_CONFIG()                                                      \
    {                                                                                    \
        .device_config =                                                                 \
            {                                                                            \
                .device_type = EZB_NWK_DEVICE_TYPE_ROUTER,                               \
                .install_code_policy = false,                                            \
                .zczr_config = {.max_children = 10},                                     \
            },                                                                           \
        .platform_config =                                                               \
            {                                                                            \
                .storage_partition_name = "nvs",                                         \
                .radio_config = {.radio_mode = ESP_ZIGBEE_RADIO_MODE_NATIVE},            \
            },                                                                           \
    }

static TaskHandle_t s_zb_task = nullptr;
static app_zigbee_network_joined_cb_t s_network_joined_cb = nullptr;
static esp_timer_handle_t s_commission_retry_timer = nullptr;
static ezb_bdb_comm_mode_mask_t s_commission_retry_mode = 0;

static void commission_retry_cb(void *arg)
{
    (void)arg;
    if (!esp_zigbee_is_started()) {
        return;
    }
    esp_zigbee_lock_acquire(portMAX_DELAY);
    (void)ezb_bdb_start_top_level_commissioning(s_commission_retry_mode);
    esp_zigbee_lock_release();
}

static void schedule_commission_retry(ezb_bdb_comm_mode_mask_t mode)
{
    s_commission_retry_mode = mode;
    if (s_commission_retry_timer == nullptr) {
        const esp_timer_create_args_t args = {
            .callback = commission_retry_cb,
            .name = "zb_comm_retry",
        };
        ESP_ERROR_CHECK(esp_timer_create(&args, &s_commission_retry_timer));
    }
    esp_timer_stop(s_commission_retry_timer);
    esp_timer_start_once(s_commission_retry_timer, 3000000);
}

static void light_set_on_off_attribute(const ezb_zcl_attribute_t *attribute)
{
    ESP_RETURN_ON_FALSE(attribute,, TAG, "attribute is invalid");
    switch (attribute->id) {
    case EZB_ZCL_ATTR_ON_OFF_ON_OFF_ID: {
        app_driver_handle_t handle = app_driver_light_get_handle();
        ESP_RETURN_ON_FALSE(handle,, TAG, "light handle is invalid");
        esp_matter_attr_val_t val = esp_matter_bool(*(uint8_t *)attribute->data.value != 0);
        app_driver_light_set_power(handle, &val);
        break;
    }
    default:
        ESP_LOGW(TAG, "Unsupported attribute ID(0x%04x)", attribute->id);
        break;
    }
}

static void light_set_level_attribute(const ezb_zcl_attribute_t *attribute)
{
    ESP_RETURN_ON_FALSE(attribute,, TAG, "attribute is invalid");
    switch (attribute->id) {
    case EZB_ZCL_ATTR_LEVEL_CURRENT_LEVEL_ID: {
        app_driver_handle_t handle = app_driver_light_get_handle();
        ESP_RETURN_ON_FALSE(handle,, TAG, "light handle is invalid");
        esp_matter_attr_val_t val = esp_matter_uint8(*(uint8_t *)attribute->data.value);
        app_driver_light_set_brightness(handle, &val);
        break;
    }
    default:
        ESP_LOGW(TAG, "Unsupported attribute ID(0x%04x)", attribute->id);
        break;
    }
}

static void light_set_color_attribute(const ezb_zcl_attribute_t *attribute)
{
    static uint16_t cur_color_x = 0;
    static uint16_t cur_color_y = 0;
    static uint16_t new_color_x = 0;
    static uint16_t new_color_y = 0;

    ESP_RETURN_ON_FALSE(attribute,, TAG, "attribute is invalid");

    switch (attribute->id) {
    case EZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID:
        new_color_x = *(uint16_t *)attribute->data.value;
        break;
    case EZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID:
        new_color_y = *(uint16_t *)attribute->data.value;
        break;
    default:
        ESP_LOGW(TAG, "Unsupported attribute ID(0x%04x)", attribute->id);
        return;
    }

    if (new_color_x == cur_color_x && new_color_y == cur_color_y) {
        return;
    }

    cur_color_x = new_color_x;
    cur_color_y = new_color_y;
    app_driver_handle_t handle = app_driver_light_get_handle();
    if (handle) {
        app_driver_light_set_xy(handle, cur_color_x, cur_color_y);
    }
}

static void zcl_set_attr_value_handler(ezb_zcl_set_attr_value_message_t *message)
{
    ESP_RETURN_ON_FALSE(message,, TAG, "message is empty");
    switch (message->info.cluster_id) {
    case EZB_ZCL_CLUSTER_ID_ON_OFF:
        light_set_on_off_attribute(&message->in.attribute);
        break;
    case EZB_ZCL_CLUSTER_ID_LEVEL:
        light_set_level_attribute(&message->in.attribute);
        break;
    case EZB_ZCL_CLUSTER_ID_COLOR_CONTROL:
        light_set_color_attribute(&message->in.attribute);
        break;
    default:
        ESP_LOGW(TAG, "Unsupported cluster ID(0x%04x)", message->info.cluster_id);
        break;
    }
}

extern "C" void zcl_core_action_handler(ezb_zcl_core_action_callback_id_t callback_id, void *message)
{
    switch (callback_id) {
    case EZB_ZCL_CORE_SET_ATTR_VALUE_CB_ID:
        zcl_set_attr_value_handler((ezb_zcl_set_attr_value_message_t *)message);
        break;
    default:
        break;
    }
}

extern "C" bool app_signal_handler(const ezb_app_signal_t *app_signal)
{
    ezb_app_signal_type_t signal_type = ezb_app_signal_get_type(app_signal);

    switch (signal_type) {
    case EZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Initialize Zigbee stack");
        ezb_bdb_start_top_level_commissioning(EZB_BDB_MODE_INITIALIZATION);
        break;
    case EZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case EZB_BDB_SIGNAL_DEVICE_REBOOT: {
        ezb_bdb_comm_status_t status = *((ezb_bdb_comm_status_t *)ezb_app_signal_get_params(app_signal));
        if (status == EZB_BDB_STATUS_SUCCESS) {
            if (ezb_bdb_is_factory_new()) {
                ESP_LOGI(TAG, "Running in dual commissioning mode (Matter + Zigbee)");
                ezb_bdb_start_top_level_commissioning(EZB_BDB_MODE_NETWORK_STEERING);
            } else {
                ESP_LOGI(TAG, "Running in Zigbee mode");
                if (s_network_joined_cb) {
                    s_network_joined_cb();
                }
            }
        } else if (esp_zigbee_is_started()) {
            ESP_LOGW(TAG, "%s failed with status(0x%02x), retry again", ezb_app_signal_to_string(signal_type), status);
            schedule_commission_retry(EZB_BDB_MODE_INITIALIZATION);
        }
    } break;
    case EZB_BDB_SIGNAL_STEERING: {
        ezb_bdb_comm_status_t status = *((ezb_bdb_comm_status_t *)ezb_app_signal_get_params(app_signal));
        if (status == EZB_BDB_STATUS_SUCCESS) {
            ezb_extpanid_t extended_pan_id;
            ezb_nwk_get_extended_panid(&extended_pan_id);
            ESP_LOGI(TAG, "Joined network: PAN ID(0x%04hx, EXT: 0x%llx), Channel(%d), Short Address(0x%04hx)",
                     ezb_nwk_get_panid(), extended_pan_id.u64, ezb_nwk_get_current_channel(), ezb_nwk_get_short_address());
            if (s_network_joined_cb) {
                s_network_joined_cb();
            }
        } else if (esp_zigbee_is_started()) {
            ESP_LOGW(TAG, "Failed to join network with status(0x%02x)", status);
            schedule_commission_retry(EZB_BDB_MODE_NETWORK_STEERING);
        }
    } break;
    default:
        ESP_LOGI(TAG, "Zigbee APP Signal: %s(type: 0x%02x)", ezb_app_signal_to_string(signal_type), signal_type);
        break;
    }
    return true;
}

static esp_err_t create_color_dimmable_light(void)
{
    ezb_af_device_desc_t dev_desc = ezb_af_create_device_desc();
    ezb_zha_color_dimmable_light_config_t light_cfg = EZB_ZHA_COLOR_DIMMABLE_LIGHT_CONFIG();
    light_cfg.on_off_cfg.on_off = DEFAULT_POWER ? 1 : 0;
    light_cfg.level_cfg.current_level = DEFAULT_BRIGHTNESS;
    ezb_af_ep_desc_t ep_desc = ezb_zha_create_color_dimmable_light(ESP_ZIGBEE_HA_COLOR_DIMMABLE_LIGHT_EP_ID, &light_cfg);
    ezb_zcl_cluster_desc_t basic_desc =
        ezb_af_endpoint_get_cluster_desc(ep_desc, EZB_ZCL_CLUSTER_ID_BASIC, EZB_ZCL_CLUSTER_SERVER);

    ezb_zcl_basic_cluster_desc_add_attr(basic_desc, EZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (void *)ESP_MANUFACTURER_NAME);
    ezb_zcl_basic_cluster_desc_add_attr(basic_desc, EZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (void *)ESP_MODEL_IDENTIFIER);
    ESP_ERROR_CHECK(ezb_af_device_add_endpoint_desc(dev_desc, ep_desc));
    ESP_ERROR_CHECK(ezb_af_device_desc_register(dev_desc));
    ezb_zcl_core_action_handler_register(zcl_core_action_handler);

    return ESP_OK;
}

static esp_err_t setup_commissioning(void)
{
    ezb_aps_secur_enable_distributed_security(false);
    ESP_ERROR_CHECK(ezb_bdb_set_primary_channel_set(ESP_ZIGBEE_PRIMARY_CHANNEL_MASK));
    ESP_ERROR_CHECK(ezb_bdb_set_secondary_channel_set(ESP_ZIGBEE_SECONDARY_CHANNEL_MASK));
    ESP_ERROR_CHECK(ezb_app_signal_add_handler(app_signal_handler));

    return ESP_OK;
}

static void zigbee_stack_cleanup(void)
{
    if (s_commission_retry_timer) {
        esp_timer_stop(s_commission_retry_timer);
    }
    if (esp_zigbee_is_started()) {
        esp_zigbee_lock_acquire(portMAX_DELAY);
        esp_zigbee_deinit();
        esp_zigbee_lock_release();
    }
}

extern "C" void stack_main_task(void *pvParameters)
{
    (void)pvParameters;

    esp_zigbee_config_t config = ESP_ZIGBEE_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(esp_zigbee_init(&config));
    ESP_ERROR_CHECK(setup_commissioning());
    ESP_ERROR_CHECK(create_color_dimmable_light());
    ESP_ERROR_CHECK(esp_zigbee_start(false));

    esp_zigbee_launch_mainloop();

    s_zb_task = nullptr;
    vTaskDelete(nullptr);
}

void app_zigbee_register_network_joined_cb(app_zigbee_network_joined_cb_t cb)
{
    s_network_joined_cb = cb;
}

bool app_zigbee_stack_is_running()
{
    return s_zb_task != nullptr;
}

void app_zigbee_stack_start()
{
    if (s_zb_task) {
        return;
    }

    ESP_LOGI(TAG, "Start ESP Zigbee Stack");
    if (xTaskCreate(stack_main_task, "Zigbee_main", 4096, nullptr, 5, &s_zb_task) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create Zigbee main task");
        s_zb_task = nullptr;
    }
}

void app_zigbee_stack_stop()
{
    TaskHandle_t task = s_zb_task;
    if (!task) {
        return;
    }

    ESP_LOGI(TAG, "Stop ESP Zigbee Stack");
    s_zb_task = nullptr;
    vTaskDelete(task); // TODO: use esp_zigbee_stop() when it's available
    zigbee_stack_cleanup();
}
