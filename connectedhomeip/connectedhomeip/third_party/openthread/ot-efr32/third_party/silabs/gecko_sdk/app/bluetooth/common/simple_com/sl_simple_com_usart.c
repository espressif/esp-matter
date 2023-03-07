/***************************************************************************//**
 * @file
 * @brief Simple Communication Interface (UART)
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sl_status.h"
#include "em_common.h"
#include "em_core.h"
#include "sl_uartdrv_instances.h"
#include "app_assert.h"
#include "sl_simple_com.h"
#include "sl_simple_com_config.h"
#ifdef EFR32BG1_USART_E202_WORKAROUND
#include "sl_sleeptimer.h"
#endif // EFR32BG1_USART_E202_WORKAROUND

// Store UARTDRV handle to use with Simple COM
static UARTDRV_Handle_t uartdrv_handle = NULL;
static IRQn_Type irq_number;

// store the maximum waiting time for a callback
static uint32_t time_to_wait_tx_callback = 0;

// Uart receive and transmit buffers
static uint8_t rx_buf[SL_SIMPLE_COM_RX_BUF_SIZE] = { 0 };
static uint8_t tx_buf[SL_SIMPLE_COM_TX_BUF_SIZE] = { 0 };

#ifdef EFR32BG1_USART_E202_WORKAROUND
// Internal timer and counter for receive
// EFR32BG1 - USART_E202 workaround
static sl_sleeptimer_timer_handle_t receive_timer;
static uint32_t received_count = 0;
#endif // EFR32BG1_USART_E202_WORKAROUND

// Internal Uart receive and transmit callback
static void transmit_cb(UARTDRV_Handle_t handle,
                        Ecode_t transferStatus,
                        uint8_t *data,
                        UARTDRV_Count_t transferCount);
static void receive_cb(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount);
// Helper functions to ensure reception
static Ecode_t abort_receive(UARTDRV_Handle_t handle);
static Ecode_t get_tail_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                               UARTDRV_Buffer_t **buffer);
static Ecode_t dequeue_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                              UARTDRV_Buffer_t **buffer);
static void disable_receiver(UARTDRV_Handle_t handle);

static IRQn_Type irq_number_from_handle(UARTDRV_Handle_t handle);

void sli_simple_com_isr(void);

// -----------------------------------------------------------------------------
// Public functions (API implementation)

/**************************************************************************//**
 * Simple Comm Init.
 *****************************************************************************/
void sl_simple_com_init(void)
{
  // clear RX and TX buffers
  memset(rx_buf, 0, sizeof(rx_buf));
  memset(tx_buf, 0, sizeof(tx_buf));

  // Get the default UARTDRV handle to use for Simple COM
  uartdrv_handle = sl_uartdrv_get_default();
  irq_number = irq_number_from_handle(uartdrv_handle);
  app_assert(irq_number >= (IRQn_Type)(0),
             "Unknown UARTDRV handle\n");

  // calculate the maximum amount of time to wait for UART TX buffer to empty
  time_to_wait_tx_callback = CMU_ClockFreqGet(cmuClock_CORE)
                             / USART_BaudrateGet(uartdrv_handle->peripheral.uart)
                             * 4;
}

/**************************************************************************//**
 * Uart transmit function
 *
 * Transmits len bytes of data through Uart interface using DMA.
 *
 * @param[out] len Message lenght
 * @param[out] data Message data
 *****************************************************************************/
void sl_simple_com_transmit(uint32_t len, uint8_t *data)
{
  Ecode_t ec;
  // make a copy of the data to be sent to guarantee its integrity until
  // transmission completes
  app_assert(len <= SL_SIMPLE_COM_TX_BUF_SIZE,
             "TX length is bigger than allocated buffer\n");
  memcpy((void *)tx_buf, (void *)data, (size_t)len);
  // Transmit data using a non-blocking transmit function
  ec = UARTDRV_Transmit(uartdrv_handle,
                        tx_buf,
                        len,
                        transmit_cb);
  app_assert(ECODE_EMDRV_UARTDRV_OK == ec,
             "[E: 0x%04x] Failed to start transmitting\n",
             (int)ec);
  (void)ec;
}

/**************************************************************************//**
 * Uart receive function
 *
 * Starts reception on Uart interface using DMA.
 *
 * @note The problem with standard uart reception is that it needs the exact
 * amount of bytes to read. This implementation starts a timer to measure idle
 * time on the bus and based on that puts the received message into the buffer.
 *****************************************************************************/
void sl_simple_com_receive(void)
{
  Ecode_t ec;
  // Clear pending RX interrupt flag in NVIC
  NVIC_ClearPendingIRQ(irq_number);
  NVIC_EnableIRQ(irq_number);
  // Setup RX timeout to 255 bit-time
  uartdrv_handle->peripheral.uart->TIMECMP1 = \
    (USART_TIMECMP1_TSTOP_RXACT
     | USART_TIMECMP1_TSTART_RXEOF
     | (0xff << _USART_TIMECMP1_TCMPVAL_SHIFT));
  // Clear any USART interrupt flags
  USART_IntClear(uartdrv_handle->peripheral.uart,
                 _USART_IF_MASK);
  USART_IntEnable(uartdrv_handle->peripheral.uart,
                  USART_IF_TXIDLE | USART_IF_TCMP1);
  // Start reception with callback set
  ec = UARTDRV_Receive(uartdrv_handle,
                       rx_buf,
                       sizeof(rx_buf),
                       receive_cb);
  app_assert(ECODE_EMDRV_UARTDRV_OK == ec,
             "[E: 0x%04x] Failed to start receiving\n",
             (int)ec);
  (void)ec;
}

/**************************************************************************//**
 * Uart transmit completed callback
 *
 * Called after Uart transmit is finished.
 *
 * @param[in] status Status of the transmission
 *
 * @note Weak implementation
 *  Called after DMA is complete, the Uart transmission might be still ongoing
 *****************************************************************************/
SL_WEAK void sl_simple_com_transmit_cb(sl_status_t status)
{
  (void)status;
}

/**************************************************************************//**
 * Uart receive completed callback
 *
 * Called after Uart receive is finished.
 *
 * @param[in] status Status of the reception
 * @param[in] len Received message length
 * @param[in] data Data received
 *
 * @note Weak implementation
 *****************************************************************************/
SL_WEAK void sl_simple_com_receive_cb(sl_status_t status,
                                      uint32_t len,
                                      uint8_t *data)
{
  (void)status;
  (void)data;
  (void)len;
}

// -----------------------------------------------------------------------------
// Private functions

#ifdef EFR32BG1_USART_E202_WORKAROUND
// EFR32BG1 - USART_E202 workaround
static void timer_callback(sl_sleeptimer_timer_handle_t *handle,
                           void *data)
{
  (void)handle;
  (void)data;
  // Get received bytes
  uint8_t* buffer = NULL;
  uint32_t received = 0;
  uint32_t remaining = 0;
  UARTDRV_GetReceiveStatus(uartdrv_handle, &buffer, &received, &remaining);

  // if no change in that time callback is needed
  if (received_count == received) {
    sl_sleeptimer_stop_timer(&receive_timer);
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    // abort receive operation
    (void)abort_receive(uartdrv_handle);
    sl_simple_com_receive();
    CORE_EXIT_ATOMIC();
    received_count = 0;
  } else {
    // otherwise register received bytes for the next run
    received_count = received;
  }
}

#endif // EFR32BG1_USART_E202_WORKAROUND

/**************************************************************************//**
 * Uart interrupt handler
 *
 * Called when the set timer for tx idle states finished.
 *
 * @note automatically restarts the timer and starts a new reception.
 * @note TODO: this should be provided by UARTDRV
 *****************************************************************************/
void sli_simple_com_isr(void)
{
  // RX timeout, stop transfer and handle what we got in buffer
  if (uartdrv_handle->peripheral.uart->IF & USART_IF_TCMP1) {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    // stop the timer
    uartdrv_handle->peripheral.uart->TIMECMP1 &= \
      ~_USART_TIMECMP1_TSTART_MASK;
    uartdrv_handle->peripheral.uart->TIMECMP1 |= \
      USART_TIMECMP1_TSTART_RXEOF;
    // clear timer interrupt
    USART_IntClear(uartdrv_handle->peripheral.uart,
                   USART_IF_TCMP1);
#ifdef EFR32BG1_USART_E202_WORKAROUND
    // EFR32BG1 - USART_E202 workaround
    sl_sleeptimer_stop_timer(&receive_timer);
    sl_sleeptimer_start_periodic_timer(
      &receive_timer,
      sl_sleeptimer_ms_to_tick(1),
      timer_callback,
      NULL,
      0,
      0);
    uint8_t* buffer = NULL;
    uint32_t received = 0;
    uint32_t remaining = 0;
    UARTDRV_GetReceiveStatus(uartdrv_handle, &buffer, &received, &remaining);
    received_count = received;
#else
    // abort receive operation
    (void)abort_receive(uartdrv_handle);
    sl_simple_com_receive();
#endif // EFR32BG1_USART_E202_WORKAROUND
    CORE_EXIT_ATOMIC();
  }
}

/**************************************************************************//**
 * Internal Uart transmit completed callback
 *
 * Called after Uart transmit is finished.
 *
 * @param[in] handle Connection handle
 * @param[in] transferStatus Status of the transfer
 * @param[in] data Transmitted data
 * @param[in] transferCount Number of sent bytes
 *
 * @note Calls public sl_simple_com_transmit_cb
 *  Called after DMA is complete, the Uart transmission might be still ongoing
 *****************************************************************************/
static void transmit_cb(UARTDRV_Handle_t handle,
                        Ecode_t transferStatus,
                        uint8_t *data,
                        UARTDRV_Count_t transferCount)
{
  uint16_t timeout = 0;
  (void)data;
  (void)transferCount;

  // wait until UART finished transfer
  while ((!(UARTDRV_GetPeripheralStatus(handle) & UARTDRV_STATUS_TXC))
         && (timeout < time_to_wait_tx_callback)) {
    timeout++;
  }
  // Call public callback API
  sl_simple_com_transmit_cb(ECODE_EMDRV_UARTDRV_OK == transferStatus
                            ? SL_STATUS_OK : SL_STATUS_FAIL);
  // Clear TX buffer
  memset(tx_buf, 0, sizeof(tx_buf));
}

/**************************************************************************//**
 * Internal Uart receive completed callback
 *
 * Called after Uart receive is finished.
 *
 * @param[in] handle Connection handle
 * @param[in] transferStatus Status of the transfer
 * @param[in] data Received data
 * @param[in] transferCount Number of received bytes
 *
 * @note Calls public sl_simple_com_receive_cb
 *****************************************************************************/
static void receive_cb(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount)
{
  (void)handle;
  // Call public callback API
  sl_simple_com_receive_cb(ECODE_EMDRV_UARTDRV_OK == transferStatus
                           ? SL_STATUS_OK : SL_STATUS_FAIL,
                           transferCount,
                           data);
  // Clear RX buffer
  memset(rx_buf, 0, sizeof(rx_buf));
}

/**************************************************************************//**
 * Aborted reception handler
 *
 * @param[in] handle Connection handle
 *****************************************************************************/
static Ecode_t abort_receive(UARTDRV_Handle_t handle)
{
  UARTDRV_Buffer_t *rxBuffer;
  Ecode_t status;
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if (handle->rxQueue->used == 0) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_IDLE;
  }

  // -------------------------------
  // Stop the current transfer
  (void)DMADRV_StopTransfer(handle->rxDmaCh);
  handle->rxDmaActive = false;
  // Update the transfer status of the active transfer
  status = get_tail_buffer(handle->rxQueue, &rxBuffer);
  // If an abort was in progress when DMA completed, the ISR could be deferred
  // until after the critical section. In this case, the buffers no longer
  // exist, even though the DMA complete callback was called.
  if (status == ECODE_EMDRV_UARTDRV_QUEUE_EMPTY) {
    return ECODE_EMDRV_UARTDRV_QUEUE_EMPTY;
  }
  EFM_ASSERT(rxBuffer != NULL);
  (void)DMADRV_TransferRemainingCount(handle->rxDmaCh,
                                      (int *)&rxBuffer->itemsRemaining);
  rxBuffer->transferStatus = ECODE_EMDRV_UARTDRV_ABORTED;

  // -------------------------------
  // Dequeue all transfers and call callback
  while (handle->rxQueue->used > 0) {
    (void)dequeue_buffer(handle->rxQueue, &rxBuffer);
    // Call the callback
    if (rxBuffer->callback != NULL) {
      if (rxBuffer->callback != NULL) {
        rxBuffer->callback(handle,
                           ECODE_EMDRV_UARTDRV_OK,
                           rxBuffer->data,
                           rxBuffer->transferCount - rxBuffer->itemsRemaining);
      }
    }
  }

  // -------------------------------
  // Disable the receiver
  if (handle->fcType != uartdrvFlowControlHwUart) {
    disable_receiver(handle);
  }
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_UARTDRV_OK;
}

/**************************************************************************//**
 * Gets the buffer tail.
 *
 * @param[in] queue Input buffer
 * @param[out] buffer Output buffer
 *****************************************************************************/
static Ecode_t get_tail_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                               UARTDRV_Buffer_t **buffer)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (queue->used == 0) {
    *buffer = NULL;
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_QUEUE_EMPTY;
  }
  *buffer = &queue->fifo[queue->tail];

  CORE_EXIT_ATOMIC();
  return ECODE_EMDRV_UARTDRV_OK;
}

/**************************************************************************//**
 * Dequeues buffer
 *
 * Moves through the buffer.
 *
 * @param[in] queue Input buffer
 * @param[out] buffer Output buffer
 *****************************************************************************/
static Ecode_t dequeue_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                              UARTDRV_Buffer_t **buffer)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (queue->used == 0) {
    *buffer = NULL;
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_QUEUE_EMPTY;
  }
  *buffer = &queue->fifo[queue->tail];
  queue->tail = (queue->tail + 1) % queue->size;
  queue->used--;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_UARTDRV_OK;
}

/**************************************************************************//**
 * Disables receiver.
 *
 * @param[in] handle Connection handle
 *****************************************************************************/
static void disable_receiver(UARTDRV_Handle_t handle)
{
#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) \
  && !defined(_SILICON_LABS_32B_SERIES_2))       \
  || (defined(EUART_COUNT) && (EUART_COUNT > 0) )
  if (handle->type == uartdrvUartTypeUart)
#endif
  {
    // Disable Rx route
    #if defined(USART_ROUTEPEN_RXPEN)
    handle->peripheral.uart->ROUTEPEN &= ~USART_ROUTEPEN_RXPEN;
    #elif defined(USART_ROUTE_RXPEN)
    handle->peripheral.uart->ROUTE &= ~USART_ROUTE_RXPEN;
    #elif defined(GPIO_USART_ROUTEEN_RXPEN)
    GPIO->USARTROUTE_CLR[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN;
    #endif
    // Disable Rx
    handle->peripheral.uart->CMD = USART_CMD_RXDIS;
  }
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) \
  && !defined(_SILICON_LABS_32B_SERIES_2)
  else if (handle->type == uartdrvUartTypeLeuart) {
    // Wait for prevous register writes to sync
    while ((handle->peripheral.leuart->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
    }

    // Disable Rx route
    #if defined(LEUART_ROUTEPEN_RXPEN)
    handle->peripheral.leuart->ROUTEPEN &= ~LEUART_ROUTEPEN_RXPEN;
    #else
    handle->peripheral.leuart->ROUTE &= ~LEUART_ROUTE_RXPEN;
    #endif
    // Disable Rx
    handle->peripheral.leuart->CMD = LEUART_CMD_RXDIS;
  }
#elif defined(EUART_COUNT) && (EUART_COUNT > 0)
  else if (handle->type == uartdrvUartTypeEuart) {
    if (EUSART_StatusGet(handle->peripheral.euart) &  EUSART_STATUS_TXENS) {
      EUSART_Enable(handle->peripheral.euart, eusartEnableTx);
    } else {
      EUSART_Enable(handle->peripheral.euart, eusartDisable);
    }
  }
#endif
}

/**************************************************************************//**
 * Get NVIC IRQ number from UARTDRV handle
 *
 * @param[in] handle Connection handle
 * @return Interrupt number
 *****************************************************************************/
static IRQn_Type irq_number_from_handle(UARTDRV_Handle_t handle)
{
  IRQn_Type irq_number = (IRQn_Type)(-1);

  switch ((uint32_t)handle->peripheral.uart) {
#if defined(USART0_BASE)
    case USART0_BASE:
      irq_number = USART0_RX_IRQn;
      break;
#endif
#if defined(USART1_BASE)
    case USART1_BASE:
      irq_number = USART1_RX_IRQn;
      break;
#endif
#if defined(USART2_BASE)
    case USART2_BASE:
      irq_number = USART2_RX_IRQn;
      break;
#endif
#if defined(USART3_BASE)
    case USART3_BASE:
      irq_number = USART3_RX_IRQn;
      break;
#endif
    default:
      app_assert(false,
                 "Unknown UARTDRV handle\n");
      break;
  }

  return irq_number;
}
