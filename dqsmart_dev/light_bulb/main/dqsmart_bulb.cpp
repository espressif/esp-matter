
// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include "app_priv.h"
#include "math.h"

extern uint16_t light_endpoint_id;

static const char *TAG = "dqsmart_bulb";
// static bool current_power = false;
// static uint8_t current_brightness = 0;
// static uint32_t current_temp = 0;
// static HS_color_t current_HS = {0, 0};
// static RGB_color_t mRGB;



/* Do any conversions/remapping for the actual value here */
// static esp_err_t app_driver_light_set_power(led_driver_handle_t handle, esp_matter_attr_val_t *val)
// {
//     return led_driver_set_power(handle, val->val.b);
// }

// static esp_err_t app_driver_light_set_brightness(led_driver_handle_t handle, esp_matter_attr_val_t *val)
// {
//     int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
//     return led_driver_set_brightness(handle, value);
// }

// static esp_err_t app_driver_light_set_hue(led_driver_handle_t handle, esp_matter_attr_val_t *val)
// {
//     int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
//     return led_driver_set_hue(handle, value);
// }

// static esp_err_t app_driver_light_set_saturation(led_driver_handle_t handle, esp_matter_attr_val_t *val)
// {
//     int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
//     return led_driver_set_saturation(handle, value);
// }

// static esp_err_t app_driver_light_set_temperature(led_driver_handle_t handle, esp_matter_attr_val_t *val)
// {
//     uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
//     return led_driver_set_temperature(handle, value);
// }

// define a clamp macro to substitute the std::clamp macro which is available from C++17 onwards
#define clamp(a, min, max) ((a) < (min) ? (min) : ((a) > (max) ? (max) : (a)))

led_driver_handle_t DQSmartBulb::Init()
{
    ESP_LOGI(TAG, "Initializing light driver");
    esp_err_t err = ESP_OK;
    gpio_num_t mGPIONum                       = (gpio_num_t) 2;
    ledc_timer_config_t ledc_timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE, // timer mode
        .duty_resolution = LEDC_TIMER_8_BIT,    // resolution of PWM duty
        .timer_num       = LEDC_TIMER_1,        // timer index
        .freq_hz         = 5000,                // frequency of PWM signal
        .clk_cfg         = LEDC_AUTO_CLK,       // Auto select the source clock
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
        .gpio_num   = mGPIONum,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_1,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&ledc_channel);
    mGPIOR          = (gpio_num_t) 5;
    mGPIOG          = (gpio_num_t) 23;
    mGPIOB          = (gpio_num_t) 4;
    mGPIOW          = (gpio_num_t) 19;
    mGPIOD          = (gpio_num_t) 21;

    ledc_channel_config_t channel_red = {
        .gpio_num   = mGPIOR,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_1,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_1,
        .duty       = 0,
        .hpoint     = 0,
	};
	ledc_channel_config_t channel_green = {
	    .gpio_num   = mGPIOG,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_2,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_1,
        .duty       = 0,
        .hpoint     = 0,
	};
	ledc_channel_config_t channel_blue = {
		.gpio_num   = mGPIOB,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_3,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_1,
        .duty       = 0,
        .hpoint     = 0,
	};

    ledc_channel_config_t channel_w = {
		.gpio_num   = mGPIOW,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_4,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_1,
        .duty       = 0,
        .hpoint     = 0,
	};

    ledc_channel_config_t channel_d = {
		.gpio_num   = mGPIOD,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_5,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_1,
        .duty       = 0,
        .hpoint     = 0,
	};

	ledc_channel_config(&channel_red);
	ledc_channel_config(&channel_green);
	ledc_channel_config(&channel_blue);
    ledc_channel_config(&channel_w);
    ledc_channel_config(&channel_d);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,5);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
	
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 5);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
	
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3,5);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5, 0);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5);

    return (led_driver_handle_t)LEDC_CHANNEL_1 + 1;
}

void DQSmartBulb::mSetStateCommissing()
{

    this->mVal = ! this->mVal;
    if(mVal)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,10);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
    }
    else 
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,0);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
    }

}

void DQSmartBulb::mSetStateResetFactory()
{
     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1,0);
	 ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 50);
     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        
     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, 0);
     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
}   


void DQSmartBulb::SetPower(bool power)
{
    ESP_LOGI(TAG, "xxxxxxxxxxxSetPower %d", power);
    this->mState = power;

    this->ControlBulb();
}

void DQSmartBulb::SetHue(uint8_t Hue)
{
    ESP_LOGI(TAG, "xxxxxxxxxxx Hue %d", Hue);
    this->mHue = Hue;
    this->ControlBulb();
}
void DQSmartBulb::SetSaturation(uint8_t Saturation)
{
    ESP_LOGI(TAG, "SetSaturation %d",Saturation);
    this->mSaturation = Saturation;
    this->ControlBulb();
}
void DQSmartBulb::SetBrightness(uint8_t brightness)
{
    ESP_LOGI(TAG, "xxxxxxxxxxxSetBrightness %d", brightness);
    this->mBrightness = brightness;
    this->ControlBulb();
}

void DQSmartBulb::SetTemperature(uint32_t temperature)
{
    ESP_LOGI(TAG, "xxxxxxxxxxxSetTemperature %d", temperature);
    this->mTemperature = temperature;
    this->ControlBulbTemperature();
}

DQSmartBulb::DQSmartBulb()
{
    ESP_LOGI(TAG, "xxxxxxxxxxx Contruction DQSmartBulb class !!!");
}

RgbColor_t DQSmartBulb::HsvToRgb(HsvColor_t hsv)
{
    //  RgbColor_t rgb;

    // unsigned char region, remainder, p, q, t;
    
    // if (hsv.s == 0)
    // {
    //     rgb.r = hsv.v;
    //     rgb.g = hsv.v;
    //     rgb.b = hsv.v;
    //     return rgb;
    // }
    
    // region = hsv.h / 42.5;
    // remainder = (hsv.h - (region * 42.5)) * 6; 
    
    // p = (hsv.v * (255 - hsv.s)) >> 8;
    // q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    // t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;
    
    // switch (region)
    // {
    //     case 0:
    //         rgb.r = hsv.v; rgb.g = t; rgb.b = p;
    //         break;
    //     case 1:
    //         rgb.r = q; rgb.g = hsv.v; rgb.b = p;
    //         break;
    //     case 2:
    //         rgb.r = p; rgb.g = hsv.v; rgb.b = t;
    //         break;
    //     case 3:
    //         rgb.r = p; rgb.g = q; rgb.b = hsv.v;
    //         break;
    //     case 4:
    //         rgb.r = t; rgb.g = p; rgb.b = hsv.v;
    //         break;
    //     default:
    //         rgb.r = hsv.v; rgb.g = p; rgb.b = q;
    //         break;
    // }

    // return rgb;
      RgbColor_t rgb;

    uint8_t region, p, q, t;
    uint32_t h, s, v, remainder;

    if (hsv.s == 0)
    {
        rgb.r = rgb.g = rgb.b = hsv.v;
    }
    else
    {
        h = hsv.h;
        s = hsv.s;
        v = hsv.v;

        region    = h / 43;
        remainder = (h - (region * 43)) * 6;
        p         = (v * (255 - s)) >> 8;
        q         = (v * (255 - ((s * remainder) >> 8))) >> 8;
        t         = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
        switch (region)
        {
        case 0:
            rgb.r = v, rgb.g = t, rgb.b = p;
            break;
        case 1:
            rgb.r = q, rgb.g = v, rgb.b = p;
            break;
        case 2:
            rgb.r = p, rgb.g = v, rgb.b = t;
            break;
        case 3:
            rgb.r = p, rgb.g = q, rgb.b = v;
            break;
        case 4:
            rgb.r = t, rgb.g = p, rgb.b = v;
            break;
        case 5:
        default:
            rgb.r = v, rgb.g = p, rgb.b = q;
            break;
        }
    }

    return rgb;
}

RgbColor_t DQSmartBulb::CTToRgb(CtColor_t ct)
{
    RgbColor_t rgb;
    float r, g, b;

    // Algorithm credits to Tanner Helland: https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html

    // Convert Mireds to centiKelvins. k = 1,000,000/mired
    float ctCentiKelvin = 10000 / ct.ctMireds;
    
    // Red
    if (ctCentiKelvin <= 66)
    {
        r = 255;
    }
    else
    {
        r = 329.698727446f * pow(ctCentiKelvin - 60, -0.1332047592f);
    }

    // Green
    if (ctCentiKelvin <= 66)
    {
        g = 99.4708025861f * log(ctCentiKelvin) - 161.1195681661f;
    }
    else
    {
        g = 288.1221695283f * pow(ctCentiKelvin - 60, -0.0755148492f);
    }

    // Blue
    if (ctCentiKelvin >= 66)
    {
        b = 255;
    }
    else
    {
        if (ctCentiKelvin <= 19)
        {
            b = 0;
        }
        else
        {
            b = 138.5177312231 * log(ctCentiKelvin - 10) - 305.0447927307;
        }
    }
    rgb.r = (uint8_t) clamp(r, 0, 255);
    rgb.g = (uint8_t) clamp(g, 0, 255);
    rgb.b = (uint8_t) clamp(b, 0, 255);

    return rgb;
}



esp_err_t DQSmartBulb::app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        led_driver_handle_t handle = (led_driver_handle_t)driver_handle;
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                //err = app_driver_light_set_power(handle, val);
                this->SetPower(val->val.b);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                //err = app_driver_light_set_brightness(handle, val);
               // int value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
                this->SetBrightness(val->val.u8);
            }
        } else if (cluster_id == ColorControl::Id) {
            if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                //int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
                this->SetHue(val->val.u8);
                //err = app_driver_light_set_hue(handle, val);
            } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                //err = app_driver_light_set_saturation(handle, val);
                //int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
                this->SetSaturation(val->val.u8);
            } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
                //err = app_driver_light_set_temperature(handle, val);
                //uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
                this->SetTemperature(val->val.u8);
            }
        }
    }
    return err;
}

void DQSmartBulb::SetOff()
{
        this->mHue         = 0;
        this->mSaturation  = 0;
        this->mTemperature = 0;
}

void DQSmartBulb::ControlBulb()
{
    uint8_t brightness = mState ? mBrightness : 0;
   // if(brightness == 0) {this->SetOff();}
    ESP_LOGI(TAG, "xxxxxxxxxxxxxxxxxxxxxxxxxXXXXXXXXXx %d %d %d",  mHue,  mSaturation, brightness);
    HsvColor_t hsv = { mHue, mSaturation, brightness };
    RgbColor_t rgb = this->HsvToRgb(hsv);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, rgb.r);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
	
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, rgb.g);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
	
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, rgb.b);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5, 0);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5);

    ESP_LOGI(TAG, "Convert %d %d %d",  rgb.r,  rgb.g, rgb.b);

}



void DQSmartBulb::ControlBulbTemperature()
{
    uint8_t brightness = mState ? mBrightness : 0;
    if(brightness == 0) {this->SetOff();}
    CtColor_t _color;
    _color.ctMireds = this->mTemperature;
    RgbColor_t rgb = this->CTToRgb(_color);
    
    
    if(_color.ctMireds > 200)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, rgb.r);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, rgb.g);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, rgb.b);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, rgb.b);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5, 0);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5);
    }
    else
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, rgb.r);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, rgb.g);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, rgb.b);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5, rgb.b);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_5);
    }


    ESP_LOGI(TAG, "Convert %d %d %d %d",  rgb.r,  rgb.g, rgb.b, this->mTemperature);

}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    led_driver_handle_t handle = (led_driver_handle_t)priv_data;
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = NULL;
    attribute_t *attribute = NULL;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Setting power */
    cluster = cluster::get(endpoint, OnOff::Id);
    attribute = attribute::get(cluster, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);

    return err;
}