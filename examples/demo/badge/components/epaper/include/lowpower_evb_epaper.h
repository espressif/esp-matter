/* Lowpower EVB Example

   For other examples please check:
   https://github.com/espressif/esp-iot-solution/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#ifndef _IOT_LOWPOWER_EVB_EPAPER_H_
#define _IOT_LOWPOWER_EVB_EPAPER_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize epaper.
 */

void epaper_display_init();

void epaper_show_page_loop();

void epaper_show_page_init(void);

void epaper_matter_code(void);

void epaper_light_power(bool power);

void display_commission_qr_code();

void epaper_display_espressif_logo();

void display_vcard(char *vcard, uint32_t x_offset, uint32_t y_offset);

void epaper_display_badge(char *name, char *company_name, char *email, char *contact, char *event_name);

/**
 * @brief disable epaper.
 */
void epaper_disable();

#ifdef __cplusplus
}
#endif

#endif

