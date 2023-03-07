/*
 * Copyright (c) 2013-2018 ARM Limited. All rights reserved.
 * Copyright (c) 2019-2020 Cypress Semiconductor Corporation. All rights reserved.
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

/* This is a modified copy of the ref_twincpu version at
 * platform/ext/target/ref_twincpu/CMSIS_Driver/Driver_USART.c
 */

#include "Driver_USART.h"
#include "cmsis.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

#include "cycfg.h"
#include "cy_device.h"
#include "cy_scb_uart.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

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

typedef struct {
    CySCB_Type* base;                  /* UART device structure */
    uint32_t tx_nbr_bytes;             /* Number of bytes transfered */
    uint32_t rx_nbr_bytes;             /* Number of bytes recevied */
    ARM_USART_SignalEvent_t cb_event;  /* Callback function for events */
} UARTx_Resources;

static int32_t USARTx_convert_retval(cy_en_scb_uart_status_t val)
{
    switch (val) {
    case CY_SCB_UART_SUCCESS:
        return ARM_DRIVER_OK;
    case CY_SCB_UART_BAD_PARAM:
        return ARM_DRIVER_ERROR_PARAMETER;
    case CY_SCB_UART_RECEIVE_BUSY:
    case CY_SCB_UART_TRANSMIT_BUSY:
        return ARM_DRIVER_ERROR_BUSY;
    }
}

static int32_t ARM_USARTx_Initialize(UARTx_Resources* uart_dev)
{
    cy_en_scb_uart_status_t retval;

#if CY_SYSTEM_CPU_CM0P
    cy_stc_scb_uart_config_t config = KITPROG_UART_config;

    /* Assign and configure pins, assign clock divider */
    retval = Cy_SCB_UART_Init(uart_dev->base, &config, NULL);

    Cy_SCB_UART_ClearRxFifo(uart_dev->base);
    Cy_SCB_UART_ClearTxFifo(uart_dev->base);

    if (retval == CY_SCB_UART_SUCCESS)
        Cy_SCB_UART_Enable(uart_dev->base);
#else
    // all hw initializations is done on the cm0p side
    retval = CY_SCB_UART_SUCCESS;

#endif
    return USARTx_convert_retval(retval);
}

static uint32_t ARM_USARTx_Uninitialize(UARTx_Resources* uart_dev)
{
    Cy_SCB_UART_Disable(uart_dev->base, NULL);

    Cy_SCB_UART_DeInit(uart_dev->base);

    return ARM_DRIVER_OK;
}


static int32_t ARM_USARTx_PowerControl(UARTx_Resources* uart_dev,
                                       ARM_POWER_STATE state)
{
    ARG_UNUSED(uart_dev);

    switch (state) {
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all the enumeration values are
     *           covered in the switch.*/
    }
}

static int32_t ARM_USARTx_Send(UARTx_Resources* uart_dev, const void *data,
                               uint32_t num)
{
    void *p_data = (void *)data;

    if ((data == NULL) || (num == 0U)) {
        /* Invalid parameters */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    Cy_SCB_UART_PutArrayBlocking(uart_dev->base, p_data, num);

    while (!Cy_SCB_UART_IsTxComplete(uart_dev->base))
        ;

    uart_dev->tx_nbr_bytes = num;

    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_Receive(UARTx_Resources* uart_dev,
                                  void *data, uint32_t num)
{
    if ((data == NULL) || (num == 0U)) {
        // Invalid parameters
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    Cy_SCB_UART_GetArrayBlocking(uart_dev->base, data, num);

    uart_dev->rx_nbr_bytes = num;

    return ARM_DRIVER_OK;
}

static int32_t ARM_USARTx_Transfer(UARTx_Resources* uart_dev,
                                   const void *data_out, void *data_in,
                                   uint32_t num)
{
    ARG_UNUSED(uart_dev);
    ARG_UNUSED(data_out);
    ARG_UNUSED(data_in);
    ARG_UNUSED(num);

    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static uint32_t ARM_USARTx_GetTxCount(UARTx_Resources* uart_dev)
{
    return uart_dev->tx_nbr_bytes;
}

static uint32_t ARM_USARTx_GetRxCount(UARTx_Resources* uart_dev)
{
    return uart_dev->rx_nbr_bytes;
}

static int32_t USARTx_SetDataBits(uint32_t control,
                                   cy_stc_scb_uart_config_t *config)
{
    switch (control & ARM_USART_DATA_BITS_Msk) {
    case ARM_USART_DATA_BITS_5:
        config->dataWidth = 5;
        break;

    case ARM_USART_DATA_BITS_6:
        config->dataWidth = 6;
        break;

    case ARM_USART_DATA_BITS_7:
        config->dataWidth = 7;
        break;

    case ARM_USART_DATA_BITS_8:
        config->dataWidth = 8;
        break;

    case ARM_USART_DATA_BITS_9:
        config->dataWidth = 9;
        break;

    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t USARTx_SetParity(uint32_t control,
                                 cy_stc_scb_uart_config_t *config)
{
    switch (control & ARM_USART_PARITY_Msk) {
    case ARM_USART_PARITY_NONE:
        config->parity = CY_SCB_UART_PARITY_NONE;
        break;

    case ARM_USART_PARITY_EVEN:
        config->parity = CY_SCB_UART_PARITY_EVEN;
        break;

    case ARM_USART_PARITY_ODD:
        config->parity = CY_SCB_UART_PARITY_ODD;
        break;

    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t USARTx_SetStopBits(uint32_t control,
                                   cy_stc_scb_uart_config_t *config)
{
    switch (control & ARM_USART_STOP_BITS_Msk) {
    case ARM_USART_STOP_BITS_1:
        config->stopBits = CY_SCB_UART_STOP_BITS_1;
        break;

    case ARM_USART_STOP_BITS_2:
        config->stopBits = CY_SCB_UART_STOP_BITS_2;
        break;

    case ARM_USART_STOP_BITS_1_5:
        config->stopBits = CY_SCB_UART_STOP_BITS_1_5;
        break;

    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static void USARTx_SetFlowControl(uint32_t control,
                                  cy_stc_scb_uart_config_t *config)
{
    /* First, deal with CTS */
    switch (control & ARM_USART_FLOW_CONTROL_Msk) {
    case ARM_USART_FLOW_CONTROL_NONE:
    case ARM_USART_FLOW_CONTROL_RTS:
        config->enableCts = false;
        config->ctsPolarity = CY_SCB_UART_ACTIVE_LOW;
        break;

    case ARM_USART_FLOW_CONTROL_CTS:
    case ARM_USART_FLOW_CONTROL_RTS_CTS:
        config->enableCts = true;
        config->ctsPolarity = CY_SCB_UART_ACTIVE_LOW;
        break;
    }

    /* Then RTS */
    switch (control & ARM_USART_FLOW_CONTROL_Msk) {
    case ARM_USART_FLOW_CONTROL_NONE:
    case ARM_USART_FLOW_CONTROL_CTS:
        config->rtsRxFifoLevel = 0;
        config->rtsPolarity = CY_SCB_UART_ACTIVE_LOW;
        break;

    case ARM_USART_FLOW_CONTROL_RTS:
    case ARM_USART_FLOW_CONTROL_RTS_CTS:
        config->rtsRxFifoLevel = 8; /* TODO What's a sensible value ? */
        config->rtsPolarity = CY_SCB_UART_ACTIVE_LOW;
        break;
    }
}

static int32_t ARM_USARTx_Control(UARTx_Resources* uart_dev, uint32_t control,
                                  uint32_t arg)
{
    cy_stc_scb_uart_config_t config = KITPROG_UART_config;
    uint32_t retval;

    switch (control & ARM_USART_CONTROL_Msk) {
        case ARM_USART_MODE_ASYNCHRONOUS:
            /* TODO Default values work for 115200 baud,
             * but we should set config.oversample to
             * a value derived from the divider
             */
            break;
        /* Unsupported command */
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    Cy_SCB_UART_Disable(uart_dev->base, NULL);

    /* UART Data bits */
    retval = USARTx_SetDataBits(control, &config);
    if (retval != ARM_DRIVER_OK)
        return retval;

    /* UART Parity */
    retval = USARTx_SetParity(control, &config);
    if (retval != ARM_DRIVER_OK)
        return retval;

    /* USART Stop bits */
    retval = USARTx_SetStopBits(control, &config);
    if (retval != ARM_DRIVER_OK)
        return retval;

    /* USART Flow Control */
    USARTx_SetFlowControl(control, &config);

    Cy_SCB_UART_ClearRxFifo(uart_dev->base);
    Cy_SCB_UART_ClearTxFifo(uart_dev->base);

    Cy_SCB_UART_Enable(uart_dev->base);

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS ARM_USARTx_GetStatus(UARTx_Resources* uart_dev)
{
    ARM_USART_STATUS status = {0, 0, 0, 0, 0, 0, 0, 0};
    return status;
}

static int32_t ARM_USARTx_SetModemControl(UARTx_Resources* uart_dev,
                                          ARM_USART_MODEM_CONTROL control)
{
    ARG_UNUSED(control);
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS ARM_USARTx_GetModemStatus(UARTx_Resources* uart_dev)
{
    ARM_USART_MODEM_STATUS modem_status = {0, 0, 0, 0, 0};
    return modem_status;
}

/* Per-UART macros */
#define DEFINE_UARTX(N) static UARTx_Resources USART##N##_DEV = { \
    .base = SCB##N, \
    .tx_nbr_bytes = 0, \
    .rx_nbr_bytes = 0, \
    .cb_event = NULL, \
}; \
\
static int32_t ARM_USART##N##_Initialize(ARM_USART_SignalEvent_t cb_event) \
{ \
    USART##N##_DEV.cb_event = cb_event; \
    return ARM_USARTx_Initialize(&USART##N##_DEV); \
} \
\
static int32_t ARM_USART##N##_Uninitialize(void) \
{ \
    return ARM_USARTx_Uninitialize(&USART##N##_DEV); \
} \
\
static int32_t ARM_USART##N##_PowerControl(ARM_POWER_STATE state) \
{ \
    return ARM_USARTx_PowerControl(&USART##N##_DEV, state); \
} \
 \
static int32_t ARM_USART##N##_Send(const void *data, uint32_t num) \
{ \
    return ARM_USARTx_Send(&USART##N##_DEV, data, num); \
} \
 \
static int32_t ARM_USART##N##_Receive(void *data, uint32_t num) \
{ \
    return ARM_USARTx_Receive(&USART##N##_DEV, data, num); \
} \
 \
static int32_t ARM_USART##N##_Transfer(const void *data_out, void *data_in, \
                                   uint32_t num) \
{ \
    return ARM_USARTx_Transfer(&USART##N##_DEV, data_out, data_in, num); \
} \
 \
static uint32_t ARM_USART##N##_GetTxCount(void) \
{ \
    return ARM_USARTx_GetTxCount(&USART##N##_DEV); \
} \
 \
static uint32_t ARM_USART##N##_GetRxCount(void) \
{ \
    return ARM_USARTx_GetRxCount(&USART##N##_DEV); \
} \
static int32_t ARM_USART##N##_Control(uint32_t control, uint32_t arg) \
{ \
    return ARM_USARTx_Control(&USART##N##_DEV, control, arg); \
} \
 \
static ARM_USART_STATUS ARM_USART##N##_GetStatus(void) \
{ \
    return ARM_USARTx_GetStatus(&USART##N##_DEV); \
} \
 \
static int32_t ARM_USART##N##_SetModemControl(ARM_USART_MODEM_CONTROL control) \
{ \
    return ARM_USARTx_SetModemControl(&USART##N##_DEV, control); \
} \
 \
static ARM_USART_MODEM_STATUS ARM_USART##N##_GetModemStatus(void) \
{ \
    return ARM_USARTx_GetModemStatus(&USART##N##_DEV); \
} \
 \
extern ARM_DRIVER_USART Driver_USART##N; \
ARM_DRIVER_USART Driver_USART##N = { \
    ARM_USART_GetVersion, \
    ARM_USART_GetCapabilities, \
    ARM_USART##N##_Initialize, \
    ARM_USART##N##_Uninitialize, \
    ARM_USART##N##_PowerControl, \
    ARM_USART##N##_Send, \
    ARM_USART##N##_Receive, \
    ARM_USART##N##_Transfer, \
    ARM_USART##N##_GetTxCount, \
    ARM_USART##N##_GetRxCount, \
    ARM_USART##N##_Control, \
    ARM_USART##N##_GetStatus, \
    ARM_USART##N##_SetModemControl, \
    ARM_USART##N##_GetModemStatus \
};

#if (RTE_USART0)
DEFINE_UARTX(0)
#endif

#if (RTE_USART1)
DEFINE_UARTX(1)
#endif

#if (RTE_USART2)
DEFINE_UARTX(2)
#endif

#if (RTE_USART3)
DEFINE_UARTX(3)
#endif

#if (RTE_USART4)
DEFINE_UARTX(4)
#endif

#if (RTE_USART5)
DEFINE_UARTX(5)
#endif

#if (RTE_USART6)
DEFINE_UARTX(6)
#endif

#if (RTE_USART7)
DEFINE_UARTX(7)
#endif
