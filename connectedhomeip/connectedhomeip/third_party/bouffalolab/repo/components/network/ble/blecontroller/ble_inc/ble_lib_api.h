/*
 * Copyright (c) 2016-2022 Bouffalolab.
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
#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h> 

void ble_controller_init(uint8_t task_priority);
void ble_controller_deinit(void);
#if !defined(CFG_FREERTOS) && !defined(CFG_AOS) && !defined(CFG_NUTTX)
void blecontroller_main(void);
#endif
#if defined(CFG_BT_RESET)
void ble_controller_reset(void);
#endif
char * ble_controller_get_lib_ver(void);

// if 0, success.
// if -1, fail, 
int8_t ble_controller_set_scan_filter_table_size(uint8_t size);

// return sleep duration, in unit of 1/32768s
// if 0, means not allow sleep
// if -1, means allow sleep, but there is no end of sleep interrupt (ble core deep sleep is not enabled)
int32_t ble_controller_sleep(int32_t max_sleep_cycles);
void ble_controller_sleep_restore(void);
bool ble_controller_sleep_is_ongoing(void);
void ble_controller_set_tx_pwr(int ble_tx_power);
void ble_rf_set_tx_channel(uint16_t tx_channel);
void ble_controller_disable_adv_random_delay(bool disable);

#if defined(CONFIG_BLE_MFG)
enum
{
    BLE_TEST_TX                  = 0x00,
    BLE_TEST_RX,
    BLE_TEST_RXTX,
    BLE_TEST_END
};

///HCI LE Receiver Test Command parameters structure
struct le_rx_test_cmd
{
    ///RX frequency for Rx test
    uint8_t        rx_freq;
};

///HCI LE Transmitter Test Command parameters structure
struct le_tx_test_cmd
{
    ///TX frequency for Tx test
    uint8_t        tx_freq;
    ///TX test data length
    uint8_t        test_data_len;
    ///TX test payload type - see enum
    uint8_t        pk_payload_type;
};

struct le_enhanced_rx_test_cmd
{
    ///RX frequency for Rx test
    uint8_t        rx_freq;
    ///RX PHY for Rx test
    uint8_t        rx_phy;
    ///Modulation index: Assume transmitter will have a standard or stable modulation index
    uint8_t        modulation_index;
};

///HCI LE Enhanced Transmitter Test Command parameters structure
struct le_enhanced_tx_test_cmd
{
    ///TX frequency for Tx test
    uint8_t        tx_freq;
    ///TX test data length
    uint8_t        test_data_len;
    ///TX test payload type - see enum
    uint8_t        pk_payload_type;
    ///TX PHY for Rx test
    uint8_t        tx_phy;
};

int le_rx_test_cmd_handler(uint16_t src_id, void *param, bool from_hci);
int le_tx_test_cmd_handler(uint16_t src_id, void *param, bool from_hci);
int le_test_end_cmd_handler(bool from_hci);
uint8_t le_get_direct_test_type(void);
void le_test_mode_custom_aa(uint32_t access_code);

#if defined(CONFIG_BLE_MFG_HCI_CMD)
int reset_cmd_handler(void);
#endif
#endif

#endif
