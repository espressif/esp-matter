/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <esp_matter.h>
#include "bsp/esp-bsp.h"

#include <app_priv.h>

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

#include <lowpower_evb_epaper.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";
extern uint16_t light_endpoint_id;

static bool nodelabel_input = false;
bool badge_cluster_input_exists = false;

static esp_err_t set_default_epaper(esp_matter_attr_val_t *val)
{
    epaper_light_power(val->val.b);
    vTaskDelay(pdMS_TO_TICKS(1000));
    return ESP_OK;
}

/* Do any conversions/remapping for the actual value here */
static esp_err_t app_driver_light_set_power(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    return set_default_epaper(val);
}

static esp_err_t set_badge_name(esp_matter_attr_val_t *val)
{
   if (val->val.a.b[0]) {
        if (!strchr((char*)val->val.a.b, '/')) {
            return ESP_FAIL;
        }
        uint8_t* buffer = (uint8_t*)malloc(val->val.a.s * sizeof(uint8_t) + 1);
        memcpy(buffer, val->val.a.b, val->val.a.s);
        buffer[val->val.a.s] = '\0';
        char *name = strtok((char*)buffer, "/");
        char *company_name = strtok(NULL, "\0");
        epaper_display_badge(name, company_name, "\0", "\0", "CSA Member Meeting November 2023");
        nodelabel_input = true;
        free(buffer);
        return ESP_OK;
    }
    return ESP_FAIL;
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.b = !val.val.b;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    if (cluster_id == BADGE_CLUSTER_ID) {
        char vcard_data[MAX_VCARD_ATTR][MAX_ATTR_SIZE];
        memset(vcard_data, '\0', sizeof(MAX_VCARD_ATTR * MAX_ATTR_SIZE));
        esp_matter_attr_val_t _val = esp_matter_invalid(NULL);
        attribute_t *attribute = NULL;

        uint32_t _attribute_id = NAME_ATTRIBUTE_ID;

        for (int i = 0; i < MAX_VCARD_ATTR; i++) {
            if (i == attribute_id) {
                memcpy(vcard_data[i], val->val.a.b, val->val.a.s);
                vcard_data[i][val->val.a.s] = '\0';
                _attribute_id++;
                continue;
            } 
            attribute = attribute::get(0x0, cluster_id, _attribute_id);
            attribute::get_val(attribute, &_val);
            memcpy(vcard_data[i], _val.val.a.b, _val.val.a.s);
            vcard_data[i][_val.val.a.s] = '\0';
            _attribute_id++;
        }
        badge_cluster_input_exists = true;
        epaper_display_badge(vcard_data[NAME], vcard_data[COMPANY_NAME], vcard_data[EMAIL], vcard_data[CONTACT], vcard_data[EVENT_NAME]);
        return ESP_OK;
    }

    /* Update NodeLabel based name and company_name input */
    if (cluster_id == BasicInformation::Id && attribute_id == BasicInformation::Attributes::NodeLabel::Id && !badge_cluster_input_exists) {
        if(set_badge_name(val) == ESP_OK)
            return ESP_OK;
    }

    /* Update Light Display */
    if (endpoint_id == light_endpoint_id && !nodelabel_input && !badge_cluster_input_exists) {
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                epaper_light_power(val->val.b);
            }
        }
    }
    return ESP_OK;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    led_indicator_handle_t handle = (led_indicator_handle_t)priv_data;
    attribute_t *attribute = NULL;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    attribute = attribute::get(0x0, BADGE_CLUSTER_ID, NAME_ATTRIBUTE_ID);
    attribute::get_val(attribute, &val);
    if (val.val.a.b[NAME]) {
        char vcard_data[MAX_VCARD_ATTR][MAX_ATTR_SIZE];
        memset(vcard_data, '\0', sizeof(vcard_data));
        int i = 0;
        uint32_t attribute_id = NAME_ATTRIBUTE_ID;
        do {
            esp_matter_attr_val_t _val = esp_matter_invalid(NULL);
            attribute = attribute::get(0x0, BADGE_CLUSTER_ID, attribute_id);
            attribute::get_val(attribute, &_val);
            memcpy(vcard_data[i], _val.val.a.b, _val.val.a.s);
            vcard_data[i][_val.val.a.s] = '\0';
            attribute_id++;
            i++;
        } while (i < MAX_VCARD_ATTR);
        badge_cluster_input_exists = true;
        epaper_display_badge(vcard_data[NAME], vcard_data[COMPANY_NAME], vcard_data[EMAIL], vcard_data[CONTACT], vcard_data[EVENT_NAME]);
        return ESP_OK;
    }

    /* If nodelabel value is set then display the nodelabel_input */
    attribute = attribute::get(0x0, BasicInformation::Id, BasicInformation::Attributes::NodeLabel::Id);
    attribute::get_val(attribute, &val);
    if (val.val.a.s && !badge_cluster_input_exists) {
        set_badge_name(&val);
        return ESP_OK;
    }

    /* Setting power */
    if (!nodelabel_input) {
        attribute = attribute::get(0x0, OnOff::Id, OnOff::Attributes::OnOff::Id);
        attribute::get_val(attribute, &val);
        if (chip::Server::GetInstance().GetFabricTable().FabricCount())
            err |= app_driver_light_set_power(handle, &val);
    }

    return err;
}

static void factory_reset_badge(void *arg, void *data)
{
    ESP_LOGI(TAG, "Factory Resetting the Espressif Badge");
    esp_matter::factory_reset();
}

app_driver_handle_t app_driver_button_init()
{
    /* Initialize button */
    button_handle_t btns[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
    ESP_ERROR_CHECK(iot_button_register_cb(btns[0], BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL));
    ESP_ERROR_CHECK(iot_button_register_cb(btns[0], BUTTON_LONG_PRESS_START, factory_reset_badge, NULL));

    return (app_driver_handle_t)btns[0];
}
