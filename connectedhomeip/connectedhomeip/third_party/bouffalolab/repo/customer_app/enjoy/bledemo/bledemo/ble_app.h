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
#ifndef __BLE_APP_H_
#define __BLE_APP_H_


#define NAME_LEN        30
#define CHAR_SIZE_MAX       512
#define BT_UUID_TEST        BT_UUID_DECLARE_16(0xFFF0)
#define BT_UUID_TEST_RX     BT_UUID_DECLARE_16(0xFFF1)
#define BT_UUID_TEST_TX     BT_UUID_DECLARE_16(0xFFF2)


#define EV_BLE_TEST     0x0504 
#define BLE_ADV_START   0x01
#define BLE_ADV_STOP    0x02
#define BLE_DEV_CONN    0x03
#define BLE_DEV_DISCONN 0x04
#define BLE_SCAN_START  0x05
#define BLE_SCAN_STOP   0x06

#define LED_RED_PIN         (17)
#define LED_GREEN_PIN       (14)
#define LED_BLUE_PIN        (11)

enum app_ble_role
{
    PERIPHERAL=1,
    CENTRAL
};

void apps_ble_start (void);
void bleapps_adv_starting(void);
void bleapps_adv_stop(void);
void ble_bl_notify_task(void);
void ble_bl_disconnect(void);



void ble_appc_start(void);
void start_scan(void);
void ble_appc_stopscan(void);
void bl_gatt_write_without_rsp(void);
void ble_appc_disconnected(void);

#endif
