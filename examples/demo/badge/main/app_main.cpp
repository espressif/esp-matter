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
#include <esp_matter_attribute_utils.h>

#include <common_macros.h>
#include <app_priv.h>
#include <app_reset.h>
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

#include <epaper.h>
#include <lowpower_evb_epaper.h>

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

constexpr auto k_timeout_seconds = 300;

#if CONFIG_ENABLE_ENCRYPTED_OTA
extern const char decryption_key_start[] asm("_binary_esp_image_encryption_key_pem_start");
extern const char decryption_key_end[] asm("_binary_esp_image_encryption_key_pem_end");

static const char *s_decryption_key = decryption_key_start;
static const uint16_t s_decryption_key_len = decryption_key_end - decryption_key_start;
#endif // CONFIG_ENABLE_ENCRYPTED_OTA

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        epaper_light_power(true);
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
        display_commission_qr_code();
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
        {
            ESP_LOGI(TAG, "Fabric removed successfully");
            if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0)
            {
                chip::CommissioningWindowManager & commissionMgr = chip::Server::GetInstance().GetCommissioningWindowManager();
                constexpr auto kTimeoutSeconds = chip::System::Clock::Seconds16(k_timeout_seconds);
                if (!commissionMgr.IsCommissioningWindowOpen())
                {
                    /* After removing last fabric, this example does not remove the Wi-Fi credentials
                     * and still has IP connectivity so, only advertising on DNS-SD.
                     */
                    CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(kTimeoutSeconds,
                                                    chip::CommissioningWindowAdvertisement::kDnssdOnly);
                    if (err != CHIP_NO_ERROR)
                    {
                        ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
                    }
                }
            }
            esp_matter::factory_reset();
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
    default:
        break;
    }
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id, effect_variant);
    return ESP_OK;
}

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

#if CONFIG_ENABLE_CHIP_SHELL
esp_err_t change_badge_name(int argc, char **argv)
{
    if (argc != 1 || !argv[0]) {
        ESP_LOGE("CHANGE_BADGE_NAME_CONSOLE", "Usage: matter esp change_name '{UserName}/{CompanyName}/{Email}/{Contact}/{Event Name}'");
        return ESP_FAIL;
    }
    size_t count = 0;

    char delimiter = '/';
    for (int i = 0; argv[0][i] != '\0'; ++i) {
        if (delimiter == argv[0][i])
            ++count;
    }
    if (count != 4) {
        ESP_LOGE("CHANGE_BADGE_NAME_CONSOLE", "Usage: matter esp change_name '{UserName}/{CompanyName}/{Email}/{Contact}/{Event Name}'");
        return ESP_FAIL;
    }
    char vcard_data[MAX_VCARD_ATTR][MAX_ATTR_SIZE];
    char *in_ptr = argv[0];
    char *o_ptr;
    int i = 0;
    while((o_ptr = strsep(&in_ptr, "/")) != NULL) {
        strcpy(vcard_data[i], o_ptr);
        i++;
    }
    if (i == EVENT_NAME) {
        strcpy(vcard_data[EVENT_NAME], "\0");
    }
    if (chip::Server::GetInstance().GetFabricTable().FabricCount()) {
        uint16_t endpoint_id = 0x0;
        uint32_t cluster_id = BADGE_CLUSTER_ID;
        uint32_t attribute_id = NAME_ATTRIBUTE_ID;
        for (int i = 0; i < MAX_VCARD_ATTR; i++) {
            esp_matter_attr_val_t val = esp_matter_long_char_str(vcard_data[i], strlen(vcard_data[i]));
            attribute::report(endpoint_id, cluster_id, attribute_id, &val);
            attribute_id++;
        }
        badge_cluster_input_exists = true;
    }
    epaper_display_badge(vcard_data[NAME], vcard_data[COMPANY_NAME], vcard_data[EMAIL], vcard_data[CONTACT], vcard_data[EVENT_NAME]);
    return ESP_OK;
}
#endif

void display_badge(void *data)
{
    char **vcard_data = (char**)data;
    epaper_display_badge(vcard_data[NAME], vcard_data[COMPANY_NAME], vcard_data[EMAIL], vcard_data[CONTACT], vcard_data[EVENT_NAME]);
    free(vcard_data);
    vcard_data = NULL;
    vTaskDelete(NULL);
}

/* Custom command callback to write the badge info */
static esp_err_t update_badge_command_info(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    using namespace chip::app;

    ESP_LOGI(TAG, "The endpoint: %d, cluster_id: %ld, command_id : %ld", endpoint_id, cluster_id, command_id);

    if (chip::TLV::kTLVType_Structure != tlv_data.GetType()) {
        return ESP_FAIL;
    }
    chip::TLV::TLVType mOuter;
    if (CHIP_NO_ERROR != tlv_data.EnterContainer(mOuter)) {
        return ESP_FAIL;
    }
    char **vcard_data = (char**)malloc(sizeof(char*) * MAX_VCARD_ATTR);
    for (int i = 0; i < MAX_VCARD_ATTR; i++)
        vcard_data[i] = NULL;
    while(1) {
        CHIP_ERROR err = tlv_data.Next();
        if (err == CHIP_ERROR_END_OF_TLV)
            break;
        else if (CHIP_NO_ERROR != err)
            return ESP_FAIL;

        chip::TLV::Tag tag = tlv_data.GetTag();
        if (!IsContextTag(tag)) {
            ESP_LOGE(TAG, "Tag is not valid");
            return ESP_FAIL;
        }

        chip::CharSpan config_value;
        if (CHIP_NO_ERROR != DataModel::Decode(tlv_data, config_value)) {
            ESP_LOGE(TAG, "Unable to decode the datamodel");
            return ESP_FAIL;
        }
        if (TagNumFromTag(tag) >= MAX_VCARD_ATTR) {
            ESP_LOGE(TAG, "tag not valid");
            return ESP_FAIL;
        }

        uint16_t endpoint_id = 0x0;
        uint32_t cluster_id = BADGE_CLUSTER_ID;
        uint32_t attribute_id;
        size_t size = config_value.size() < 32 ? config_value.size() : 32;
        esp_matter_attr_val_t val = esp_matter_long_char_str((char*)config_value.data(), size);
        switch(TagNumFromTag(tag)) {
            case NAME:
                attribute_id = NAME_ATTRIBUTE_ID;
                break;
            case COMPANY_NAME:
                attribute_id = COMPANY_NAME_ATTRIBUTE_ID;
                break;
            case EMAIL:
                attribute_id = EMAIL_ATTRIBUTE_ID;
                break;
            case CONTACT:
                attribute_id = CONTACT_ATTRIBUTE_ID;
                break;
            case EVENT_NAME:
                attribute_id = EVENT_NAME_ATTRIBUTE_ID;
                break;
            default:
                ESP_LOGE(TAG, "No such TAG found");
                return ESP_FAIL;
        }
        attribute::report(endpoint_id, cluster_id, attribute_id, &val);
        vcard_data[TagNumFromTag(tag)] = (char*)malloc(sizeof(char) * (size + 1));
        memcpy(vcard_data[TagNumFromTag(tag)], config_value.data(), size);
        vcard_data[TagNumFromTag(tag)][size] = '\0';
    }
    if (CHIP_NO_ERROR != tlv_data.ExitContainer(mOuter)) {
        return ESP_FAIL;
    }

    badge_cluster_input_exists = true;

    if (!vcard_data[NAME] || !vcard_data[COMPANY_NAME] || !vcard_data[EMAIL]) {
        ESP_LOGW(TAG, "Name, Company Name and Email field are mandatory fields for vcard");
    }
    for (int i = 0; i < MAX_VCARD_ATTR; i++) {
        if (!vcard_data[i]) {
            vcard_data[i] = (char*)malloc(sizeof(char));
            vcard_data[i][0] = '\0';
        }
    }
    xTaskCreate(display_badge, "Displaying on the badge", 5120, (void*)vcard_data, 5, NULL);
    return ESP_OK;
}

extern "C" void app_main()
{
    epaper_display_init();
    epaper_display_espressif_logo();

    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize button */
    app_driver_handle_t button_handle = app_driver_button_init();
    app_reset_button_register(button_handle);

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    on_off_light::config_t light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.on_off.lighting.start_up_on_off = nullptr;
    endpoint_t *endpoint = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(endpoint != nullptr, ESP_LOGE(TAG, "Failed to create on off light endpoint"));

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Create custom badge cluster in basic-information endpoint */
    uint32_t custom_cluster_id = BADGE_CLUSTER_ID;
    cluster_t *badge_cluster = cluster::create(endpoint::get(node, 0x0), custom_cluster_id, CLUSTER_FLAG_SERVER);
    ABORT_APP_ON_FAILURE(badge_cluster != nullptr, ESP_LOGE(TAG, "Failed to create badge cluster"));

    /* Create custom attributes in badge cluster for retrieving the vcard properties */
    attribute::create(badge_cluster, NAME_ATTRIBUTE_ID, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_long_char_str("", MAX_ATTR_SIZE));
    attribute::create(badge_cluster, CONTACT_ATTRIBUTE_ID, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_long_char_str("", MAX_ATTR_SIZE));
    attribute::create(badge_cluster, EMAIL_ATTRIBUTE_ID, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_long_char_str("", MAX_ATTR_SIZE));
    attribute::create(badge_cluster, EVENT_NAME_ATTRIBUTE_ID, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_long_char_str("", MAX_ATTR_SIZE));
    attribute::create(badge_cluster, COMPANY_NAME_ATTRIBUTE_ID, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_long_char_str("", MAX_ATTR_SIZE));

    uint32_t custom_command_id = 0x00;
    command::create(badge_cluster, custom_command_id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM, update_badge_command_info);
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

    /* Starting driver with default values */
    if (chip::Server::GetInstance().GetFabricTable().FabricCount())
        app_driver_light_set_defaults(light_endpoint_id);

#if CONFIG_ENABLE_ENCRYPTED_OTA
    err = esp_matter_ota_requestor_encrypted_init(s_decryption_key, s_decryption_key_len);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to initialized the encrypted OTA, err: %d", err));
#endif // CONFIG_ENABLE_ENCRYPTED_OTA

#if CONFIG_ENABLE_CHIP_SHELL
    static const esp_matter::console::command_t change_name = {
        .name = "change_name",
        .description = "Change the name on badge",
        .handler = change_badge_name,
    };
    esp_matter::console::add_commands(&change_name, 1);

    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif
}
