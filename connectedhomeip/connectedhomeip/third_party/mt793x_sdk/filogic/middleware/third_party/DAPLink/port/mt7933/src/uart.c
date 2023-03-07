/*
*
* Copyright (C) 2021 MediaTek Inc., this file is modified on 2/7/2022  * by MediaTek Inc. based on Apache License, Version 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
*
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "string.h"

#include "uart.h"
#include "gpio.h"
#include "util.h"
#include "circ_buf.h"
#include "IO_Config.h"

#include "common.h"
#include "hal_uart.h"
#include "hal_uart_internal.h"

static hal_uart_port_t uart_port[] = {HAL_UART_2, HAL_UART_1, HAL_UART_3};
static UART_Configuration configuration = {
    .Baudrate = 921600,
    .DataBits = UART_DATA_BITS_8,
    .Parity = UART_PARITY_NONE,
    .StopBits = UART_STOP_BITS_1,
    .FlowControl = UART_FLOW_CONTROL_NONE,
};

extern uint32_t SystemCoreClock;

static hal_uart_baudrate_t uart_convert_baudrate(uint32_t baud)
{
    hal_uart_baudrate_t r = HAL_UART_BAUDRATE_921600;
    switch(baud) {
        case 9600:
            r = HAL_UART_BAUDRATE_9600;
            break;
        case 19200:
            r = HAL_UART_BAUDRATE_19200;
            break;
        case 38400:
            r = HAL_UART_BAUDRATE_38400;
            break;
        case 57600:
            r = HAL_UART_BAUDRATE_57600;
            break;
        case 115200:
            r = HAL_UART_BAUDRATE_115200;
            break;
        case 921600:
            r = HAL_UART_BAUDRATE_921600;
            break;
        default:
            r = HAL_UART_BAUDRATE_921600;
    }
    return r;
}

int32_t uart_initialize(void)
{
    int32_t ret = 0;
    /* Configure UART PORT */
    hal_uart_config_t uart_config;
    uart_config.baudrate = uart_convert_baudrate(configuration.Baudrate);
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    for(uint32_t i = 0; i < sizeof(uart_port) / sizeof(hal_uart_port_t); ++i) {
        ret += (int32_t)hal_uart_init(uart_port[i], &uart_config);
    }

    return ret;
}

int32_t uart_uninitialize(void)
{
    int32_t ret = 0;
    for(uint32_t i = 0; i < sizeof(uart_port) / sizeof(hal_uart_port_t); ++i) {
        ret += (int32_t)hal_uart_deinit(uart_port[i]);
    }
    return ret;
}

int32_t uart_reset(void)
{
    return 0;
}

void uart_set_control_line_state(uint16_t ctrl_bmp)
{

}

int32_t uart_set_configuration(UART_Configuration *config)
{
    int32_t ret = 0;
    memcpy((void *)&configuration, (void *)config, sizeof(UART_Configuration));
    for(uint32_t i = 0; i < sizeof(uart_port) / sizeof(hal_uart_port_t); ++i) {
        /**
         * Only support baudrate changes in HAL UART API
         */
        ret += hal_uart_set_baudrate(uart_port[i], uart_convert_baudrate(configuration.Baudrate));
    }
    return ret?0:1; //1: successfull,0: fail
}

int32_t uart_get_configuration(UART_Configuration *config)
{
    memcpy((void *)config, (void *)&configuration, sizeof(UART_Configuration));
    return 0;
}

int32_t uart_write_free(uint32_t fd)
{
    return (int32_t)hal_uart_get_available_send_space(uart_port[fd]);
}

int32_t uart_write_data(uint32_t fd, uint8_t *data, uint16_t size)
{
    return hal_uart_send_dma(uart_port[fd], data, size);
}

int32_t uart_read_data(uint32_t fd, uint8_t *data, uint16_t size)
{
    return hal_uart_receive_dma(uart_port[fd], data, size);
}

