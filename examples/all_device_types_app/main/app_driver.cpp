/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_matter.h"
#include "esp_err.h"

#include <device.h>
#include <protocols/Protocols.h>
#include <app_priv.h>
#include <app/clusters/fan-control-server/fan-control-delegate.h>
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#include "fan_hal_stepper_motor.h"
#include "fan_hal_bldc.h"
#endif

using namespace chip::app::Clusters;
using namespace chip::app::Clusters::FanControl;
using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

static const char *TAG = "app_driver";
extern uint16_t app_endpoint_id;

static void get_attribute(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    attribute::get_val(attribute, val);
}

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
chip::Protocols::InteractionModel::Status FanDelegateImpl::HandleStep(chip::app::Clusters::FanControl::StepDirectionEnum aDirection, bool aWrap, bool aLowestOff)
{
    uint8_t lowest = 0;
    static int8_t max_speed = HIGH_MODE_PERCENT_MAX;
    int8_t tmp = 0;

    esp_matter_attr_val_t val;
    get_attribute(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
    tmp = val.val.u8;

    ESP_LOGI(TAG, "Step received value: %d %d %d", (uint8_t)aDirection, aWrap, aLowestOff);
    /* Update percent-setting when speed is changed by the step command */
    if (aLowestOff) {
        lowest = 0;
    } else {
        lowest = 1;
    }
    if ((uint8_t)aDirection == 0) {
        tmp += 1;
        if (aWrap == 0) {
            tmp = tmp > max_speed ? max_speed : tmp;
        } else {
            tmp = tmp > max_speed ? lowest : tmp;
        }
    } else if ((uint8_t)aDirection == 1) {
        tmp -= 1;
        if (aWrap == 0) {
            tmp = tmp < lowest ? lowest : tmp;
        } else {
            tmp = tmp < lowest ? max_speed : tmp;
        }
    }
    val.val.u8 = tmp;
    attribute::update(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);

    return chip::Protocols::InteractionModel::Status::Success;
}

static esp_err_t app_driver_fan_set_mode(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Mode received value = %u ", val->val.u8);
    hal_bldc_set_wind(0);
    if (val->val.u8 == 0) {
        hal_stepper_motor_set_rock(0);
    }
    hal_bldc_set_fanmode(val->val.u8);

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_percent(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    hal_bldc_set_wind(0);
    ESP_LOGI(TAG, "Percent received value = %u ", val->val.u8);
    hal_bldc_set_percent_speed(val->val.u8);

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_rock(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Shake received value = %u ", val->val.u8);
    return hal_stepper_motor_set_rock(val->val.u8);
}

static esp_err_t app_driver_fan_set_wind(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Wind received value = %u", val->val.u8);
    hal_bldc_set_wind(val->val.u8);
    /* When exit nature wind mode, set fan speed value to the pre-set percent-setting */
    if (val->val.u8 != 2) {
        esp_matter_attr_val_t val;
        get_attribute(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
        attribute::update(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
    }

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_airflow_direction(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Airflow_Direction received value = %d", val->val.b);
    return hal_bldc_set_airflow_direction(val->val.b);
}

#else

static esp_err_t app_driver_fan_set_mode(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t app_driver_fan_set_percent(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t app_driver_fan_set_rock(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t app_driver_fan_set_wind(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t app_driver_fan_set_airflow_direction(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    return ESP_ERR_NOT_SUPPORTED;
}
#endif

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;

    if (endpoint_id == app_endpoint_id) {
        fan_driver_handle_t handle = (fan_driver_handle_t)driver_handle;

        if (cluster_id == FanControl::Id) {
            /* FanControl is a code-driven cluster: the SDK derives PercentSetting/SpeedSetting
               from FanMode (ApplyFanModeSideEffects), FanMode from PercentSetting
               (ComputeFanModeFromPercent), and maintains PercentCurrent/SpeedCurrent. The driver
               must only actuate hardware here; writing sibling attributes back would re-enter the
               update callback and recurse until the stack overflows. */
            if (attribute_id == FanControl::Attributes::FanMode::Id) {
                err = app_driver_fan_set_mode(handle, val);
            } else if (attribute_id == FanControl::Attributes::PercentSetting::Id) {
                err = app_driver_fan_set_percent(handle, val);
            } else if (attribute_id == FanControl::Attributes::RockSetting::Id) {
                err = app_driver_fan_set_rock(handle, val);
            } else if (attribute_id == FanControl::Attributes::WindSetting::Id) {
                err = app_driver_fan_set_wind(handle, val);
            } else if (attribute_id == FanControl::Attributes::AirflowDirection::Id) {
                err = app_driver_fan_set_airflow_direction(handle, val);
            }
        }
    }
    return err;
}

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
esp_err_t fan_driver_init()
{
    esp_err_t err = ESP_OK;
    /* initializing brushless motor */
    bldc_variable_init();
    err = hal_bldc_init(CW);

    /* initializing step motor */
    err = hal_stepper_motor_init((gpio_num_t)CONFIG_STEP_MOTOR_A0_GPIO, (gpio_num_t)CONFIG_STEP_MOTOR_A1_GPIO, (gpio_num_t)CONFIG_STEP_MOTOR_B0_GPIO, (gpio_num_t)CONFIG_STEP_MOTOR_B1_GPIO);

    return err;
}
#endif

esp_err_t app_driver_init()
{
    esp_err_t err = ESP_OK;
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
    /* initializing fan driver */
    fan_driver_init();
#endif

    return err;
}
