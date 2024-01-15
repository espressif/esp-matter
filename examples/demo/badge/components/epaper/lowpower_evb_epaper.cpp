/* Lowpower EVB Example

   For other examples please check:
   https://github.com/espressif/esp-iot-solution/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "epaper.h"
#include "epaper_fonts.h"
#include "imagedata.h"
#include "lowpower_evb_epaper.h"
#include "driver/gpio.h"

#include "qrcode.h"
static const char *TAG = "lowpower_evb_epaper";

#define EPAPER_POWER_CNTL_IO        ((gpio_num_t) 14)

/* Epaper pin definition */
#define MOSI_PIN        23
#define SCK_PIN         18
#define BUSY_PIN        4
#define DC_PIN          17
#define RST_PIN         16
#define CS_PIN          5

// CPowerCtrl *epaper_power = NULL;

static epaper_handle_t epaper = NULL;
const uint8_t Whiteboard[4000] = {0x00};

static void epaper_power_on()
{
    // epaper_power = new CPowerCtrl(EPAPER_POWER_CNTL_IO);
    // epaper_power->on();
}

static void epaper_gpio_init()
{
    epaper_conf_t epaper_conf;
    epaper_conf.busy_pin = BUSY_PIN;
    epaper_conf.cs_pin = CS_PIN;
    epaper_conf.dc_pin = DC_PIN;
    epaper_conf.mosi_pin = MOSI_PIN;
    epaper_conf.reset_pin = RST_PIN;
    epaper_conf.sck_pin = SCK_PIN;

    gpio_config_t epaper_gpio_config;
    epaper_gpio_config.pin_bit_mask = ((uint64_t)(((uint64_t)1)<<epaper_conf.busy_pin));
    epaper_gpio_config.mode = GPIO_MODE_INPUT;
    epaper_gpio_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    epaper_gpio_config.pull_up_en = GPIO_PULLUP_DISABLE;
    epaper_gpio_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&epaper_gpio_config);

    epaper_gpio_config.pin_bit_mask = (1<<epaper_conf.cs_pin)   | (1<<epaper_conf.dc_pin)   |
                                      (1<<epaper_conf.mosi_pin) | (1<<epaper_conf.reset_pin)| 
                                      (1<<epaper_conf.sck_pin);
    epaper_gpio_config.mode = GPIO_MODE_OUTPUT;

    gpio_config(&epaper_gpio_config);
    gpio_set_level((gpio_num_t)epaper_conf.cs_pin, 0);
    gpio_set_level((gpio_num_t)epaper_conf.dc_pin, 0);
    gpio_set_level((gpio_num_t)epaper_conf.mosi_pin, 0);
    gpio_set_level((gpio_num_t)epaper_conf.reset_pin, 0);
    gpio_set_level((gpio_num_t)epaper_conf.sck_pin, 0);
}

static void epaper_init()
{
    epaper_conf_t epaper_conf;
    epaper_conf.busy_pin = BUSY_PIN;
    epaper_conf.cs_pin = CS_PIN;
    epaper_conf.dc_pin = DC_PIN;
    epaper_conf.mosi_pin = MOSI_PIN;
    epaper_conf.reset_pin = RST_PIN;
    epaper_conf.sck_pin = SCK_PIN;

    epaper_conf.rst_active_level = 0;
    epaper_conf.dc_lev_data = 1;
    epaper_conf.dc_lev_cmd = 0;

    epaper_conf.clk_freq_hz = 20 * 1000 * 1000;
    epaper_conf.spi_host = HSPI_HOST;

    epaper_conf.color_inv = 0;

    ESP_LOGI(TAG, "before epaper init, heap: %ld", esp_get_free_heap_size());
    epaper = iot_epaper_create(NULL, &epaper_conf);
    iot_epaper_set_rotate(epaper, 3);
}

void epaper_show_page_loop()           //Partial refresh
{
    char string_show[70];
    static uint16_t time = 0;

    sprintf(string_show, "Time:%d", time);
    time++;
    iot_epaper_draw_string(epaper, 50, 110, string_show, &epaper_font_16, 0x0000);
    iot_epaper_updateWindow(epaper, 50, 110,  100,  18, true);
    iot_drawBitmapBM(epaper, Whiteboard, 50, 110, 100, 18, GxEPD_BLACK, true);

}
void epaper_matter_code(void)       //Display Matter pairing
{
    iot_drawBitmapBM(epaper, Whiteboard, 0, 0, 250, 128, GxEPD_BLACK, true);           //clean paint
    iot_drawBitmapBM(epaper, matter_comminsion, 0, 11, 250, 110, GxEPD_BLACK, true);
    //iot_epaper_update(epaper);                          
    iot_epaper_updateWindow(epaper, 0, 0,  250,  128, true);   //make screen pure!
}

void epaper_show_page_init(void)   //Display the initial interface
{
    iot_drawBitmapBM(epaper, matter_logo, 0, 0, 250, 122, GxEPD_BLACK, true);
    iot_epaper_draw_string(epaper, 40, 90, "Badge-Demo", &epaper_font_24, GxEPD_BLACK);
    //iot_epaper_update(epaper);                                
    iot_epaper_updateWindow(epaper, 0, 0,  250,  128, true);   //make screen pure!
}

void epaper_display_espressif_logo(void)
{
    // iot_drawBitmapBM(epaper, Whiteboard, 0, 0, 250, 128, GxEPD_BLACK, true);
    iot_drawBitmapBM(epaper, esp_logo, 0, 0, 250, 122, GxEPD_BLACK, true);
    iot_epaper_updateWindow(epaper, 0, 0,  250,  128, true);   //make screen pure!
}

void epaper_light_power(bool power)
{
    if (power) {
        iot_drawBitmapBM(epaper, Whiteboard, 0, -7, 250, 128, GxEPD_BLACK, true);
        iot_drawBitmapBM(epaper, matter_light_on, 0, 0, 250, 122, GxEPD_BLACK, true);
        iot_epaper_draw_string(epaper, 40, 90, "light On", &epaper_font_24, GxEPD_BLACK);
        iot_epaper_updateWindow(epaper, 0, 0, 250, 122, true);
    } else {
        iot_drawBitmapBM(epaper, Whiteboard, 0, -7, 250, 128, GxEPD_BLACK, true);
        iot_drawBitmapBM(epaper, matter_light_off, 0, 0, 250, 122, GxEPD_BLACK, true);
        iot_epaper_draw_string(epaper, 40, 90, "light Off", &epaper_font_24, GxEPD_BLACK);
        iot_epaper_updateWindow(epaper, 0, 0, 250, 122, true);
    }
}

void display_commission_qr_code()
{
    iot_drawBitmapBM(epaper, Whiteboard, 0, 0, 250, 122, GxEPD_BLACK, true);
    iot_drawBitmapBM(epaper, qr_code, 0, -7, 250, 122, GxEPD_BLACK, true);
    iot_epaper_updateWindow(epaper, 0, 0, 250, 122, true);
}

void display_vcard(char *vcard, uint32_t x_offset, uint32_t y_offset)
{
    QRCode qrcode;
    uint8_t version = 9, ecc = 1;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, ecc, vcard);
    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            iot_drawPixel(epaper, x + x_offset, y + y_offset, qrcode_getModule(&qrcode, x, y) ? BLACK_COLORED : UNCOLORED);
        }
    }
}

void epaper_display_badge(char *name, char *company_name, char *email, char *contact, char *event_name)
{
    iot_drawBitmapBM(epaper, Whiteboard, 0, -7, 250, 122, GxEPD_BLACK, true);
    iot_epaper_updateWindow(epaper, 0, 0, 250, 122, true);
    iot_drawBitmapBM(epaper, Whiteboard, 0, -7, 250, 122, GxEPD_BLACK, true);
    iot_epaper_updateWindow(epaper, 0, 0, 250, 122, true);
    char *vcard;// = (char*) malloc(sizeof(char) * vcard_size);
    asprintf(&vcard,"BEGIN:VCARD\nVERSION:3.0\nN:%s\nORG:%s\nEMAIL:%s\nTEL;TYPE=voice,work,pref:%s\nEND:VCARD", name, company_name, email, contact);
    display_vcard(vcard, 137, 60);
    iot_epaper_draw_string(epaper, 5, 5, name, &epaper_font_meslo_11, GxEPD_BLACK);
    iot_epaper_draw_string(epaper, 5, 35, company_name, &epaper_font_meslo_11, GxEPD_BLACK);
    iot_epaper_draw_horizontal_line(epaper, 0, 54, 250, BLACK_COLORED);
    char event_name_1[19];
    memcpy(event_name_1, event_name, strlen(event_name) > 19 ? 19 : strlen(event_name));
    event_name_1[strlen(event_name) > 19 ? 19 : strlen(event_name)] = '\0';
    iot_epaper_draw_string(epaper, 5, 70, event_name_1, &epaper_font_meslo_8, GxEPD_BLACK);
    if (strlen(event_name) > 19) {
        char event_name_2[19];
        memcpy(event_name_2, &event_name[19], strlen(&event_name[19]) > 19 ? 19 : strlen(&event_name[19]));
        event_name_2[strlen(&event_name[19]) > 19 ? 19 : strlen(&event_name[19])] = '\0';
        iot_epaper_draw_string(epaper, 5, 85, event_name_2, &epaper_font_meslo_8, GxEPD_BLACK);
    }
    iot_drawBitmapBM(epaper, esp_logo_1, 195, 60, 53, 53, GxEPD_BLACK, true);
    iot_epaper_updateWindow(epaper, 0, 0, 250, 122, true);
    free(vcard);
}

void epaper_disable()
{
    iot_epaper_delete(epaper, true);
}

void epaper_display_init()
{
    epaper_gpio_init();
    epaper_power_on();
    vTaskDelay(10 / portTICK_PERIOD_MS);  // 10ms delay after power on
    epaper_init();
}

