/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_matter.h>
#include <esp_log.h>
#include <led_driver.h>
#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include <nvs_flash.h>

/** Standard max values (used for remapping attributes) */
#define STANDARD_BRIGHTNESS 100
#define STANDARD_HUE 360
#define STANDARD_SATURATION 100
#define STANDARD_TEMPERATURE_FACTOR 1000000

/** Matter max values (used for remapping attributes) */
#define MATTER_BRIGHTNESS 254
#define MATTER_HUE 254
#define MATTER_SATURATION 254
#define MATTER_TEMPERATURE_FACTOR 1000000

/** Default attribute values used during initialization */
#define DEFAULT_POWER true
#define DEFAULT_BRIGHTNESS 64
#define DEFAULT_HUE 128
#define DEFAULT_SATURATION 254

typedef void *app_driver_handle_t;


using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;





struct RgbColor_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct HsvColor_t
{
    uint8_t h;
    uint8_t s;
    uint8_t v;
};

struct CtColor_t
{
    uint16_t ctMireds;
};

class DQSmartBulb
{
    private:
        gpio_num_t mGPIOR;
        gpio_num_t mGPIOG;
        gpio_num_t mGPIOB;
        gpio_num_t mGPIOW;
        gpio_num_t mGPIOD;

        uint8_t mBrightness;
        bool mState;
        uint8_t mHue;
        uint8_t mSaturation;
        uint32_t mTemperature;
        bool mVal = 0;
    
    public:
        nvs_handle_t my_handle;
        int32_t counter_reset_factory = 0; 
        bool b_check_reset_factory_connected = 0;   
        uint8_t enable_led_status = 0;

        DQSmartBulb();
        RgbColor_t HsvToRgb(HsvColor_t hsv);
        RgbColor_t CTToRgb(CtColor_t ct);
        led_driver_handle_t Init();
        void SetOff();
        void SetPower(bool power);
        void SetHue(uint8_t Hue);
        void SetSaturation(uint8_t Saturation);
        void SetBrightness(uint8_t brightness);
        void SetTemperature(uint32_t temperature);
        void ControlBulb();
        void ControlBulbTemperature();
        void mSetStateCommissing();
        void mSetStateResetFactory();

        esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);
};


esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id);



