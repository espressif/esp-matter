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

#include <stdio.h>

#include "DAP_config.h"
#include "gpio.h"
#include "daplink.h"
#include "util.h"
#include "cortex_m.h"

#include "sys_init.h"
#include "hal_platform.h"
#include "hal_uart.h"
#include "hal_uart_internal.h"

void sdk_init(void)
{
    system_init();
    printf("System initialized!\r\n");
}

hal_uart_status_t log_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_status_t ret;

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_921600;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    ret = hal_uart_init(port, &uart_config);

    return ret;
}
