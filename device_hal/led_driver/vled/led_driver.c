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

#include <driver/ledc.h>
#include <esp_log.h>
#include <esp_system.h>
#include <string.h>
#include <tft.h>
#include <tftspi.h>

#include <color_format.h>
#include <led_driver.h>

#define TFT_SPI_CLOCK_INIT_HZ 8000000
#define LEDC_PWM_HZ 1000
#define BRIGHTNESS_MAX 255

static const char *TAG = "led_driver_vled";
static bool current_power = false;
static uint8_t current_brightness = 0;
static HS_color_t current_HS = {0, 0};
static uint32_t current_temperature = 0;
static RGB_color_t mRGB;
static uint16_t DisplayHeight;
static uint16_t DisplayWidth;
static int led_driver_channel = -1;

static void SetupBrightnessControl(led_driver_config_t *config)
{
    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer));
    led_driver_channel = config->channel;

    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // resolution of PWM duty
    ledc_timer.freq_hz = LEDC_PWM_HZ; // frequency of PWM signal
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE; // timer mode
    ledc_timer.timer_num = LEDC_TIMER_0; // timer index
    ledc_timer_config(&ledc_timer);
    ledc_timer_set(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, LEDC_PWM_HZ, LEDC_TIMER_8_BIT, LEDC_REF_TICK);

    ledc_channel_config_t ledc_channel;
    memset(&ledc_channel, 0, sizeof(ledc_channel));
    ledc_channel.channel = led_driver_channel;
    ledc_channel.duty = BRIGHTNESS_MAX;
    ledc_channel.gpio_num = config->gpio;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel);
}

static void SetDisplayBrightness(uint8_t brightness)
{
    if (ledc_set_duty(LEDC_HIGH_SPEED_MODE, led_driver_channel, brightness) ||
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, led_driver_channel)) {
        ESP_LOGE(TAG, "Failed to set display brightness...");
    }
}

static esp_err_t InitDisplay()
{
    esp_err_t err;
    spi_lobo_device_handle_t spi;
    spi_lobo_bus_config_t buscfg;
    memset((void *)&buscfg, 0, sizeof(buscfg));
    buscfg.miso_io_num = PIN_NUM_MISO; // set SPI MISO pin
    buscfg.mosi_io_num = PIN_NUM_MOSI; // set SPI MOSI pin
    buscfg.sclk_io_num = PIN_NUM_CLK; // set SPI CLK pin
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    spi_lobo_device_interface_config_t devcfg;
    memset((void *)&devcfg, 0, sizeof(devcfg));
    devcfg.clock_speed_hz = TFT_SPI_CLOCK_INIT_HZ;
    devcfg.mode = 0; // SPI mode 0
    devcfg.spics_io_num = -1; // we will use external CS pin
    devcfg.spics_ext_io_num = PIN_NUM_CS; // external CS pi
    devcfg.flags = LB_SPI_DEVICE_HALFDUPLEX; // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    tft_max_rdclock = TFT_SPI_CLOCK_INIT_HZ;

    // Initialize all pins used by display driver.
    TFT_PinsInit();
    // Initialize SPI bus and add a device for the display.
    err = spi_lobo_bus_add_device(TFT_HSPI_HOST, &buscfg, &devcfg, &spi);
    if (err != ESP_OK) {
        return err;
    }

    // Configure the display to use the new SPI device.
    tft_disp_spi = spi;
    err = spi_lobo_device_select(spi, 1);
    if (err != ESP_OK) {
        return err;
    }
    err = spi_lobo_device_deselect(spi);
    if (err != ESP_OK) {
        return err;
    }
    // Initialize the display driver.
    TFT_display_init();
    // Detect maximum read speed and set it.
    tft_max_rdclock = find_rd_speed();
    spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);

    TFT_setGammaCurve(0);
    TFT_setRotation(LANDSCAPE);
    TFT_resetclipwin();

    DisplayWidth = (uint16_t)(1 + tft_dispWin.x2 - tft_dispWin.x1);
    DisplayHeight = (uint16_t)(1 + tft_dispWin.y2 - tft_dispWin.y1);
    ESP_LOGI(TAG, "Display initialized (height %u, width %u)", DisplayHeight, DisplayWidth);

    TFT_invertDisplay(INVERT_OFF);

    return ESP_OK;
}

led_driver_handle_t led_driver_init(led_driver_config_t *config)
{
    static bool init_done = false;
    if (init_done) {
        ESP_LOGE(TAG, "This led driver only supports one led initialisation.");
        return NULL;
    }
    ESP_LOGI(TAG, "Initializing led driver");
    esp_err_t err = ESP_OK;
    err = InitDisplay();
    if (err != ESP_OK) {
        return NULL;
    }
    SetupBrightnessControl(config);
    SetDisplayBrightness(BRIGHTNESS_MAX);
    init_done = true;

    /* Temporary handle. Not being used anywhere. */
    static int handle = 1;
    return (led_driver_handle_t)handle;
}

esp_err_t led_driver_set_power(led_driver_handle_t handle, bool power)
{
    current_power = power;
    return led_driver_set_brightness(handle, current_brightness);
}

esp_err_t led_driver_set_RGB(led_driver_handle_t handle)
{
    TFT_fillWindow(TFT_BLACK);
    TFT_fillCircle(DisplayWidth / 2, DisplayHeight / 2, DisplayWidth / 4, (color_t){mRGB.red, mRGB.green, mRGB.blue});
    TFT_drawCircle(DisplayWidth / 2, DisplayHeight / 2, DisplayWidth / 4, (color_t){255, 255, 255});
    return ESP_OK;
}

esp_err_t led_driver_set_brightness(led_driver_handle_t handle, uint8_t brightness)
{
    if (brightness != 0) {
        current_brightness = brightness;
    }
    if (!current_power) {
        brightness = 0;
    }
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}

esp_err_t led_driver_set_hue(led_driver_handle_t handle, uint16_t hue)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    current_HS.hue = hue;
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}

esp_err_t led_driver_set_saturation(led_driver_handle_t handle, uint8_t saturation)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    current_HS.saturation = saturation;
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}

esp_err_t led_driver_set_temperature(led_driver_handle_t handle, uint32_t temperature)
{
    uint8_t brightness = current_power ? current_brightness : 0;
    current_temperature = temperature;
    temp_to_hs(current_temperature, &current_HS);
    hsv_to_rgb(current_HS, brightness, &mRGB);
    return led_driver_set_RGB(handle);
}
