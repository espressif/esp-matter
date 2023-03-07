/***************************************************************************//**
 * @file
 * @brief SPIDRV API implementation.
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

#define CURRENT_MODULE_NAME    "SPIDRV"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "em_device.h"
#include "em_gpio.h"
#include "em_core.h"
#if defined (USART_PRESENT)
#include "em_usart.h"
#endif
#if defined(EUSART_PRESENT)
#include "em_eusart.h"
#endif
#include "dmadrv.h"
#include "spidrv.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#if defined(EUSART_PRESENT)
#include "sl_slist.h"
#endif
#endif
#include <string.h>

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

//****************************************************************************

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
#define SPI_DMA_IRQ   DMA_IRQn

#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
#define SPI_DMA_IRQ   LDMA_IRQn

#else
#error "No valid SPIDRV DMA engine defined."
#endif

#define EMDRV_SPIDRV_USART_FRAMELENGTH_REGVALUE_OFFSET  (3U)
#define EMDRV_SPIDRV_EUSART_FRAMELENGTH_REGVALUE_OFFSET (6U)

//****************************************************************************

/**
 * @brief SPI Pins structure used when mapping from location to gpio port+pin.
 */
typedef struct {
  uint8_t mosiPort;
  uint8_t mosiPin;
  uint8_t misoPort;
  uint8_t misoPin;
  uint8_t clkPort;
  uint8_t clkPin;
  uint8_t csPort;
  uint8_t csPin;
} SPI_Pins_t;

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && defined(EUSART_PRESENT)
static sl_power_manager_em_transition_event_handle_t on_power_manager_event_handle;

static void on_power_manager_event(sl_power_manager_em_t from,
                                   sl_power_manager_em_t to);

static sl_power_manager_em_transition_event_info_t on_power_manager_event_info =
{
  .event_mask = (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2 \
                 | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3),
  .on_event = on_power_manager_event,
};

// List of EUSART handles
sl_slist_node_t *eusart_handle_list = NULL;
#endif

static bool     spidrvIsInitialized = false;

#if defined(USART_PRESENT)
static Ecode_t SPIDRV_InitUsart(SPIDRV_Handle_t handle, SPIDRV_Init_t *initData);
#endif

#if defined(EUSART_PRESENT)
static Ecode_t SPIDRV_InitEusart(SPIDRV_Handle_t handle, SPIDRV_Init_t *initData);
#endif

static void     BlockingComplete(SPIDRV_Handle_t handle,
                                 Ecode_t transferStatus,
                                 int itemsTransferred);

static Ecode_t  ConfigGPIO(SPIDRV_Handle_t handle, bool enable);

static bool     RxDMAComplete(unsigned int channel,
                              unsigned int sequenceNo,
                              void *userParam);

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
static void     SlaveTimeout(sl_sleeptimer_timer_handle_t *handle, void *data);
#endif

static void     StartReceiveDMA(SPIDRV_Handle_t handle,
                                void *buffer,
                                int count,
                                SPIDRV_Callback_t callback);

static void     StartTransferDMA(SPIDRV_Handle_t handle,
                                 const void *txBuffer,
                                 void *rxBuffer,
                                 int count,
                                 SPIDRV_Callback_t callback);

static void     StartTransmitDMA(SPIDRV_Handle_t handle,
                                 const void *buffer,
                                 int count,
                                 SPIDRV_Callback_t callback);

static Ecode_t  TransferApiPrologue(SPIDRV_Handle_t handle,
                                    void *buffer,
                                    int count);

static Ecode_t  TransferApiBlockingPrologue(SPIDRV_Handle_t handle,
                                            void *buffer,
                                            int count);

static void     WaitForTransferCompletion(SPIDRV_Handle_t handle);

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
static Ecode_t  WaitForIdleLine(SPIDRV_Handle_t handle);
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && defined(EUSART_PRESENT)
static Ecode_t sli_spidrv_exit_em23(SPIDRV_Handle_t handle);
static Ecode_t sli_spidrv_enter_em23(SPIDRV_Handle_t handle);
#endif

/// @endcond

/***************************************************************************//**
 * @brief Power management functions.
 ******************************************************************************/
static void em1RequestAdd(SPIDRV_Handle_t handle)
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

static void em1RequestRemove(SPIDRV_Handle_t handle)
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

static void emRequestInit(SPIDRV_Handle_t handle)
{
  handle->em1RequestCount = 0;
}

static void emRequestDeinit(SPIDRV_Handle_t handle)
{
  if (handle->em1RequestCount > 0) {
    handle->em1RequestCount = 1;
    em1RequestRemove(handle);
  }
}

/***************************************************************************//**
 * @brief
 *    Initialize an SPI driver instance.
 *
 * @param[out] handle  Pointer to an SPI driver handle; refer to @ref
 *                     SPIDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure;
 *                     refer to @ref SPIDRV_Init.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_Init(SPIDRV_Handle_t handle, SPIDRV_Init_t *initData)
{
#if defined (EUSART_PRESENT)
  Ecode_t result = ECODE_EMDRV_SPIDRV_PARAM_ERROR;

  if (EUSART_NUM((EUSART_TypeDef*)initData->port) != -1) {
    result = SPIDRV_InitEusart(handle, initData);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Subscribe to notification to re-enable eusart after deepsleep.
    if (eusart_handle_list == NULL) {
      sl_power_manager_subscribe_em_transition_event(&on_power_manager_event_handle, &on_power_manager_event_info);
    }
    sl_slist_push(&eusart_handle_list, &handle->node);
#endif

    return result;
  }
#endif

#if defined (USART_PRESENT)
  return SPIDRV_InitUsart(handle, initData);
#else
  return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
#endif
}

#if defined (USART_PRESENT)
/***************************************************************************//**
 * @brief
 *    Initialize an SPI driver usart instance.
 *
 * @param[out] handle  Pointer to an SPI driver handle; refer to @ref
 *                     SPIDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure;
 *                     refer to @ref SPIDRV_Init.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
static Ecode_t SPIDRV_InitUsart(SPIDRV_Handle_t handle, SPIDRV_Init_t *initData)
{
  Ecode_t retVal;
  CORE_DECLARE_IRQ_STATE;
  USART_InitSync_TypeDef usartInit = USART_INITSYNC_DEFAULT;
#if defined(_SILICON_LABS_32B_SERIES_2)
  int8_t spiPortNum = -1;
#endif

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if (initData == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  memset(handle, 0, sizeof(SPIDRV_HandleData_t));
  emRequestInit(handle);

  if (0) {
#if defined(USART0)
  } else if ((USART_TypeDef*)initData->port == USART0) {
    handle->usartClock  = cmuClock_USART0;
    handle->txDMASignal = dmadrvPeripheralSignal_USART0_TXBL;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    handle->rxDMASignal = dmadrvPeripheralSignal_USART0_RXDATAV;
    spiPortNum = 0;
  #else
    handle->rxDMASignal = dmadrvPeripheralSignal_USART0_RXDATAV;
  #endif
#endif
#if defined(USART1)
  } else if ((USART_TypeDef*)initData->port == USART1) {
    handle->usartClock  = cmuClock_USART1;
    handle->txDMASignal = dmadrvPeripheralSignal_USART1_TXBL;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    handle->rxDMASignal = dmadrvPeripheralSignal_USART1_RXDATAV;
    spiPortNum = 1;
  #else
    handle->rxDMASignal = dmadrvPeripheralSignal_USART1_RXDATAV;
  #endif
#endif
#if defined(USART2)
  } else if ((USART_TypeDef*)initData->port == USART2) {
    handle->usartClock  = cmuClock_USART2;
    handle->txDMASignal = dmadrvPeripheralSignal_USART2_TXBL;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    handle->rxDMASignal = dmadrvPeripheralSignal_USART2_RXDATAV;
    spiPortNum = 2;
  #else
    handle->rxDMASignal = dmadrvPeripheralSignal_USART2_RXDATAV;
  #endif
#endif
#if defined(USART3)
  } else if ((USART_TypeDef*)initData->port == USART3) {
    handle->usartClock  = cmuClock_USART3;
    handle->txDMASignal = dmadrvPeripheralSignal_USART3_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USART3_RXDATAV;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    spiPortNum = 3;
  #endif
#endif
#if defined(USART4)
  } else if ((USART_TypeDef*)initData->port == USART4) {
    handle->usartClock  = cmuClock_USART4;
    handle->txDMASignal = dmadrvPeripheralSignal_USART4_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USART4_RXDATAV;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    spiPortNum = 4;
  #endif
#endif
#if defined(USART5)
  } else if ((USART_TypeDef*)initData->port == USART5) {
    handle->usartClock  = cmuClock_USART5;
    handle->txDMASignal = dmadrvPeripheralSignal_USART5_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USART5_RXDATAV;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    spiPortNum = 5;
  #endif
#endif
#if defined(USARTRF0)
  } else if ((USART_TypeDef*)initData->port == USARTRF0) {
    handle->usartClock  = cmuClock_USARTRF0;
    handle->txDMASignal = dmadrvPeripheralSignal_USARTRF0_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USARTRF0_RXDATAV;
#endif
#if defined(USARTRF1)
  } else if ((USART_TypeDef*)initData->port == USARTRF1) {
    handle->usartClock  = cmuClock_USARTRF1;
    handle->txDMASignal = dmadrvPeripheralSignal_USARTRF1_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USARTRF1_RXDATAV;
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  handle->peripheral.usartPort  = initData->port;
  handle->peripheralType        = spidrvPeripheralTypeUsart;
  handle->initData              = *initData;

  if (initData->bitOrder == spidrvBitOrderMsbFirst) {
    usartInit.msbf = true;
  }

  if (initData->clockMode == spidrvClockMode0) {
    usartInit.clockMode = usartClockMode0;
  } else if (initData->clockMode == spidrvClockMode1) {
    usartInit.clockMode = usartClockMode1;
  } else if (initData->clockMode == spidrvClockMode2) {
    usartInit.clockMode = usartClockMode2;
  } else if (initData->clockMode == spidrvClockMode3) {
    usartInit.clockMode = usartClockMode3;
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if (initData->type == spidrvSlave) {
    usartInit.master = false;
    usartInit.baudrate = 1000;      // Dummy value needed by USART_InitSync()
  } else {
    usartInit.baudrate = initData->bitRate;
  }

#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(handle->usartClock, true);

  if ((initData->frameLength < 4U) || (initData->frameLength > 16U)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
  uint32_t databits = initData->frameLength - 4U + _USART_FRAME_DATABITS_FOUR;
  usartInit.databits = (USART_Databits_TypeDef)databits;

  USART_InitSync(initData->port, &usartInit);

  if ((initData->type == spidrvMaster)
      && (initData->csControl == spidrvCsControlAuto)) {
    handle->peripheral.usartPort->CTRL |= USART_CTRL_AUTOCS;
  }

  if (initData->csControl == spidrvCsControlAuto) {
    // SPI 4 wire mode
#if defined(USART_ROUTEPEN_TXPEN)
    handle->peripheral.usartPort->ROUTELOC0 = (handle->peripheral.usartPort->ROUTELOC0
                                               & ~(_USART_ROUTELOC0_TXLOC_MASK
                                                   | _USART_ROUTELOC0_RXLOC_MASK
                                                   | _USART_ROUTELOC0_CLKLOC_MASK
                                                   | _USART_ROUTELOC0_CSLOC_MASK))
                                              | (initData->portLocationTx  << _USART_ROUTELOC0_TXLOC_SHIFT)
                                              | (initData->portLocationRx  << _USART_ROUTELOC0_RXLOC_SHIFT)
                                              | (initData->portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT)
                                              | (initData->portLocationCs  << _USART_ROUTELOC0_CSLOC_SHIFT);

    handle->peripheral.usartPort->ROUTEPEN = USART_ROUTEPEN_TXPEN
                                             | USART_ROUTEPEN_RXPEN
                                             | USART_ROUTEPEN_CLKPEN
                                             | USART_ROUTEPEN_CSPEN;
#elif defined (_GPIO_USART_ROUTEEN_MASK)
    GPIO->USARTROUTE[spiPortNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
                                           | GPIO_USART_ROUTEEN_RXPEN
                                           | GPIO_USART_ROUTEEN_CLKPEN
                                           | GPIO_USART_ROUTEEN_CSPEN;

    GPIO->USARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx
                                            << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinTx
                                              << _GPIO_USART_TXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx
                                            << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinRx
                                              << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].CLKROUTE = ((uint32_t)initData->portClk
                                             << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                            | ((uint32_t)initData->pinClk
                                               << _GPIO_USART_CLKROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].CSROUTE = ((uint32_t)initData->portCs
                                            << _GPIO_USART_CSROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinCs
                                              << _GPIO_USART_CSROUTE_PIN_SHIFT);
#else
    handle->peripheral.usartPort->ROUTE = USART_ROUTE_TXPEN
                                          | USART_ROUTE_RXPEN
                                          | USART_ROUTE_CLKPEN
                                          | USART_ROUTE_CSPEN
                                          | (initData->portLocation
                                             << _USART_ROUTE_LOCATION_SHIFT);
#endif
  } else {
    // SPI 3 wire mode
#if defined(USART_ROUTEPEN_TXPEN)
    handle->peripheral.usartPort->ROUTELOC0 = (handle->peripheral.usartPort->ROUTELOC0
                                               & ~(_USART_ROUTELOC0_TXLOC_MASK
                                                   | _USART_ROUTELOC0_RXLOC_MASK
                                                   | _USART_ROUTELOC0_CLKLOC_MASK))
                                              | (initData->portLocationTx  << _USART_ROUTELOC0_TXLOC_SHIFT)
                                              | (initData->portLocationRx  << _USART_ROUTELOC0_RXLOC_SHIFT)
                                              | (initData->portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT);

    handle->peripheral.usartPort->ROUTEPEN = USART_ROUTEPEN_TXPEN
                                             | USART_ROUTEPEN_RXPEN
                                             | USART_ROUTEPEN_CLKPEN;
#elif defined (GPIO_USART_ROUTEEN_TXPEN)
    GPIO->USARTROUTE[spiPortNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
                                           | GPIO_USART_ROUTEEN_RXPEN
                                           | GPIO_USART_ROUTEEN_CLKPEN;

    GPIO->USARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx
                                            << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinTx
                                              << _GPIO_USART_TXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx
                                            << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinRx
                                              << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].CLKROUTE = ((uint32_t)initData->portClk
                                             << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                            | ((uint32_t)initData->pinClk
                                               << _GPIO_USART_CLKROUTE_PIN_SHIFT);
#else
    handle->peripheral.usartPort->ROUTE = USART_ROUTE_TXPEN
                                          | USART_ROUTE_RXPEN
                                          | USART_ROUTE_CLKPEN
                                          | (initData->portLocation
                                             << _USART_ROUTE_LOCATION_SHIFT);
#endif
  }

  if ((retVal = ConfigGPIO(handle, true)) != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  CORE_ENTER_ATOMIC();
  if (!spidrvIsInitialized) {
    spidrvIsInitialized = true;
    CORE_EXIT_ATOMIC();

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
    sl_sleeptimer_init();
#endif
  } else {
    CORE_EXIT_ATOMIC();
  }

  // Initialize DMA.
  DMADRV_Init();

  if (DMADRV_AllocateChannel(&handle->txDMACh, NULL) != ECODE_EMDRV_DMADRV_OK) {
    return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
  }

  if (DMADRV_AllocateChannel(&handle->rxDMACh, NULL) != ECODE_EMDRV_DMADRV_OK) {
    return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
  }

  return ECODE_EMDRV_SPIDRV_OK;
}
#endif // defined USART_PRESENT

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * @brief
 *    Initialize an SPI driver eusart instance.
 *
 * @param[out] handle  Pointer to an SPI driver handle; refer to @ref
 *                     SPIDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure;
 *                     refer to @ref SPIDRV_Init.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
static Ecode_t SPIDRV_InitEusart(SPIDRV_Handle_t handle, SPIDRV_Init_t *initData)
{
  Ecode_t retVal;
  CORE_DECLARE_IRQ_STATE;
  EUSART_SpiAdvancedInit_TypeDef eusartAdvancedSpiInit = EUSART_SPI_ADVANCED_INIT_DEFAULT;
  EUSART_SpiInit_TypeDef eusartSpiInit = EUSART_SPI_MASTER_INIT_DEFAULT_HF;
  int8_t spiPortNum = -1;

  eusartSpiInit.advancedSettings = &eusartAdvancedSpiInit;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if (initData == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  memset(handle, 0, sizeof(SPIDRV_HandleData_t));
  emRequestInit(handle);

  if (0) {
#if defined(EUSART0)
  } else if (initData->port == EUSART0) {
    handle->usartClock  = cmuClock_EUSART0;
    handle->txDMASignal = dmadrvPeripheralSignal_EUSART0_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_EUSART0_RXDATAV;
    spiPortNum = 0;
#endif
#if defined(EUSART1)
  } else if (initData->port == EUSART1) {
    handle->usartClock  = cmuClock_EUSART1;
    handle->txDMASignal = dmadrvPeripheralSignal_EUSART1_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_EUSART1_RXDATAV;
    spiPortNum = 1;
#endif
#if defined(EUSART2)
  } else if (initData->port == EUSART2) {
    handle->usartClock  = cmuClock_EUSART2;
    handle->txDMASignal = dmadrvPeripheralSignal_EUSART2_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_EUSART2_RXDATAV;
    spiPortNum = 2;
#endif
#if defined(EUSART3)
  } else if (initData->port == EUSART3) {
    handle->usartClock  = cmuClock_EUSART3;
    handle->txDMASignal = dmadrvPeripheralSignal_EUSART3_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_EUSART3_RXDATAV;
    spiPortNum = 3;
#endif
#if defined(EUSART4)
  } else if (initData->port == EUSART4) {
    handle->usartClock  = cmuClock_EUSART4;
    handle->txDMASignal = dmadrvPeripheralSignal_EUSART4_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_EUSART4_RXDATAV;
    spiPortNum = 4;
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  handle->peripheral.eusartPort = initData->port;
  handle->peripheralType        = spidrvPeripheralTypeEusart;
  handle->initData              = *initData;

  if (initData->bitOrder == spidrvBitOrderMsbFirst) {
    eusartAdvancedSpiInit.msbFirst = true;
  }

  if (initData->clockMode == spidrvClockMode0) {
    eusartSpiInit.clockMode = eusartClockMode0;
  } else if (initData->clockMode == spidrvClockMode1) {
    eusartSpiInit.clockMode = eusartClockMode1;
  } else if (initData->clockMode == spidrvClockMode2) {
    eusartSpiInit.clockMode = eusartClockMode2;
  } else if (initData->clockMode == spidrvClockMode3) {
    eusartSpiInit.clockMode = eusartClockMode3;
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if (initData->type == spidrvSlave) {
    if (initData->bitRate >= 5000000) {
      // If baud-rate is more than 5MHz, a value of 4 is required
      eusartSpiInit.advancedSettings->setupWindow = 4;
    } else {
      // If baud-rate is less than 5MHz, a value of 5 is required
      eusartSpiInit.advancedSettings->setupWindow = 5;
    }
    eusartSpiInit.master  = false;
    eusartSpiInit.bitRate = 1000000;
  } else {
    eusartAdvancedSpiInit.forceLoad = false;
    eusartSpiInit.bitRate = initData->bitRate;
  }
  // Leave eusartAdvancedSpiInit.defaultTxData = 0, not initData->dummyTxValue
  // for EUSART compatibility with USART behavior.

  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(handle->usartClock, true);

  if ((initData->frameLength < 7U) || (initData->frameLength > 16U)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  uint32_t databits = initData->frameLength - 7U + EUSART_FRAMECFG_DATABITS_SEVEN;
  eusartSpiInit.databits = (EUSART_Databits_TypeDef)databits;

  if (initData->type == spidrvMaster) {
    // Don't assume EUSART_SPI_ADVANCED_INIT_DEFAULT has desired autoCsEnable
    eusartAdvancedSpiInit.autoCsEnable = (initData->csControl == spidrvCsControlAuto);
  }

  if ((retVal = ConfigGPIO(handle, true)) != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  GPIO->EUSARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx
                                           << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
                                          | ((uint32_t)initData->pinTx
                                             << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx
                                           << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
                                          | ((uint32_t)initData->pinRx
                                             << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[spiPortNum].SCLKROUTE = ((uint32_t)initData->portClk
                                             << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
                                            | ((uint32_t)initData->pinClk
                                               << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);

  if (initData->csControl == spidrvCsControlAuto) {
    // SPI 4 wire mode, Chip Select controled by the peripheral
    GPIO->EUSARTROUTE[spiPortNum].CSROUTE = ((uint32_t)initData->portCs
                                             << _GPIO_EUSART_CSROUTE_PORT_SHIFT)
                                            | ((uint32_t)initData->pinCs
                                               << _GPIO_EUSART_CSROUTE_PIN_SHIFT);
  }

  // SPI 4 wire mode
  if (initData->csControl == spidrvCsControlAuto) {
    GPIO->EUSARTROUTE[spiPortNum].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN
                                            | GPIO_EUSART_ROUTEEN_RXPEN
                                            | GPIO_EUSART_ROUTEEN_SCLKPEN
                                            | GPIO_EUSART_ROUTEEN_CSPEN;
  } else {
    GPIO->EUSARTROUTE[spiPortNum].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN
                                            | GPIO_EUSART_ROUTEEN_RXPEN
                                            | GPIO_EUSART_ROUTEEN_SCLKPEN;
  }

  EUSART_SpiInit(initData->port, &eusartSpiInit);

  CORE_ENTER_ATOMIC();
  if (!spidrvIsInitialized) {
    spidrvIsInitialized = true;
    CORE_EXIT_ATOMIC();

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
    sl_sleeptimer_init();
#endif
  } else {
    CORE_EXIT_ATOMIC();
  }

  // Initialize DMA.
  DMADRV_Init();

  if (DMADRV_AllocateChannel(&handle->txDMACh, NULL) != ECODE_EMDRV_DMADRV_OK) {
    return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
  }

  if (DMADRV_AllocateChannel(&handle->rxDMACh, NULL) != ECODE_EMDRV_DMADRV_OK) {
    return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
  }

  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

/***************************************************************************//**
 * @brief
 *    Deinitialize an SPI driver instance.
 *
 * @warning
 *  This function should only be called with an initialized spidrv instance handle.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_DeInit(SPIDRV_Handle_t handle)
{
  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  // Stop DMAs.
  DMADRV_StopTransfer(handle->rxDMACh);
  DMADRV_StopTransfer(handle->txDMACh);

  ConfigGPIO(handle, false);

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if (handle->initData.type == spidrvSlave) {
    sl_sleeptimer_stop_timer(&handle->timer);
  }
#endif

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    USART_Reset(handle->peripheral.usartPort);
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    EUSART_Reset(handle->peripheral.eusartPort);
  }
#endif

  CMU_ClockEnable(handle->usartClock, false);

  DMADRV_FreeChannel(handle->txDMACh);
  DMADRV_FreeChannel(handle->rxDMACh);
  DMADRV_DeInit();
  emRequestDeinit(handle);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && defined(EUSART_PRESENT)
  // Unsubscribe to notification to re-enable eusart after deepsleep.
  sl_slist_remove(&eusart_handle_list, &handle->node);
  if (eusart_handle_list == NULL) {
    sl_power_manager_unsubscribe_em_transition_event(&on_power_manager_event_handle);
  }
#endif

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Abort an ongoing SPI transfer.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_IDLE if
 *    SPI is idle. On failure, an appropriate SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_AbortTransfer(SPIDRV_Handle_t handle)
{
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if (handle->state == spidrvStateIdle) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_IDLE;
  }

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if (handle->initData.type == spidrvSlave) {
    sl_sleeptimer_stop_timer(&handle->timer);
  }
#endif

  // Stop DMA's.
  DMADRV_StopTransfer(handle->rxDMACh);
  DMADRV_StopTransfer(handle->txDMACh);
  DMADRV_TransferRemainingCount(handle->rxDMACh, &handle->remaining);
  handle->transferStatus    = ECODE_EMDRV_SPIDRV_ABORTED;
  handle->state             = spidrvStateIdle;
  handle->transferStatus    = ECODE_EMDRV_SPIDRV_ABORTED;
  handle->blockingCompleted = true;

  em1RequestRemove(handle);

  if (handle->userCallback != NULL) {
    handle->userCallback(handle,
                         ECODE_EMDRV_SPIDRV_ABORTED,
                         handle->transferCount - handle->remaining);
  }
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get current SPI bus bitrate.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[out] bitRate Current SPI bus bitrate.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_GetBitrate(SPIDRV_Handle_t handle, uint32_t *bitRate)
{
  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if (bitRate == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    *bitRate = USART_BaudrateGet(handle->peripheral.usartPort);
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    *bitRate = EUSART_BaudrateGet(handle->peripheral.eusartPort);
  }
#endif

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get current SPI framelength.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[out] frameLength Current SPI bus framelength.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_GetFramelength(SPIDRV_Handle_t handle, uint32_t *frameLength)
{
  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if (frameLength == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  *frameLength = handle->initData.frameLength;

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get the status of an SPI transfer.
 *
 * @details
 *    Returns status of an ongoing transfer. If no transfer is in progress,
 *    the status of the last transfer is reported.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[out] itemsTransferred Number of items (frames) transferred.
 *
 * @param[out] itemsRemaining Number of items (frames) remaining.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_GetTransferStatus(SPIDRV_Handle_t handle,
                                 int *itemsTransferred,
                                 int *itemsRemaining)
{
  int remaining;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ((itemsTransferred == NULL) || (itemsRemaining == NULL)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ATOMIC_SECTION(
    if (handle->state == spidrvStateIdle) {
    remaining = handle->remaining;
  } else {
    DMADRV_TransferRemainingCount(handle->rxDMACh, &remaining);
  }
    )

  * itemsTransferred = (handle->transferCount - remaining);
  *itemsRemaining = remaining;

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master receive transfer.
 *
 * @note
 *    The MOSI wire will transmit @ref SPIDRV_Init.dummyTxValue.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MReceive(SPIDRV_Handle_t handle,
                        void *buffer,
                        int count,
                        SPIDRV_Callback_t callback)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiPrologue(handle, buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  StartReceiveDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking receive transfer.
 *
 * @note
 *    The MOSI wire will transmit @ref SPIDRV_Init.dummyTxValue.
 *    @n This function is blocking and returns when the transfer is complete
 *    or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MReceiveB(SPIDRV_Handle_t handle,
                         void *buffer,
                         int count)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiBlockingPrologue(handle, buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  StartReceiveDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master transfer.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransfer(SPIDRV_Handle_t handle,
                         const void *txBuffer,
                         void *rxBuffer,
                         int count,
                         SPIDRV_Callback_t callback)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiPrologue(handle, (void*)txBuffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (rxBuffer == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking transfer.
 *
 * @note
 *    This function is blocking and returns when the transfer is complete
 *    or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransferB(SPIDRV_Handle_t handle,
                          const void *txBuffer,
                          void *rxBuffer,
                          int count)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiBlockingPrologue(handle, (void*)txBuffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (rxBuffer == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking single item (frame) transfer.
 *
 * @note
 *    This function is blocking and returns when the transfer is complete
 *    or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] txValue Value to transmit.
 *
 * @param[out] rxValue Value received.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransferSingleItemB(SPIDRV_Handle_t handle,
                                    uint32_t txValue,
                                    void *rxValue)
{
  void *pRx;
  CORE_DECLARE_IRQ_STATE;
  uint32_t rxBuffer;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if (handle->state != spidrvStateIdle) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }
  handle->state = spidrvStateTransferring;
  CORE_EXIT_ATOMIC();

  if ((pRx = rxValue) == NULL) {
    pRx = &rxBuffer;
  }

  StartTransferDMA(handle, &txValue, pRx, 1, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master transmit transfer.
 *
 * @note
 *    The data received on the MISO wire is discarded.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] buffer Transmit data buffer.
 *
 * @param[in] count Number of bytes in transfer.
 *
 * @param[in] callback Transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransmit(SPIDRV_Handle_t handle,
                         const void *buffer,
                         int count,
                         SPIDRV_Callback_t callback)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiPrologue(handle, (void*)buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  StartTransmitDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking transmit transfer.
 *
 * @note
 *    The data received on the MISO wire is discarded.
 *    @n This function is blocking and returns when the transfer is complete.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] buffer Transmit data buffer.
 *
 * @param[in] count Number of bytes in transfer.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransmitB(SPIDRV_Handle_t handle,
                          const void *buffer,
                          int count)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvSlave) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiBlockingPrologue(handle, (void*)buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  StartTransmitDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Set SPI bus bitrate.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] bitRate New SPI bus bitrate.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_SetBitrate(SPIDRV_Handle_t handle, uint32_t bitRate)
{
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if (handle->state != spidrvStateIdle) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }

  handle->initData.bitRate = bitRate;

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    USART_BaudrateSyncSet(handle->peripheral.usartPort, 0, bitRate);
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    EUSART_BaudrateSet(handle->peripheral.eusartPort, 0, bitRate);
  }
#endif
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Set SPI framelength.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] frameLength New SPI bus framelength.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_SetFramelength(SPIDRV_Handle_t handle, uint32_t frameLength)
{
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    frameLength -= EMDRV_SPIDRV_USART_FRAMELENGTH_REGVALUE_OFFSET;

    if ((frameLength < _USART_FRAME_DATABITS_FOUR) || (frameLength > _USART_FRAME_DATABITS_SIXTEEN)) {
      return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    frameLength -= EMDRV_SPIDRV_EUSART_FRAMELENGTH_REGVALUE_OFFSET;

    if ((frameLength < _EUSART_FRAMECFG_DATABITS_SEVEN) || (frameLength > _EUSART_FRAMECFG_DATABITS_SIXTEEN)) {
      return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
    }
  }
#endif

  CORE_ENTER_ATOMIC();
  if (handle->state != spidrvStateIdle) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    handle->initData.frameLength = frameLength + EMDRV_SPIDRV_USART_FRAMELENGTH_REGVALUE_OFFSET;

    handle->peripheral.usartPort->FRAME = (handle->peripheral.usartPort->FRAME
                                           & ~_USART_FRAME_DATABITS_MASK)
                                          | (frameLength
                                             << _USART_FRAME_DATABITS_SHIFT);
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    handle->initData.frameLength = frameLength + EMDRV_SPIDRV_EUSART_FRAMELENGTH_REGVALUE_OFFSET;

    EUSART_Enable(handle->peripheral.eusartPort, eusartDisable);
    handle->peripheral.eusartPort->FRAMECFG = (handle->peripheral.eusartPort->FRAMECFG
                                               & ~_EUSART_FRAMECFG_DATABITS_MASK)
                                              | (frameLength
                                                 << _EUSART_FRAMECFG_DATABITS_SHIFT);
    EUSART_Enable(handle->peripheral.eusartPort, eusartEnable);
  }
#endif
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
/***************************************************************************//**
 * @brief
 *    Start an SPI slave receive transfer.
 *
 * @note
 *    The MISO wire will transmit @ref SPIDRV_Init.dummyTxValue.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_SReceive(SPIDRV_Handle_t handle,
                        void *buffer,
                        int count,
                        SPIDRV_Callback_t callback,
                        int timeoutMs)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvMaster) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiPrologue(handle, buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (timeoutMs) {
    sl_sleeptimer_start_timer_ms(&handle->timer, timeoutMs, SlaveTimeout, handle, 0, 0);
  }

  if (handle->initData.slaveStartMode == spidrvSlaveStartDelayed) {
    if ((retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK) {
      return retVal;
    }
  }

  StartReceiveDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave blocking receive transfer.
 *
 * @note
 *    The MISO wire will transmit @ref SPIDRV_Init.dummyTxValue.
 *    @n This function is blocking and returns when the transfer is complete,
 *    on timeout, or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT on
 *    timeout or @ref ECODE_EMDRV_SPIDRV_ABORTED if @ref SPIDRV_AbortTransfer()
 *    has been called. On failure, an appropriate SPIDRV @ref Ecode_t is
 *    returned.
 ******************************************************************************/
Ecode_t SPIDRV_SReceiveB(SPIDRV_Handle_t handle,
                         void *buffer,
                         int count,
                         int timeoutMs)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvMaster) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiBlockingPrologue(handle, buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (timeoutMs) {
    sl_sleeptimer_start_timer_ms(&handle->timer, timeoutMs, SlaveTimeout, handle, 0, 0);
  }

  if (handle->initData.slaveStartMode == spidrvSlaveStartDelayed) {
    if ((retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK) {
      return retVal;
    }
  }

  StartReceiveDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave transfer.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransfer(SPIDRV_Handle_t handle,
                         const void *txBuffer,
                         void *rxBuffer,
                         int count,
                         SPIDRV_Callback_t callback,
                         int timeoutMs)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvMaster) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiPrologue(handle, (void*)txBuffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (rxBuffer == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if (timeoutMs) {
    sl_sleeptimer_start_timer_ms(&handle->timer, timeoutMs, SlaveTimeout, handle, 0, 0);
  }

  if (handle->initData.slaveStartMode == spidrvSlaveStartDelayed) {
    if ((retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK) {
      return retVal;
    }
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave blocking transfer.
 *
 * @note
 *    @n This function is blocking and returns when the transfer is complete,
 *    on timeout, or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT on
 *    timeout or @ref ECODE_EMDRV_SPIDRV_ABORTED if @ref SPIDRV_AbortTransfer()
 *    has been called. On failure, an appropriate SPIDRV @ref Ecode_t is
 *    returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransferB(SPIDRV_Handle_t handle,
                          const void *txBuffer,
                          void *rxBuffer,
                          int count,
                          int timeoutMs)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvMaster) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiBlockingPrologue(handle, (void*)txBuffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (rxBuffer == NULL) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if (timeoutMs) {
    sl_sleeptimer_start_timer_ms(&handle->timer, timeoutMs, SlaveTimeout, handle, 0, 0);
  }

  if (handle->initData.slaveStartMode == spidrvSlaveStartDelayed) {
    if ((retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK) {
      return retVal;
    }
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave transmit transfer.
 *
 * @note
 *    The data received on the MOSI wire is discarded.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  buffer Transmit data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransmit(SPIDRV_Handle_t handle,
                         const void *buffer,
                         int count,
                         SPIDRV_Callback_t callback,
                         int timeoutMs)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvMaster) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiPrologue(handle, (void*)buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (timeoutMs) {
    sl_sleeptimer_start_timer_ms(&handle->timer, timeoutMs, SlaveTimeout, handle, 0, 0);
  }

  if (handle->initData.slaveStartMode == spidrvSlaveStartDelayed) {
    if ((retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK) {
      return retVal;
    }
  }

  StartTransmitDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave blocking transmit transfer.
 *
 * @note
 *    The data received on the MOSI wire is discarded.
 *    @n This function is blocking and returns when the transfer is complete,
 *    on timeout, or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  buffer Transmit data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT on
 *    timeout or @ref ECODE_EMDRV_SPIDRV_ABORTED if @ref SPIDRV_AbortTransfer()
 *    has been called. On failure, an appropriate SPIDRV @ref Ecode_t is
 *    returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransmitB(SPIDRV_Handle_t handle,
                          const void *buffer,
                          int count,
                          int timeoutMs)
{
  Ecode_t retVal;

  if (handle->initData.type == spidrvMaster) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ((retVal = TransferApiBlockingPrologue(handle, (void*)buffer, count))
      != ECODE_EMDRV_SPIDRV_OK) {
    return retVal;
  }

  if (timeoutMs) {
    sl_sleeptimer_start_timer_ms(&handle->timer, timeoutMs, SlaveTimeout, handle, 0, 0);
  }

  if (handle->initData.slaveStartMode == spidrvSlaveStartDelayed) {
    if ((retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK) {
      return retVal;
    }
  }

  StartTransmitDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * @brief
 *    Transfer complete callback function used by blocking transfer API
 *    functions. Called by DMA interrupt handler, timer timeout handler
 *    or @ref SPIDRV_AbortTransfer() function.
 ******************************************************************************/
static void BlockingComplete(SPIDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             int itemsTransferred)
{
  (void)itemsTransferred;

  handle->transferStatus    = transferStatus;
  handle->blockingCompleted = true;
}

#if defined(_SILICON_LABS_32B_SERIES_0)
/***************************************************************************//**
 * @brief Get SPI pins for Series 0 devices.
 ******************************************************************************/
static Ecode_t GetSpiPins(SPIDRV_Handle_t handle, SPI_Pins_t * pins)
{
  uint32_t location;

  location = handle->initData.portLocation;

  if (0) {
#if defined(USART0)
  } else if (handle->peripheral.usartPort == USART0) {
    pins->mosiPort = AF_USART0_TX_PORT(location);
    pins->misoPort = AF_USART0_RX_PORT(location);
    pins->clkPort  = AF_USART0_CLK_PORT(location);
    pins->csPort   = AF_USART0_CS_PORT(location);
    pins->mosiPin  = AF_USART0_TX_PIN(location);
    pins->misoPin  = AF_USART0_RX_PIN(location);
    pins->clkPin   = AF_USART0_CLK_PIN(location);
    pins->csPin    = AF_USART0_CS_PIN(location);
#endif
#if defined(USART1)
  } else if (handle->peripheral.usartPort == USART1) {
    pins->mosiPort = AF_USART1_TX_PORT(location);
    pins->misoPort = AF_USART1_RX_PORT(location);
    pins->clkPort  = AF_USART1_CLK_PORT(location);
    pins->csPort   = AF_USART1_CS_PORT(location);
    pins->mosiPin  = AF_USART1_TX_PIN(location);
    pins->misoPin  = AF_USART1_RX_PIN(location);
    pins->clkPin   = AF_USART1_CLK_PIN(location);
    pins->csPin    = AF_USART1_CS_PIN(location);
#endif
#if defined(USART2)
  } else if (handle->peripheral.usartPort == USART2) {
    pins->mosiPort = AF_USART2_TX_PORT(location);
    pins->misoPort = AF_USART2_RX_PORT(location);
    pins->clkPort  = AF_USART2_CLK_PORT(location);
    pins->csPort   = AF_USART2_CS_PORT(location);
    pins->mosiPin  = AF_USART2_TX_PIN(location);
    pins->misoPin  = AF_USART2_RX_PIN(location);
    pins->clkPin   = AF_USART2_CLK_PIN(location);
    pins->csPin    = AF_USART2_CS_PIN(location);
#endif
#if defined(USARTRF0)
  } else if (handle->peripheral.usartPort == USARTRF0) {
    pins->mosiPort = AF_USARTRF0_TX_PORT(location);
    pins->misoPort = AF_USARTRF0_RX_PORT(location);
    pins->clkPort  = AF_USARTRF0_CLK_PORT(location);
    pins->csPort   = AF_USARTRF0_CS_PORT(location);
    pins->mosiPin  = AF_USARTRF0_TX_PIN(location);
    pins->misoPin  = AF_USARTRF0_RX_PIN(location);
    pins->clkPin   = AF_USARTRF0_CLK_PIN(location);
    pins->csPin    = AF_USARTRF0_CS_PIN(location);
#endif
#if defined(USARTRF1)
  } else if (handle->peripheral.usartPort == USARTRF1) {
    pins->mosiPort = AF_USARTRF1_TX_PORT(location);
    pins->misoPort = AF_USARTRF1_RX_PORT(location);
    pins->clkPort  = AF_USARTRF1_CLK_PORT(location);
    pins->csPort   = AF_USARTRF1_CS_PORT(location);
    pins->mosiPin  = AF_USARTRF1_TX_PIN(location);
    pins->misoPin  = AF_USARTRF1_RX_PIN(location);
    pins->clkPin   = AF_USARTRF1_CLK_PIN(location);
    pins->csPin    = AF_USARTRF1_CS_PIN(location);
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief Get SPI pins for Series 1 devices.
 ******************************************************************************/
static Ecode_t GetSpiPins(SPIDRV_Handle_t handle, SPI_Pins_t * pins)
{
  if (0) {
#if defined(USART0)
  } else if (handle->peripheral.usartPort == USART0) {
    pins->mosiPort = AF_USART0_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USART0_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USART0_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USART0_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USART0_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USART0_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USART0_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USART0_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USART1)
  } else if (handle->peripheral.usartPort == USART1) {
    pins->mosiPort = AF_USART1_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USART1_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USART1_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USART1_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USART1_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USART1_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USART1_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USART1_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USART2)
  } else if (handle->peripheral.usartPort == USART2) {
    pins->mosiPort = AF_USART2_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USART2_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USART2_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USART2_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USART2_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USART2_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USART2_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USART2_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USART3)
  } else if (handle->peripheral.usartPort == USART3) {
    pins->mosiPort = AF_USART3_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USART3_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USART3_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USART3_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USART3_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USART3_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USART3_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USART3_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USART4)
  } else if (handle->peripheral.usartPort == USART4) {
    pins->mosiPort = AF_USART4_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USART4_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USART4_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USART4_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USART4_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USART4_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USART4_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USART4_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USART5)
  } else if (handle->peripheral.usartPort == USART5) {
    pins->mosiPort = AF_USART5_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USART5_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USART5_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USART5_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USART5_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USART5_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USART5_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USART5_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USARTRF0)
  } else if (handle->peripheral.usartPort == USARTRF0) {
    pins->mosiPort = AF_USARTRF0_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USARTRF0_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USARTRF0_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USARTRF0_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USARTRF0_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USARTRF0_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USARTRF0_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USARTRF0_CS_PIN(handle->initData.portLocationCs);
#endif
#if defined(USARTRF1)
  } else if (handle->peripheral.usartPort == USARTRF1) {
    pins->mosiPort = AF_USARTRF1_TX_PORT(handle->initData.portLocationTx);
    pins->misoPort = AF_USARTRF1_RX_PORT(handle->initData.portLocationRx);
    pins->clkPort  = AF_USARTRF1_CLK_PORT(handle->initData.portLocationClk);
    pins->csPort   = AF_USARTRF1_CS_PORT(handle->initData.portLocationCs);
    pins->mosiPin  = AF_USARTRF1_TX_PIN(handle->initData.portLocationTx);
    pins->misoPin  = AF_USARTRF1_RX_PIN(handle->initData.portLocationRx);
    pins->clkPin   = AF_USARTRF1_CLK_PIN(handle->initData.portLocationClk);
    pins->csPin    = AF_USARTRF1_CS_PIN(handle->initData.portLocationCs);
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief Get SPI pins for Series 2 devices.
 ******************************************************************************/
static Ecode_t GetSpiPins(SPIDRV_Handle_t handle, SPI_Pins_t * pins)
{
  pins->mosiPort = handle->initData.portTx;
  pins->misoPort = handle->initData.portRx;
  pins->clkPort  = handle->initData.portClk;
  pins->csPort   = handle->initData.portCs;
  pins->mosiPin  = handle->initData.pinTx;
  pins->misoPin  = handle->initData.pinRx;
  pins->clkPin   = handle->initData.pinClk;
  pins->csPin    = handle->initData.pinCs;

  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

/***************************************************************************//**
 * @brief Configure/deconfigure SPI GPIO pins.
 ******************************************************************************/
static Ecode_t ConfigGPIO(SPIDRV_Handle_t handle, bool enable)
{
  SPI_Pins_t pins;
  Ecode_t ret;

  ret = GetSpiPins(handle, &pins);
  if (ret != ECODE_EMDRV_SPIDRV_OK) {
    return ret;
  }
  handle->portCs = (GPIO_Port_TypeDef)pins.csPort;
  handle->pinCs  = pins.csPin;

  if (enable) {
    if (handle->initData.type == spidrvMaster) {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin,
                      gpioModePushPull, 0);
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin,
                      gpioModeInput, 0);

      if ((handle->initData.clockMode == spidrvClockMode0)
          || (handle->initData.clockMode == spidrvClockMode1)) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModePushPull, 0);
      } else {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModePushPull, 1);
      }

      if (handle->initData.csControl == spidrvCsControlAuto) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)handle->portCs, handle->pinCs,
                        gpioModePushPull, 1);
      }
    } else {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin,
                      gpioModeInput, 0);
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin,
                      gpioModePushPull, 0);

      if ((handle->initData.clockMode == spidrvClockMode0)
          || (handle->initData.clockMode == spidrvClockMode1)) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModeInputPull, 0);
      } else {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModeInputPull, 1);
      }

      if (handle->initData.csControl == spidrvCsControlAuto) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)handle->portCs, handle->pinCs,
                        gpioModeInputPull, 1);
      }
    }
  } else {
    GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin, gpioModeInputPull, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin, gpioModeInputPull, 0);

    if ((handle->initData.clockMode == spidrvClockMode0)
        || (handle->initData.clockMode == spidrvClockMode1)) {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModeInputPull, 0);
    } else {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModeInputPull, 1);
    }

    if (handle->initData.csControl == spidrvCsControlAuto) {
      GPIO_PinModeSet((GPIO_Port_TypeDef)handle->portCs, handle->pinCs,
                      gpioModeDisabled, 0);
    }
  }

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief DMA transfer completion callback. Called by the DMA interrupt handler.
 ******************************************************************************/
static bool RxDMAComplete(unsigned int channel,
                          unsigned int sequenceNo,
                          void *userParam)
{
  CORE_DECLARE_IRQ_STATE;
  SPIDRV_Handle_t handle;
  (void)channel;
  (void)sequenceNo;

  CORE_ENTER_ATOMIC();

  handle = (SPIDRV_Handle_t)userParam;

  handle->transferStatus = ECODE_EMDRV_SPIDRV_OK;
  handle->state          = spidrvStateIdle;
  handle->remaining      = 0;

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if (handle->initData.type == spidrvSlave) {
    sl_sleeptimer_stop_timer(&handle->timer);
  }
#endif

  if (handle->userCallback != NULL) {
    handle->userCallback(handle, ECODE_EMDRV_SPIDRV_OK, handle->transferCount);
  }

  CORE_EXIT_ATOMIC();
  em1RequestRemove(handle);

  return true;
}

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
/***************************************************************************//**
 * @brief Slave transfer timeout callback function.
 ******************************************************************************/
static void SlaveTimeout(sl_sleeptimer_timer_handle_t *sleepdriver_handle, void *user)
{
  bool active, pending;
  SPIDRV_Handle_t handle;
  (void)sleepdriver_handle;

  handle = (SPIDRV_Handle_t)user;

  if (handle->state == spidrvStateTransferring) {
    DMADRV_TransferActive(handle->rxDMACh, &active);
    if (active) {
      // Stop running DMAs
      DMADRV_StopTransfer(handle->rxDMACh);
      DMADRV_StopTransfer(handle->txDMACh);
      DMADRV_TransferRemainingCount(handle->rxDMACh, &handle->remaining);
    } else {
      // DMA is either completed or not yet started
      DMADRV_TransferCompletePending(handle->txDMACh, &pending);
      if (pending) {
        // A DMA interrupt is pending; let the DMA handler do the rest
        return;
      }
      handle->remaining = handle->transferCount;
    }
    handle->transferStatus = ECODE_EMDRV_SPIDRV_TIMEOUT;
    handle->state          = spidrvStateIdle;

    if (handle->userCallback != NULL) {
      handle->userCallback(handle,
                           ECODE_EMDRV_SPIDRV_TIMEOUT,
                           handle->transferCount - handle->remaining);
    }
  }
}
#endif

#if defined(EUSART_PRESENT)
/***************************************************************************//**
 * @brief Manually clear eusart Tx fifo.
 ******************************************************************************/
static void clearEusartFifos(EUSART_TypeDef *eusart)
{
  // EUSART_CMD_CLEARTX reportedly only affects UART, not SPI mode,
  // and there is no EUSART_CMD_CLEARRX. Only way to clear the
  // FIFOs is via the big hammer of disabling then reenabling it.
  EUSART_Enable(eusart, eusartDisable);
  EUSART_Enable(eusart, eusartEnable);
}
#endif

/***************************************************************************//**
 * @brief Start an SPI receive DMA.
 ******************************************************************************/
static void StartReceiveDMA(SPIDRV_Handle_t handle,
                            void *buffer,
                            int count,
                            SPIDRV_Callback_t callback)
{
  void *rxPort, *txPort;
  DMADRV_DataSize_t size;

  handle->blockingCompleted  = false;
  handle->transferCount      = count;
  handle->userCallback       = callback;

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    handle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

    if (handle->initData.frameLength > 9) {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDOUBLE);
      txPort = (void *)&(handle->peripheral.usartPort->TXDOUBLE);
    } else if (handle->initData.frameLength == 9) {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDATAX);
      txPort = (void *)&(handle->peripheral.usartPort->TXDATAX);
    } else {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDATA);
      txPort = (void *)&(handle->peripheral.usartPort->TXDATA);
    }
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    clearEusartFifos(handle->peripheral.eusartPort);

    rxPort = (void *)&(handle->peripheral.eusartPort->RXDATA);
    txPort = (void *)&(handle->peripheral.eusartPort->TXDATA);
  }
#endif
  else {
    return;
  }

  if (handle->initData.frameLength > 8) {
    size = dmadrvDataSize2;
  } else {
    size = dmadrvDataSize1;
  }

  em1RequestAdd(handle);

  // Start receive DMA.
  DMADRV_PeripheralMemory(handle->rxDMACh,
                          handle->rxDMASignal,
                          (void*)buffer,
                          rxPort,
                          true,
                          count,
                          size,
                          RxDMAComplete,
                          handle);

  // Start transmit DMA.
  DMADRV_MemoryPeripheral(handle->txDMACh,
                          handle->txDMASignal,
                          txPort,
                          (void *)&(handle->initData.dummyTxValue),
                          false,
                          count,
                          size,
                          NULL,
                          NULL);
}

/***************************************************************************//**
 * @brief Start an SPI transmit/receive DMA.
 ******************************************************************************/
static void StartTransferDMA(SPIDRV_Handle_t handle,
                             const void *txBuffer,
                             void *rxBuffer,
                             int count,
                             SPIDRV_Callback_t callback)
{
  void *rxPort, *txPort;
  DMADRV_DataSize_t size;

  handle->blockingCompleted  = false;
  handle->transferCount      = count;
  handle->userCallback       = callback;

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    handle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

    if (handle->initData.frameLength > 9) {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDOUBLE);
      txPort = (void *)&(handle->peripheral.usartPort->TXDOUBLE);
    } else if (handle->initData.frameLength == 9) {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDATAX);
      txPort = (void *)&(handle->peripheral.usartPort->TXDATAX);
    } else {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDATA);
      txPort = (void *)&(handle->peripheral.usartPort->TXDATA);
    }
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    clearEusartFifos(handle->peripheral.eusartPort);

    rxPort = (void *)&(handle->peripheral.eusartPort->RXDATA);
    txPort = (void *)&(handle->peripheral.eusartPort->TXDATA);
  }
#endif
  else {
    return;
  }

  if (handle->initData.frameLength > 8) {
    size = dmadrvDataSize2;
  } else {
    size = dmadrvDataSize1;
  }

  em1RequestAdd(handle);

  // Start receive DMA.
  DMADRV_PeripheralMemory(handle->rxDMACh,
                          handle->rxDMASignal,
                          rxBuffer,
                          rxPort,
                          true,
                          count,
                          size,
                          RxDMAComplete,
                          handle);

  // Start transmit DMA.
  DMADRV_MemoryPeripheral(handle->txDMACh,
                          handle->txDMASignal,
                          txPort,
                          (void*)txBuffer,
                          true,
                          count,
                          size,
                          NULL,
                          NULL);
}

/***************************************************************************//**
 * @brief Start an SPI transmit DMA.
 ******************************************************************************/
static void StartTransmitDMA(SPIDRV_Handle_t handle,
                             const void *buffer,
                             int count,
                             SPIDRV_Callback_t callback)
{
  void *rxPort, *txPort;
  DMADRV_DataSize_t size;

  handle->blockingCompleted  = false;
  handle->transferCount      = count;
  handle->userCallback       = callback;

  if (0) {
  }
#if defined(USART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeUsart) {
    handle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

    if (handle->initData.frameLength > 9) {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDOUBLE);
      txPort = (void *)&(handle->peripheral.usartPort->TXDOUBLE);
    } else if (handle->initData.frameLength == 9) {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDATAX);
      txPort = (void *)&(handle->peripheral.usartPort->TXDATAX);
    } else {
      rxPort = (void *)&(handle->peripheral.usartPort->RXDATA);
      txPort = (void *)&(handle->peripheral.usartPort->TXDATA);
    }
  }
#endif
#if defined(EUSART_PRESENT)
  else if (handle->peripheralType == spidrvPeripheralTypeEusart) {
    clearEusartFifos(handle->peripheral.eusartPort);

    rxPort = (void *)&(handle->peripheral.eusartPort->RXDATA);
    txPort = (void *)&(handle->peripheral.eusartPort->TXDATA);
  }
#endif
  else {
    return;
  }

  if (handle->initData.frameLength > 8) {
    size = dmadrvDataSize2;
  } else {
    size = dmadrvDataSize1;
  }

  em1RequestAdd(handle);

  // Receive DMA runs only to get precise numbers for SPIDRV_GetTransferStatus()
  // Start receive DMA.
  DMADRV_PeripheralMemory(handle->rxDMACh,
                          handle->rxDMASignal,
                          &(handle->dummyRx),
                          rxPort,
                          false,
                          count,
                          size,
                          RxDMAComplete,
                          handle);

  // Start transmit DMA.
  DMADRV_MemoryPeripheral(handle->txDMACh,
                          handle->txDMASignal,
                          txPort,
                          (void*)buffer,
                          true,
                          count,
                          size,
                          NULL,
                          NULL);
}

/***************************************************************************//**
 * @brief Parameter checking function for blocking transfer API functions.
 ******************************************************************************/
static Ecode_t TransferApiBlockingPrologue(SPIDRV_Handle_t handle,
                                           void *buffer,
                                           int count)
{
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ((buffer == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if (handle->state != spidrvStateIdle) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }
  handle->state = spidrvStateTransferring;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief Parameter checking function for non-blocking transfer API functions.
 ******************************************************************************/
static Ecode_t TransferApiPrologue(SPIDRV_Handle_t handle,
                                   void *buffer,
                                   int count)
{
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ((buffer == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if (handle->state != spidrvStateIdle) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }
  handle->state = spidrvStateTransferring;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief Wait for transfer completion.
 ******************************************************************************/
static void WaitForTransferCompletion(SPIDRV_Handle_t handle)
{
  if (CORE_IrqIsBlocked(SPI_DMA_IRQ)) {
    // Poll for completion by calling IRQ handler.
    while (handle->blockingCompleted == false) {
#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
      DMA_IRQHandler();
#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
      LDMA_IRQHandler();
#else
#error "No valid SPIDRV DMA engine defined."
#endif
    }
  } else {
    while (handle->blockingCompleted == false) ;
  }
}

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
/***************************************************************************//**
 * @brief Wait for CS deassertion. Used by slave transfer API functions.
 ******************************************************************************/
static Ecode_t WaitForIdleLine(SPIDRV_Handle_t handle)
{
  while (!GPIO_PinInGet((GPIO_Port_TypeDef)handle->portCs, handle->pinCs)
         && (handle->state != spidrvStateIdle)) ;

  if (handle->state == spidrvStateIdle) {
    return handle->transferStatus;
  }

  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && defined(EUSART_PRESENT)
/***************************************************************************//**
 * @brief Enable EUSART and SPI IOs after deepsleep
 ******************************************************************************/
static Ecode_t sli_spidrv_exit_em23(SPIDRV_Handle_t handle)
{
  EUSART_TypeDef *eusart = handle->peripheral.eusartPort;

  EUSART_Enable(eusart, eusartEnable);
  BUS_RegMaskedWrite(&GPIO->EUSARTROUTE[EUSART_NUM(eusart)].ROUTEEN,
                     _GPIO_EUSART_ROUTEEN_TXPEN_MASK | _GPIO_EUSART_ROUTEEN_SCLKPEN_MASK,
                     GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_SCLKPEN);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief Disable SPIO IOs before deepsleep
 ******************************************************************************/
static Ecode_t sli_spidrv_enter_em23(SPIDRV_Handle_t handle)
{
  EUSART_TypeDef *eusart = handle->peripheral.eusartPort;

  BUS_RegMaskedWrite(&GPIO->EUSARTROUTE[EUSART_NUM(eusart)].ROUTEEN,
                     _GPIO_EUSART_ROUTEEN_TXPEN_MASK | _GPIO_EUSART_ROUTEEN_SCLKPEN_MASK,
                     0);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * Power Manager callback notification for EUSART.
 * It is used to prepare EUSART module before/after deepsleeping.
 ******************************************************************************/
static void on_power_manager_event(sl_power_manager_em_t from,
                                   sl_power_manager_em_t to)
{
  (void)from;
  SPIDRV_Handle_t handle;

  if (to == SL_POWER_MANAGER_EM1
      || to == SL_POWER_MANAGER_EM0) {
    SL_SLIST_FOR_EACH_ENTRY(eusart_handle_list, handle, SPIDRV_HandleData_t, node) {
      sli_spidrv_exit_em23(handle);
    }
  }

  if (to == SL_POWER_MANAGER_EM2
      || to == SL_POWER_MANAGER_EM3) {
    SL_SLIST_FOR_EACH_ENTRY(eusart_handle_list, handle, SPIDRV_HandleData_t, node) {
      sli_spidrv_enter_em23(handle);
    }
  }
}
#endif

/// @endcond

/* *INDENT-OFF* */
/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @addtogroup spidrv SPIDRV - SPI Driver
 * @brief Serial Peripheral Interface Driver
 * @{

   @details
   The spidrv.c and spidrv.h source files for the SPI driver library are in the
   emdrv/spidrv folder.

   @li @ref spidrv_intro
   @li @ref spidrv_conf
   @li @ref spidrv_api
   @li @ref spidrv_example

   @n @section spidrv_intro Introduction
   The SPI driver supports the SPI capabilities of EFM32/EZR32/EFR32 USARTs.
   The driver is fully reentrant, supports several driver instances, and
   does not buffer or queue data. Both synchronous and asynchronous transfer
   functions are included for both master and slave SPI mode. Synchronous
   transfer functions are blocking and do
   not return before the transfer is complete. Asynchronous transfer
   functions report transfer completion with callback functions. Transfers are
   handled using DMA.

   @note Transfer completion callback functions are called from within the DMA
   interrupt handler with interrupts disabled.

   @n @section spidrv_conf Configuration Options

   Some properties of the SPIDRV driver are compile-time configurable. These
   properties are stored in a file named spidrv_config.h. A template for
   this file, containing default values, is in the emdrv/config folder.
   Currently the configuration options are as follows:
   @li Inclusion of slave API transfer functions.

   To configure SPIDRV, provide a custom configuration file. This is a
   sample spidrv_config.h file:
   @verbatim
#ifndef __SILICON_LABS_SPIDRV_CONFIG_H__
#define __SILICON_LABS_SPIDRV_CONFIG_H__

// SPIDRV configuration option. Use this define to include the
// slave part of the SPIDRV API.
#define EMDRV_SPIDRV_INCLUDE_SLAVE

#endif
   @endverbatim

   The properties of each SPI driver instance are set at run-time using the
   @ref SPIDRV_Init data structure input parameter to the @ref SPIDRV_Init()
   function.

   @n @section spidrv_api The API

   This section contains brief descriptions of the API functions. For
   detailed information on input and output parameters and return values,
   click on the hyperlinked function names. Most functions return an error
   code, @ref ECODE_EMDRV_SPIDRV_OK is returned on success,
   see ecode.h and spidrv.h for other error codes.

   The application code must include @em spidrv.h.

   @ref SPIDRV_Init(), @ref SPIDRV_DeInit() @n
    These functions initialize or deinitializes the SPIDRV driver. Typically,
    @htmlonly SPIDRV_Init() @endhtmlonly is called once in the startup code.

   @ref SPIDRV_GetTransferStatus() @n
    Query the status of a transfer. Reports number of items (frames) transmitted
    and remaining.

   @ref SPIDRV_AbortTransfer() @n
    Stop an ongoing transfer.

   @ref SPIDRV_SetBitrate(), @ref SPIDRV_GetBitrate() @n
    Set or query the SPI bus bitrate.

   @ref SPIDRV_SetFramelength(), @ref SPIDRV_GetFramelength() @n
    Set or query SPI the bus frame length.

   SPIDRV_MReceive(), SPIDRV_MReceiveB() @n
   SPIDRV_MTransfer(), SPIDRV_MTransferB(), SPIDRV_MTransferSingleItemB() @n
   SPIDRV_MTransmit(), SPIDRV_MTransmitB() @n
   SPIDRV_SReceive(), SPIDRV_SReceiveB() @n
   SPIDRV_STransfer(), SPIDRV_STransferB() @n
   SPIDRV_STransmit(), SPIDRV_STransmitB() @n
    SPI transfer functions for SPI masters have an uppercase M in their name,
    the slave counterparts have an S.

    As previously mentioned, transfer functions are synchronous and asynchronous.
    The synchronous versions have an uppercase B (for Blocking) at the end of
    their function name.

    @em Transmit functions discard received data, @em receive functions transmit
    a fixed data pattern set when the driver is initialized
    (@ref SPIDRV_Init.dummyTxValue). @em Transfer functions both receive and
    transmit data.

    All slave transfer functions have a millisecond timeout parameter. Use 0
    for no (infinite) timeout.

   @n @section spidrv_example Example
   @verbatim
#include "spidrv.h"

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

void TransferComplete(SPIDRV_Handle_t handle,
                      Ecode_t transferStatus,
                      int itemsTransferred)
{
  if (transferStatus == ECODE_EMDRV_SPIDRV_OK) {
   // Success !
  }
}

int main(void)
{
  uint8_t buffer[10];
  SPIDRV_Init_t initData = SPIDRV_MASTER_USART2;

  // Initialize an SPI driver instance.
  SPIDRV_Init(handle, &initData);

  // Transmit data using a blocking transmit function.
  SPIDRV_MTransmitB(handle, buffer, 10);

  // Transmit data using a callback to catch transfer completion.
  SPIDRV_MTransmit(handle, buffer, 10, TransferComplete);
}
   @endverbatim

 * @} end group spidrv ****************************************************/
