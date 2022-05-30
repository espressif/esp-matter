/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <zigbee_bridge.h>

/*Zigbee Configuration*/
#define IEEE_CHANNEL_MASK (1l << 22) /**< Zigbee default channel */
#define ERASE_PERSISTENT_CONFIG ZB_TRUE /**< Full device erase for all network devices before running example. */
#define MAX_CHILDREN 10 /**< The maximum amount of connected devices */

#define MATCH_DESC_REQ_ROLE ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE
#define MATCH_BRIDGED_DEVICE_START_DELAY (2 * ZB_TIME_ONE_SECOND)
#define MATCH_BRIDGED_DEVICE_TIMEOUT (5 * ZB_TIME_ONE_SECOND)

#define ZB_ESP_DEFAULT_RADIO_CONFIG()                      \
    {                                                      \
        .radio_mode = RADIO_MODE_UART_RCP,                 \
        .radio_uart_config = {                             \
            .port = 1,                                     \
            .uart_config =                                 \
                {                                          \
                    .baud_rate = 115200,                   \
                    .data_bits = UART_DATA_8_BITS,         \
                    .parity = UART_PARITY_DISABLE,         \
                    .stop_bits = UART_STOP_BITS_1,         \
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, \
                    .rx_flow_ctrl_thresh = 0,              \
                    .source_clk = UART_SCLK_APB,           \
                },                                         \
            .rx_pin = 4,                                   \
            .tx_pin = 5,                                   \
        },                                                 \
    }

#define ZB_ESP_DEFAULT_HOST_CONFIG()                       \
    {                                                      \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE, \
    }

void launch_app_zboss(void);
#ifdef __cplusplus
}
#endif
