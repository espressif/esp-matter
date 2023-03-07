/*
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fsl_common.h"
#include "fsl_usart.h"

#include "Driver_USART.h"
#include "board.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

typedef struct {
    ARM_USART_SignalEvent_t cb_event;  /* Callback function for events */
} UARTx_Resources;

/* Driver version */
#define ARM_USART_DRV_VERSION  ARM_DRIVER_VERSION_MAJOR_MINOR(2, 2)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USARTx_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USARTx_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USARTx_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USARTx_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USARTx_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USARTx_EVENT_RI */
    0  /* Reserved */
};

static ARM_DRIVER_VERSION ARM_USART_GetVersion(void)
{
    return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
{
    return DriverCapabilities;
}

int32_t ARM_USARTx_Initialize(USART_Type *usart, usart_config_t *usartConfig)
{
    uint32_t usartClkFreq = 0;

    usartClkFreq = CLOCK_GetFlexCommClkFreq(0);

    USART_Init(usart, usartConfig, usartClkFreq);

    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_PowerControl(USART_Type* usart,
                                       ARM_POWER_STATE state)
{
    ARG_UNUSED(usart);

    switch (state) {
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
    default:
        return ARM_DRIVER_ERROR_PARAMETER;
    }
}

int32_t ARM_USARTx_Deinitialize(USART_Type *base)
{
    USART_Deinit(base);

    return ARM_DRIVER_OK;
}

int32_t ARM_USARTx_Send(USART_Type *base, const uint8_t *data, size_t length)
{
    USART_WriteBlocking(base, data, length);

    return ARM_DRIVER_OK;
}

int32_t ARM_USARTx_Receive(USART_Type *base, uint8_t *data, size_t length)
{
    status_t status;

    status = USART_ReadBlocking(base, data, length);

    return ARM_DRIVER_OK;
}

static int32_t ARM_USART0_Initialize(ARM_USART_SignalEvent_t cb_event)
{
    usart_config_t uart0Config;

     /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUSART_ParityDisabled;
     * config.stopBitCount = kUSART_OneStopBit;
     * config.loopback = false;
     * config.enableTxFifo = false;
     * config.enableRxFifo = false;
     */
	  USART_GetDefaultConfig(&uart0Config);
	  uart0Config.baudRate_Bps = 115200U;
	  uart0Config.enableRx = true;
	  uart0Config.enableTx = true;

    return ARM_USARTx_Initialize(USART0, &uart0Config);
}

static int32_t ARM_USART0_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_USART0_PowerControl(ARM_POWER_STATE state)
{
    return ARM_USARTx_PowerControl(USART0, state);
}

static int32_t ARM_USART0_Send(const void *data, uint32_t num)
{
    return ARM_USARTx_Send(USART0, data, num);
}

static int32_t ARM_USART0_Receive(void *data, uint32_t num)
{
    return ARM_USARTx_Receive(USART0, data, num);
}

static int32_t ARM_USART0_Transfer(const void *data_out, void *data_in,
                                   uint32_t num)
{
    ARG_UNUSED(data_out);
    ARG_UNUSED(data_in);
    ARG_UNUSED(num);

    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static uint32_t ARM_USART0_GetTxCount(void)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static uint32_t ARM_USART0_GetRxCount(void)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static int32_t ARM_USART0_Control(uint32_t control, uint32_t arg)
{
    return ARM_DRIVER_OK; //ARM_USARTx_Control(&USART0_DEV, control, arg);
}

static ARM_USART_STATUS ARM_USART0_GetStatus(void)
{
    ARM_USART_STATUS status = {0, 0, 0, 0, 0, 0, 0, 0};
    return status;
}

static int32_t ARM_USART0_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    ARG_UNUSED(control);
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS ARM_USART0_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS modem_status = {0, 0, 0, 0, 0};
    return modem_status;
}

extern ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART Driver_USART0 = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART0_Initialize,
    ARM_USART0_Uninitialize,
    ARM_USART0_PowerControl,
    ARM_USART0_Send,
    ARM_USART0_Receive,
    ARM_USART0_Transfer,
    ARM_USART0_GetTxCount,
    ARM_USART0_GetRxCount,
    ARM_USART0_Control,
    ARM_USART0_GetStatus,
    ARM_USART0_SetModemControl,
    ARM_USART0_GetModemStatus
};
