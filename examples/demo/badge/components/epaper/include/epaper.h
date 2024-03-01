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
#ifndef _IOT_E_PAPER_H_
#define _IOT_E_PAPER_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "driver/spi_master.h"

typedef struct
{
    uint16_t width;
    uint16_t height;
    const uint8_t *font_table;
} epaper_font_t;

#define BLACK_COLORED   0
#define RED_COLORED     1
#define UNCOLORED       2

// epd2in7 commands
#define E_PAPER_PANEL_SETTING                               0x00
#define E_PAPER_POWER_SETTING                               0x01
#define E_PAPER_POWER_OFF                                   0x02
#define E_PAPER_POWER_OFF_SEQUENCE_SETTING                  0x03
#define E_PAPER_POWER_ON                                    0x04
#define E_PAPER_POWER_ON_MEASURE                            0x05
#define E_PAPER_BOOSTER_SOFT_START                          0x06
#define E_PAPER_DEEP_SLEEP                                  0x07
#define E_PAPER_DATA_START_TRANSMISSION_1                   0x10
#define E_PAPER_DATA_STOP                                   0x11
#define E_PAPER_DISPLAY_REFRESH                             0x12
#define E_PAPER_DATA_START_TRANSMISSION_2                   0x13
#define E_PAPER_PARTIAL_DATA_START_TRANSMISSION_1           0x14
#define E_PAPER_PARTIAL_DATA_START_TRANSMISSION_2           0x15
#define E_PAPER_PARTIAL_DISPLAY_REFRESH                     0x16
#define E_PAPER_LUT_FOR_VCOM                                0x20
#define E_PAPER_LUT_WHITE_TO_WHITE                          0x21
#define E_PAPER_LUT_BLACK_TO_WHITE                          0x22
#define E_PAPER_LUT_WHITE_TO_BLACK                          0x23
#define E_PAPER_LUT_BLACK_TO_BLACK                          0x24
#define E_PAPER_PLL_CONTROL                                 0x30
#define E_PAPER_TEMPERATURE_SENSOR_COMMAND                  0x40
#define E_PAPER_TEMPERATURE_SENSOR_CALIBRATION              0x41
#define E_PAPER_TEMPERATURE_SENSOR_WRITE                    0x42
#define E_PAPER_TEMPERATURE_SENSOR_READ                     0x43
#define E_PAPER_VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define E_PAPER_LOW_POWER_DETECTION                         0x51
#define E_PAPER_TCON_SETTING                                0x60
#define E_PAPER_TCON_RESOLUTION                             0x61
#define E_PAPER_SOURCE_AND_GATE_START_SETTING               0x62
#define E_PAPER_GET_STATUS                                  0x71
#define E_PAPER_AUTO_MEASURE_VCOM                           0x80
#define E_PAPER_VCOM_VALUE                                  0x81
#define E_PAPER_VCM_DC_SETTING_REGISTER                     0x82
#define E_PAPER_PROGRAM_MODE                                0xA0
#define E_PAPER_ACTIVE_PROGRAM                              0xA1
#define E_PAPER_READ_OTP_DATA                               0xA2

#define GxDEPG0213BN_X_PIXELS 128
#define GxDEPG0213BN_Y_PIXELS 250

#define GxDEPG0213BN_WIDTH GxDEPG0213BN_X_PIXELS
#define GxDEPG0213BN_HEIGHT GxDEPG0213BN_Y_PIXELS

#define GxDEPG0213BN_PAGES 4

#define GxDEPG0213BN_PAGE_HEIGHT (GxDEPG0213BN_HEIGHT / GxDEPG0213BN_PAGES)
#define GxDEPG0213BN_PAGE_SIZE (GxDEPG0213BN_BUFFER_SIZE / GxDEPG0213BN_PAGES)

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#define GxEPD_WHITE     0xFFFF
#define GxEPD_BLACK     0x0000

enum bm_mode //BM_ModeSet
{
        bm_normal = 0,
        bm_default = 1, // for use for BitmapExamples
        // these potentially can be combined
        bm_invert = (1 << 1),
        bm_flip_x = (1 << 2),
        bm_flip_y = (1 << 3),
        bm_r90 = (1 << 4),
        bm_r180 = (1 << 5),
        bm_r270 = bm_r90 | bm_r180,
        bm_partial_update = (1 << 6),
        bm_invert_red = (1 << 7),
        bm_transparent = (1 << 8)
};

/* Paint properties */
typedef struct {
    uint8_t Rotation;
    int width;
    int height;
} epaper_paint_t;

/* EPD properties */
typedef struct {
    int reset_pin;
    int dc_pin;
    int cs_pin;
    int busy_pin;
    int mosi_pin;
    int miso_pin;
    int sck_pin;

    uint8_t rst_active_level;
    uint8_t dc_lev_data;
    uint8_t dc_lev_cmd;

    int clk_freq_hz;
    spi_host_device_t spi_host;

    int width;
    int height;
    bool color_inv;
} epaper_conf_t;

typedef void* epaper_handle_t; /*handle of epaper*/

/**
 * @brief Create and init epaper and return a epaper handle
 *
 * @param bus handle of spi device
 * @param epconf configure struct for epaper device
 *
 * @return
 *     - handle of epaper
 */
epaper_handle_t iot_epaper_create(spi_device_handle_t bus, epaper_conf_t * epconf);

/**
 * @brief   delete epaper handle_t
 *
 * @param dev object handle of epaper
 * @param del_bus whether to delete spi bus
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t iot_epaper_delete(epaper_handle_t dev, bool del_bus);

/**
 * @brief clear display frame buffer
 *
 * @param dev object handle of epaper
 * @param colored to set display color
 */
void iot_epaper_clean_paint(epaper_handle_t dev, int colored);

/**
 * @brief get paint width
 *
 * @param dev object handle of epaper
 *
 * @return
 *     - paint width
 */
int iot_epaper_get_width(epaper_handle_t dev);

/**
 * @brief   set paint width
 * @param dev object handle of epaper
 * @param width paint width
 */
void iot_epaper_set_width(epaper_handle_t dev, int width);

/**
 * @brief   get paint height
 * @param   dev object handle of epaper
 * @return
 *     - paint height
 */
int iot_epaper_get_height(epaper_handle_t dev);

/**
 * @brief   set paint height
 * @param   dev object handle of epaper
 * @param   paint height
 */
void iot_epaper_set_height(epaper_handle_t dev, int height);

/**
 * @brief   get paint rotate
 * @param   dev object handle of epaper
 * @return
 *     - current rotation
 */
int iot_epaper_get_rotate(epaper_handle_t dev);

/**
 * @brief set paint rotate
 * @param dev object handle of epaper
 * @param rotation
 */
void iot_epaper_set_rotate(epaper_handle_t dev, int rotate);

/**
 * @brief get display data
 * @param dev object handle of epaper
 * @return
 *     - Pointer to display data
 */
unsigned char* iot_epaper_get_image(epaper_handle_t dev);

/**
 * @brief   draw string start on point(x,y) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 * @param dev object handle of epaper
 * @param x  poing (x)
 * @param y  poing (y)
 * @param text  display string
 * @param font Font style
 * @param colored display color
 */
void iot_epaper_draw_string(epaper_handle_t dev, int x, int y, const char* text,
        epaper_font_t* font, int colored);

/**
 * @brief   draw char and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 * @param   dev object handle of epaper
 * @param  x poing (x)
 * @param  y poing (y)
 * @param  ascii_char display char
 * @param  font font style
 * @param  colored display color
 */
void iot_epaper_draw_char(epaper_handle_t dev, int x, int y, char ascii_char,
        epaper_font_t* font, int colored);

/**
 * @brief   draw line start on point(x0,y0) end on point(x1,y1) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 *
 * @param   dev object handle of epaper
 * @param  x0 poing (x0)
 * @param  y0 poing (y0)
 * @param  x1 poing (x1)
 * @param  y1 poing (y1)
 * @param  colored display color
 */
void iot_epaper_draw_line(epaper_handle_t dev, int x0, int y0, int x1, int y1,
        int colored);

/**
 * @brief   draw horizontal line start on point(x,y) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 * @param   dev object handle of epaper
 * @param  x poing (x)
 * @param  y poing (y)
 * @param  width line width
 * @param  colored display color
 */
void iot_epaper_draw_horizontal_line(epaper_handle_t dev, int x, int y,
        int width, int colored);

/**
 * @brief   draw vertical line start on point(x,y) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 *
 * @param   dev object handle of epaper
 * @param  x poing (x)
 * @param  y poing (y)
 * @param   line height
 * @param   display color
 */
void iot_epaper_draw_vertical_line(epaper_handle_t dev, int x, int y,
        int height, int colored);

/**
 * @brief   draw rectangle point(x0,y0) (x1,y1) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 *
 * @param  dev object handle of epaper
 * @param  x0 point(x0,y0)
 * @param  y0 point(x0,y0)
 * @param  x1 point(x1,y1)
 * @param  y1 point(x1,y1)
 * @param  colored display color
 */
void iot_epaper_draw_rectangle(epaper_handle_t dev, int x0, int y0, int x1,
        int y1, int colored);

/**
 * @brief   draw fill rectangle point(x0,y0) (x1,y1) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 *
 * @param  dev object handle of epaper
 * @param  x0 point(x0,y0)
 * @param  y0 point(x0,y0)
 * @param  x1 point(x1,y1)
 * @param  y1 point(x1,y1)
 * @param  colored display color
 */
void iot_epaper_draw_filled_rectangle(epaper_handle_t dev, int x0, int y0,
        int x1, int y1, int colored);

/**
 * @brief   draw a circle at point(x,y) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 *
 * @param  dev object handle of epaper
 * @param  x point(x,y)
 * @param  y point(x,y)
 * @param  colored display color
 */
void iot_epaper_draw_circle(epaper_handle_t dev, int x, int y, int radius,
        int colored);

/**
 * @brief   draw a fill circle at point(x,y) and save on display data array,
 *          screen will display when call iot_epaper_display_frame function.
 *
 * @param  dev object handle of epaper
 * @param  x point(x,y)
 * @param  y point(x,y)
 * @param  radius radius of the circle
 * @param  colored display color
 */
void iot_epaper_draw_filled_circle(epaper_handle_t dev, int x, int y,
        int radius, int colored);

/**
 * @brief  wait until idle
 * @param  dev object handle of epaper
 */
void iot_epaper_wait_idle(epaper_handle_t dev);

/**
 * @brief  reset device
 *
 * @param  dev object handle of epaper
 */
void iot_epaper_reset(epaper_handle_t dev);

/**
 * @brief  init device hardware
 *
 * @param  dev object handle of epaper
 */
void iot_epaper_epd_init(epaper_handle_t dev, uint8_t Data);

/**
 * @brief  setRamDataEntryMode
 *
 * @param  dev object handle of epaper 
 * @param  em  command 
 */
void iot_setRamDataEntryMode(epaper_handle_t dev, uint8_t em);

/**
 * @brief  SetRamArea
 *
 * @param  dev object handle of epaper 
 */
void iot_SetRamArea(epaper_handle_t dev, uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1);

/**
 * @brief  SetRamPointer
 *
 * @param  dev object handle of epaper 
 */
void iot_SetRamPointer(epaper_handle_t dev, uint8_t addrX, uint8_t addrY, uint8_t addrY1);

/**
 * @brief  Draw a picture with a bit map
 *
 * @param  dev object handle of epaper 
 * @param  bitmap a bitmap object
 * @param  x point(x,y)
 * @param  y point(x,y)
 * @param  w bitmap_width
 * @param  h bitmap_hight
 * @param  color fill_in_color
 * @param  mode  select roation mode
 */
void iot_drawBitmapBM(epaper_handle_t dev, const uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int16_t mode);

/**
 * @brief  Init_Part
 *
 * @param  dev object handle of epaper 
 * @param  em  commad
 */
void iot_Init_Part(epaper_handle_t dev,uint8_t em);

/**
 * @brief  InitDisplay
 *
 * @param  dev object handle of epaper 
 * @param  em  mode select
 */
void iot_InitDisplay(epaper_handle_t dev, uint8_t em);

/**
 * @brief  Update part of the display
 *
 * @param  dev object handle of epaper 
 */
void iot_Update_Part(epaper_handle_t dev);

/**
 * @brief  Init for full update the display
 *
 * @param  dev object handle of epaper 
 * @param  em  mode select
 */
void iot_Init_Full(epaper_handle_t dev,uint8_t em);

/**
 * @brief  Update full of the display
 *
 * @param  dev object handle of epaper 
 */
void iot_Update_Full(epaper_handle_t dev);

/**
 * @brief  Update part of the display
 *
 * @param  dev object handle of epaper 
 * @param  x point(x,y)
 * @param  y point(x,y)
 * @param  w bitmap_width
 * @param  h bitmap_hight
 * @param  using_rotation choose to use rotation
 */
void iot_epaper_updateWindow(epaper_handle_t dev,uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation);

/**
 * @brief  Draw a pixel of the display
 *
 * @param  dev object handle of epaper 
 * @param  x point(x,y)
 * @param  y point(x,y)
 * @param  color fill_in_color
 */
void iot_drawPixel(epaper_handle_t dev, int16_t x, int16_t y, uint16_t color);

/**
 * @brief  Turn on the display's power
 *
 * @param  dev object handle of epaper 
 */
void iot_PowerOn(epaper_handle_t dev);

/**
 * @brief  Turn off the display's power
 *
 * @param  dev object handle of epaper 
 */
void iot_PowerOff(epaper_handle_t dev);

/**
 * @brief  Update full of the display
 *
 * @param  dev object handle of epaper 
 */
void iot_epaper_update(epaper_handle_t dev);

/**
 * @brief  Exchange data in preparation for the screen rotation.
 *
 * @param  x point(x,y)
 * @param  y point(x,y)
 * @param  w bitmap_width
 * @param  h bitmap_hight
 */
void iot_rotate(uint16_t* x, uint16_t* y, uint16_t* w, uint16_t* h);

/**
 * @brief  EraseDisplay(Not tested yet.)
 *
 * @param  dev object handle of epaper 
 * @param  using_partial_update choose to use partial_update
 */
void iot_eraseDisplay(epaper_handle_t dev, bool using_partial_update);

/**
 * @brief  Find a smaller data
 *
 * @param  a 
 * @param  b 
 */
static inline uint16_t gx_uint16_min(uint16_t a, uint16_t b) {return (a < b ? a : b);};

#ifdef __cplusplus
}
#endif

#endif

