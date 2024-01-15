// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
// limitations under the License.
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "epaper.h"
#include "esp_log.h"

static const char* TAG = "epaper";
uint8_t _buffer[4000] = {0x00};
bool iot_power_is_on;
int16_t iot_current_page;

#define EPAPER_CS_SETUP_NS      55
#define EPAPER_CS_HOLD_NS       60
#define EPAPER_1S_NS            1000000000
#define EPAPER_QUE_SIZE_DEFAULT 10

const unsigned char lut_partial[] =
{
  0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x40, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2,
  0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0,
};

#define EPAPER_ENTER_CRITICAL(mux)    portENTER_CRITICAL(mux)
#define EPAPER_EXIT_CRITICAL(mux)     portEXIT_CRITICAL(mux)

bool iot_using_partial_mode;
// LCD data/command
typedef struct {
    uint8_t dc_io;
    uint8_t dc_level;
} epaper_dc_t;

typedef struct {
    spi_device_handle_t bus;
    epaper_conf_t pin;       /* EPD properties */
    epaper_paint_t paint;   /* Paint properties */
    epaper_dc_t dc;
    SemaphoreHandle_t spi_mux;
} epaper_dev_t;

/*This function is called (in irq context!) just before a transmission starts.
It will set the D/C line to the value indicated in the user field */
static void iot_epaper_pre_transfer_callback(spi_transaction_t *t)
{
    epaper_dc_t *dc = (epaper_dc_t *) t->user;
    gpio_set_level((int)dc->dc_io, (int)dc->dc_level);
}

static esp_err_t _iot_epaper_spi_send(spi_device_handle_t spi, spi_transaction_t* t)
{
    return spi_device_transmit(spi, t);
}

void iot_epaper_send(spi_device_handle_t spi, const uint8_t *data, int len, epaper_dc_t *dc)
{
    esp_err_t ret;
    if (len == 0) {
        return;    //no need to send anything
    }
    spi_transaction_t t = {
        .length = len * 8,              // Len is in bytes, transaction length is in bits.
        .tx_buffer = data,              // Data
        .user = (void *) dc,            // D/C needs to be set to 1
    };
    ret = _iot_epaper_spi_send(spi, &t);
    assert(ret == ESP_OK);              // Should have had no issues.
}

static void iot_epaper_send_command(epaper_handle_t dev, unsigned char command)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    device->dc.dc_io = device->pin.dc_pin;
    device->dc.dc_level = device->pin.dc_lev_cmd;
    iot_epaper_send(device->bus, &command, 1, &device->dc);
}

static void iot_epaper_send_byte(epaper_handle_t dev, const uint8_t data)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    device->dc.dc_io = device->pin.dc_pin;
    device->dc.dc_level = device->pin.dc_lev_data;
    iot_epaper_send(device->bus, &data, 1, &device->dc);
}

static void iot_epaper_send_data(epaper_handle_t dev, const uint8_t *data, int length)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    device->dc.dc_io = device->pin.dc_pin;
    device->dc.dc_level = device->pin.dc_lev_data;
    // This SPI slave only support slow write mode
    // We can just send data byte by byte.
    int idx = 0;
    while(idx < length) {
        iot_epaper_send_byte(dev, data[idx++]);
    }
}

static void iot_epaper_gpio_init(epaper_conf_t * pin)
{
    esp_rom_gpio_pad_select_gpio(pin->cs_pin);
    gpio_set_direction(pin->cs_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin->cs_pin, 1);

    esp_rom_gpio_pad_select_gpio(pin->dc_pin);
    gpio_set_direction(pin->dc_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin->dc_pin, 1);

    esp_rom_gpio_pad_select_gpio(pin->reset_pin);
    gpio_set_direction(pin->reset_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin->reset_pin, 1);

    esp_rom_gpio_pad_select_gpio(pin->busy_pin);
    gpio_set_direction(pin->busy_pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin->busy_pin, GPIO_PULLUP_ONLY);
}

static esp_err_t iot_epaper_spi_init(epaper_handle_t dev, spi_device_handle_t *e_spi, epaper_conf_t *pin)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,                   // set SPI MISO pin
        .mosi_io_num = pin->mosi_pin,        // set SPI MOSI pin
        .sclk_io_num = pin->sck_pin,         // set SPI CLK pin
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,                // max transfer size is 4k bytes
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = pin->clk_freq_hz, // SPI clock is 40 MHz
        .mode = 0,                        // SPI mode 0
        .spics_io_num = pin->cs_pin,               // we will use external CS pin
        .cs_ena_pretrans = EPAPER_CS_SETUP_NS / (EPAPER_1S_NS / (pin->clk_freq_hz)) + 2,
        .cs_ena_posttrans = EPAPER_CS_HOLD_NS / (EPAPER_1S_NS / (pin->clk_freq_hz)) + 2,
        .queue_size = EPAPER_QUE_SIZE_DEFAULT,                //
        .flags = (SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE), // ALWAYS SET to HALF DUPLEX MODE for display spi !!
        .pre_cb = iot_epaper_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(pin->spi_host, &buscfg, 1);
    assert(ret == ESP_OK);
    //Attach the EPD to the SPI bus
    ret = spi_bus_add_device(pin->spi_host, &devcfg, e_spi);
    assert(ret == ESP_OK);
    return ret;
}

void iot_epaper_epd_init(epaper_handle_t dev, uint8_t Data)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    /* EPD hardware init start */
    iot_epaper_reset(dev);
    iot_Init_Full(dev,0x03);

    for (uint16_t x = 0; x < sizeof(_buffer); x++)
    {
        _buffer[x] = Data;
    }

    //update
    iot_epaper_send_command(dev,0x24);
    for (uint16_t y = 0; y < GxDEPG0213BN_HEIGHT; y++)
    {
        for (uint16_t x = 0; x < GxDEPG0213BN_WIDTH / 8; x++)
        {
        uint16_t idx = y * (GxDEPG0213BN_WIDTH / 8) + x;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        iot_epaper_send_byte(dev,~data);
        }
    }
    iot_epaper_send_command(dev,0x26); // both buffers same for full b/w
    for (uint16_t y = 0; y < GxDEPG0213BN_HEIGHT; y++)
    {
        for (uint16_t x = 0; x < GxDEPG0213BN_WIDTH / 8; x++)
        {
        uint16_t idx = y * (GxDEPG0213BN_WIDTH / 8) + x;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        iot_epaper_send_byte(dev,~data);
        }
    }
    //update_full
    iot_Update_Full(dev);

    //power off
    iot_PowerOff(dev);
    //esp_rom_delay_us(500000);
    iot_current_page = -1;
    xSemaphoreGiveRecursive(device->spi_mux);
    /* EPD hardware init end */
}

epaper_handle_t iot_epaper_create(spi_device_handle_t bus, epaper_conf_t *epconf)
{
    epaper_dev_t* dev = (epaper_dev_t*) calloc(1, sizeof(epaper_dev_t));
    dev->spi_mux = xSemaphoreCreateRecursiveMutex();
    iot_epaper_gpio_init(epconf);
    ESP_LOGI(TAG, "gpio init ok");
    if (bus) {
        dev->bus = bus;
    } else {
        iot_epaper_spi_init(dev, &dev->bus, epconf);
        ESP_LOGI(TAG, "spi init ok");
    }
    dev->pin = *epconf;
    iot_epaper_epd_init(dev, 0x00);
    return (epaper_handle_t) dev;
}

esp_err_t iot_epaper_delete(epaper_handle_t dev, bool del_bus)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    iot_epaper_send_command(dev, E_PAPER_POWER_OFF);
    spi_bus_remove_device(device->bus);
    if (del_bus) {
        spi_bus_free(device->pin.spi_host);
    }
    vSemaphoreDelete(device->spi_mux);
    free(device);
    return ESP_OK;
}

int iot_epaper_get_width(epaper_handle_t dev)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    return device->paint.width;
}

void iot_epaper_set_width(epaper_handle_t dev, int width)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    device->paint.width = width % 8 ? width + 8 - (width % 8) : width;
    xSemaphoreGiveRecursive(device->spi_mux);

}

int iot_epaper_get_height(epaper_handle_t dev)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    return device->paint.height;
}

void iot_epaper_set_height(epaper_handle_t dev, int height)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    device->paint.height = height;
    xSemaphoreGiveRecursive(device->spi_mux);
}

int iot_epaper_get_rotate(epaper_handle_t dev)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    return device->paint.Rotation;
}

void iot_epaper_set_rotate(epaper_handle_t dev, int rotate)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    device->paint.Rotation = rotate;
    xSemaphoreGiveRecursive(device->spi_mux);
}

void iot_epaper_clean_paint(epaper_handle_t dev, int colored)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    for (int x = 0; x < device->paint.width; x++) {
        for (int y = 0; y < device->paint.height; y++) {
            iot_drawPixel(dev, x, y, colored);
        }
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this displays a string on the frame buffer but not refresh
 */
void iot_epaper_draw_string(epaper_handle_t dev, int x, int y, const char* text, epaper_font_t* font, int colored)
{
    const char* p_text = text;
    unsigned int counter = 0;
    int refcolumn = x;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    /* Send the string character by character on EPD */
    while (*p_text != 0) {
        /* Display one character on EPD */
        iot_epaper_draw_char(dev, refcolumn, y, *p_text, font, colored);
        /* Decrement the column position by 16 */
        refcolumn += font->width;
        /* Point on the next character */
        p_text++;
        counter++;
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a charactor on the frame buffer but not refresh
 */
void iot_epaper_draw_char(epaper_handle_t dev, int x, int y, char ascii_char, epaper_font_t* font, int colored)
{
    int i, j;
    unsigned int char_offset = (ascii_char - ' ') * font->height * (font->width / 8 + (font->width % 8 ? 1 : 0));
    const unsigned char* ptr = &font->font_table[char_offset];
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    for (j = 0; j < font->height; j++) {
        for (i = 0; i < font->width; i++) {
            if (*ptr & (0x80 >> (i % 8))) {
                iot_drawPixel(dev, x + i, y + j, colored);
            }
            if (i % 8 == 7) {
                ptr++;
            }
        }
        if (font->width % 8 != 0) {
            ptr++;
        }
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a line on the frame buffer
 */
void iot_epaper_draw_line(epaper_handle_t dev, int x0, int y0, int x1, int y1,
        int colored)
{
    /* Bresenham algorithm */
    int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    while ((x0 != x1) && (y0 != y1)) {
        iot_drawPixel(dev, x0, y0, colored);
        if (2 * err >= dy) {
            err += dy;
            x0 += sx;
        }
        if (2 * err <= dx) {
            err += dx;
            y0 += sy;
        }
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a horizontal line on the frame buffer
 */
void iot_epaper_draw_horizontal_line(epaper_handle_t dev, int x, int y, int width, int colored)
{
    int i;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    for (i = x; i < x + width; i++) {
        iot_drawPixel(dev, i, y, colored);
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a vertical line on the frame buffer
 */
void iot_epaper_draw_vertical_line(epaper_handle_t dev, int x, int y, int height, int colored)
{
    int i;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    for (i = y; i < y + height; i++) {
        iot_drawPixel(dev, x, i, colored);
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a rectangle
 */
void iot_epaper_draw_rectangle(epaper_handle_t dev, int x0, int y0, int x1, int y1, int colored)
{
    int min_x, min_y, max_x, max_y;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    iot_epaper_draw_horizontal_line(dev, min_x, min_y, max_x - min_x + 1, colored);
    iot_epaper_draw_horizontal_line(dev, min_x, max_y, max_x - min_x + 1, colored);
    iot_epaper_draw_vertical_line(dev, min_x, min_y, max_y - min_y + 1, colored);
    iot_epaper_draw_vertical_line(dev, max_x, min_y, max_y - min_y + 1, colored);
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a filled rectangle
 */
void iot_epaper_draw_filled_rectangle(epaper_handle_t dev, int x0, int y0, int x1, int y1, int colored)
{
    int min_x, min_y, max_x, max_y;
    int i;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    for (i = min_x; i <= max_x; i++) {
        iot_epaper_draw_vertical_line(dev, i, min_y, max_y - min_y + 1, colored);
    }
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a circle
 */
void iot_epaper_draw_circle(epaper_handle_t dev, int x, int y, int radius,
        int colored)
{
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    do {
        iot_drawPixel(dev, x - x_pos, y + y_pos, colored);
        iot_drawPixel(dev, x + x_pos, y + y_pos, colored);
        iot_drawPixel(dev, x + x_pos, y - y_pos, colored);
        iot_drawPixel(dev, x - x_pos, y - y_pos, colored);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 *  @brief: this draws a filled circle
 */
void iot_epaper_draw_filled_circle(epaper_handle_t dev, int x, int y, int radius, int colored)
{
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    do {
        iot_drawPixel(dev, x - x_pos, y + y_pos, colored);
        iot_drawPixel(dev, x + x_pos, y + y_pos, colored);
        iot_drawPixel(dev, x + x_pos, y - y_pos, colored);
        iot_drawPixel(dev, x - x_pos, y - y_pos, colored);
        iot_epaper_draw_horizontal_line(dev, x + x_pos, y + y_pos, 2 * (-x_pos) + 1, colored);
        iot_epaper_draw_horizontal_line(dev, x + x_pos, y - y_pos, 2 * (-x_pos) + 1, colored);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 * @brief  wait until idle
 */
void iot_epaper_wait_idle(epaper_handle_t dev)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    while (gpio_get_level((gpio_num_t) device->pin.busy_pin) == 1) {      //1: busy, 0: idle fixed
        vTaskDelay(10 / portTICK_PERIOD_MS);
        //ESP_LOGI(TAG, "Busy");
    }
    //ESP_LOGI(TAG, "Done!");
}

/**
 * @brief  setRamDataEntryMode
 */
void iot_setRamDataEntryMode(epaper_handle_t dev, uint8_t em)
{
  const uint16_t xPixelsPar = GxDEPG0213BN_X_PIXELS - 1;
  const uint16_t yPixelsPar = GxDEPG0213BN_Y_PIXELS - 1;
  em = gx_uint16_min(em, 0x03);
  iot_epaper_send_command(dev,0x11);
  iot_epaper_send_byte(dev,em);
  switch (em)
  {
    case 0x00: // x decrease, y decrease
      iot_SetRamArea(dev,xPixelsPar / 8, 0x00, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
      iot_SetRamPointer(dev,xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256); // set ram
      break;
    case 0x01: // x increase, y decrease : as in demo code
      iot_SetRamArea(dev,0x00, xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
      iot_SetRamPointer(dev,0x00, yPixelsPar % 256, yPixelsPar / 256); // set ram
      break;
    case 0x02: // x decrease, y increase
      iot_SetRamArea(dev,xPixelsPar / 8, 0x00, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
      iot_SetRamPointer(dev,xPixelsPar / 8, 0x00, 0x00); // set ram
      break;
    case 0x03: // x increase, y increase : normal mode
      iot_SetRamArea(dev,0x00, xPixelsPar / 8, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
      iot_SetRamPointer(dev,0x00, 0x00, 0x00); // set ram
      break;
  }
}

/**
 * @brief  SetRamArea
 */
void iot_SetRamArea(epaper_handle_t dev, uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
  iot_epaper_send_command(dev,0x44);
  iot_epaper_send_byte(dev,Xstart);
  iot_epaper_send_byte(dev,Xend);
  iot_epaper_send_command(dev,0x45);
  iot_epaper_send_byte(dev,Ystart);
  iot_epaper_send_byte(dev,Ystart1);
  iot_epaper_send_byte(dev,Yend);
  iot_epaper_send_byte(dev,Yend1);
}

/**
 * @brief  SetRamPointer
 */
void iot_SetRamPointer(epaper_handle_t dev, uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
  iot_epaper_send_command(dev,0x4e);
  iot_epaper_send_byte(dev,addrX);
  iot_epaper_send_command(dev,0x4f);
  iot_epaper_send_byte(dev,addrY);
  iot_epaper_send_byte(dev,addrY1);
}

/**
 * @brief  Draw a picture with a bit map
 */
void iot_drawBitmapBM(epaper_handle_t dev, const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode)
{
  uint16_t inverse_color = (color != GxEPD_WHITE) ? GxEPD_WHITE : GxEPD_BLACK;
  uint16_t fg_color = (mode & bm_invert) ? inverse_color : color;
  uint16_t bg_color = (mode & bm_invert) ? color : inverse_color;
  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;
  if (mode & bm_transparent)
  {
    for (uint16_t j = 0; j < h; j++)
    {
      for (uint16_t i = 0; i < w; i++ )
      {
        if (i & 7) byte <<= 1;
        else
        {
          byte = bitmap[j * byteWidth + i / 8];
        }
        // transparent mode
        if (!(byte & 0x80))
        {
          uint16_t xd = x + i;
          uint16_t yd = y + j;
          if (mode & bm_flip_x) xd = x + w - i;
          if (mode & bm_flip_y) yd = y + h - j;
          iot_drawPixel(dev, xd, yd, color);
        }
      }
    }
  }
  else
  {
    for (uint16_t j = 0; j < h; j++)
    {
      for (uint16_t i = 0; i < w; i++ )
      {
        if (i & 7) byte <<= 1;
        else
        {
          byte = bitmap[j * byteWidth + i / 8];
        }
        // keep using overwrite mode
        uint16_t pixelcolor = (byte & 0x80) ? fg_color  : bg_color;
        uint16_t xd = x + i;
        uint16_t yd = y + j;
        if (mode & bm_flip_x) xd = x + w - i;
        if (mode & bm_flip_y) yd = y + h - j;
        iot_drawPixel(dev, xd, yd, pixelcolor);
      }
    }
  }
}

/**
 * @brief  InitDisplay
 */
void iot_InitDisplay(epaper_handle_t dev, uint8_t em)
{
    iot_epaper_send_command(dev,0x12);  //SWRESET
    esp_rom_delay_us(10000);
    iot_epaper_send_command(dev,0x01); //Driver output control
    iot_epaper_send_byte(dev,0x27);
    iot_epaper_send_byte(dev,0x01);
    iot_epaper_send_byte(dev,0x00);
    iot_epaper_send_command(dev,0x3C); //BorderWavefrom
    iot_epaper_send_byte(dev,0x05);
    iot_epaper_send_command(dev,0x21); //  Display update control
    iot_epaper_send_byte(dev,0x00);
    iot_epaper_send_byte(dev,0x80);
    iot_epaper_send_command(dev,0x18); //Read built-in temperature sensor
    iot_epaper_send_byte(dev,0x80);
    iot_setRamDataEntryMode(dev, em);
}

/**
 * @brief  Update part of the display
 */
void iot_Update_Part(epaper_handle_t dev)
{
  iot_epaper_send_command(dev,0x22);
  iot_epaper_send_byte(dev,0xcc);
  iot_epaper_send_command(dev,0x20);
  iot_epaper_wait_idle(dev);
}

/**
 * @brief  Update full of the display
 */
void iot_Update_Full(epaper_handle_t dev)
{
  iot_epaper_send_command(dev,0x22);
  iot_epaper_send_byte(dev,0xf7); // disable analog (powerOff() here)
  iot_epaper_send_command(dev,0x20);
  iot_epaper_wait_idle(dev);
}

/**
 * @brief  Init for full update the display
 */
void iot_Init_Full(epaper_handle_t dev,uint8_t em)
{
  iot_InitDisplay(dev, em);
  //iot_PowerOn(dev);         //bug here?
}

/**
 * @brief  Init_Part
 */
void iot_Init_Part(epaper_handle_t dev,uint8_t em)
{
  iot_InitDisplay(dev,em);
  iot_epaper_send_command(dev, 0x32);
  for (uint16_t i = 0; i < sizeof(lut_partial); i++)
  {
    iot_epaper_send_byte(dev,pgm_read_byte(&lut_partial[i]));
  }
  //iot_PowerOn(dev);         //bug here?
}

/**
 * @brief  Draw a pixel of the display
 */
void iot_drawPixel(epaper_handle_t dev, int16_t x, int16_t y, uint16_t color)
{
  y = y + 6;       //avoid can't write pixel 
  if ((x < 0) || (x >= 250) || (y < 0) || (y >= 128)) return;
  uint16_t t;
  switch (iot_epaper_get_rotate(dev))
  {
    case 1:
      t = x;
      x = y;
      y = t;
      x = GxDEPG0213BN_WIDTH - x - 1;
      break;
    case 2:
      x = GxDEPG0213BN_WIDTH - x - 1;
      y = GxDEPG0213BN_HEIGHT - y - 1;
      break;
    case 3:
      t = x;
      x = y;
      y = t;
      y = GxDEPG0213BN_HEIGHT - y - 1;
      break;
  }
  // check rotation, move pixel around if necessary
  uint16_t i = x / 8 + y * GxDEPG0213BN_WIDTH / 8;
  if (iot_current_page < 1)
  {
    if (i >= sizeof(_buffer)) return;
  }
  else
  {
    y -= iot_current_page * GxDEPG0213BN_PAGE_HEIGHT;
    if ((y < 0) || (y >= GxDEPG0213BN_PAGE_HEIGHT)) return;
    i = x / 8 + y * GxDEPG0213BN_WIDTH / 8;
  }

  if (!color)
    _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
  else
    _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}

/**
 * @brief  Turn on the display's power
 */
void iot_PowerOn(epaper_handle_t dev)
{
  if (!iot_power_is_on)
  {
    iot_epaper_send_command(dev,(0x22));
    iot_epaper_send_byte(dev,(0xc0));
    iot_epaper_send_command(dev,(0x20));
    iot_epaper_wait_idle(dev);
    iot_power_is_on = true;
  }
}

/**
 * @brief  Turn off the display's power
 */
void iot_PowerOff(epaper_handle_t dev)
{
  if (iot_power_is_on)
  {
    iot_epaper_send_command(dev,(0x22));
    iot_epaper_send_byte(dev,(0x83));
    iot_epaper_send_command(dev,(0x20));
    iot_epaper_wait_idle(dev);
    iot_power_is_on = false;
  }
}

/**
 * @brief  Update part of the display
 */
void iot_epaper_updateWindow(epaper_handle_t dev,uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
  if (using_rotation) iot_rotate(&x, &y, &w, &h);
  if (x >= GxDEPG0213BN_WIDTH) return;
  if (y >= GxDEPG0213BN_HEIGHT) return;
  uint16_t xe = gx_uint16_min(GxDEPG0213BN_WIDTH, x + w) - 1;
  uint16_t ye = gx_uint16_min(GxDEPG0213BN_HEIGHT, y + h) - 1;
  uint16_t xs_d8 = x / 8;
  uint16_t xe_d8 = xe / 8;
  iot_Init_Part(dev, 0x03);

  iot_SetRamArea(dev, xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
  iot_SetRamPointer(dev, xs_d8, y % 256, y / 256); // set ram
  iot_epaper_wait_idle(dev);   //should exist?
  iot_epaper_send_command(dev,0x24);
  for (uint16_t y1 = y; y1 <= ye; y1++)
  {
    for (uint16_t x1 = xs_d8; x1 <= xe_d8; x1++)
    {
      uint16_t idx = y1 * (GxDEPG0213BN_WIDTH / 8) + x1;
      uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
      iot_epaper_send_byte(dev,~data);
    }
  }
  iot_Update_Part(dev);
  esp_rom_delay_us(300000);

  // update previous buffer
  iot_SetRamArea(dev, xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
  iot_SetRamPointer(dev,xs_d8, y % 256, y / 256); // set ram
  iot_epaper_wait_idle(dev);
  iot_epaper_send_command(dev,(0x26));
  for (uint16_t y1 = y; y1 <= ye; y1++)
  {
    for (uint16_t x1 = xs_d8; x1 <= xe_d8; x1++)
    {
      uint16_t idx = y1 * (GxDEPG0213BN_WIDTH / 8) + x1;
      uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
      iot_epaper_send_byte(dev,(~data));
    }
  }
  esp_rom_delay_us(300000);
}

/**
 * @brief  Update full of the display
 */
void iot_epaper_update(epaper_handle_t dev)
{
    //iot_epaper_reset(dev);

    iot_Init_Full(dev,0x03);

    //update
    iot_epaper_send_command(dev,0x24);
    for (uint16_t y = 0; y < GxDEPG0213BN_HEIGHT; y++)
    {
        for (uint16_t x = 0; x < GxDEPG0213BN_WIDTH / 8; x++)
        {
        uint16_t idx = y * (GxDEPG0213BN_WIDTH / 8) + x;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        iot_epaper_send_byte(dev,~data);
        }
    }
    iot_epaper_send_command(dev,0x26); // both buffers same for full b/w
    for (uint16_t y = 0; y < GxDEPG0213BN_HEIGHT; y++)
    {
        for (uint16_t x = 0; x < GxDEPG0213BN_WIDTH / 8; x++)
        {
        uint16_t idx = y * (GxDEPG0213BN_WIDTH / 8) + x;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        iot_epaper_send_byte(dev,~data);
        }
    }
    //update_full
    iot_Update_Full(dev);

    //power off
    iot_PowerOff(dev);
    //iot_current_page = -1;
}

/**
 * @brief  reset device
 */
void iot_epaper_reset(epaper_handle_t dev)
{
    epaper_dev_t* device = (epaper_dev_t*) dev;
    xSemaphoreTakeRecursive(device->spi_mux, portMAX_DELAY);
    esp_rom_delay_us(20000);
    gpio_set_level((gpio_num_t) device->pin.reset_pin, 0);             //module reset
    esp_rom_delay_us(20000);
    gpio_set_level((gpio_num_t) device->pin.reset_pin, 1);
    esp_rom_delay_us(200000);
    iot_epaper_wait_idle(dev);
    xSemaphoreGiveRecursive(device->spi_mux);
}

/**
 * @brief  Exchange data in preparation for the screen rotation.
 */
void iot_rotate(uint16_t* x, uint16_t* y, uint16_t* w, uint16_t* h)
{
    uint16_t t = *x;
    *x = *y;
    *y = t;
    //swap(x, y);
    t = *w;
    *w = *h;
    *h = t;
    //swap(w, h);
    *x = GxDEPG0213BN_WIDTH - *x - *w;
}

/**
 * @brief  EraseDisplay(Not tested yet.)
 */
void iot_eraseDisplay(epaper_handle_t dev, bool using_partial_update)
{
  if (iot_current_page != -1) return;
  if (using_partial_update)
  {
    iot_using_partial_mode = true; // remember
    iot_Init_Part(dev, 0x01);
    iot_epaper_send_command(dev,(0x24));
    for (uint32_t i = 0; i < 4000; i++)
    {
      iot_epaper_send_byte(dev,(0xFF));
    }
    iot_Update_Part(dev);
    esp_rom_delay_us(300000);
    // update previous buffer
    iot_epaper_send_command(dev,(0x26));
    for (uint32_t i = 0; i < 4000; i++)
    {
      iot_epaper_send_byte(dev,(0xFF));
    }
    esp_rom_delay_us(300000);
  }
  else
  {
    iot_using_partial_mode = false; // remember
    iot_Init_Full(dev,0x01);
    iot_epaper_send_command(dev,(0x24));
    for (uint32_t i = 0; i < 4000; i++)
    {
      iot_epaper_send_byte(dev,(0xFF));
    }
    iot_epaper_send_command(dev,(0x26)); // both buffers same for full b/w
    for (uint32_t i = 0; i < 4000; i++)
    {
      iot_epaper_send_byte(dev,0x00);
    }
    iot_Update_Full(dev);
    iot_PowerOff(dev);
  }
}
