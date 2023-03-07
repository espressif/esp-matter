/*
 *  Copyright (c) 2022, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for SPI communication.
 *
 */

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "em_core.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_ldma.h"

#include "gpiointerrupt.h"
#include "spidrv.h"

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#include "sl_ncp_spidrv_usart_config.h"

#include "error.h"
#include "spi-slave.h"
#include "openthread-system.h"
#include "utils/code_utils.h"
#include "platform-efr32.h"

// DEFINES
#define MAX_DMA_DESCRIPTOR_TRANSFER_COUNT                       ((_LDMA_CH_CTRL_XFERCNT_MASK >> _LDMA_CH_CTRL_XFERCNT_SHIFT) + 1U)

#define SL_OT_SPIDRV_SPI_CONCAT_PASTER(first, second, third)    first ##  second ## third

#define SL_OT_SPIDRV_SPI_LDMA_RX_PERIPH_TRIGGER(periph_nbr)   SL_OT_SPIDRV_SPI_CONCAT_PASTER(ldmaPeripheralSignal_USART, periph_nbr, _RXDATAV)
#define SL_OT_SPIDRV_SPI_LDMA_TX_PERIPH_TRIGGER(periph_nbr)   SL_OT_SPIDRV_SPI_CONCAT_PASTER(ldmaPeripheralSignal_USART, periph_nbr, _TXBL)

//  MEMBER VARIABLES
static volatile bool should_process_transaction = false;

static volatile bool transaction_active;

// DMA
static LDMA_TransferCfg_t rx_dma_transfer_config;
static LDMA_TransferCfg_t tx_dma_transfer_config;

static uint8_t default_tx_value;
static volatile LDMA_Descriptor_t tx_descriptor[2];

static volatile LDMA_Descriptor_t rx_descriptor;

// TRANSACTION EVENT'S CALLBACK
static volatile otPlatSpiSlaveTransactionCompleteCallback complete_callback;
static volatile otPlatSpiSlaveTransactionProcessCallback process_callback;
static volatile void *context;

// SPI Peripheral
static volatile SPIDRV_HandleData_t sl_spidrv_handle_data;

static void rcp_spidrv_set_host_request(void)
{
#if defined(SL_NCP_SPIDRV_USART_HOST_INT_PORT) && defined(SL_NCP_SPIDRV_USART_HOST_INT_PIN)
  GPIO_PinOutClear(SL_NCP_SPIDRV_USART_HOST_INT_PORT,
                   SL_NCP_SPIDRV_USART_HOST_INT_PIN);
#else
  int empty = 0;
  (void)empty;
#endif
}

static void rcp_spidrv_deassert_host_request(void)
{
#if defined(SL_NCP_SPIDRV_USART_HOST_INT_PORT) && defined(SL_NCP_SPIDRV_USART_HOST_INT_PIN)
  GPIO_PinOutSet(SL_NCP_SPIDRV_USART_HOST_INT_PORT, SL_NCP_SPIDRV_USART_HOST_INT_PIN);
#else
  int empty = 0;
  (void)empty;
#endif
}

static void rcp_spidrv_spi_transaction_end_interrupt(uint8_t intNo)
{
  if (intNo == SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO) 
  {
    transaction_active = true;
    return;
  } else if (intNo == SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO) {
    // Must be done before calling the "complete_callback" since
    // this callback will use otPlatSpiSlavePrepareTransaction who
    // would not setup the buffers if a transaction is ongoing.
    transaction_active = false;
    rcp_spidrv_deassert_host_request();
  }

  uint32_t tx_transaction_size = 0U;

  LDMA_StopTransfer(sl_spidrv_handle_data.txDMACh);

  uint32_t tx_dma_channel_nb = sl_spidrv_handle_data.txDMACh;

  // Use the current loader DMA transfer struct to asses the number of transfered bytes.
  uint32_t tx_dma_channel_remaining_xfercnt = (LDMA->CH[tx_dma_channel_nb].CTRL & _LDMA_CH_CTRL_XFERCNT_MASK) >> _LDMA_CH_CTRL_XFERCNT_SHIFT;
  tx_dma_channel_remaining_xfercnt += 1U;
  tx_dma_channel_remaining_xfercnt += (sl_spidrv_handle_data.peripheral.usartPort->STATUS & _USART_STATUS_TXBUFCNT_MASK) >> _USART_STATUS_TXBUFCNT_SHIFT;

  uint32_t current_tx_descritor_link = (LDMA->CH[tx_dma_channel_nb].LINK & _LDMA_CH_LINK_LINK_MASK) >> _LDMA_CH_LINK_LINK_SHIFT;

  uint8_t *old_tx_buffer = (uint8_t*) tx_descriptor[0].xfer.srcAddr;
  uint16_t old_tx_buffer_size = tx_descriptor[0].xfer.xferCnt + 1U;

  uint8_t *old_rx_buffer = (uint8_t*) rx_descriptor.xfer.dstAddr;
  uint16_t old_rx_buffer_size = rx_descriptor.xfer.xferCnt + 1U;

  if (current_tx_descritor_link == 0U) { 
    // Since the link bit is not set in the tx dma channel descriptor the second tx descriptor was loaded.
    tx_transaction_size = tx_descriptor[0].xfer.xferCnt + 1U;
    tx_transaction_size += (tx_descriptor[1].xfer.xferCnt + 1U) - tx_dma_channel_remaining_xfercnt;
  } else {
    tx_transaction_size = (tx_descriptor[0].xfer.xferCnt - tx_dma_channel_remaining_xfercnt) + 1U;
  }

  // call's otPlatSpiSlavePrepareTransaction in the background, the DMA buffer's will be ready after this call.
  if (complete_callback((void*) context, (uint8_t*) old_tx_buffer, old_tx_buffer_size, (uint8_t*) old_rx_buffer, old_rx_buffer_size, tx_transaction_size)) {
    otSysEventSignalPending();
    should_process_transaction = true;
  }
}

otError otPlatSpiSlaveEnable(otPlatSpiSlaveTransactionCompleteCallback aCompleteCallback, otPlatSpiSlaveTransactionProcessCallback aProcessCallback, void *aContext)
{
  CORE_DECLARE_IRQ_STATE;
  otError error = OT_ERROR_NONE;

  // If driver was already configured, an error is returned.
  otEXPECT_ACTION(complete_callback == NULL, error = OT_ERROR_ALREADY);
  otEXPECT_ACTION(process_callback == NULL, error = OT_ERROR_ALREADY);
  otEXPECT_ACTION(context == NULL, error = OT_ERROR_ALREADY);

  CMU_ClockEnable(cmuClock_GPIO, true);

  SPIDRV_Init_t init_data = (SPIDRV_Init_t)
  {
    SL_NCP_SPIDRV_USART_PERIPHERAL, ///< The USART used for SPI.
#if defined(_USART_ROUTELOC0_MASK)
    SL_NCP_SPIDRV_USART_TX_LOC,     ///< A location number for the SPI Tx pin.
    SL_NCP_SPIDRV_USART_RX_LOC,     ///< A location number for the SPI Rx pin.
    SL_NCP_SPIDRV_USART_CLK_LOC,    ///< A location number for the SPI Clk pin.
    SL_NCP_SPIDRV_USART_CS_LOC,     ///< A location number for the SPI Cs pin.
#elif defined(_GPIO_USART_ROUTEEN_MASK)
    SL_NCP_SPIDRV_USART_TX_PORT,    ///< Tx port.
    SL_NCP_SPIDRV_USART_RX_PORT,    ///< Rx port.
    SL_NCP_SPIDRV_USART_CLK_PORT,   ///< Clock port.
    SL_NCP_SPIDRV_USART_CS_PORT,    ///< Chip select port.
    SL_NCP_SPIDRV_USART_TX_PIN,     ///< Tx pin.
    SL_NCP_SPIDRV_USART_RX_PIN,     ///< Rx pin.
    SL_NCP_SPIDRV_USART_CLK_PIN,    ///< Clock pin.
    SL_NCP_SPIDRV_USART_CS_PIN,     ///< Chip select pin.
#endif
    0U,                             ///< An SPI bitrate.
    8,                              ///< An SPI framelength, valid numbers are 4..16
    0,                              ///< The value to transmit when using SPI receive API functions.
    spidrvSlave,                    ///< An SPI type, slave.
    SL_NCP_SPIDRV_USART_BIT_ORDER,  ///< A bit order on the SPI bus, MSB or LSB first.
    SL_NCP_SPIDRV_USART_CLOCK_MODE, ///< SPI mode, CLKPOL/CLKPHASE setting.
    spidrvCsControlAuto,            ///< A select master mode chip select (CS) control scheme.
    spidrvSlaveStartImmediate,      ///< A slave mode transfer start scheme.
  };

  otEXPECT_ACTION(SPIDRV_Init((SPIDRV_HandleData_t *)&sl_spidrv_handle_data, &init_data) == ECODE_EMDRV_SPIDRV_OK, error = OT_ERROR_FAILED);

  // Client callback functions.
  complete_callback = aCompleteCallback;
  process_callback = aProcessCallback;
  context = aContext;

  // Client complete callback request foreground processing.
  should_process_transaction = false;

  // SPI transaction status.
  transaction_active = false;

  // TX default value.
  default_tx_value = 0xFFU;

  // DMA transfer structures.
  rx_dma_transfer_config = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(SL_OT_SPIDRV_SPI_LDMA_RX_PERIPH_TRIGGER(SL_NCP_SPIDRV_USART_PERIPHERAL_NO));
  tx_dma_transfer_config = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(SL_OT_SPIDRV_SPI_LDMA_TX_PERIPH_TRIGGER(SL_NCP_SPIDRV_USART_PERIPHERAL_NO));

  rx_descriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(sl_spidrv_handle_data.peripheral.usartPort->RXDATA), NULL, 1U);
  rx_descriptor.xfer.doneIfs = 0U;

  tx_descriptor[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&default_tx_value, &(sl_spidrv_handle_data.peripheral.usartPort->TXDATA), 1, 1);
  tx_descriptor[0].xfer.doneIfs = 0U;

  tx_descriptor[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(&default_tx_value, &(sl_spidrv_handle_data.peripheral.usartPort->TXDATA), MAX_DMA_DESCRIPTOR_TRANSFER_COUNT);
  tx_descriptor[1].xfer.srcInc = ldmaCtrlSrcIncNone;
  tx_descriptor[1].xfer.doneIfs = 0U;

  // Configuring Host INT line. Active low
#if defined(SL_NCP_SPIDRV_USART_HOST_INT_PORT) && defined(SL_NCP_SPIDRV_USART_HOST_INT_PIN)
  GPIO_PinModeSet(SL_NCP_SPIDRV_USART_HOST_INT_PORT, SL_NCP_SPIDRV_USART_HOST_INT_PIN, gpioModePushPull, 1U);
#endif

  GPIOINT_Init();

  GPIO_PinModeSet(SL_NCP_SPIDRV_USART_CS_PORT, SL_NCP_SPIDRV_USART_CS_PIN, gpioModeInputPullFilter, 1);

  CORE_ENTER_ATOMIC();

  // Initialization during transaction is not supported.
  otEXPECT_ACTION(GPIO_PinInGet(SL_NCP_SPIDRV_USART_CS_PORT, SL_NCP_SPIDRV_USART_CS_PIN) != 0U, error = OT_ERROR_FAILED);

  GPIO_ExtIntConfig(SL_NCP_SPIDRV_USART_CS_PORT, SL_NCP_SPIDRV_USART_CS_PIN, SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO, true, false, true);
  GPIO_ExtIntConfig(SL_NCP_SPIDRV_USART_CS_PORT, SL_NCP_SPIDRV_USART_CS_PIN, SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO, false, true, true);

  GPIOINT_CallbackRegister(SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO, rcp_spidrv_spi_transaction_end_interrupt);
  GPIOINT_CallbackRegister(SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO, rcp_spidrv_spi_transaction_end_interrupt);

  // Clear the usart RX/TX FIFO before configuring the dma transfers.
  sl_spidrv_handle_data.peripheral.usartPort->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;

  // Load the default value descriptor.
  LDMA_StartTransfer(sl_spidrv_handle_data.txDMACh, (LDMA_TransferCfg_t*) &tx_dma_transfer_config, (LDMA_Descriptor_t*) &tx_descriptor[1]);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  CORE_EXIT_ATOMIC();

  exit:
  return error;
}

void otPlatSpiSlaveDisable(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  // Disable CS GPIO IRQ.
  GPIO_ExtIntConfig(SL_NCP_SPIDRV_USART_CS_PORT, SL_NCP_SPIDRV_USART_CS_PIN, SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO, true, false, false);
  GPIO_ExtIntConfig(SL_NCP_SPIDRV_USART_CS_PORT, SL_NCP_SPIDRV_USART_CS_PIN, SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO, false, true, false);

  GPIOINT_CallbackUnRegister(SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO);
  GPIOINT_CallbackUnRegister(SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO);

  CORE_EXIT_ATOMIC();

  // Stop any reception DMA transfers.
  LDMA_StopTransfer(sl_spidrv_handle_data.txDMACh);
  LDMA_StopTransfer(sl_spidrv_handle_data.rxDMACh);

  // Host INT line.
  rcp_spidrv_deassert_host_request();
#if defined(SL_NCP_SPIDRV_USART_HOST_INT_PORT) && defined(SL_NCP_SPIDRV_USART_HOST_INT_PIN)
  GPIO_PinModeSet(SL_NCP_SPIDRV_USART_HOST_INT_PORT, SL_NCP_SPIDRV_USART_HOST_INT_PIN, gpioModeInput, 0U);
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  should_process_transaction = false;
  transaction_active = false;

  complete_callback = NULL;
  process_callback = NULL;
  context = NULL;
}

otError otPlatSpiSlavePrepareTransaction(uint8_t *aOutputBuf, uint16_t aOutputBufLen, uint8_t *aInputBuf, uint16_t aInputBufLen, bool aRequestTransactionFlag)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  otError error = OT_ERROR_NONE;

  otEXPECT_ACTION(!transaction_active, error = OT_ERROR_BUSY);
  otEXPECT_ACTION(aOutputBufLen <= MAX_DMA_DESCRIPTOR_TRANSFER_COUNT, error = OT_ERROR_FAILED);
  otEXPECT_ACTION(aInputBufLen <= MAX_DMA_DESCRIPTOR_TRANSFER_COUNT, error = OT_ERROR_FAILED);

  uint32_t tx_dma_channel_number = sl_spidrv_handle_data.txDMACh;
  uint32_t rx_dma_channel_number = sl_spidrv_handle_data.rxDMACh;

  if (aInputBuf != NULL) {
    LDMA_StopTransfer(rx_dma_channel_number);
    sl_spidrv_handle_data.peripheral.usartPort->CMD = USART_CMD_CLEARRX;

    rx_descriptor.xfer.xferCnt = aInputBufLen - 1U;
    rx_descriptor.xfer.dstAddr = (uint32_t) aInputBuf;

    LDMA_StartTransfer(rx_dma_channel_number, (LDMA_TransferCfg_t*) &rx_dma_transfer_config, (LDMA_Descriptor_t*) &rx_descriptor);
  }

  if (aOutputBuf != NULL) {
    LDMA_StopTransfer(tx_dma_channel_number);
    sl_spidrv_handle_data.peripheral.usartPort->CMD = USART_CMD_CLEARTX;

    tx_descriptor[0].xfer.xferCnt = aOutputBufLen - 1U;
    tx_descriptor[0].xfer.srcAddr = (uint32_t) aOutputBuf;

    LDMA_StartTransfer(tx_dma_channel_number, (LDMA_TransferCfg_t*) &tx_dma_transfer_config, (LDMA_Descriptor_t*) &(tx_descriptor[0]));
  }

  if (aRequestTransactionFlag) {
    rcp_spidrv_set_host_request();
  } else {
    rcp_spidrv_deassert_host_request();
  }

  exit:

  CORE_EXIT_ATOMIC();
  return error;
}

void efr32SpiProcess(void)
{
  if (should_process_transaction)
  {
    if (context) {
      process_callback((void*) context);
    }

    should_process_transaction = false;
  }
}
