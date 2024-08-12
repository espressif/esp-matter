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

static esp_err_t set_attribute(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t val)
{
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    return attribute::set_val(attribute, &val);
}

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
chip::Protocols::InteractionModel::Status FanDelegateImpl::HandleStep(chip::app::Clusters::FanControl::StepDirectionEnum aDirection, bool aWrap, bool aLowestOff)
{
    uint8_t lowest = 0;
    static int8_t max_speed = HIGH_MODE_PERCENT_MAX;
    int8_t tmp = 0;

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    get_attribute(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
    tmp = val.val.u8;

    ESP_LOGI(TAG, "Step received value: %d %d %d", (uint8_t)aDirection, aWrap, aLowestOff);
    /* Update percent-setting when speed is changed by the step command */
    if (aLowestOff)
        lowest = 0;
    else
        lowest = 1;
    if ((uint8_t)aDirection == 0) {
        tmp += 1;
        if (aWrap == 0) {
            tmp = tmp > max_speed ? max_speed:tmp;
        } else {
            tmp = tmp > max_speed ? lowest:tmp;
        }
    } else if ((uint8_t)aDirection == 1) {
        tmp -= 1;
        if (aWrap == 0) {
            tmp = tmp < lowest ? lowest:tmp;
        } else {
            tmp = tmp < lowest ? max_speed:tmp;
        }
    }
    val.val.u8 = tmp;
    attribute::update(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);

    return chip::Protocols::InteractionModel::Status::Success;
}

static esp_err_t app_driver_fan_set_mode(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Mode received value = %d ", val->val.i);
    hal_bldc_set_wind(0);
    if (val->val.i == 0) {
        hal_stepper_motor_set_rock(0);
    }
    hal_bldc_set_fanmode(val->val.i);

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_percent(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    hal_bldc_set_wind(0);
    ESP_LOGI(TAG, "Percent received value = %d ", val->val.i);
    hal_bldc_set_percent_speed(val->val.i);

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_rock(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Shake received value = %d ", val->val.i);
    return hal_stepper_motor_set_rock(val->val.i);
}

static esp_err_t app_driver_fan_set_wind(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Wind received value = %d", val->val.i);
    hal_bldc_set_wind(val->val.i);
    /* When exit nature wind mode, set fan speed value to the pre-set percent-setting */
    if (val->val.i != 2) {
        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
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

static bool check_if_mode_percent_match(uint8_t fan_mode, uint8_t percent)
{
    switch (fan_mode) {
        case chip::to_underlying(FanModeEnum::kHigh): {
            if (percent < HIGH_MODE_PERCENT_MIN) {
                return false;
            }
            break;
        }
        case chip::to_underlying(FanModeEnum::kMedium): {
            if ((percent < MED_MODE_PERCENT_MIN) || (percent > MED_MODE_PERCENT_MAX)) {
               return false;
            }
            break;
        }
        case chip::to_underlying(FanModeEnum::kLow): {
            if ((percent < LOW_MODE_PERCENT_MIN) || (percent > LOW_MODE_PERCENT_MAX)) {
                return false;
            }
            break;
        }
        default:
            return false;
    }

    return true;
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;

    if (endpoint_id == app_endpoint_id) {
       fan_driver_handle_t handle = (fan_driver_handle_t)driver_handle;

        if (cluster_id == FanControl::Id) {
            if (attribute_id == FanControl::Attributes::FanMode::Id) {
                esp_matter_attr_val_t val_a = esp_matter_invalid(NULL);
                get_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, &val_a);
                /* When FanMode attribute change , should check the persent setting value, if this value not match
                   FanMode, need write the persent setting value to corresponding value. Now we set it to the max
                   value of the FanMode */
                if (!check_if_mode_percent_match(val->val.u8, val_a.val.u8)) {
                    switch (val->val.u8) {
                        case chip::to_underlying(FanModeEnum::kHigh): {
                                val_a.val.u8 = HIGH_MODE_PERCENT_MAX;
                                set_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, val_a);
                                set_attribute(endpoint_id, FanControl::Id, Attributes::SpeedSetting::Id, val_a);
                            break;
                        }
                        case chip::to_underlying(FanModeEnum::kMedium): {
                                val_a.val.u8 = MED_MODE_PERCENT_MAX;
                                set_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, val_a);
                                set_attribute(endpoint_id, FanControl::Id, Attributes::SpeedSetting::Id, val_a);
                            break;
                        }
                        case chip::to_underlying(FanModeEnum::kLow): {
                                val_a.val.u8 = LOW_MODE_PERCENT_MAX;
                                set_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, val_a);
                                set_attribute(endpoint_id, FanControl::Id, Attributes::SpeedSetting::Id, val_a);
                            break;
                        }
                        case chip::to_underlying(FanModeEnum::kAuto): {
                            /* add auto mode driver for auto logic */
                            break;
                        }
                        case chip::to_underlying(FanModeEnum::kOff): {
                            /* add off mode driver if needed */
                            break;
                        }
                        default:
                            break;
                    }
                }
                set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id, val_a);
                set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id, val_a);
                err = app_driver_fan_set_mode(handle, val);
            } else if (attribute_id == FanControl::Attributes::PercentSetting::Id) {
                esp_matter_attr_val_t val_b = esp_matter_invalid(NULL);
                get_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, &val_b);
                if (!check_if_mode_percent_match(val_b.val.u8, val->val.u8)) {
                    if (val->val.u8 >= HIGH_MODE_PERCENT_MIN) {
                        /* set high mode */
                        val_b.val.u8 = chip::to_underlying(FanModeEnum::kHigh);
                        set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, val_b);
                    } else if (val->val.u8 >= MED_MODE_PERCENT_MIN) {
                        /* set med mode */
                        val_b.val.u8 = chip::to_underlying(FanModeEnum::kMedium);
                        set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, val_b);
                    } else if (val->val.u8 >= LOW_MODE_PERCENT_MIN) {
                        /* set low mode */
                        val_b.val.u8 = chip::to_underlying(FanModeEnum::kLow);
                        set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, val_b);
                    }
                }
                set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id, *val);
                set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id, *val);
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

