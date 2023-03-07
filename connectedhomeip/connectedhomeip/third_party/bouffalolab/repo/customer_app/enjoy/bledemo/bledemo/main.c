/*
 * Copyright (c) 2020 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

#include <bl602_glb.h>
#include <bl602_hbn.h>
#include "bl602_adc.h"

#include <bl_sys.h>
#include <bl_uart.h>
#include <bl_chip.h>
#include <bl_sec.h>
#include <bl_cks.h>
#include <bl_irq.h>
#include <bl_timer.h>
#include <bl_dma.h>
#include <bl_gpio.h>
#include <bl_gpio_cli.h>
#include <bl_wdt_cli.h>
#include <hal_sys.h>
#include <hal_gpio.h>
#include <hal_hbn.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <hal_button.h>
#include <looprt.h>
#include <loopset.h>
#include <bl_sys_time.h>
#include <bl_romfs.h>
#include <fdt.h>
#include <vfs.h>
#include <hal/soc/pwm.h>
#include <device/vfs_pwm.h>
#include <utils_log.h>
#include <libfdt.h>
#include <blog.h>
#include "ble_lib_api.h"
#include "ble_app.h"
#include "hal_pds.h"
#include "bl_rtc.h"
#include "utils_string.h"

static enum app_ble_role app_role;
static void event_cb_key_event(input_event_t *event, void *private_data)
{
    switch (event->code) {
        case KEY_1:
        {
            printf("[KEY_1] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            printf("short press \r\n");
            apps_ble_start();
            app_role=PERIPHERAL;
        }
        break;
        case KEY_2:
        {
            printf("[KEY_2] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            printf("long press \r\n");
            ble_appc_start();
            app_role=CENTRAL;
        }
        break;
        case KEY_3:
        {
            printf("[KEY_3] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            printf("longlong press \r\n");
            if(app_role==PERIPHERAL){
   
                ble_bl_notify_task();
            }else{

                bl_gatt_write_without_rsp();
            }
       
        }
        break;
        default:
        {
            printf("[KEY] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

static void borad_rgb_init(void)
{
    
    bl_gpio_enable_output(LED_RED_PIN,1,0);
    bl_gpio_enable_output(LED_GREEN_PIN,1,0);
    bl_gpio_enable_output(LED_BLUE_PIN,1,0);
    bl_gpio_output_set(LED_RED_PIN,1);
    bl_gpio_output_set(LED_GREEN_PIN,1);
    bl_gpio_output_set(LED_BLUE_PIN,1);

}
static void borad_rgb_off(void)
{

    bl_gpio_output_set(LED_RED_PIN,1);
    bl_gpio_output_set(LED_GREEN_PIN,1);
    bl_gpio_output_set(LED_BLUE_PIN,1);

}
static void event_cb_ble_event(input_event_t *event, void *private_data)
{
    switch (event->code) {
        case BLE_ADV_START:
        {
            borad_rgb_off();
            printf("ble adv start \r\n");
            bl_gpio_output_set(LED_BLUE_PIN,0);
         

        }
        break;
        case BLE_ADV_STOP:
        {
            printf("ble adv stop \r\n");
            borad_rgb_off();
      
        }
        break;
        case BLE_DEV_CONN:
        {
            borad_rgb_off();
            bl_gpio_output_set(LED_GREEN_PIN,0);

            printf("ble dev connnected \r\n");
           

        }
        break;
        case BLE_DEV_DISCONN:
        {
            borad_rgb_off();
            bl_gpio_output_set(LED_RED_PIN,0);

            printf("ble dev disconnnected\r\n");
            vTaskDelay(5000);
            if(app_role==PERIPHERAL){

                bleapps_adv_starting();
            }else{

                start_scan();
            }
        }
        break;
        case BLE_SCAN_START:
        {
            borad_rgb_off();
            bl_gpio_output_set(LED_BLUE_PIN,0);
            printf("ble dev scanning\r\n");
        }
        break;
        case BLE_SCAN_STOP:
        {
            borad_rgb_off();
            printf("ble dev stop scan\r\n");
        }
        break;
        default:
        {
            printf("[KEY] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

static void proc_main_entry(void *pvParameters)
{
    aos_register_event_filter(EV_KEY, event_cb_key_event, NULL);
    aos_register_event_filter(EV_BLE_TEST, event_cb_ble_event, NULL);    
    //tsen_adc_init();

    #if defined(CONFIG_BT_TL)
    //uart's pinmux has been configured in vfs_uart_init(load uart1's pin info from devicetree)
    ble_uart_init(1);
    ble_controller_init(configMAX_PRIORITIES - 1);
    #endif

    vTaskDelete(NULL);
}

void main()
{
    bl_sys_init();

    borad_rgb_init();

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);

}
