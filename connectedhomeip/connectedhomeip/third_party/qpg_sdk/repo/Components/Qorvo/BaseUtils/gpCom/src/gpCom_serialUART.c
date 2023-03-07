/*
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 * gpCom.c
 *
 * This file contains the implementation of the serial communication module.
 * It implements the GreenPeak serial protocol.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_COM

#include "hal.h"
#include "gpUtils.h"
#include "gpCom.h"
#include "gpCom_defs.h"

#include "gpLog.h"
#include "gpAssert.h"

#ifdef GP_DIVERSITY_FREERTOS
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "timers.h"
#endif
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_FREERTOS
#define GP_COM_UART1_TASK_NOTIFY_RX_MASK 0x01
#define GP_COM_UART2_TASK_NOTIFY_RX_MASK 0x02

#define GP_COM_UART_TASK_NOTIFY_ALL_MASK       (GP_COM_UART1_TASK_NOTIFY_RX_MASK | GP_COM_UART2_TASK_NOTIFY_RX_MASK)
#ifndef HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK
#error "Diversity flag HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK is required"
#endif
#endif //GP_DIVERSITY_FREERTOS
#if defined(HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK)
static void Com_cbUartRx(UInt8*buffer, UInt16 size);
#else
static void Com_cbUartRx(Int16 rxbyte); //Rx only function
#endif



/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if defined(GP_DIVERSITY_FREERTOS) 
#define UART_TASK_NAME               ("UART task")
#define UART_TASK_PRIORITY           (configMAX_PRIORITIES - 2)
#define UART_STACK_SIZE              300
#if !defined(HAL_UART_RX_BUFFER_SIZE)
#define UART_RX_STORAGE_SIZE_BYTES   64U
#else
#define UART_RX_STORAGE_SIZE_BYTES   HAL_UART_RX_BUFFER_SIZE
#endif //if !defined(HAL_UART_RX_BUFFER_SIZE)

/* Defines the memory that will actually hold the streams within the stream
buffer. */
static uint8_t ucUartRxBuffStorage[ UART_RX_STORAGE_SIZE_BYTES ];

/* The variable used to hold the stream RX buffer structure. */
StaticStreamBuffer_t xStreamUartRxBuffStruct;
StreamBufferHandle_t xStreamUartRxBuff;

#if (GP_COM_NUM_UART == 2)
/* Defines the memory that will actually hold the streams within the stream
buffer. */
static uint8_t ucUart2RxBuffStorage[ UART_RX_STORAGE_SIZE_BYTES ];

/* The variable used to hold the stream RX buffer structure. */
StaticStreamBuffer_t xStreamUart2RxBuffStruct;
StreamBufferHandle_t xStreamUart2RxBuff;
#endif

/* Buffer that the task being created will use as its stack.  Note this is
an array of StackType_t variables.  The size of StackType_t is dependent on
the RTOS port. */
StackType_t xUartStack[ UART_STACK_SIZE ];

/* Structure that will hold the TCB of the task being created. */
StaticTask_t xUartTaskBuffer;

TaskHandle_t xUartTaskh = NULL;
#endif // GP_DIVERSITY_FREERTOS && !GP_COM_DIVERSITY_NO_RX

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
#if defined(GP_DIVERSITY_FREERTOS) 
static void vUartTask(void* pvParameters);
static void Com_cbUartRxDefer(UInt8* buffer, UInt16 size);
#endif
/*****************************************************************************
 *                    Static Function
 *****************************************************************************/
static Int16 Com_cbUartGetTxData(gpCom_CommunicationId_t commId)
{
    Int16 returnValue = -1;

    HAL_DISABLE_GLOBAL_INT();

    if (Com_IsDataWaiting(commId))
    {
        returnValue = Com_GetData(commId);
    }

    HAL_ENABLE_GLOBAL_INT();

    return returnValue;
}

static Int16 Com_cbUart1GetTxData(void)
{
    return Com_cbUartGetTxData(GP_COM_COMM_ID_UART1);
}
#if GP_COM_NUM_UART == 2 
static Int16 Com_cbUart2GetTxData(void)
{
    return Com_cbUartGetTxData(GP_COM_COMM_ID_UART2);
}
#endif
//RX only function
#if defined(HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK)
static void Com_cbUartRx(UInt8 *buffer, UInt16 size)
{
    gpCom_ProtocolState_t* const state = &gpComUart_RxState[0];
    state->commId = GP_COM_COMM_ID_UART1;
#ifdef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    ComNoSynNoCrcProtocol_ParseBuffer(buffer, size, state);
#else
    ComSynProtocol_ParseBuffer(buffer, size, state);
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
}

#if (GP_COM_NUM_UART == 2)
static void Com_cbUart2Rx(UInt8 *buffer, UInt16 size)
{
    gpCom_ProtocolState_t* const state = &gpComUart_RxState[1];
    state->commId = GP_COM_COMM_ID_UART2;
#ifdef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
    ComNoSynNoCrcProtocol_ParseBuffer(buffer, size, state);
#else
    ComSynProtocol_ParseBuffer(buffer, size, state);
#endif // GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC

}
#endif
#else
static void Com_cbUartRx(Int16 rxbyte)
{
    Com_ParseProtocol(rxbyte, GP_COM_COMM_ID_UART1);
}

#if (GP_COM_NUM_UART == 2)
static void Com_cbUart2Rx(Int16 rxbyte)
{
    Com_ParseProtocol(rxbyte, GP_COM_COMM_ID_UART2);
}
#endif
#endif

#ifdef GP_DIVERSITY_FREERTOS
void vUartTask( void * pvParameters )
{
    (void) pvParameters;

    for(;;)
    {
        uint32_t notificationVal;

        if (xTaskNotifyWait(0, GP_COM_UART_TASK_NOTIFY_ALL_MASK, &notificationVal, portMAX_DELAY) == pdTRUE)
        {
            if ((notificationVal & GP_COM_UART1_TASK_NOTIFY_RX_MASK) != 0UL)
            {
                uint8_t ucRxData[ UART_RX_STORAGE_SIZE_BYTES ];
                size_t xReceivedBytes;

                /* Receive up to another sizeof( ucRxData ) bytes from the stream buffer.
                Not wait in the Blocked state (so not using any CPU processing time)
                for the full sizeof( ucRxData ) number of bytes to be available. */
                xReceivedBytes = xStreamBufferReceive( xStreamUartRxBuff,
                                                       ( void * ) ucRxData,
                                                       sizeof(ucRxData),
                                                       ( TickType_t ) 0  );
                if( xReceivedBytes > 0 )
                {
                    GP_LOG_PRINTF("Uart receive: %d bytes", 0, xReceivedBytes);
                    Com_cbUartRx(ucRxData, xReceivedBytes);
                }
            }
#if (GP_COM_NUM_UART == 2)
            if ((notificationVal & GP_COM_UART2_TASK_NOTIFY_RX_MASK) != 0UL)
            {
                uint8_t ucRxData[ UART_RX_STORAGE_SIZE_BYTES ];
                size_t xReceivedBytes;

                /* Receive up to another sizeof( ucRxData ) bytes from the stream buffer.
                Not wait in the Blocked state (so not using any CPU processing time)
                for the full sizeof( ucRxData ) number of bytes to be available. */
                xReceivedBytes = xStreamBufferReceive( xStreamUart2RxBuff,
                                                       ( void * ) ucRxData,
                                                       sizeof(ucRxData),
                                                       ( TickType_t ) 0  );
                if( xReceivedBytes > 0 )
                {
                    GP_LOG_PRINTF("Uart2 receive: %d bytes", 0, xReceivedBytes);
                    Com_cbUart2Rx(ucRxData, xReceivedBytes);
                }
            }

#endif //GP_COM_NUM_UART == 2
        }
    }
}

static void Com_cbUartRxDefer(UInt8 *buffer, UInt16 size)
{
    size_t xBytesSent;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */

    /* Attempt to send the string to the stream buffer. */
    xBytesSent = xStreamBufferSendFromISR( xStreamUartRxBuff,
                                           ( void * ) buffer,
                                           size,
                                           &xHigherPriorityTaskWoken );

    if( xBytesSent != size )
    {
        // uart rx buffer too small
        GP_ASSERT_SYSTEM(false);
    }
    else
    {
        if (xTaskNotifyFromISR(xUartTaskh, GP_COM_UART1_TASK_NOTIFY_RX_MASK, eSetBits, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            GP_LOG_PRINTF("Uart RX receive: %d bytes", 0, size);
        }
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

#if (GP_COM_NUM_UART == 2)
static void Com_cbUart2RxDefer(UInt8 *buffer, UInt16 size)
{
    size_t xBytesSent;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; /* Initialised to pdFALSE. */

    /* Attempt to send the string to the stream buffer. */
    xBytesSent = xStreamBufferSendFromISR( xStreamUart2RxBuff,
                                           ( void * ) buffer,
                                           size,
                                           &xHigherPriorityTaskWoken );

    if( xBytesSent != size )
    {
        // uart rx buffer too small
        GP_ASSERT_SYSTEM(false);
    }
    else
    {
        if (xTaskNotifyFromISR(xUartTaskh, GP_COM_UART2_TASK_NOTIFY_RX_MASK, eSetBits, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            GP_LOG_PRINTF("Uart2 RX receive: %d bytes", 0, size);
        }
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

#endif //GP_COM_NUM_UART == 2
#endif //GP_DIVERSITY_FREERTOS

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/****************************************************************************
 ****************************************************************************
 **                       Initialization                                   **
 ****************************************************************************
 ****************************************************************************/

void gpComUart_Init(void)
{
    // Initialize the UART (serial port)
#if defined(GP_DIVERSITY_FREERTOS) 
    const size_t xTriggerLevel = 1;

#if configSUPPORT_STATIC_ALLOCATION
    xStreamUartRxBuff = xStreamBufferCreateStatic( sizeof( ucUartRxBuffStorage ),
                                                   xTriggerLevel,
                                                   ucUartRxBuffStorage,
                                                   &xStreamUartRxBuffStruct );
#else
    xStreamUartRxBuff = xStreamBufferCreate( sizeof( ucUartRxBuffStorage ),
                                             xTriggerLevel );
#endif
    GP_ASSERT_SYSTEM( xStreamUartRxBuff );

#if configSUPPORT_STATIC_ALLOCATION
    xUartTaskh = xTaskCreateStatic( vUartTask,              /* Function that implements the task. */
                                    UART_TASK_NAME,         /* Text name for the task. */
                                    UART_STACK_SIZE,        /* Number of indexes in the xStack array. */
                                    NULL,                   /* Parameter passed into the task. */
                                    UART_TASK_PRIORITY,     /* Priority at which the task is created. */
                                    xUartStack,             /* Array to use as the task's stack. */
                                    &xUartTaskBuffer );     /* Variable to hold the task's data structure. */
    GP_ASSERT_SYSTEM( xUartTaskh );
#else
    (void)xTaskCreate(vUartTask,
                      UART_TASK_NAME,
                      UART_STACK_SIZE,
                      NULL,
                      UART_TASK_PRIORITY,
                      &xUartTaskh);
#endif
    HAL_UART_COM_START(Com_cbUartRxDefer, Com_cbUart1GetTxData);
#else
    HAL_UART_COM_START(Com_cbUartRx, Com_cbUart1GetTxData);
#endif //GP_DIVERSITY_FREERTOS
#if GP_COM_NUM_UART == 2 
#ifdef GP_DIVERSITY_FREERTOS
#if configSUPPORT_STATIC_ALLOCATION
    xStreamUart2RxBuff = xStreamBufferCreateStatic( sizeof( ucUart2RxBuffStorage ),
                                                    xTriggerLevel,
                                                    ucUart2RxBuffStorage,
                                                    &xStreamUart2RxBuffStruct );
#else
    xStreamUart2RxBuff = xStreamBufferCreate( sizeof( ucUart2RxBuffStorage ),
                                              xTriggerLevel);
#endif
    HAL_UART_COM2_START( Com_cbUart2RxDefer, Com_cbUart2GetTxData);
#else
    HAL_UART_COM2_START( Com_cbUart2Rx, Com_cbUart2GetTxData);
#endif //GP_DIVERSITY_FREERTOS
#endif //(GP_COM_NUM_UART == 2) && !defined(GP_COM_DIVERSITY_NO_RX) && !defined(GP_COM_DIVERSITY_UART2_DIRECT_TEST_MODE)

}

void gpComUart_DeInit(void)
{
    // De-Initialize the UART
    HAL_UART_COM_STOP();
    HAL_UART_COM_POWERDOWN();

#if GP_COM_NUM_UART == 2 
    HAL_UART_COM2_STOP();
    HAL_UART_COM2_POWERDOWN();
#endif
}

void gpComUart_Flush(void)
{
#ifdef HAVE_HAL_UART_FLUSH
    hal_UartComFlush(GP_BSP_UART_COM1);
#ifdef GP_BSP_UART_COM2
    hal_UartComFlush(GP_BSP_UART_COM2);
#endif //GP_BSP_UART_COM2
#endif
}

void ComUart_FlushRx(void)
{
#ifdef HAVE_HAL_UART_RX_FLUSH
    hal_UartRxComFlush(GP_BSP_UART_COM1);
#ifdef GP_BSP_UART_COM2
    hal_UartRxComFlush(GP_BSP_UART_COM2);
#endif //GP_BSP_UART_COM2
#endif
}


void ComUart_TriggerTx(UInt8 uart)
{
#if GP_COM_NUM_UART == 2
    if(uart == 1)
    {
        HAL_UART_COM2_TX_NEW_DATA();
    }
    else
#endif
    if(uart == 0)
    {
        HAL_UART_COM_TX_NEW_DATA();
    }
}


