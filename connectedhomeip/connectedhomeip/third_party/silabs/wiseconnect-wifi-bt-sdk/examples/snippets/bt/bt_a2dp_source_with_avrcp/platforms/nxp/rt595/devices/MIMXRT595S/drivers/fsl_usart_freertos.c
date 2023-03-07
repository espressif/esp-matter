/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_usart_freertos.h"
#include <FreeRTOS.h>
#include <event_groups.h>
#include <semphr.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexcomm_usart_freertos"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void USART_RTOS_Callback(USART_Type *base, usart_handle_t *state, status_t status, void *param)
{
    usart_rtos_handle_t *handle = (usart_rtos_handle_t *)param;
    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;
    xResult                  = pdFAIL;

    if (status == kStatus_USART_RxIdle)
    {
        xResult = xEventGroupSetBitsFromISR(handle->rxEvent, RTOS_USART_COMPLETE, &xHigherPriorityTaskWoken);
    }
    else if (status == kStatus_USART_TxIdle)
    {
        xResult = xEventGroupSetBitsFromISR(handle->txEvent, RTOS_USART_COMPLETE, &xHigherPriorityTaskWoken);
    }
    else if (status == kStatus_USART_RxRingBufferOverrun)
    {
        xResult = xEventGroupSetBitsFromISR(handle->rxEvent, RTOS_USART_RING_BUFFER_OVERRUN, &xHigherPriorityTaskWoken);
    }

    if (xResult != pdFAIL)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : USART_RTOS_Init
 * Description   : Initializes the USART instance for application
 *
 *END**************************************************************************/
/*!
 * brief Initializes a USART instance for operation in RTOS.
 *
 * param handle The RTOS USART handle, the pointer to allocated space for RTOS context.
 * param t_handle The pointer to allocated space where to store transactional layer internal state.
 * param cfg The pointer to the parameters required to configure the USART after initialization.
 * return 0 succeed, others fail.
 */
int USART_RTOS_Init(usart_rtos_handle_t *handle, usart_handle_t *t_handle, const struct rtos_usart_config *cfg)
{
    usart_config_t defcfg;

    if (NULL == handle)
    {
        return kStatus_InvalidArgument;
    }
    if (NULL == t_handle)
    {
        return kStatus_InvalidArgument;
    }
    if (NULL == cfg)
    {
        return kStatus_InvalidArgument;
    }
    if (NULL == cfg->base)
    {
        return kStatus_InvalidArgument;
    }
    if (0 == cfg->srcclk)
    {
        return kStatus_InvalidArgument;
    }
    if (0 == cfg->baudrate)
    {
        return kStatus_InvalidArgument;
    }

    handle->base    = cfg->base;
    handle->t_state = t_handle;

    handle->txSemaphore = xSemaphoreCreateMutex();
    if (NULL == handle->txSemaphore)
    {
        return kStatus_Fail;
    }
    handle->rxSemaphore = xSemaphoreCreateMutex();
    if (NULL == handle->rxSemaphore)
    {
        vSemaphoreDelete(handle->txSemaphore);
        return kStatus_Fail;
    }
    handle->txEvent = xEventGroupCreate();
    if (NULL == handle->txEvent)
    {
        vSemaphoreDelete(handle->rxSemaphore);
        vSemaphoreDelete(handle->txSemaphore);
        return kStatus_Fail;
    }
    handle->rxEvent = xEventGroupCreate();
    if (NULL == handle->rxEvent)
    {
        vEventGroupDelete(handle->txEvent);
        vSemaphoreDelete(handle->rxSemaphore);
        vSemaphoreDelete(handle->txSemaphore);
        return kStatus_Fail;
    }
    USART_GetDefaultConfig(&defcfg);

    defcfg.baudRate_Bps = cfg->baudrate;
    defcfg.parityMode   = cfg->parity;
    defcfg.enableTx     = true;
    defcfg.enableRx     = true;

    USART_Init(handle->base, &defcfg, cfg->srcclk);
    USART_TransferCreateHandle(handle->base, handle->t_state, USART_RTOS_Callback, handle);
    USART_TransferStartRingBuffer(handle->base, handle->t_state, cfg->buffer, cfg->buffer_size);

    return 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : USART_RTOS_Deinit
 * Description   : Deinitializes the USART instance and frees resources
 *
 *END**************************************************************************/
/*!
 * brief Deinitializes a USART instance for operation.
 *
 * This function deinitializes the USART module, sets all register values to reset value,
 * and releases the resources.
 *
 * param handle The RTOS USART handle.
 */
int USART_RTOS_Deinit(usart_rtos_handle_t *handle)
{
    USART_Deinit(handle->base);

    vEventGroupDelete(handle->txEvent);
    vEventGroupDelete(handle->rxEvent);

    /* Give the semaphore. This is for functional safety */
    xSemaphoreGive(handle->txSemaphore);
    xSemaphoreGive(handle->rxSemaphore);

    vSemaphoreDelete(handle->txSemaphore);
    vSemaphoreDelete(handle->rxSemaphore);

    /* Invalidate the handle */
    handle->base    = NULL;
    handle->t_state = NULL;

    return 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : USART_RTOS_Send
 * Description   : Initializes the USART instance for application
 *
 *END**************************************************************************/
/*!
 * brief Sends data in the background.
 *
 * This function sends data. It is a synchronous API.
 * If the hardware buffer is full, the task is in the blocked state.
 *
 * param handle The RTOS USART handle.
 * param buffer The pointer to buffer to send.
 * param length The number of bytes to send.
 */
int USART_RTOS_Send(usart_rtos_handle_t *handle, const uint8_t *buffer, uint32_t length)
{
    EventBits_t ev;
    int retval = kStatus_Success;

    if (NULL == handle->base)
    {
        /* Invalid handle. */
        return kStatus_Fail;
    }
    if (0 == length)
    {
        return 0;
    }
    if (NULL == buffer)
    {
        return kStatus_InvalidArgument;
    }

    if (pdFALSE == xSemaphoreTake(handle->txSemaphore, 0))
    {
        /* We could not take the semaphore, exit with 0 data received */
        return kStatus_Fail;
    }

    handle->txTransfer.data     = (uint8_t *)buffer;
    handle->txTransfer.dataSize = (uint32_t)length;

    /* Non-blocking call */
    USART_TransferSendNonBlocking(handle->base, handle->t_state, &handle->txTransfer);

    ev = xEventGroupWaitBits(handle->txEvent, RTOS_USART_COMPLETE, pdTRUE, pdFALSE, portMAX_DELAY);
    if (!(ev & RTOS_USART_COMPLETE))
    {
        retval = kStatus_Fail;
    }

    if (pdFALSE == xSemaphoreGive(handle->txSemaphore))
    {
        /* We could not post the semaphore, exit with error */
        retval = kStatus_Fail;
    }

    return retval;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : USART_RTOS_Recv
 * Description   : Receives chars for the application
 *
 *END**************************************************************************/
/*!
 * brief Receives data.
 *
 * This function receives data from USART. It is a synchronous API. If data is immediately available,
 * it is returned immediately and the number of bytes received.
 *
 * param handle The RTOS USART handle.
 * param buffer The pointer to buffer where to write received data.
 * param length The number of bytes to receive.
 * param received The pointer to a variable of size_t where the number of received data is filled.
 */
int USART_RTOS_Receive(usart_rtos_handle_t *handle, uint8_t *buffer, uint32_t length, size_t *received)
{
    EventBits_t ev;
    size_t n              = 0;
    int retval            = kStatus_Fail;
    size_t local_received = 0;

    if (NULL == handle->base)
    {
        /* Invalid handle. */
        return kStatus_Fail;
    }
    if (0 == length)
    {
        if (received != NULL)
        {
            *received = n;
        }
        return 0;
    }
    if (NULL == buffer)
    {
        return kStatus_InvalidArgument;
    }

    /* New transfer can be performed only after current one is finished */
    if (pdFALSE == xSemaphoreTake(handle->rxSemaphore, portMAX_DELAY))
    {
        /* We could not take the semaphore, exit with 0 data received */
        return kStatus_Fail;
    }

    handle->rxTransfer.data     = buffer;
    handle->rxTransfer.dataSize = (uint32_t)length;

    /* Non-blocking call */
    USART_TransferReceiveNonBlocking(handle->base, handle->t_state, &handle->rxTransfer, &n);

    ev = xEventGroupWaitBits(handle->rxEvent, RTOS_USART_COMPLETE | RTOS_USART_RING_BUFFER_OVERRUN, pdTRUE, pdFALSE,
                             portMAX_DELAY);
    if (ev & RTOS_USART_RING_BUFFER_OVERRUN)
    {
        /* Stop data transfer to application buffer, ring buffer is still active */
        USART_TransferAbortReceive(handle->base, handle->t_state);
        /* Prevent false indication of successful transfer in next call of USART_RTOS_Receive.
           RTOS_USART_COMPLETE flag could be set meanwhile overrun is handled */
        xEventGroupClearBits(handle->rxEvent, RTOS_USART_COMPLETE);
        retval         = kStatus_USART_RxRingBufferOverrun;
        local_received = 0;
    }
    else if (ev & RTOS_USART_COMPLETE)
    {
        retval         = kStatus_Success;
        local_received = length;
    }

    /* Prevent repetitive NULL check */
    if (received != NULL)
    {
        *received = local_received;
    }

    /* Enable next transfer. Current one is finished */
    if (pdFALSE == xSemaphoreGive(handle->rxSemaphore))
    {
        /* We could not post the semaphore, exit with error */
        retval = kStatus_Fail;
    }
    return retval;
}
