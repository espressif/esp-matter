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

#include <common_macros.h>
#include <enable_esp_insights.h>
#include <app_priv.h>
#include <app_reset.h>
#include <app/util/attribute-storage.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif

static const char *TAG = "app_main";

static uint16_t configured_buttons = 0;
static button_endpoint button_list[CONFIG_MAX_CONFIGURABLE_BUTTONS];

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;
using namespace chip::app::Clusters;

#if CONFIG_ENABLE_ESP_INSIGHTS_TRACE
extern const char insights_auth_key_start[] asm("_binary_insights_auth_key_txt_start");
extern const char insights_auth_key_end[] asm("_binary_insights_auth_key_txt_end");
#endif

namespace {
// Please refer to https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/namespaces
constexpr const uint8_t kNamespaceSwitches = 43;
// Common Number Namespace: 7, tag 0 (Zero)
constexpr const uint8_t kTagSwitchOn = 0;
// Common Number Namespace: 7, tag 1 (One)
constexpr const uint8_t kTagSwitchOff = 1;

constexpr const uint8_t kNamespacePosition = 8;
// Common Position Namespace: 8, tag: 0 (Left)
constexpr const uint8_t kTagPositionLeft = 0;
// Common Position Namespace: 8, tag: 1 (Right)
constexpr const uint8_t kTagPositionRight = 1;

const Descriptor::Structs::SemanticTagStruct::Type gEp1TagList[] = {
    {.namespaceID = kNamespaceSwitches, .tag = kTagSwitchOn},
    {.namespaceID = kNamespacePosition, .tag = kTagPositionRight}};
const Descriptor::Structs::SemanticTagStruct::Type gEp2TagList[] = {
    {.namespaceID = kNamespaceSwitches, .tag = kTagSwitchOff},
    {.namespaceID = kNamespacePosition, .tag = kTagPositionLeft}};

}

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

    default:
        break;
    }
}

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Driver update */
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        err = app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}

static esp_err_t create_button(struct gpio_button* button, node_t* node)
{
    esp_err_t err = ESP_OK;

    /* Initialize driver */
    app_driver_handle_t button_handle = app_driver_button_init(button);

    /* Create a new endpoint. */
    generic_switch::config_t switch_config;
    endpoint_t *endpoint = generic_switch::create(node, &switch_config, ENDPOINT_FLAG_NONE, button_handle);

    cluster_t* descriptor = cluster::get(endpoint,Descriptor::Id);
    descriptor::feature::taglist::add(descriptor);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint)
    {
        ESP_LOGE(TAG, "Matter node creation failed");
        err = ESP_FAIL;
        return err;
    }

    for (int i = 0; i < configured_buttons; i++) {
        if (button_list[i].button == button) {
            break;
        }
    }

    /* Check for maximum physical buttons that can be configured. */
    if (configured_buttons <CONFIG_MAX_CONFIGURABLE_BUTTONS) {
        button_list[configured_buttons].button = button;
        button_list[configured_buttons].endpoint = endpoint::get_id(endpoint);
        configured_buttons++;
    }
    else
    {
        ESP_LOGI(TAG, "Cannot configure more buttons");
        err = ESP_FAIL;
        return err;
    }

    static uint16_t generic_switch_endpoint_id = 0;
    generic_switch_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Generic Switch created with endpoint_id %d", generic_switch_endpoint_id);

    cluster::fixed_label::config_t fl_config;
    cluster_t *fl_cluster = cluster::fixed_label::create(endpoint, &fl_config, CLUSTER_FLAG_SERVER);

    cluster::user_label::config_t ul_config;
    cluster_t *ul_cluster = cluster::user_label::create(endpoint, &ul_config, CLUSTER_FLAG_SERVER);

    /* Add additional features to the node */
    cluster_t *cluster = cluster::get(endpoint, Switch::Id);

#if CONFIG_GENERIC_SWITCH_TYPE_LATCHING
    cluster::switch_cluster::feature::latching_switch::add(cluster);
#endif

#if CONFIG_GENERIC_SWITCH_TYPE_MOMENTARY
    cluster::switch_cluster::feature::momentary_switch::add(cluster);
#endif

    return err;
}

int get_endpoint(gpio_button* button) {
    for (int i = 0; i < configured_buttons; i++) {
        if (button_list[i].button == button) {
            return button_list[i].endpoint;
        }
    }
    return -1;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    /* Call for Boot button */
    err = create_button(NULL, node);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to create generic switch button"));

    /* Use the code snippet commented below to create more physical buttons. */

    /*  // Creating a gpio button. More buttons can be created in the same fashion specifying GPIO_PIN_VALUE.
     *  struct gpio_button button;
     *  button.GPIO_PIN_VALUE = GPIO_NUM_6;
     *  // Call to createButton function to configure your button.
     *  create_button(&button, node);
     */
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    /* Set OpenThread platform config */
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));

#if CONFIG_ENABLE_ESP_INSIGHTS_TRACE
    enable_insights(insights_auth_key_start);
#endif

    SetTagList(1, chip::Span<const Descriptor::Structs::SemanticTagStruct::Type>(gEp1TagList));
    SetTagList(2, chip::Span<const Descriptor::Structs::SemanticTagStruct::Type>(gEp2TagList));

    nvs_handle_t handle;
    nvs_open_from_partition(CONFIG_CHIP_FACTORY_NAMESPACE_PARTITION_LABEL, "chip-factory", NVS_READWRITE, &handle);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to open namespace:chip-factory from partition:"
                                                    CONFIG_CHIP_FACTORY_NAMESPACE_PARTITION_LABEL ", err:%d", err));

    int32_t out_value = 0;
    if (nvs_get_i32(handle, "fl-sz/1", &out_value) == ESP_ERR_NVS_NOT_FOUND)
    {
       nvs_set_i32(handle, "fl-sz/1", 2);
       nvs_set_str(handle, "fl-k/1/0", "myEP1LBL1");
       nvs_set_str(handle, "fl-v/1/0", "valEP1LBL1");
       nvs_set_str(handle, "fl-k/1/1", "myEP1LBL2");
       nvs_set_str(handle, "fl-v/1/1", "valEP1LBL2");
    }

    nvs_commit(handle);
    nvs_close(handle);

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#endif

}
