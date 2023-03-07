/***************************************************************************//**
 * @file
 * @brief UARTDRV API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#define CURRENT_MODULE_NAME    "UARTDRV"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "uartdrv.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_core.h"
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
#include "gpiointerrupt.h"
#endif
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#include "sl_sleeptimer.h"
#else
#include "em_emu.h"
#endif

#include <string.h>

//****************************************************************************

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
#define UART_HW_FLOW_CONTROL_SUPPORT
#elif (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE) && defined(USART_CTRLX_CTSEN)
#define UART_HW_FLOW_CONTROL_SUPPORT
#endif

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
#define HANDLES_ARE_AVAILABLE   1
#elif defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
#define HANDLES_ARE_AVAILABLE   1
#else
#define HANDLES_ARE_AVAILABLE   0
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
#define UART_DMA_IRQ          DMA_IRQn
#define UART_DMA_IRQHANDLER() DMA_IRQHandler()
#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
#define UART_DMA_IRQ          LDMA_IRQn
#define UART_DMA_IRQHANDLER() LDMA_IRQHandler()
#else
#error "No valid UARTDRV DMA engine defined."
#endif

//****************************************************************************

#if (HANDLES_ARE_AVAILABLE)
static bool uartdrvHandleIsInitialized = false;
static UARTDRV_Handle_t uartdrvHandle[EMDRV_UARTDRV_MAX_DRIVER_INSTANCES];
#endif
static bool enableRxWhenSleeping = UARTDRV_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION;

//****************************************************************************

static bool ReceiveDmaComplete(unsigned int channel,
                               unsigned int sequenceNo,
                               void *userParam);
static bool TransmitDmaComplete(unsigned int channel,
                                unsigned int sequenceNo,
                                void *userParam);

/***************************************************************************//**
 * @brief Power management functions for the uartdrv.
 ******************************************************************************/
static void em1RequestAdd(UARTDRV_Handle_t handle)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (handle->em1RequestCount == 0) {
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  }
  handle->em1RequestCount++;
  CORE_EXIT_ATOMIC();
#else
  handle->em1RequestCount++;
#endif
}

static void em1RequestRemove(UARTDRV_Handle_t handle)
{
  EFM_ASSERT(handle->em1RequestCount > 0);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  handle->em1RequestCount--;
  if (handle->em1RequestCount == 0) {
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
  }
  CORE_EXIT_ATOMIC();
#else
  handle->em1RequestCount--;
#endif
}

#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
static void emRequestInit(UARTDRV_Handle_t handle)
{
  handle->em1RequestCount = 0;
}
#endif

static void emRequestDeinit(UARTDRV_Handle_t handle)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  CORE_DECLARE_IRQ_STATE;
  sl_status_t status;
  bool running;

  CORE_ENTER_ATOMIC();
  status = sl_sleeptimer_is_timer_running(&handle->delayedTxTimer, &running);
  if ((status == SL_STATUS_OK) && (running)) {
    sl_sleeptimer_stop_timer(&handle->delayedTxTimer);
  }
  CORE_EXIT_ATOMIC();
#endif
  if (handle->em1RequestCount > 0) {
    handle->em1RequestCount = 1;
    em1RequestRemove(handle);
  }
}

/***************************************************************************//**
 * @brief Get UARTDRV_Handle_t from GPIO pin number (HW FC CTS pin interrupt).
 ******************************************************************************/
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
static UARTDRV_Handle_t HwFcCtsIrqGetDrvHandle(uint32_t gpioPinNo)
{
  uint32_t i;

  for (i = 0; i < EMDRV_UARTDRV_MAX_DRIVER_INSTANCES; i++) {
    if (uartdrvHandle[i]->ctsPin == gpioPinNo) {
      return uartdrvHandle[i];
    }
  }
  return NULL;
}

/***************************************************************************//**
 * @brief Get CTS pin state.
 ******************************************************************************/
static UARTDRV_FlowControlState_t HwFcGetClearToSendPin(UARTDRV_Handle_t handle)
{
  if (handle->fcType == uartdrvFlowControlHw) {
    return (UARTDRV_FlowControlState_t)GPIO_PinInGet(handle->ctsPort, handle->ctsPin);
  } else {
    return uartdrvFlowControlOn;
  }
}

/***************************************************************************//**
 * @brief Manage CTS pin change.
 ******************************************************************************/
static void HwFcManageClearToSend(uint8_t gpioPinNo)
{
  UARTDRV_Handle_t handle = HwFcCtsIrqGetDrvHandle(gpioPinNo);

  if (handle && handle->fcType == uartdrvFlowControlHw) {
    // If not auto mode, assign the CTS pin state to the self state
    // If auto mode, also control UART Tx enable
    handle->fcSelfState = HwFcGetClearToSendPin(handle);
    // Only manage DMA if not already paused by SW
    if (handle->fcSelfCfg == uartdrvFlowControlAuto && (handle->txDmaPaused == 0)) {
      bool active = false;
      Ecode_t status = DMADRV_TransferActive(handle->txDmaCh, &active);
      if ((handle->fcSelfState == uartdrvFlowControlOn) || handle->IgnoreRestrain) {
        handle->IgnoreRestrain = false;
        DMADRV_ResumeTransfer(handle->txDmaCh);
      }
      // Only pause DMA if currently active
      else if ((status == ECODE_EMDRV_DMADRV_OK) && active) {
        DMADRV_PauseTransfer(handle->txDmaCh);
      }
    }
  }
}

static Ecode_t FcApplyState(UARTDRV_Handle_t handle)
{
  uint8_t fcSwCode;

  if (handle->fcType == uartdrvFlowControlHw) {
    if (handle->fcSelfCfg == uartdrvFlowControlOn) {
      // Assert nRTS (application control)
      GPIO_PinOutClear(handle->rtsPort, handle->rtsPin);
    } else if (handle->fcSelfCfg == uartdrvFlowControlOff) {
      // Deassert nRTS (application control)
      GPIO_PinOutSet(handle->rtsPort, handle->rtsPin);
    } else { // Auto mode
      if (handle->fcSelfState == uartdrvFlowControlOn) {
        // Assert nRTS
        GPIO_PinOutClear(handle->rtsPort, handle->rtsPin);
      } else { // Off
               // Deassert nRTS
        GPIO_PinOutSet(handle->rtsPort, handle->rtsPin);
      }
    }
  } else if (handle->fcType == uartdrvFlowControlSw) {
    if (handle->fcSelfState == uartdrvFlowControlOn) {
      fcSwCode = UARTDRV_FC_SW_XON;
      // Pause transmit to ensure XON transmits immediately
      UARTDRV_PauseTransmit(handle);
      UARTDRV_ForceTransmit(handle, &fcSwCode, 1);
      UARTDRV_ResumeTransmit(handle);
    } else {
      fcSwCode = UARTDRV_FC_SW_XOFF;
      // Pause transmit to ensure XOFF transmits immediately
      UARTDRV_PauseTransmit(handle);
      UARTDRV_ForceTransmit(handle, &fcSwCode, 1);
      UARTDRV_ResumeTransmit(handle);
    }
  }
  return ECODE_EMDRV_UARTDRV_OK;
}
#else
// CTS pin should always read as uartdrvFlowControlOn when HW FC is disabled
#define HwFcGetClearToSendPin(x) uartdrvFlowControlOn
#endif /* EMDRV_UARTDRV_FLOW_CONTROL_ENABLE */

/***************************************************************************//**
 * @brief Enqueue UART transfer buffer.
 ******************************************************************************/
static Ecode_t EnqueueBuffer(UARTDRV_Buffer_FifoQueue_t *queue,
                             UARTDRV_Buffer_t *inputBuffer,
                             UARTDRV_Buffer_t **queueBuffer)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (queue->used >= queue->size) {
    *queueBuffer = NULL;
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_QUEUE_FULL;
  }
  memcpy((void *)&queue->fifo[queue->head],
         (const void *)inputBuffer,
         sizeof(UARTDRV_Buffer_t));
  *queueBuffer = &queue->fifo[queue->head];
  queue->head = (queue->head + 1) % queue->size;
  queue->used++;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief Dequeue UART transfer buffer.
 ******************************************************************************/
static Ecode_t DequeueBuffer(UARTDRV_Buffer_FifoQueue_t *queue,
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

/***************************************************************************//**
 * @brief Get tail UART transfer buffer.
 ******************************************************************************/
static Ecode_t GetTailBuffer(UARTDRV_Buffer_FifoQueue_t *queue,
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

/***************************************************************************//**
 * @brief Enable UART transmitter.
 ******************************************************************************/
static void EnableTransmitter(UARTDRV_Handle_t handle)
{
  if (false) {
  }
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)) \
  || (defined(EUART_COUNT) && (EUART_COUNT > 0))                                          \
  || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeUart)
#endif  // LEUART || EUART || EUSART
  {
    // Enable Tx
    handle->peripheral.uart->CMD = USART_CMD_TXEN;
    // Wait for Tx to be enabled
    while (!(handle->peripheral.uart->STATUS & USART_STATUS_TXENS)) {
    }

    // Enable Tx route
#if defined(USART_ROUTEPEN_TXPEN)
    handle->peripheral.uart->ROUTEPEN |= USART_ROUTEPEN_TXPEN;
#elif defined(USART_ROUTE_TXPEN)
    handle->peripheral.uart->ROUTE |= USART_ROUTE_TXPEN;
#elif defined(GPIO_USART_ROUTEEN_TXPEN)
    GPIO->USARTROUTE_SET[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN;
#endif
  }
#endif  // UART || USART
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  else if (handle->type == uartdrvUartTypeLeuart) {
    // Wait for previous register writes to sync
    while ((handle->peripheral.leuart->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
    }

    // Enable Tx
    handle->peripheral.leuart->CMD = LEUART_CMD_TXEN;
    // Wait for Tx to be enabled
    while (!(handle->peripheral.leuart->STATUS & LEUART_STATUS_TXENS)) {
    }

    // Enable Tx route
#if defined(LEUART_ROUTEPEN_TXPEN)
    handle->peripheral.leuart->ROUTEPEN |= LEUART_ROUTEPEN_TXPEN;
#else
    handle->peripheral.leuart->ROUTE |= LEUART_ROUTE_TXPEN;
#endif
  }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeEuart) {
    if (EUSART_StatusGet(handle->peripheral.euart) & EUSART_STATUS_RXENS) {
      EUSART_Enable(handle->peripheral.euart, eusartEnable);
    } else {
      EUSART_Enable(handle->peripheral.euart, eusartEnableTx);
    }
#if defined(EUART_PRESENT)
    GPIO->EUARTROUTE_SET->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
#elif defined(EUSART_PRESENT)
    GPIO->EUSARTROUTE_SET[handle->uartNum].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;
#endif
  }
#endif
}

/***************************************************************************//**
 * @brief Disable UART transmitter.
 ******************************************************************************/
static void DisableTransmitter(UARTDRV_Handle_t handle)
{
  if (false) {
  }
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)) \
  || (defined(EUART_COUNT) && (EUART_COUNT > 0))                                          \
  || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeUart)
#endif
  {
    // Disable Tx route
#if defined(USART_ROUTEPEN_TXPEN)
    handle->peripheral.uart->ROUTEPEN &= ~USART_ROUTEPEN_TXPEN;
#elif defined(USART_ROUTE_TXPEN)
    handle->peripheral.uart->ROUTE &= ~USART_ROUTE_TXPEN;
#elif defined(GPIO_USART_ROUTEEN_TXPEN)
    GPIO->USARTROUTE_CLR[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN;
#endif
    // Disable Tx
    handle->peripheral.uart->CMD = USART_CMD_TXDIS;
  }
#endif  // UART || USART
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  else if (handle->type == uartdrvUartTypeLeuart) {
    // Wait for previous register writes to sync
    while ((handle->peripheral.leuart->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
    }

    // Disable Tx route
#if defined(LEUART_ROUTEPEN_TXPEN)
    handle->peripheral.leuart->ROUTEPEN &= ~LEUART_ROUTEPEN_TXPEN;
#else
    handle->peripheral.leuart->ROUTE &= ~LEUART_ROUTE_TXPEN;
#endif
    // Disable Tx
    handle->peripheral.leuart->CMD = LEUART_CMD_TXDIS;
  }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeEuart) {
    if (EUSART_StatusGet(handle->peripheral.euart) &  EUSART_STATUS_RXENS) {
      EUSART_Enable(handle->peripheral.euart, eusartEnableRx);
    } else {
      EUSART_Enable(handle->peripheral.euart, eusartDisable);
    }
#if defined(EUART_PRESENT)
    GPIO->EUARTROUTE_CLR->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
#elif defined(EUSART_PRESENT)
    GPIO->EUSARTROUTE_CLR[handle->uartNum].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;
#endif
  }
#endif
}

/***************************************************************************//**
 * @brief Enable UART receiver.
 ******************************************************************************/
static void EnableReceiver(UARTDRV_Handle_t handle)
{
  if (false) {
  }
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)) \
  || (defined(EUART_COUNT) && (EUART_COUNT > 0))                                          \
  || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeUart)
#endif
  {
    // Enable Rx
    handle->peripheral.uart->CMD = USART_CMD_RXEN;
    // Wait for Rx to be enabled
    while (!(handle->peripheral.uart->STATUS & USART_STATUS_RXENS)) {
    }

    // Enable Rx route
#if defined(USART_ROUTEPEN_RXPEN)
    handle->peripheral.uart->ROUTEPEN |= USART_ROUTEPEN_RXPEN;
#elif defined(USART_ROUTE_RXPEN)
    handle->peripheral.uart->ROUTE |= USART_ROUTE_RXPEN;
#elif defined(GPIO_USART_ROUTEEN_RXPEN)
    GPIO->USARTROUTE_SET[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN;
#endif
  }
#endif  // UART || USART
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  else if (handle->type == uartdrvUartTypeLeuart) {
    // Wait for previous register writes to sync
    while ((handle->peripheral.leuart->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
    }

    // Enable Rx
    handle->peripheral.leuart->CMD = LEUART_CMD_RXEN;
    // Wait for Rx to be enabled
    while (!(handle->peripheral.leuart->STATUS & LEUART_STATUS_RXENS)) {
    }

    // Enable Rx route
#if defined(LEUART_ROUTEPEN_RXPEN)
    handle->peripheral.leuart->ROUTEPEN |= LEUART_ROUTEPEN_RXPEN;
#else
    handle->peripheral.leuart->ROUTE |= LEUART_ROUTE_RXPEN;
#endif
  }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeEuart) {
    if (EUSART_StatusGet(handle->peripheral.euart) &  EUSART_STATUS_TXENS) {
      EUSART_Enable(handle->peripheral.euart, eusartEnable);
    } else {
      EUSART_Enable(handle->peripheral.euart, eusartEnableRx);
    }
  }
#endif
}

/***************************************************************************//**
 * @brief Disable UART receiver.
 ******************************************************************************/
static void DisableReceiver(UARTDRV_Handle_t handle)
{
  if (false) {
  }
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)) \
  || (defined(EUART_COUNT) && (EUART_COUNT > 0))                                          \
  || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeUart)
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
#endif  // UART || USART
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
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
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  else if (handle->type == uartdrvUartTypeEuart) {
    if (EUSART_StatusGet(handle->peripheral.euart) &  EUSART_STATUS_TXENS) {
      EUSART_Enable(handle->peripheral.euart, eusartEnableTx);
    } else {
      EUSART_Enable(handle->peripheral.euart, eusartDisable);
    }
  }
#endif
}

/***************************************************************************//**
 * @brief Start a UART DMA receive operation.
 ******************************************************************************/
static void StartReceiveDma(UARTDRV_Handle_t handle,
                            UARTDRV_Buffer_t *buffer)
{
  void *rxPort = NULL;

  handle->rxDmaActive = true;

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    rxPort = (void *)&(handle->peripheral.uart->RXDATA);
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    rxPort = (void *)&(handle->peripheral.leuart->RXDATA);
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    rxPort = (void *)&(handle->peripheral.euart->RXDATA);
#endif
  } else {
    handle->rxDmaActive = false;
    return;
  }

  if (enableRxWhenSleeping) {
    em1RequestAdd(handle);
  }
  DMADRV_PeripheralMemory(handle->rxDmaCh,
                          handle->rxDmaSignal,
                          buffer->data,
                          rxPort,
                          true,
                          buffer->transferCount,
                          dmadrvDataSize1,
                          ReceiveDmaComplete,
                          handle);
}

/***************************************************************************//**
 * @brief Start a UART DMA transmit operation.
 ******************************************************************************/
static void StartTransmitDma(UARTDRV_Handle_t handle,
                             UARTDRV_Buffer_t *buffer)
{
  void *txPort = NULL;

  handle->txDmaActive = true;

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    txPort = (void *)&(handle->peripheral.uart->TXDATA);
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    // Set TX DMA wakeup request. Needed for transmit while in EM2.
    handle->peripheral.leuart->CTRL |= LEUART_CTRL_TXDMAWU;
    txPort = (void *)&(handle->peripheral.leuart->TXDATA);
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    txPort = (void *)&(handle->peripheral.euart->TXDATA);
#endif
  } else {
    handle->txDmaActive = false;
    return;
  }

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  CORE_DECLARE_IRQ_STATE;
  sl_status_t status;
  bool running;

  CORE_ENTER_ATOMIC();
  status = sl_sleeptimer_is_timer_running(&handle->delayedTxTimer, &running);
  if ((status == 0) && (running)) {
    sl_sleeptimer_stop_timer(&handle->delayedTxTimer);
    em1RequestRemove(handle);
  }
  CORE_EXIT_ATOMIC();
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->sleep = SL_POWER_MANAGER_IGNORE;
#endif
#endif

  em1RequestAdd(handle);
  DMADRV_MemoryPeripheral(handle->txDmaCh,
                          handle->txDmaSignal,
                          txPort,
                          buffer->data,
                          true,
                          buffer->transferCount,
                          dmadrvDataSize1,
                          TransmitDmaComplete,
                          handle);
}

/***************************************************************************//**
 * @brief DMA transfer completion callback. Called by the DMA interrupt handler.
 ******************************************************************************/
static bool ReceiveDmaComplete(unsigned int channel,
                               unsigned int sequenceNo,
                               void *userParam)
{
  CORE_DECLARE_IRQ_STATE;
  UARTDRV_Handle_t handle;
  UARTDRV_Buffer_t *buffer;
  Ecode_t status;
  (void)channel;
  (void)sequenceNo;
  handle = (UARTDRV_Handle_t)userParam;
  status = GetTailBuffer(handle->rxQueue, &buffer);

  if (enableRxWhenSleeping) {
    em1RequestRemove(handle);
  }

  // If an abort was in progress when DMA completed, the ISR could be deferred
  // until after the critical section. In this case, the buffers no longer
  // exist, even though the DMA complete callback was called.
  if (status == ECODE_EMDRV_UARTDRV_QUEUE_EMPTY) {
    return true;
  }

  EFM_ASSERT(buffer != NULL);

#if defined(USART_IF_FERR)
  if (handle->type == uartdrvUartTypeUart
      && (handle->peripheral.uart->IF & USART_IF_FERR)) {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_FRAME_ERROR;
    buffer->itemsRemaining = 0;
#if defined(USART_HAS_SET_CLEAR)
    handle->peripheral.uart->IF_CLR = USART_IF_FERR;
#else
    handle->peripheral.uart->IFC = USART_IFC_FERR;
#endif
  } else
#endif
#if defined(USART_IF_PERR)
  if (handle->type == uartdrvUartTypeUart
      && (handle->peripheral.uart->IF & USART_IF_PERR)) {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_PARITY_ERROR;
    buffer->itemsRemaining = 0;
#if defined(USART_HAS_SET_CLEAR)
    handle->peripheral.uart->IF_CLR = USART_IF_PERR;
#else
    handle->peripheral.uart->IFC = USART_IFC_PERR;
#endif
  } else
#endif
#if defined(LEUART_IF_FERR)
  if (handle->type == uartdrvUartTypeLeuart
      && (handle->peripheral.leuart->IF & LEUART_IF_FERR)) {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_FRAME_ERROR;
    buffer->itemsRemaining = 0;
    handle->peripheral.leuart->IFC = LEUART_IFC_FERR;
  } else
#endif
#if defined(LEUART_IF_PERR)
  if (handle->type == uartdrvUartTypeLeuart
      && (handle->peripheral.leuart->IF & LEUART_IF_PERR)) {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_PARITY_ERROR;
    buffer->itemsRemaining = 0;
    handle->peripheral.leuart->IFC = LEUART_IFC_PERR;
  } else
#endif
#if defined(EUART_PRESENT) || defined(EUSART_PRESENT)
  if (handle->type == uartdrvUartTypeEuart
      && (handle->peripheral.euart->IF & EUSART_IF_FERR)) {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_FRAME_ERROR;
    buffer->itemsRemaining = 0;
    handle->peripheral.euart->IF_CLR = EUSART_IF_FERR;
  } else if (handle->type == uartdrvUartTypeEuart
             && (handle->peripheral.euart->IF & EUSART_IF_PERR)) {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_PARITY_ERROR;
    buffer->itemsRemaining = 0;
    handle->peripheral.euart->IF_CLR = EUSART_IF_PERR;
  } else
#endif
  {
    buffer->transferStatus = ECODE_EMDRV_UARTDRV_OK;
    buffer->itemsRemaining = 0;
  }

  CORE_ENTER_ATOMIC();

  if (buffer->callback != NULL) {
    buffer->callback(handle, buffer->transferStatus, buffer->data, buffer->transferCount - buffer->itemsRemaining);
  }
  // Dequeue the current tail Rx operation, check if more in queue
  DequeueBuffer(handle->rxQueue, &buffer);

  if (handle->rxQueue->used > 0) {
    GetTailBuffer(handle->rxQueue, &buffer);
    StartReceiveDma(handle, buffer);
  } else {
#if EMDRV_UARTDRV_FLOW_CONTROL_ENABLE
    handle->fcSelfState = uartdrvFlowControlOff;
    FcApplyState(handle);
#endif
    handle->rxDmaActive = false;

    if (handle->fcType != uartdrvFlowControlHwUart) {
      DisableReceiver(handle);
    }
  }
  CORE_EXIT_ATOMIC();
  return true;
}

/***************************************************************************//**
 * @brief Calculate the number of sleeptimer ticks to flush the uart tx buffers.
 ******************************************************************************/
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static uint32_t calculateSleeptimerTicksToFlushTxBuffers(UARTDRV_Handle_t handle)
{
  uint32_t baud = 0;
  uint32_t ticks = 0;

  switch (handle->type) {
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0)
    case uartdrvUartTypeLeuart:
      baud = LEUART_BaudrateGet(handle->peripheral.leuart);
      break;
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
    case uartdrvUartTypeEuart:
      baud = EUSART_BaudrateGet(handle->peripheral.euart);
      break;
#endif
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
    case uartdrvUartTypeUart:
    default:
      baud = USART_BaudrateGet(handle->peripheral.uart);
      break;
#endif
  }

  if (baud != 0) {  // Avoid division by 0
    // Calculate the number of sleeptimer ticks for:
    // 3 bytes: two in FIFO and one in shift register.
    // 12 bits pr byte: one start bit, 8 data bits, parity and 2 stop bits.
    ticks = (sl_sleeptimer_get_timer_frequency() * 3 * 12) / baud;
    // Round up.
    ticks++;
  }

  return ticks;
}
#endif

/***************************************************************************//**
 * @brief Delayed transmit complete timer callback.
 ******************************************************************************/
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
static void TransmitDmaCompleteDelayed(sl_sleeptimer_timer_handle_t *timer_handle, void *userParam)
{
  (void)timer_handle;
  UARTDRV_Handle_t uartdrv_handle = (UARTDRV_Handle_t)userParam;
  uint32_t reg;
  bool txComplete = false;

  // Check if transmit is completed by checking the uart registers.
  switch (uartdrv_handle->type) {
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0)
    case uartdrvUartTypeLeuart:
      reg = uartdrv_handle->peripheral.leuart->STATUS;
      txComplete = reg & LEUART_STATUS_TXC;
      break;
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
    case uartdrvUartTypeEuart:
      reg = uartdrv_handle->peripheral.euart->STATUS;
      txComplete = reg & EUSART_STATUS_TXC;
      break;
#endif
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
    case uartdrvUartTypeUart:
    default:
      reg = uartdrv_handle->peripheral.uart->STATUS;
      txComplete = reg & USART_STATUS_TXC;
      break;
#endif
  }

  if (txComplete) {
    // Remove the power manager request if completed.
    em1RequestRemove(uartdrv_handle);
#if !defined(SL_CATALOG_KERNEL_PRESENT)
    uartdrv_handle->sleep = SL_POWER_MANAGER_SLEEP;
#endif
  } else {
    // Restart the timer if not completed.
    // May be the case if flow control is used.
    uint32_t ticks = calculateSleeptimerTicksToFlushTxBuffers(uartdrv_handle);
    sl_sleeptimer_start_timer(&uartdrv_handle->delayedTxTimer, ticks, TransmitDmaCompleteDelayed, userParam, 0, 0);
  }
}
#endif

/***************************************************************************//**
 * @brief DMA transfer completion callback. Called by the DMA interrupt handler.
 ******************************************************************************/
static bool TransmitDmaComplete(unsigned int channel,
                                unsigned int sequenceNo,
                                void *userParam)
{
  CORE_DECLARE_IRQ_STATE;
  UARTDRV_Handle_t handle;
  UARTDRV_Buffer_t *buffer;
  Ecode_t status;
  (void)channel;
  (void)sequenceNo;

  handle = (UARTDRV_Handle_t)userParam;
  status = GetTailBuffer(handle->txQueue, &buffer);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  uint32_t ticks = calculateSleeptimerTicksToFlushTxBuffers(handle);
  sl_sleeptimer_start_timer(&handle->delayedTxTimer, ticks, TransmitDmaCompleteDelayed, userParam, 0, 0);
#else
  em1RequestRemove(handle);
#endif

  // If an abort was in progress when DMA completed, the ISR could be deferred
  // until after the critical section. In this case, the buffers no longer
  // exist, even though the DMA complete callback was called.
  if (status == ECODE_EMDRV_UARTDRV_QUEUE_EMPTY) {
    return true;
  }

  EFM_ASSERT(buffer != NULL);

  buffer->transferStatus = ECODE_EMDRV_UARTDRV_OK;
  buffer->itemsRemaining = 0;

  CORE_ENTER_ATOMIC();

  if (buffer->callback != NULL) {
    buffer->callback(handle, ECODE_EMDRV_UARTDRV_OK, buffer->data, buffer->transferCount);
  }
  // Dequeue the current tail Tx operation, check if more in queue
  DequeueBuffer(handle->txQueue, &buffer);

  if (handle->txQueue->used > 0) {
    GetTailBuffer(handle->txQueue, &buffer);
    StartTransmitDma(handle, buffer);
  } else {
    handle->txDmaActive = false;
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0)
    if (handle->type == uartdrvUartTypeLeuart) {
      // Clear TX DMA Wakeup request
      handle->peripheral.leuart->CTRL &= ~LEUART_CTRL_TXDMAWU;
    }
#endif
  }
  CORE_EXIT_ATOMIC();
  return true;
}

/***************************************************************************//**
 * @brief Parameter checking function for blocking transfer API functions.
 ******************************************************************************/
static Ecode_t CheckParams(UARTDRV_Handle_t handle, void *data, uint32_t count)
{
  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }
  if ((data == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT)) {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }
  return ECODE_EMDRV_UARTDRV_OK;
}

#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
/***************************************************************************//**
 * @brief Store U(S)ART GPIO pins into handle.
 ******************************************************************************/
static Ecode_t SetupGpioUart(UARTDRV_Handle_t handle,
                             const UARTDRV_InitUart_t * initData)
{
#if defined(_USART_ROUTELOC0_MASK)
  if (false) {
#if defined(USARTRF0)
  } else if (handle->peripheral.uart == USARTRF0) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USARTRF0_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USARTRF0_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USARTRF0_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USARTRF0_RX_PIN(initData->portLocationRx);
#endif
#if defined(USARTRF1)
  } else if (handle->peripheral.uart == USARTRF1) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USARTRF1_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USARTRF1_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USARTRF1_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USARTRF1_RX_PIN(initData->portLocationRx);
#endif
#if defined(USART0)
  } else if (handle->peripheral.uart == USART0) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USART0_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USART0_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USART0_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USART0_RX_PIN(initData->portLocationRx);
#endif
#if defined(USART1)
  } else if (handle->peripheral.uart == USART1) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USART1_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USART1_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USART1_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USART1_RX_PIN(initData->portLocationRx);
#endif
#if defined(USART2)
  } else if (handle->peripheral.uart == USART2) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USART2_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USART2_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USART2_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USART2_RX_PIN(initData->portLocationRx);
#endif
#if defined(USART3)
  } else if (handle->peripheral.uart == USART3) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USART3_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USART3_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USART3_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USART3_RX_PIN(initData->portLocationRx);
#endif
#if defined(USART4)
  } else if (handle->peripheral.uart == USART4) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USART4_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USART4_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USART4_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USART4_RX_PIN(initData->portLocationRx);
#endif
#if defined(USART5)
  } else if (handle->peripheral.uart == USART5) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USART5_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USART5_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_USART5_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_USART5_RX_PIN(initData->portLocationRx);
#endif
#if defined(UART0)
  } else if (handle->peripheral.uart == UART0) {
    handle->txPort = (GPIO_Port_TypeDef)AF_UART0_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_UART0_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_UART0_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_UART0_RX_PIN(initData->portLocationRx);
#endif
#if defined(UART1)
  } else if (handle->peripheral.uart == UART1) {
    handle->txPort = (GPIO_Port_TypeDef)AF_UART1_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_UART1_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_UART1_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_UART1_RX_PIN(initData->portLocationRx);
#endif
#if defined(UART2)
  } else if (handle->peripheral.uart == UART2) {
    handle->txPort = (GPIO_Port_TypeDef)AF_UART2_TX_PORT(initData->portLocationTx);
    handle->rxPort = (GPIO_Port_TypeDef)AF_UART2_RX_PORT(initData->portLocationRx);
    handle->txPin  = AF_UART2_TX_PIN(initData->portLocationTx);
    handle->rxPin  = AF_UART2_RX_PIN(initData->portLocationRx);
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

#elif defined(_USART_ROUTE_MASK)
  if (false) {
#if defined(USARTRF0)
  } else if (handle->peripheral.uart == USARTRF0) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USARTRF0_TX_PORT(initData->portLocation);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USARTRF0_RX_PORT(initData->portLocation);
    handle->txPin  = AF_USARTRF0_TX_PIN(initData->portLocation);
    handle->rxPin  = AF_USARTRF0_RX_PIN(initData->portLocation);
#endif
#if defined(USARTRF1)
  } else if (handle->peripheral.uart == USARTRF1) {
    handle->txPort = (GPIO_Port_TypeDef)AF_USARTRF1_TX_PORT(initData->portLocation);
    handle->rxPort = (GPIO_Port_TypeDef)AF_USARTRF1_RX_PORT(initData->portLocation);
    handle->txPin  = AF_USARTRF1_TX_PIN(initData->portLocation);
    handle->rxPin  = AF_USARTRF1_RX_PIN(initData->portLocation);
#endif
#if defined(USART0)
  } else if (handle->peripheral.uart == USART0) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_USART0_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_USART0_RX_PORT(initData->portLocation);
    handle->txPin   = AF_USART0_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_USART0_RX_PIN(initData->portLocation);
#endif
#if defined(USART1)
  } else if (handle->peripheral.uart == USART1) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_USART1_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_USART1_RX_PORT(initData->portLocation);
    handle->txPin   = AF_USART1_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_USART1_RX_PIN(initData->portLocation);
#endif
#if defined(USART2)
  } else if (handle->peripheral.uart == USART2) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_USART2_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_USART2_RX_PORT(initData->portLocation);
    handle->txPin   = AF_USART2_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_USART2_RX_PIN(initData->portLocation);
#endif
#if defined(UART0)
  } else if (handle->peripheral.uart == UART0) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_UART0_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_UART0_RX_PORT(initData->portLocation);
    handle->txPin   = AF_UART0_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_UART0_RX_PIN(initData->portLocation);
#endif
#if defined(UART1)
  } else if (handle->peripheral.uart == UART1) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_UART1_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_UART1_RX_PORT(initData->portLocation);
    handle->txPin   = AF_UART1_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_UART1_RX_PIN(initData->portLocation);
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

#elif defined(_GPIO_USART_ROUTEEN_MASK)
  handle->txPort  = initData->txPort;
  handle->txPin   = initData->txPin;
  handle->rxPort  = initData->rxPort;
  handle->rxPin   = initData->rxPin;
#endif

  handle->ctsPort = initData->ctsPort;
  handle->ctsPin  = initData->ctsPin;
  handle->rtsPort = initData->rtsPort;
  handle->rtsPin  = initData->rtsPin;

  return ECODE_EMDRV_UARTDRV_OK;
}
#endif // UART || USART

#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief Store LEUART GPIO pins into handle.
 ******************************************************************************/
static Ecode_t SetupGpioLeuart(UARTDRV_Handle_t handle,
                               const UARTDRV_InitLeuart_t * initData)
{
#if defined(_LEUART_ROUTELOC0_MASK)
  if (false) {
#if defined(LEUART0)
  } else if (handle->peripheral.leuart == LEUART0) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_LEUART0_TX_PORT(initData->portLocationTx);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_LEUART0_RX_PORT(initData->portLocationRx);
    handle->txPin   = AF_LEUART0_TX_PIN(initData->portLocationTx);
    handle->rxPin   = AF_LEUART0_RX_PIN(initData->portLocationRx);
#endif
#if defined(LEUART1)
  } else if (handle->peripheral.leuart == LEUART1) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_LEUART1_TX_PORT(initData->portLocationTx);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_LEUART1_RX_PORT(initData->portLocationRx);
    handle->txPin   = AF_LEUART1_TX_PIN(initData->portLocationTx);
    handle->rxPin   = AF_LEUART1_RX_PIN(initData->portLocationRx);
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

#elif defined(_USART_ROUTE_MASK)
  if (false) {
#if defined(LEUART0)
  } else if (handle->peripheral.leuart == LEUART0) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_LEUART0_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_LEUART0_RX_PORT(initData->portLocation);
    handle->txPin   = AF_LEUART0_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_LEUART0_RX_PIN(initData->portLocation);
#endif
#if defined(LEUART1)
  } else if (handle->peripheral.leuart == LEUART1) {
    handle->txPort  = (GPIO_Port_TypeDef)AF_LEUART1_TX_PORT(initData->portLocation);
    handle->rxPort  = (GPIO_Port_TypeDef)AF_LEUART1_RX_PORT(initData->portLocation);
    handle->txPin   = AF_LEUART1_TX_PIN(initData->portLocation);
    handle->rxPin   = AF_LEUART1_RX_PIN(initData->portLocation);
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

#elif defined(_GPIO_USART_ROUTEEN_MASK)
  handle->txPort  = initData->txPort;
  handle->txPin   = initData->txPin;
  handle->rxPort  = initData->rxPort;
  handle->rxPin   = initData->rxPin;
#endif

  handle->ctsPort = initData->ctsPort;
  handle->ctsPin  = initData->ctsPin;
  handle->rtsPort = initData->rtsPort;
  handle->rtsPin  = initData->rtsPin;

  return ECODE_EMDRV_UARTDRV_OK;
}
#endif

#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
/***************************************************************************//**
 * @brief Store EUART GPIO pins into handle.
 ******************************************************************************/
static Ecode_t SetupGpioEuart(UARTDRV_Handle_t handle,
                              const UARTDRV_InitEuart_t * initData)
{
  handle->txPort  = initData->txPort;
  handle->txPin   = initData->txPin;
  handle->rxPort  = initData->rxPort;
  handle->rxPin   = initData->rxPin;

  handle->ctsPort = initData->ctsPort;
  handle->ctsPin  = initData->ctsPin;
  handle->rtsPort = initData->rtsPort;
  handle->rtsPin  = initData->rtsPin;

  return ECODE_EMDRV_UARTDRV_OK;
}
#endif

/***************************************************************************//**
 * @brief Configure/deconfigure U(S)ART GPIO pins.
 ******************************************************************************/
static Ecode_t ConfigGpio(UARTDRV_Handle_t handle, bool enable)
{
  if (enable) {
    GPIO_PinModeSet(handle->txPort, handle->txPin, gpioModePushPull, 1);
    GPIO_PinModeSet(handle->rxPort, handle->rxPin, gpioModeInputPull, 1);
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
    if (handle->fcType == uartdrvFlowControlHw) {
      GPIO_PinModeSet(handle->ctsPort, handle->ctsPin, gpioModeInput, 0);
      GPIO_PinModeSet(handle->rtsPort, handle->rtsPin, gpioModePushPull, 0);
      GPIO_ExtIntConfig(handle->ctsPort, handle->ctsPin, handle->ctsPin, true, true, true);
    } else if (handle->fcType == uartdrvFlowControlHwUart) {
      GPIO_PinModeSet(handle->ctsPort, handle->ctsPin, gpioModeInput, 0);
      GPIO_PinModeSet(handle->rtsPort, handle->rtsPin, gpioModePushPull, 0);
    }
#endif
  } else {
    GPIO_PinModeSet(handle->txPort, handle->txPin, gpioModeDisabled, 0);
    GPIO_PinModeSet(handle->rxPort, handle->rxPin, gpioModeDisabled, 0);
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
    if (handle->fcType == uartdrvFlowControlHw) {
      GPIO_PinModeSet(handle->ctsPort, handle->ctsPin, gpioModeDisabled, 0);
      GPIO_PinModeSet(handle->rtsPort, handle->rtsPin, gpioModeDisabled, 0);
      GPIO_ExtIntConfig(handle->ctsPort, handle->ctsPin, handle->ctsPin, true, true, false);
    } else if (handle->fcType == uartdrvFlowControlHwUart) {
      GPIO_PinModeSet(handle->ctsPort, handle->ctsPin, gpioModeDisabled, 0);
      GPIO_PinModeSet(handle->rtsPort, handle->rtsPin, gpioModeDisabled, 0);
    }
#endif
  }
  return ECODE_EMDRV_UARTDRV_OK;
}

#if (HANDLES_ARE_AVAILABLE)
/***************************************************************************//**
 * @brief Add handle to handle array.
 ******************************************************************************/
static Ecode_t addHandle(UARTDRV_Handle_t handle)
{
  bool handleIsSet;
  uint32_t handleIdx;

  // Set handler pointer in handler array
  if (!uartdrvHandleIsInitialized) {
    for (handleIdx = 0; handleIdx < EMDRV_UARTDRV_MAX_DRIVER_INSTANCES; handleIdx++) {
      uartdrvHandle[handleIdx] = NULL;
    }
    uartdrvHandleIsInitialized = true;
  }

  // Check if its already in the array
  for (handleIdx = 0; handleIdx < EMDRV_UARTDRV_MAX_DRIVER_INSTANCES; handleIdx++) {
    if (uartdrvHandle[handleIdx] == handle) {
      return ECODE_EMDRV_UARTDRV_OK;
    }
  }

  // Insert handle
  handleIsSet = false;
  for (handleIdx = 0; handleIdx < EMDRV_UARTDRV_MAX_DRIVER_INSTANCES; handleIdx++) {
    if (uartdrvHandle[handleIdx] == NULL) {
      uartdrvHandle[handleIdx] = handle;
      handleIsSet = true;
      break;
    }
  }

  if (!handleIsSet) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief Remove handle from handle array.
 ******************************************************************************/
static Ecode_t removeHandle(UARTDRV_Handle_t handle)
{
  uint32_t handleIdx;

  if (uartdrvHandleIsInitialized) {
    for (handleIdx = 0; handleIdx < EMDRV_UARTDRV_MAX_DRIVER_INSTANCES; handleIdx++) {
      if (uartdrvHandle[handleIdx] == handle) {
        uartdrvHandle[handleIdx] = NULL;
        break;
      }
    }
  }
  return ECODE_EMDRV_UARTDRV_OK;
}
#endif

/***************************************************************************//**
 * @brief Initialize FIFO queues for a handle.
 ******************************************************************************/
static void InitializeQueues(UARTDRV_Handle_t handle,
                             UARTDRV_Buffer_FifoQueue_t * rxQueue,
                             UARTDRV_Buffer_FifoQueue_t * txQueue)
{
  handle->rxQueue = rxQueue;
  handle->rxQueue->head = 0;
  handle->rxQueue->tail = 0;
  handle->rxQueue->used = 0;
  handle->rxDmaActive = false;

  handle->txQueue = txQueue;
  handle->txQueue->head = 0;
  handle->txQueue->tail = 0;
  handle->txQueue->used = 0;
  handle->txDmaActive = false;

  handle->IgnoreRestrain = false;
}

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
/***************************************************************************//**
 * @brief Initialize GPIO-driven flow control.
 ******************************************************************************/
static void InitializeGpioFlowControl(UARTDRV_Handle_t handle)
{
  GPIOINT_CallbackRegister(handle->ctsPin, HwFcManageClearToSend);
  GPIOINT_Init();
  handle->fcPeerState = uartdrvFlowControlOn;
  handle->fcSelfState = uartdrvFlowControlOn;
  handle->fcSelfCfg = uartdrvFlowControlAuto;
  FcApplyState(handle);
}
#endif

/***************************************************************************//**
 * @brief Initialize DMA channels for a handle.
 ******************************************************************************/
static Ecode_t InitializeDma(UARTDRV_Handle_t handle)
{
  // Initialize DMA.
  DMADRV_Init();

  if (DMADRV_AllocateChannel(&handle->txDmaCh, NULL) != ECODE_EMDRV_DMADRV_OK) {
    return ECODE_EMDRV_UARTDRV_DMA_ALLOC_ERROR;
  }

  if (DMADRV_AllocateChannel(&handle->rxDmaCh, NULL) != ECODE_EMDRV_DMADRV_OK) {
    return ECODE_EMDRV_UARTDRV_DMA_ALLOC_ERROR;
  }

  return ECODE_EMDRV_UARTDRV_OK;
}
/// @endcond

#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
/***************************************************************************//**
 * @brief
 *    Initialize a U(S)ART driver instance.
 *
 * @param[out] handle  Pointer to a UARTDRV handle, refer to @ref
 *                     UARTDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure,
 *                     refer to @ref UARTDRV_InitUart_t.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success. On failure an appropriate
 *    UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_InitUart(UARTDRV_Handle_t handle,
                         const UARTDRV_InitUart_t *initData)
{
  Ecode_t retVal;
  CORE_DECLARE_IRQ_STATE;
  USART_InitAsync_TypeDef usartInit = USART_INITASYNC_DEFAULT;

  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }
  if (initData == NULL) {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

  memset(handle, 0, sizeof(UARTDRV_HandleData_t));
  emRequestInit(handle);

#if (HANDLES_ARE_AVAILABLE)
  retVal = addHandle(handle);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
#endif

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  handle->fcType = initData->fcType;
#else
  // Force init data to uartdrvFlowControlNone if flow control is excluded by EMDRV_UARTDRV_FLOW_CONTROL_ENABLE
  handle->fcType = uartdrvFlowControlNone;
#endif

  handle->peripheral.uart = initData->port;
  handle->type = uartdrvUartTypeUart;
#if defined(_GPIO_USART_ROUTEEN_MASK)
  handle->uartNum = initData->uartNum;
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->sleep = SL_POWER_MANAGER_IGNORE;
#endif

  // Set clocks and DMA requests according to available peripherals
  if (false) {
#if defined(USART0)
  } else if (initData->port == USART0) {
    handle->uartClock   = cmuClock_USART0;
    handle->txDmaSignal = dmadrvPeripheralSignal_USART0_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_USART0_RXDATAV;
#endif
#if defined(USART1)
  } else if (initData->port == USART1) {
    handle->uartClock   = cmuClock_USART1;
    handle->txDmaSignal = dmadrvPeripheralSignal_USART1_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_USART1_RXDATAV;
#endif
#if defined(USART2)
  } else if (initData->port == USART2) {
    handle->uartClock   = cmuClock_USART2;
    handle->txDmaSignal = dmadrvPeripheralSignal_USART2_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_USART2_RXDATAV;
#endif
#if defined(USART3)
  } else if (initData->port == USART3) {
    handle->uartClock   = cmuClock_USART3;
    handle->txDmaSignal = dmadrvPeripheralSignal_USART3_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_USART3_RXDATAV;
#endif
#if defined(USART4)
  } else if (initData->port == USART4) {
    handle->uartClock   = cmuClock_USART4;
    handle->txDmaSignal = dmadrvPeripheralSignal_USART4_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_USART4_RXDATAV;
#endif
#if defined(USART5)
  } else if (initData->port == USART5) {
    handle->uartClock   = cmuClock_USART5;
    handle->txDmaSignal = dmadrvPeripheralSignal_USART5_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_USART5_RXDATAV;
#endif
#if defined(UART0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (initData->port == UART0) {
    handle->uartClock   = cmuClock_UART0;
    handle->txDmaSignal = dmadrvPeripheralSignal_UART0_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_UART0_RXDATAV;
#endif
#if defined(UART1)
  } else if (initData->port == UART1) {
    handle->uartClock   = cmuClock_UART1;
    handle->txDmaSignal = dmadrvPeripheralSignal_UART1_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_UART1_RXDATAV;
#endif
#if defined(UART2)
  } else if (initData->port == UART2) {
    handle->uartClock   = cmuClock_UART2;
    handle->txDmaSignal = dmadrvPeripheralSignal_UART2_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_UART2_RXDATAV;
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

  InitializeQueues(handle, initData->rxQueue, initData->txQueue);

  usartInit.baudrate = initData->baudRate;
  usartInit.stopbits = initData->stopBits;
  usartInit.parity = initData->parity;
  usartInit.oversampling = initData->oversampling;
#if defined(USART_CTRL_MVDIS)
  usartInit.mvdis = initData->mvdis;
#endif

  // UARTDRV is fixed at 8 bit frames.
  usartInit.databits = (USART_Databits_TypeDef)USART_FRAME_DATABITS_EIGHT;

  // Enable clocks
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(handle->uartClock, true);

  // Init U(S)ART to default async config.
  // Rx/Tx enable is done on demand
  usartInit.enable = usartDisable;
  USART_InitAsync(initData->port, &usartInit);

#if defined(USART_ROUTEPEN_TXPEN)
  initData->port->ROUTELOC0 = (initData->port->ROUTELOC0
                               & ~(_USART_ROUTELOC0_TXLOC_MASK
                                   | _USART_ROUTELOC0_RXLOC_MASK))
                              | (initData->portLocationTx
                                 << _USART_ROUTELOC0_TXLOC_SHIFT)
                              | (initData->portLocationRx
                                 << _USART_ROUTELOC0_RXLOC_SHIFT);

  initData->port->ROUTEPEN = USART_ROUTEPEN_TXPEN
                             | USART_ROUTEPEN_RXPEN;
#elif defined(USART_ROUTE_TXPEN)
  initData->port->ROUTE = USART_ROUTE_TXPEN
                          | USART_ROUTE_RXPEN
                          | (initData->portLocation
                             << _USART_ROUTE_LOCATION_SHIFT);
#elif defined(GPIO_USART_ROUTEEN_TXPEN)
  GPIO->USARTROUTE[initData->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
                                                | GPIO_USART_ROUTEEN_RXPEN;
  GPIO->USARTROUTE[initData->uartNum].TXROUTE =
    (initData->txPort << _GPIO_USART_TXROUTE_PORT_SHIFT)
    | (initData->txPin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[initData->uartNum].RXROUTE =
    (initData->rxPort << _GPIO_USART_RXROUTE_PORT_SHIFT)
    | (initData->rxPin << _GPIO_USART_RXROUTE_PIN_SHIFT);
#endif

  if ((retVal = SetupGpioUart(handle, initData)) != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  if ((retVal = ConfigGpio(handle, true)) != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  CORE_ENTER_ATOMIC();

  // Configure hardware flow control pins and interrupt vectors
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  if (initData->fcType == uartdrvFlowControlHwUart) {
#if defined(UART_HW_FLOW_CONTROL_SUPPORT)
#if defined(_USART_ROUTEPEN_RTSPEN_MASK) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
    initData->port->ROUTELOC1 = (initData->portLocationCts << _USART_ROUTELOC1_CTSLOC_SHIFT)
                                | (initData->portLocationRts << _USART_ROUTELOC1_RTSLOC_SHIFT);
    initData->port->CTRLX    |= USART_CTRLX_CTSEN;
    initData->port->ROUTEPEN |= USART_ROUTEPEN_CTSPEN | USART_ROUTEPEN_RTSPEN;
#elif defined(USART_CTRLX_CTSEN)
    GPIO->USARTROUTE_SET[initData->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_RTSPEN;
    initData->port->CTRLX_SET = USART_CTRLX_CTSEN;
    GPIO->USARTROUTE_SET[initData->uartNum].RTSROUTE =
      (initData->rtsPort << _GPIO_USART_RTSROUTE_PORT_SHIFT)
      | (initData->rtsPin << _GPIO_USART_RTSROUTE_PIN_SHIFT);
    GPIO->USARTROUTE_SET[initData->uartNum].CTSROUTE =
      (initData->ctsPort << _GPIO_USART_CTSROUTE_PORT_SHIFT)
      | (initData->ctsPin << _GPIO_USART_CTSROUTE_PIN_SHIFT);
#endif
#else
    // Attempting to use USART hardware flow control on a device that does not
    // support it.
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
#endif
  } else if (initData->fcType == uartdrvFlowControlHw) {
    InitializeGpioFlowControl(handle);
  }
#endif

  // Clear any false IRQ/DMA request
  USART_IntClear(initData->port, ~0x0);

  // Enable Tx permanently as the Tx circuit consumes very little energy.
  // Rx is enabled on demand as the Rx circuit consumes some energy due to
  // continuous (over)sampling.
  if (initData->fcType == uartdrvFlowControlHwUart) {
    // Rx must be enabled permanently when using USART hw flow control
    USART_Enable(initData->port, usartEnable);
  } else {
    USART_Enable(initData->port, usartEnableTx);
  }

  // Discard false frames and/or IRQs
  initData->port->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

  // Initialize DMA.
  retVal = InitializeDma(handle);

  CORE_EXIT_ATOMIC();

  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  return ECODE_EMDRV_UARTDRV_OK;
}
#endif  // UART || USART

#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *    Initialize a LEUART driver instance.
 *
 * @param[out] handle  Pointer to a UARTDRV handle, refer to @ref
 *                     UARTDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure,
 *                     refer to @ref UARTDRV_InitLeuart_t.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success. On failure, an appropriate
 *    UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_InitLeuart(UARTDRV_Handle_t handle,
                           const UARTDRV_InitLeuart_t *initData)
{
  Ecode_t retVal;
  CORE_DECLARE_IRQ_STATE;
  LEUART_Init_TypeDef leuartInit = LEUART_INIT_DEFAULT;

  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }
  if (initData == NULL) {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }
  if (initData->fcType == uartdrvFlowControlHwUart) {
    // LEUART doesn't support peripheral hw flow control
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

  memset(handle, 0, sizeof(UARTDRV_HandleData_t));

#if (HANDLES_ARE_AVAILABLE)
  retVal = addHandle(handle);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
#endif

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  handle->fcType = initData->fcType;
#else
  // Force init data to uartdrvFlowControlNone if flow control is excluded by EMDRV_UARTDRV_FLOW_CONTROL_ENABLE
  handle->fcType = uartdrvFlowControlNone;
#endif

  handle->peripheral.leuart = initData->port;
  handle->type = uartdrvUartTypeLeuart;
#if defined(_GPIO_USART_ROUTEEN_MASK)
  handle->uartNum = initData->uartNum;
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->sleep = SL_POWER_MANAGER_IGNORE;
#endif

  // Set clocks and DMA requests according to available peripherals
  if (false) {
#if defined(LEUART0)
  } else if (initData->port == LEUART0) {
    handle->uartClock   = cmuClock_LEUART0;
    handle->txDmaSignal = dmadrvPeripheralSignal_LEUART0_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_LEUART0_RXDATAV;
#endif
#if defined(LEUART1)
  } else if (initData->port == LEUART1) {
    handle->uartClock   = cmuClock_LEUART1;
    handle->txDmaSignal = dmadrvPeripheralSignal_LEUART1_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_LEUART1_RXDATAV;
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

  InitializeQueues(handle, initData->rxQueue, initData->txQueue);

  leuartInit.baudrate   = initData->baudRate;
  leuartInit.stopbits   = initData->stopBits;
  leuartInit.parity     = initData->parity;

  // UARTDRV is fixed at 8 bit frames.
  leuartInit.databits = (LEUART_Databits_TypeDef)LEUART_CTRL_DATABITS_EIGHT;

  // Enable clocks
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_ClockEnable(cmuClock_GPIO, true);
#if defined(_CMU_HFBUSCLKEN0_MASK) || defined(CMU_HFCORECLKEN0_LE)
  CMU_ClockEnable(cmuClock_HFLE, true);
#endif

  // Only try to use LF clock if LFXO or LFRCO is enabled and requested baudrate is low
  if (CMU->STATUS & CMU_STATUS_LFXOENS
      && (leuartInit.baudrate <= SystemLFXOClockGet())) {
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  } else if (CMU->STATUS & CMU_STATUS_LFRCOENS
             && (leuartInit.baudrate <= SystemLFRCOClockGet())) {
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
  } else {
    // Try to figure out the prescaler that will give us the best stability
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_HFCLKLE);

#if defined(_SILICON_LABS_32B_SERIES_0)
    // Attainable baudrate lies between refclk and refclk/128. For maximum
    // accuracy, we want the reference clock to be as high as possible.
    uint32_t refclk = CMU_ClockFreqGet(cmuClock_LFB);
    uint8_t divisor = 0;

    while (leuartInit.baudrate <= (refclk >> (divisor + 7))) {
      divisor++;
    }

    // If we ran out of stretch on the clock division, error out.
    if (divisor > (_CMU_LFBPRESC0_LEUART0_MASK >> _CMU_LFBPRESC0_LEUART0_SHIFT)) {
      return ECODE_EMDRV_UARTDRV_CLOCK_ERROR;
    }
    CMU_ClockDivSet(handle->uartClock, (CMU_ClkDiv_TypeDef) (1 << divisor));
#elif defined(_SILICON_LABS_32B_SERIES_1)
    // Clock divider in LEUARTn is large enough for any baudrate.
#endif
  }

  CMU_ClockEnable(handle->uartClock, true);

  // Init LEUART to default async config.
  // Rx/Tx enable is done on demand
  leuartInit.enable = leuartDisable;
  LEUART_Init(initData->port, &leuartInit);

#if defined(LEUART_ROUTEPEN_TXPEN)
  initData->port->ROUTELOC0 = (initData->port->ROUTELOC0
                               & ~(_LEUART_ROUTELOC0_TXLOC_MASK
                                   | _LEUART_ROUTELOC0_RXLOC_MASK))
                              | (initData->portLocationTx
                                 << _LEUART_ROUTELOC0_TXLOC_SHIFT)
                              | (initData->portLocationRx
                                 << _LEUART_ROUTELOC0_RXLOC_SHIFT);

  initData->port->ROUTEPEN = LEUART_ROUTEPEN_TXPEN
                             | LEUART_ROUTEPEN_RXPEN;
#else
  initData->port->ROUTE = LEUART_ROUTE_TXPEN
                          | LEUART_ROUTE_RXPEN
                          | (initData->portLocation
                             << _LEUART_ROUTE_LOCATION_SHIFT);
#endif

  if ((retVal = SetupGpioLeuart(handle, initData)) != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  if ((retVal = ConfigGpio(handle, true)) != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  CORE_ENTER_ATOMIC();

  // Configure hardware flow control pins and interrupt vectors
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  if (initData->fcType == uartdrvFlowControlHw) {
    InitializeGpioFlowControl(handle);
  }
#endif

  // Clear any false IRQ/DMA request
  LEUART_IntClear(initData->port, ~0x0);

  // Enable Tx permanently as the Tx circuit consumes very little energy.
  // Rx is enabled on demand as the Rx circuit consumes some energy due to
  // continuous (over)sampling.
  LEUART_Enable(initData->port, leuartEnableTx);

  // Wait for previous write (TXEN) to sync before clearing FIFOs
  while ((initData->port->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
  }

  // Discard false frames and/or IRQs
  initData->port->CMD = LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX;
  initData->port->CTRL |= LEUART_CTRL_RXDMAWU;

  // Initialize DMA.
  retVal = InitializeDma(handle);

  CORE_EXIT_ATOMIC();

  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  // Wait for everything to be synchronized
  while (initData->port->SYNCBUSY != 0U) {
  }
  return ECODE_EMDRV_UARTDRV_OK;
}
#endif

#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0)) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *    Initialize a EUART driver instance.
 *
 * @param[out] handle  Pointer to a UARTDRV handle, refer to @ref
 *                     UARTDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure,
 *                     refer to @ref UARTDRV_InitEuart_t.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success. On failure an appropriate
 *    UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_InitEuart(UARTDRV_Handle_t handle,
                          const UARTDRV_InitEuart_t *initData)
{
  Ecode_t retVal;
  CORE_DECLARE_IRQ_STATE;
  EUSART_UartInit_TypeDef uartInit = EUSART_UART_INIT_DEFAULT_HF;
  EUSART_AdvancedInit_TypeDef uartAdvancedInit = EUSART_ADVANCED_INIT_DEFAULT;

  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }
  if (initData == NULL) {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

  memset(handle, 0, sizeof(UARTDRV_HandleData_t));

  handle->peripheral.euart = initData->port;
  handle->type = uartdrvUartTypeEuart;
  handle->uartNum = initData->uartNum;

#if (HANDLES_ARE_AVAILABLE)
  retVal = addHandle(handle);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
#endif

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  handle->fcType = initData->fcType;
#else
  // Force init data to uartdrvFlowControlNone if flow control is excluded by EMDRV_UARTDRV_FLOW_CONTROL_ENABLE
  handle->fcType = uartdrvFlowControlNone;
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->sleep = SL_POWER_MANAGER_IGNORE;
#endif

  // Set clocks and DMA requests according to available peripherals
  if (false) {
#if defined(EUART0)
  } else if (initData->port == EUART0) {
    handle->uartClock   = cmuClock_EUART0;
    handle->txDmaSignal = dmadrvPeripheralSignal_EUART0_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_EUART0_RXDATAV;
    uartAdvancedInit.dmaWakeUpOnRx = true;
    uartAdvancedInit.dmaWakeUpOnTx = false;
    handle->txDmaActive = false;
    handle->rxDmaActive = false;
#endif
#if defined(EUSART0)
  } else if (initData->port == EUSART0) {
    handle->uartClock   = cmuClock_EUSART0;
    handle->txDmaSignal = dmadrvPeripheralSignal_EUSART0_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_EUSART0_RXDATAV;
    uartAdvancedInit.dmaWakeUpOnRx = true;
    uartAdvancedInit.dmaWakeUpOnTx = false;
    handle->txDmaActive = false;
    handle->rxDmaActive = false;
#endif
#if defined(EUSART1)
  } else if (initData->port == EUSART1) {
    handle->uartClock   = cmuClock_EUSART1;
    handle->txDmaSignal = dmadrvPeripheralSignal_EUSART1_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_EUSART1_RXDATAV;
    uartAdvancedInit.dmaWakeUpOnRx = true;
    uartAdvancedInit.dmaWakeUpOnTx = false;
    handle->txDmaActive = false;
    handle->rxDmaActive = false;
#endif
#if defined(EUSART2)
  } else if (initData->port == EUSART2) {
    handle->uartClock   = cmuClock_EUSART2;
    handle->txDmaSignal = dmadrvPeripheralSignal_EUSART2_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_EUSART2_RXDATAV;
    uartAdvancedInit.dmaWakeUpOnRx = true;
    uartAdvancedInit.dmaWakeUpOnTx = false;
    handle->txDmaActive = false;
    handle->rxDmaActive = false;
#endif
#if defined(EUSART3)
  } else if (initData->port == EUSART3) {
    handle->uartClock   = cmuClock_EUSART3;
    handle->txDmaSignal = dmadrvPeripheralSignal_EUSART3_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_EUSART3_RXDATAV;
    uartAdvancedInit.dmaWakeUpOnRx = true;
    uartAdvancedInit.dmaWakeUpOnTx = false;
    handle->txDmaActive = false;
    handle->rxDmaActive = false;
#endif
#if defined(EUSART4)
  } else if (initData->port == EUSART4) {
    handle->uartClock   = cmuClock_EUSART4;
    handle->txDmaSignal = dmadrvPeripheralSignal_EUSART4_TXBL;
    handle->rxDmaSignal = dmadrvPeripheralSignal_EUSART4_RXDATAV;
    uartAdvancedInit.dmaWakeUpOnRx = true;
    uartAdvancedInit.dmaWakeUpOnTx = false;
    handle->txDmaActive = false;
    handle->rxDmaActive = false;
#endif
  } else {
    return ECODE_EMDRV_UARTDRV_PARAM_ERROR;
  }

  InitializeQueues(handle, initData->rxQueue, initData->txQueue);

  // UARTDRV is fixed at 8 bit frames.
  uartInit.databits = (EUSART_Databits_TypeDef)eusartDataBits8;

  uartInit.baudrate = initData->baudRate;
  uartInit.stopbits = initData->stopBits;
  uartInit.parity = initData->parity;
  uartInit.oversampling = initData->oversampling;
  uartInit.majorityVote = initData->mvdis;

  uartInit.advancedSettings = &uartAdvancedInit;

  // Enable clocks.
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(handle->uartClock, true);
  if (initData->useLowFrequencyMode) {
    CMU_ClockEnable(cmuClock_LFRCO, true);
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2) \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)
    CMU_ClockEnable(cmuClock_EM23GRPACLK, true);
    CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFRCO);
    CMU_ClockSelectSet(handle->uartClock, cmuSelect_EM23GRPACLK);
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)  \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4) \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
    CMU_ClockSelectSet(handle->uartClock, cmuSelect_LFRCO);
#else
  #error "Please assign a LF clock to EUSART instance"
#endif
  } else {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2) \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)
    CMU_ClockEnable(cmuClock_EM01GRPACLK, true);
    CMU_ClockSelectSet(handle->uartClock, cmuSelect_EM01GRPACLK);
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)  \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4) \
    || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
    if (handle->uartClock == cmuClock_EUSART0) {
      CMU_ClockSelectSet(handle->uartClock, cmuSelect_EM01GRPCCLK);
    }
#else
  #error "Please assign a HF clock to EUSART instance"
#endif
  }

#if defined(EUART_COUNT) && (EUART_COUNT > 0)
  GPIO->EUARTROUTE->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
  GPIO->EUARTROUTE->TXROUTE = (initData->txPort
                               << _GPIO_EUART_TXROUTE_PORT_SHIFT)
                              | (initData->txPin << _GPIO_EUART_TXROUTE_PIN_SHIFT);
  GPIO->EUARTROUTE->RXROUTE = (initData->rxPort
                               << _GPIO_EUART_RXROUTE_PORT_SHIFT)
                              | (initData->rxPin << _GPIO_EUART_RXROUTE_PIN_SHIFT);
#elif defined(EUSART_COUNT) && (EUSART_COUNT > 0)
  GPIO->EUSARTROUTE[initData->uartNum].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;
  GPIO->EUSARTROUTE[initData->uartNum].TXROUTE = (initData->txPort
                                                  << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
                                                 | (initData->txPin << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[initData->uartNum].RXROUTE = (initData->rxPort
                                                  << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
                                                 | (initData->rxPin << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
  #endif

  if ((retVal = SetupGpioEuart(handle, initData)) != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  if ((retVal = ConfigGpio(handle, true)) != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  CORE_ENTER_ATOMIC();

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  if (initData->fcType == uartdrvFlowControlHwUart) {
#if defined(EUART_PRESENT)
    GPIO->EUARTROUTE_SET->ROUTEEN = GPIO_EUART_ROUTEEN_RTSPEN;
    GPIO->EUARTROUTE_SET[0].RTSROUTE =
      (initData->rtsPort << _GPIO_EUART_RTSROUTE_PORT_SHIFT)
      | (initData->rtsPin << _GPIO_EUART_RTSROUTE_PIN_SHIFT);
    GPIO->EUARTROUTE_SET[0].CTSROUTE =
      (initData->ctsPort << _GPIO_EUART_CTSROUTE_PORT_SHIFT)
      | (initData->ctsPin << _GPIO_EUART_CTSROUTE_PIN_SHIFT);
#elif defined(EUSART_PRESENT)
    GPIO->EUSARTROUTE_SET[initData->uartNum].ROUTEEN = GPIO_EUSART_ROUTEEN_RTSPEN;
    GPIO->EUSARTROUTE_SET[initData->uartNum].RTSROUTE =
      (initData->rtsPort << _GPIO_EUSART_RTSROUTE_PORT_SHIFT)
      | (initData->rtsPin << _GPIO_EUSART_RTSROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE_SET[initData->uartNum].CTSROUTE =
      (initData->ctsPort << _GPIO_EUSART_CTSROUTE_PORT_SHIFT)
      | (initData->ctsPin << _GPIO_EUSART_CTSROUTE_PIN_SHIFT);
#endif
    uartAdvancedInit.hwFlowControl = eusartHwFlowControlCtsAndRts;
  } else if (initData->fcType == uartdrvFlowControlHw) {
    InitializeGpioFlowControl(handle);
  }
#endif

  // Set Tx enable to follow USART implementation.
  uartInit.enable = eusartEnableTx;

  if (initData->useLowFrequencyMode) {
    EUSART_UartInitLf(initData->port, &uartInit);
  } else {
    EUSART_UartInitHf(initData->port, &uartInit);
  }

  // Initialize DMA.
  retVal = InitializeDma(handle);

  CORE_EXIT_ATOMIC();

  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  return ECODE_EMDRV_UARTDRV_OK;
}
#endif

/***************************************************************************//**
 * @brief
 *    Deinitialize a UART driver instance.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success. On failure, an appropriate
 *    UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_DeInit(UARTDRV_Handle_t handle)
{
  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }

  // Stop DMA transfers.
  UARTDRV_Abort(handle, uartdrvAbortAll);

  // Do not leave any peer restrained on DeInit
  UARTDRV_FlowControlSet(handle, uartdrvFlowControlOn);

  ConfigGpio(handle, false);

#if (HANDLES_ARE_AVAILABLE)
  removeHandle(handle);
#endif

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    handle->peripheral.uart->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS;
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    LEUART_Reset(handle->peripheral.leuart);
    while ((handle->peripheral.leuart->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
    }
    handle->peripheral.leuart->CMD = LEUART_CMD_RXDIS | LEUART_CMD_TXDIS;

#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    EUSART_Reset(handle->peripheral.euart);
#endif
  }

  CMU_ClockEnable(handle->uartClock, false);

#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  if (handle->fcType == uartdrvFlowControlHw) {
    GPIOINT_CallbackRegister(handle->ctsPin, NULL);
  }
#endif

  DMADRV_FreeChannel(handle->txDmaCh);
  DMADRV_FreeChannel(handle->rxDmaCh);
  DMADRV_DeInit();

  handle->rxQueue->head = 0;
  handle->rxQueue->tail = 0;
  handle->rxQueue->used = 0;

  handle->txQueue->head = 0;
  handle->txQueue->tail = 0;
  handle->txQueue->used = 0;

  emRequestDeinit(handle);

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Abort ongoing UART transfers.
 *
 * @details
 *    All ongoing or queued operations of the given abort type will be aborted.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] type   Abort type -- whether to abort only Tx, only Rx, or both.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success, @ref ECODE_EMDRV_UARTDRV_IDLE if
 *    the UART is idle. On failure, an appropriate UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_Abort(UARTDRV_Handle_t handle, UARTDRV_AbortType_t type)
{
  UARTDRV_Buffer_t *rxBuffer, *txBuffer;
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if ((type == uartdrvAbortTransmit) && (handle->txQueue->used == 0)) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_IDLE;
  } else if ((type == uartdrvAbortReceive) && (handle->rxQueue->used == 0)) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_IDLE;
  } else if ((type == uartdrvAbortAll)
             && (handle->txQueue->used == 0)
             && (handle->rxQueue->used == 0)) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_IDLE;
  }

  // Stop DMA transfers.
  if ((type == uartdrvAbortTransmit) || (type == uartdrvAbortAll)) {
    // Stop the current transfer
    DMADRV_StopTransfer(handle->txDmaCh);
    handle->txDmaActive = false;

    if (handle->txQueue->used > 0) {
      // Update the transfer status of the active transfer
      GetTailBuffer(handle->txQueue, &txBuffer);
      DMADRV_TransferRemainingCount(handle->txDmaCh,
                                    (int*)&txBuffer->itemsRemaining);
      txBuffer->transferStatus = ECODE_EMDRV_UARTDRV_ABORTED;

      // Dequeue all transfers and call callback
      while (handle->txQueue->used > 0) {
        DequeueBuffer(handle->txQueue, &txBuffer);

        // Call the callback with ABORTED error code
        if (txBuffer->callback != NULL) {
          txBuffer->callback(handle,
                             ECODE_EMDRV_UARTDRV_ABORTED,
                             NULL,
                             txBuffer->itemsRemaining);
        }
      }
    }

    // Wait for peripheral to finish cleaning up, to prevent framing errors
    // on subsequent transfers
    while (!(UARTDRV_GetPeripheralStatus(handle) & UARTDRV_STATUS_TXIDLE)) {
    }
  }
  if ((type == uartdrvAbortReceive) || (type == uartdrvAbortAll)) {
    // Stop the current transfer
    DMADRV_StopTransfer(handle->rxDmaCh);
    handle->rxDmaActive = false;

    if (handle->rxQueue->used > 0) {
      // Update the transfer status of the active transfer
      GetTailBuffer(handle->rxQueue, &rxBuffer);
      DMADRV_TransferRemainingCount(handle->rxDmaCh,
                                    (int*)&rxBuffer->itemsRemaining);
      rxBuffer->transferStatus = ECODE_EMDRV_UARTDRV_ABORTED;

      // Dequeue all transfers and call callback
      while (handle->rxQueue->used > 0) {
        DequeueBuffer(handle->rxQueue, &rxBuffer);

        // Call the callback with ABORTED error code
        if (rxBuffer->callback != NULL) {
          rxBuffer->callback(handle,
                             ECODE_EMDRV_UARTDRV_ABORTED,
                             NULL,
                             rxBuffer->itemsRemaining);
        }
      }
    }

    // Disable the receiver
    if (handle->fcType != uartdrvFlowControlHwUart) {
      DisableReceiver(handle);
    }
  }

  if (handle->em1RequestCount > 0) {
    em1RequestRemove(handle);
  }

  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Return the status of the UART peripheral associated with a given handle.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    UART status value
 ******************************************************************************/
UARTDRV_Status_t UARTDRV_GetPeripheralStatus(UARTDRV_Handle_t handle)
{
  UARTDRV_Status_t status = 0;

#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  if (handle->type == uartdrvUartTypeUart) {
    status = handle->peripheral.uart->STATUS;
  } else if (handle->type == uartdrvUartTypeLeuart) {
    uint32_t reg = handle->peripheral.leuart->STATUS;
    if (reg & LEUART_STATUS_TXENS) {
      status |= UARTDRV_STATUS_TXEN;
    }
    if (reg & LEUART_STATUS_RXENS) {
      status |= UARTDRV_STATUS_RXEN;
    }
    if (reg & LEUART_STATUS_RXBLOCK) {
      status |= UARTDRV_STATUS_RXBLOCK;
    }
    if (reg & LEUART_STATUS_TXC) {
      status |= UARTDRV_STATUS_TXC;
    }
    if (reg & LEUART_STATUS_TXBL) {
      status |= UARTDRV_STATUS_TXBL;
    }
    if (reg & LEUART_STATUS_RXDATAV) {
      status |= UARTDRV_STATUS_RXDATAV;
    }
#if defined(LEUART_STATUS_TXIDLE)
    if (reg & LEUART_STATUS_TXIDLE) {
      status |= UARTDRV_STATUS_TXIDLE;
    }
#endif
  }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    status = handle->peripheral.uart->STATUS;
#endif
  } else if ((handle->type == uartdrvUartTypeEuart)) {
    status = EUSART_StatusGet(handle->peripheral.euart);
  }
#else
  status = handle->peripheral.uart->STATUS;
#endif

#if defined(_SILICON_LABS_32B_SERIES_0)
  // Series 0 devices does not have the TXIDLE flag, so we emulate it
  if ((status & UARTDRV_STATUS_TXC) || !(handle->hasTransmitted)) {
    status |= UARTDRV_STATUS_TXIDLE;
  }
#endif

  return status;
}

/***************************************************************************//**
 * @brief
 *    Return the number of queued receive operations.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    The number of queued operations.
 ******************************************************************************/
uint8_t UARTDRV_GetReceiveDepth(UARTDRV_Handle_t handle)
{
  return (uint8_t)handle->rxQueue->used;
}

/***************************************************************************//**
 * @brief
 *    Check the status of the UART and gather information about any ongoing
 *    receive operations.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[out] buffer Pointer to the current data buffer.
 *
 * @param[out] itemsReceived Current bytes received count.
 *
 * @param[out] itemsRemaining Current bytes remaining count.
 *
 * @return
 *    UART status.
 ******************************************************************************/
UARTDRV_Status_t UARTDRV_GetReceiveStatus(UARTDRV_Handle_t handle,
                                          uint8_t **buffer,
                                          UARTDRV_Count_t *itemsReceived,
                                          UARTDRV_Count_t *itemsRemaining)
{
  UARTDRV_Buffer_t *rxBuffer = NULL;
  Ecode_t retVal = ECODE_EMDRV_UARTDRV_OK;
  uint32_t remaining = 0;

  if (handle->rxQueue->used > 0) {
    retVal = GetTailBuffer(handle->rxQueue, &rxBuffer);
    DMADRV_TransferRemainingCount(handle->rxDmaCh,
                                  (int*)&remaining);
  }

  if (rxBuffer && (retVal == ECODE_EMDRV_UARTDRV_OK)) {
    *itemsReceived = rxBuffer->transferCount - remaining;
    *itemsRemaining = remaining;
    *buffer = rxBuffer->data;
  } else {
    *itemsRemaining = 0;
    *itemsReceived = 0;
    *buffer = NULL;
  }

  return UARTDRV_GetPeripheralStatus(handle);
}

/***************************************************************************//**
 * @brief
 *    Returns the number of queued transmit operations.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    The number of queued operations.
 ******************************************************************************/
uint8_t UARTDRV_GetTransmitDepth(UARTDRV_Handle_t handle)
{
  return (uint8_t)handle->txQueue->used;
}

/***************************************************************************//**
 * @brief
 *    Check the status of the UART and gather information about any ongoing
 *    transmit operations.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[out] buffer Pointer to the current data buffer.
 *
 * @param[out] itemsSent Current bytes sent count.
 *
 * @param[out] itemsRemaining Current bytes remaining count.
 *
 * @return
 *    UART status.
 ******************************************************************************/
UARTDRV_Status_t UARTDRV_GetTransmitStatus(UARTDRV_Handle_t handle,
                                           uint8_t **buffer,
                                           UARTDRV_Count_t *itemsSent,
                                           UARTDRV_Count_t *itemsRemaining)
{
  UARTDRV_Buffer_t *txBuffer = NULL;
  Ecode_t retVal = ECODE_EMDRV_UARTDRV_OK;
  uint32_t remaining = 0;

  if (handle->txQueue->used > 0) {
    retVal = GetTailBuffer(handle->txQueue, &txBuffer);
    DMADRV_TransferRemainingCount(handle->txDmaCh,
                                  (int*)&remaining);
  }

  if (txBuffer && (retVal == ECODE_EMDRV_UARTDRV_OK)) {
    *itemsSent = txBuffer->transferCount - remaining;
    *itemsRemaining = remaining;
    *buffer = txBuffer->data;
  } else {
    *itemsRemaining = 0;
    *itemsSent = 0;
    *buffer = NULL;
  }

  return UARTDRV_GetPeripheralStatus(handle);
}

/***************************************************************************//**
 * @brief
 *    Set UART flow control state. Set nRTS pin if hardware flow control
 *    is enabled. Send XON/XOFF if software flow control is enabled.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] state Flow control state.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_FlowControlSet(UARTDRV_Handle_t handle, UARTDRV_FlowControlState_t state)
{
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  if (handle->fcType == uartdrvFlowControlHwUart) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_OPERATION;
  }
  handle->fcSelfCfg = state;
  if (state != uartdrvFlowControlAuto) {
    handle->fcSelfState = state;
  }
  return FcApplyState(handle);
#else
  (void) handle;
  (void) state;
  return ECODE_EMDRV_UARTDRV_OK;
#endif
}

/***************************************************************************//**
 * @brief
 *    Set peer UART flow control state. Pause/resume transmit accordingly.
 *    Only for manual software flow control.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] state Flow control state.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_FlowControlSetPeerStatus(UARTDRV_Handle_t handle, UARTDRV_FlowControlState_t state)
{
  if ( (handle->fcType != uartdrvFlowControlSw)
       || (state == uartdrvFlowControlAuto) ) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_OPERATION;
  }

  if (handle->fcPeerState != state) {
    handle->fcPeerState = state;
    if (state == uartdrvFlowControlOn) {
      UARTDRV_ResumeTransmit(handle);
    } else if (state == uartdrvFlowControlOff) {
      UARTDRV_PauseTransmit(handle);
    }
  }

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Check the peer's flow control status.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    Returns uartdrvFlowControlOn if clear to send.
 ******************************************************************************/
UARTDRV_FlowControlState_t UARTDRV_FlowControlGetPeerStatus(UARTDRV_Handle_t handle)
{
  return handle->fcPeerState;
}

/***************************************************************************//**
 * @brief
 *    Check the self flow control status.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    Returns uartdrvFlowControlOn if requesting to send.
 ******************************************************************************/
UARTDRV_FlowControlState_t UARTDRV_FlowControlGetSelfStatus(UARTDRV_Handle_t handle)
{
  return handle->fcSelfState;
}

/***************************************************************************//**
 * @brief
 *    Enable transmission when restrained by flow control.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_FlowControlIgnoreRestrain(UARTDRV_Handle_t handle)
{
  handle->IgnoreRestrain = true;

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Direct receive without interrupts or callback. This is a blocking function.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] data pointer to buffer.
 *
 * @param[in] maxCount Maximum number of bytes to receive.
 *
 * @return
 *    Number of bytes received.
 ******************************************************************************/
UARTDRV_Count_t UARTDRV_ForceReceive(UARTDRV_Handle_t handle,
                                     uint8_t *data,
                                     UARTDRV_Count_t maxCount)
{
  Ecode_t retVal;
  uint32_t rxState;
  UARTDRV_Count_t i = 0;

  retVal = CheckParams(handle, data, maxCount);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return 0;
  }

  // Wait for DMA receive to complete and clear
  while (handle->rxQueue->used > 0) {
  }

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    rxState = (handle->peripheral.uart->STATUS & USART_STATUS_RXENS);
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    rxState = (handle->peripheral.leuart->STATUS & LEUART_STATUS_RXENS);
#endif
#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    rxState = (handle->peripheral.euart->STATUS & EUSART_STATUS_RXENS);
#endif
  } else {
    EFM_ASSERT(false);
    rxState = 0;
  }

  if (!rxState) {
    EnableReceiver(handle);
  }

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    while ((handle->peripheral.uart->STATUS & USART_STATUS_RXDATAV) != 0U) {
      *data = (uint8_t)handle->peripheral.uart->RXDATA;
      data++;
      i++;
      if (i >= maxCount) {
        break;
      }
    }
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    while ((handle->peripheral.leuart->STATUS & LEUART_STATUS_RXDATAV) != 0U) {
      *data = (uint8_t)handle->peripheral.leuart->RXDATA;
      data++;
      i++;
      if (i >= maxCount) {
        break;
      }
    }
#endif
#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    while ((handle->peripheral.euart->STATUS & EUSART_STATUS_RXFL) != 0U) {
      *data = (uint8_t)handle->peripheral.euart->RXDATA;
      data++;
      i++;
      if (i >= maxCount) {
        break;
      }
    }
#endif
  }

  data -= i;

  if (!rxState) {
    DisableReceiver(handle);
  }
  return i;
}

/***************************************************************************//**
 * @brief
 *    Direct transmit without interrupts or callback. This is a blocking function.
 *    that ignores flow control if enabled.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] data Pointer to the buffer.
 *
 * @param[in] count A number of bytes to transmit.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t  UARTDRV_ForceTransmit(UARTDRV_Handle_t handle,
                               uint8_t *data,
                               UARTDRV_Count_t count)
{
  Ecode_t retVal;
  uint32_t txState;
  bool callDmaIrqHandler;

  retVal = CheckParams(handle, data, count);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }

  // Wait for DMA transmit to complete and clear
  callDmaIrqHandler = CORE_IrqIsBlocked(UART_DMA_IRQ); // Loop invariant
  while ((handle->txQueue->used > 0) && (!handle->txDmaPaused)) {
    if (callDmaIrqHandler) {
      UART_DMA_IRQHANDLER();
    }
  }

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    txState = (handle->peripheral.leuart->STATUS & LEUART_STATUS_TXENS);
#endif
#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    txState = EUSART_StatusGet(handle->peripheral.euart) & EUSART_STATUS_TXENS;
#endif
  } else {
    EFM_ASSERT(false);
    txState = 0;
  }

  if (!txState) {
    EnableTransmitter(handle);
  }

  handle->hasTransmitted = true;

  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    while (count-- != 0U) {
      USART_Tx(handle->peripheral.uart, *data++);
    }
    // Wait for Tx completion
    while (!(handle->peripheral.uart->STATUS & USART_STATUS_TXC)) {
    }
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  } else if (handle->type == uartdrvUartTypeLeuart) {
    while (count-- != 0U) {
      LEUART_Tx(handle->peripheral.leuart, *data++);
    }
    // Wait for Tx completion
    while (!(handle->peripheral.leuart->STATUS & LEUART_STATUS_TXC)) {
    }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeEuart) {
    while (count-- != 0U) {
      EUSART_Tx(handle->peripheral.euart, *data++);
    }
    while (~EUSART_StatusGet(handle->peripheral.euart) & (EUSART_STATUS_TXC | EUSART_STATUS_TXIDLE)) {
    }
#endif
  }

  if (!txState) {
    DisableTransmitter(handle);
  }

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *  Pause an ongoing transmit operation.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *  @ref ECODE_EMDRV_UARTDRV_OK on success. On failure, an appropriate
 *  UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_PauseTransmit(UARTDRV_Handle_t handle)
{
  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }
  // Pause the transfer if 1) pause counter is 0
  //                       2) HW flow control hasn't already paused the DMA
  if ( (handle->txDmaPaused == 0) && (HwFcGetClearToSendPin(handle) == uartdrvFlowControlOn) ) {
    DMADRV_PauseTransfer(handle->txDmaCh);
  }
  // Increment counter to allow nested calls
  handle->txDmaPaused++;
  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start a non-blocking receive.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] data A receive data buffer.
 *
 * @param[in] count A number of bytes received.
 *
 * @param[in]  callback A transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_Receive(UARTDRV_Handle_t handle,
                        uint8_t *data,
                        UARTDRV_Count_t count,
                        UARTDRV_Callback_t callback)
{
  Ecode_t retVal;
  UARTDRV_Buffer_t outputBuffer;
  UARTDRV_Buffer_t *queueBuffer;

  retVal = CheckParams(handle, data, count);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  outputBuffer.data = data;
  outputBuffer.transferCount = count;
  outputBuffer.itemsRemaining = count;
  outputBuffer.callback = callback;
  outputBuffer.transferStatus = ECODE_EMDRV_UARTDRV_WAITING;

  retVal = EnqueueBuffer(handle->rxQueue, &outputBuffer, &queueBuffer);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  if (!(handle->rxDmaActive)) {
    EnableReceiver(handle);
    StartReceiveDma(handle, queueBuffer);
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
    if (handle->fcType != uartdrvFlowControlHwUart) {
      handle->fcSelfState = uartdrvFlowControlOn;
      FcApplyState(handle);
    }
#endif
  } // else: started by ReceiveDmaComplete

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start a blocking receive.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] data A receive data buffer.
 *
 * @param[in] count A number of bytes received.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_ReceiveB(UARTDRV_Handle_t handle,
                         uint8_t *data,
                         UARTDRV_Count_t count)
{
  Ecode_t retVal;
  UARTDRV_Buffer_t inputBuffer;
  UARTDRV_Buffer_t *queueBuffer;

  retVal = CheckParams(handle, data, count);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  inputBuffer.data = data;
  inputBuffer.transferCount = count;
  inputBuffer.itemsRemaining = count;
  inputBuffer.callback = NULL;
  inputBuffer.transferStatus = ECODE_EMDRV_UARTDRV_WAITING;

  retVal = EnqueueBuffer(handle->rxQueue, &inputBuffer, &queueBuffer);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  // Active wait, the system must be in EM0 or EM1 anyway
  while (handle->rxQueue->used > 1) {
  }
  EnableReceiver(handle);
#if (EMDRV_UARTDRV_FLOW_CONTROL_ENABLE)
  if (handle->fcType != uartdrvFlowControlHwUart) {
    handle->fcSelfState = uartdrvFlowControlOn;
    FcApplyState(handle);
  }
#endif
  StartReceiveDma(handle, queueBuffer);
  // Active wait, the system must be in EM0 or EM1 anyway
  while (handle->rxDmaActive) {
  }
  return queueBuffer->transferStatus;
}

/***************************************************************************//**
 * @brief
 *  Resume a paused transmit operation.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @return
 *  @ref ECODE_EMDRV_UARTDRV_OK on success. On failure, an appropriate
 *  UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t UARTDRV_ResumeTransmit(UARTDRV_Handle_t handle)
{
  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }

  if (handle->txDmaPaused > 0) {
    // Resume the transfer if 1) pause counter is 1
    //                        2) HW flow control doesn't need to pause the DMA
    if ( (handle->txDmaPaused == 1) && (HwFcGetClearToSendPin(handle) == uartdrvFlowControlOn) ) {
      DMADRV_ResumeTransfer(handle->txDmaCh);
    }
    handle->txDmaPaused--;
  } else {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_OPERATION;
  }
  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start a non-blocking transmit.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] data A transmit data buffer.
 *
 * @param[in] count A number of bytes to transmit.
 *
 * @param[in]  callback A transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_Transmit(UARTDRV_Handle_t handle,
                         uint8_t *data,
                         UARTDRV_Count_t count,
                         UARTDRV_Callback_t callback)
{
  Ecode_t retVal;
  UARTDRV_Buffer_t inputBuffer;
  UARTDRV_Buffer_t *queueBuffer;
  uint32_t txState;

  retVal = CheckParams(handle, data, count);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  inputBuffer.data = data;
  inputBuffer.transferCount = count;
  inputBuffer.itemsRemaining = count;
  inputBuffer.callback = callback;
  inputBuffer.transferStatus = ECODE_EMDRV_UARTDRV_WAITING;

#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif
  } else if (handle->type == uartdrvUartTypeLeuart) {
    txState = (handle->peripheral.leuart->STATUS & LEUART_STATUS_TXENS);
  } else {
    EFM_ASSERT(false);
    txState = 0;
  }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif
  } else if (handle->type == uartdrvUartTypeEuart) {
    txState = EUSART_StatusGet(handle->peripheral.euart) & EUSART_STATUS_TXENS;
  } else {
    EFM_ASSERT(false);
    txState = 0;
  }
#else
  txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif

  if (!txState) {
    EnableTransmitter(handle);
  }

  retVal = EnqueueBuffer(handle->txQueue, &inputBuffer, &queueBuffer);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  if (!(handle->txDmaActive)) {
    CORE_ATOMIC_SECTION(
      if (handle->txQueue->used > 0) {
      StartTransmitDma(handle, queueBuffer);
      handle->hasTransmitted = true;
    }
      )
  } // else: started by TransmitDmaComplete

  return ECODE_EMDRV_UARTDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start a blocking transmit.
 *
 * @param[in] handle Pointer to a UART driver handle.
 *
 * @param[in] data A transmit data buffer.
 *
 * @param[in] count A number of bytes to transmit.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success.
 ******************************************************************************/
Ecode_t UARTDRV_TransmitB(UARTDRV_Handle_t handle,
                          uint8_t *data,
                          UARTDRV_Count_t count)
{
  Ecode_t retVal;
  UARTDRV_Buffer_t outputBuffer;
  UARTDRV_Buffer_t *queueBuffer;
  uint32_t txState;

  retVal = CheckParams(handle, data, count);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  outputBuffer.data = data;
  outputBuffer.transferCount = count;
  outputBuffer.itemsRemaining = count;
  outputBuffer.callback = NULL;
  outputBuffer.transferStatus = ECODE_EMDRV_UARTDRV_WAITING;

#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif
  } else if (handle->type == uartdrvUartTypeLeuart) {
    txState = (handle->peripheral.leuart->STATUS & LEUART_STATUS_TXENS);
  } else {
    EFM_ASSERT(false);
    txState = 0;
  }
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  if (false) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  } else if (handle->type == uartdrvUartTypeUart) {
    txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif
  } else if (handle->type == uartdrvUartTypeEuart) {
    txState = EUSART_StatusGet(handle->peripheral.euart) & EUSART_STATUS_TXENS;
  } else {
    EFM_ASSERT(false);
    txState = 0;
  }
#else
  txState = (handle->peripheral.uart->STATUS & USART_STATUS_TXENS);
#endif

  if (!txState) {
    EnableTransmitter(handle);
  }

  retVal = EnqueueBuffer(handle->txQueue, &outputBuffer, &queueBuffer);
  if (retVal != ECODE_EMDRV_UARTDRV_OK) {
    return retVal;
  }
  // Active wait, the system must be in EM0 or EM1 anyway
  while (handle->txQueue->used > 1) {
  }
  StartTransmitDma(handle, queueBuffer);
  handle->hasTransmitted = true;
  // Active wait, the system must be in EM0 or EM1 anyway
  while (handle->txDmaActive) {
  }

  return queueBuffer->transferStatus;
}

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
sl_power_manager_on_isr_exit_t sl_uartdrv_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t result = SL_POWER_MANAGER_IGNORE;
  uint32_t handleIdx;
  UARTDRV_Handle_t handle;

  if (uartdrvHandleIsInitialized) {
    for (handleIdx = 0; handleIdx < EMDRV_UARTDRV_MAX_DRIVER_INSTANCES; handleIdx++) {
      handle = uartdrvHandle[handleIdx];
      if (handle != NULL) {
        if ((sl_power_manager_on_isr_exit_t)handle->sleep == SL_POWER_MANAGER_SLEEP) {
          handle->sleep = SL_POWER_MANAGER_IGNORE;
          result = SL_POWER_MANAGER_SLEEP;
        }
      }
    }
  }

  return result;
}
#endif

/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @addtogroup uartdrv UARTDRV - UART Driver
 * @brief Universal Asynchronous Receiver/Transmitter Driver
 * @{

   @details
   The source files for the UART driver library, uartdrv.c and uartdrv.h, are in the
   emdrv/uartdrv folder.

   @li @ref uartdrv_intro
   @li @ref uartdrv_conf
   @li @ref uartdrv_api
   @li @ref uartdrv_fc
   @li @ref uartdrv_example

   @n @section uartdrv_intro Introduction
   The UART driver supports the UART capabilities of the USART, UART, and LEUART
   peripherals. The driver is fully reentrant and supports multiple driver instances.
   The driver does not buffer or queue data. However, it queues UART transmit
   and receive operations. Both blocking and non-blocking transfer functions are
   available. Non-blocking transfer functions report transfer completion with
   callback functions. Transfers are done using DMA. Simple direct/forced
   transmit and receive functions are also available. Note that these functions
   are blocking and not suitable for low energy applications because they use CPU
   polling.

   UART hardware flow control (CTS/RTS) is fully supported by the driver. UART
   software flow control (XON/XOFF) is partially supported by the driver. For
   more information about flow control support, see @ref uartdrv_fc.

   @note Transfer completion callback functions are called from within the DMA
   interrupt handler with interrupts disabled.

   @n @section uartdrv_conf Configuration Options

   Some properties of the UARTDRV driver are compile-time configurable. These
   properties are set in a uartdrv_config.h file. A template for this
   file, containing default values, is in the emdrv/config folder.
   To configure UARTDRV for your application, provide a custom configuration file,
   or override the defines on the compiler command line.
   These are the available configuration parameters with default values defined.
   @code

   // Set to 1 to enable hardware flow control.
 #define EMDRV_UARTDRV_FLOW_CONTROL_ENABLE       1

   // Maximum number of driver instances.
 #define EMDRV_UARTDRV_MAX_DRIVER_INSTANCES      4

   // UART software flow control code: request peer to start Tx.
 #define UARTDRV_FC_SW_XON                       0x11

   // UART software flow control code: request peer to stop Tx.
 #define UARTDRV_FC_SW_XOFF                      0x13
   @endcode

   The properties of each UART driver instance are set at run-time via the
   @ref UARTDRV_InitUart_t data structure input parameter to the @ref UARTDRV_InitUart()
   function for UART and USART peripherals, and the @ref UARTDRV_InitLeuart_t
   data structure input parameter to the @ref UARTDRV_InitLeuart() function for
   LEUART peripherals.

   @n @section uartdrv_api The API

   This section contains brief descriptions of the functions in the API. For more
   information on input and output parameters and return values,
   click on the hyperlinked function names. Most functions return an error
   code, @ref ECODE_EMDRV_UARTDRV_OK is returned on success,
   see ecode.h and uartdrv.h for other error codes.

   The application code must include @em uartdrv.h header file.

   @ref UARTDRV_InitUart(), @ref UARTDRV_InitLeuart() and @ref UARTDRV_DeInit() @n
    These functions initialize and deinitialize the UARTDRV driver. Typically,
    @htmlonly UARTDRV_InitUart() @endhtmlonly (for UART/USART) or
    @htmlonly UARTDRV_InitLeuart() @endhtmlonly (for LEUART) are called once in
    the startup code.

   @ref UARTDRV_GetReceiveStatus() and @ref UARTDRV_GetTransmitStatus() @n
    Query the status of a current transmit or receive operations. Reports number
    of items (frames) transmitted and remaining.

   @ref UARTDRV_GetReceiveDepth() and  @ref UARTDRV_GetTransmitDepth() @n
    Get the number of queued receive or transmit operations.

   @ref UARTDRV_Transmit(), UARTDRV_Receive() @n
   UARTDRV_TransmitB(), UARTDRV_ReceiveB() @n
   UARTDRV_ForceTransmit() and UARTDRV_ForceReceive() @n
    Blocking and non-blocking transfer functions are included.
    The blocking versions have an uppercase B (for Blocking) at the end of
    their function name. Blocking functions do not return before the transfer
    is complete. The non-blocking functions signal a transfer completion with a
    callback function. @ref UARTDRV_ForceTransmit() and
    @ref UARTDRV_ForceReceive() are also blocking. These two functions access
    the UART peripheral directly without using DMA or interrupts.
    @ref UARTDRV_ForceTransmit() does not respect flow control.
    @ref UARTDRV_ForceReceive() forces RTS low.

   @ref UARTDRV_Abort() @n
    Abort current transmit or receive operations and remove all queued
    operations.

   @ref UARTDRV_FlowControlSet(), @ref UARTDRV_FlowControlGetSelfStatus(), @ref UARTDRV_FlowControlSetPeerStatus() and @ref UARTDRV_FlowControlGetPeerStatus() @n
    Set and get flow control status of self or peer device. Note that the return
    value from these two functions depends on the flow control mode set by
    @ref UARTDRV_FlowControlSet(), @ref UARTDRV_InitUart(), or
    @ref UARTDRV_InitLeuart().

   @ref UARTDRV_FlowControlIgnoreRestrain() @n
    Enables transmission when restrained by flow control.

   @ref UARTDRV_PauseTransmit() and @ref UARTDRV_ResumeTransmit() @n
    Pause a currently active transmit operation by preventing the DMA from loading
    the UART FIFO. Will not override HW flow control state (if applicable), but
    can be used in conjunction.

   @n @section uartdrv_fc Flow Control Support

   If UART flow control is not required, make sure that
   EMDRV_UARTDRV_FLOW_CONTROL_ENABLE is set to 0. This reduces the code size
   and complexity of the driver.

   Both hardware and software flow control are supported. To
   enable either of these, set EMDRV_UARTDRV_FLOW_CONTROL_ENABLE to 1 in
   uartdrv_config.h.

   @n @subsection uartdrv_fc_hw Hardware Flow Control

   UART hardware flow control uses two additional pins for flow control
   handshaking, the clear-to-send (CTS) and ready-to-send (RTS) pins.
   RTS is an output and CTS is an input. These are active-low signals.
   When CTS is high, the UART transmitter should stop sending frames.
   A receiver should set RTS high when it is no longer capable of
   receiving data.

   @par Peripheral Hardware Flow Control

   Newer devices natively support CTS/RTS in
   the USART peripheral hardware. To enable hardware flow control, perform the
   following steps:

   - Set EMDRV_UARTDRV_FLOW_CONTROL_ENABLE to 1.
   - In the @ref UARTDRV_InitUart_t struct passed to @ref UARTDRV_InitUart(), set
    @ref UARTDRV_InitUart_t.fcType = uartdrvFlowControlHwUart.
   - Define the pins for CTS and RTS by setting ctsPort, ctsPin, rtsPort
    and rtsPin in the init struct.
   - Also define the CTS and RTS locations by setting portLocationCts and
    portLocationRts in the init struct.

   @par GPIO Hardware Flow Control

   To support hardware flow control on devices that don't have UART CTS/RTS
   hardware support, the driver includes the GPIOINT driver to emulate a
   hardware implementation of UART CTS/RTS flow control on these devices.

   To enable hardware flow control, perform the following steps:

   - Set EMDRV_UARTDRV_FLOW_CONTROL_ENABLE to 1.
   - UART/USART: In the @ref UARTDRV_InitUart_t struct passed to
    @ref UARTDRV_InitUart(), set
    @ref UARTDRV_InitUart_t.fcType = uartdrvFlowControlHw.
   - LEUART: In the @ref UARTDRV_InitLeuart_t struct passed to
    @ref UARTDRV_InitLeuart(), set
    @ref UARTDRV_InitLeuart_t.fcType = uartdrvFlowControlHw.
   - Define the pins for CTS and RTS by setting ctsPort, ctsPin, rtsPort and
    rtsPin in the same init struct.

   @note Because of the limitations in GPIO interrupt hardware, you cannot select
   CTS pins in multiple driver instances with the same pin number. For example, pin A0 and
   B0 cannot serve as CTS pins in two concurrent driver instances.

   RTS is set high whenever there are no Rx operations queued. The UART
   transmitter is halted when the CTS pin goes high. The transmitter completes
   the current frame before halting. DMA transfers are also halted.

   @n @subsection uartdrv_fc_sw Software Flow Control

   UART software flow control uses in-band signaling, meaning the receiver sends
   special flow control characters to the transmitter and thereby removes
   the need for dedicated wires for flow control. The two symbols
   UARTDRV_FC_SW_XON and UARTDRV_FC_SW_XOFF are defined in uartdrv_config.h.

   To enable support for software flow control, perform the following steps:

   - Set EMDRV_UARTDRV_FLOW_CONTROL_ENABLE to 1.
   - UART/USART: In the @ref UARTDRV_InitUart_t structure passed to
    @ref UARTDRV_InitUart(), set
    @ref UARTDRV_InitUart_t.fcType = uartdrvFlowControlSw.
   - LEUART: In the @ref UARTDRV_InitLeuart_t structure passed to
    @ref UARTDRV_InitLeuart(), set
    @ref UARTDRV_InitLeuart_t.fcType = uartdrvFlowControlSw.

   @note Software flow control is partial only.

   The application must monitor buffers and make decisions on when to send XON/
   XOFF. XON/XOFF can be sent to the peer using @ref UARTDRV_FlowControlSet().
   Though @ref UARTDRV_FlowControlSet() will pause the active transmit operation
   to send a flow control character, there is no way to guarantee the order.
   If the application implements a specific packet format where the flow control
   codes may appear only in fixed positions, the application should not
   use @ref UARTDRV_FlowControlSet() but implement read and write of XON/XOFF
   into packet buffers. If the application code fully implements all the flow
   control logic, EMDRV_UARTDRV_FLOW_CONTROL_ENABLE should be set to 0
   to reduce code space.

   @n @section uartdrv_example Example
   @if DOXYDOC_P1_DEVICE
    @if DOXYDOC_EFM32G
    @include uartdrv_example_p1_nomvdis.c
    @endif
    @if DOXYDOC_EZR32HG
    @include uartdrv_example_p1_usart0.c
    @endif
    @ifnot (DOXYDOC_EFM32G || DOXYDOC_EZR32HG)
    @include uartdrv_example_p1.c
    @endif
   @endif
   @if DOXYDOC_P2_DEVICE
   @include uartdrv_example_p2.c
   @endif
   @if DOXYDOC_S2_DEVICE
   @include uartdrv_example_s2.c
   @endif

 * @} end group uartdrv *********************************************************/
