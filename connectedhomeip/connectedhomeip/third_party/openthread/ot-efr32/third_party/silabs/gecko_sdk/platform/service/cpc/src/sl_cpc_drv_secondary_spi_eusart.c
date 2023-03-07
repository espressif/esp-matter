/***************************************************************************/ /**
 * @file
 * @brief CPC SPI SECONDARY implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_status.h"
#include "sl_atomic.h"
#include "sl_slist.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#include "em_eusart.h"
#include "em_ldma.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "spidrv.h"
#include "dmadrv.h"
#include "gpiointerrupt.h"

#include "sli_cpc.h"
#include "sli_cpc_drv.h"
#include "sli_cpc_hdlc.h"
#include "sli_cpc_debug.h"
#include "sl_cpc_config.h"
#include "sl_cpc_drv_secondary_spi_config.h"

#define SL_CPC_CONCAT_PASTER(first, second, third)  first ##  second ## third

#define SL_CPC_TX_IRQn(periph_nbr)                  SL_CPC_CONCAT_PASTER(EUSART, periph_nbr, _TX_IRQn)
#define SL_CPC_ISR_TX_RHANDLER(periph_nbr)          SL_CPC_CONCAT_PASTER(EUSART, periph_nbr, _TX_IRQHandler)

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

typedef struct {
  sl_slist_node_t node;
  sl_cpc_buffer_handle_t *handle;
  uint16_t payload_len;
} sli_buf_entry_t;

typedef enum {
  SPI_STATE_IDLE,
  SPI_STATE_TX,
  SPI_STATE_RX
} sli_spi_state_t;

#define SLI_CPC_DRV_SPI_TYPE                  spidrvSlave
#define SLI_CPC_DRV_SPI_CS_CONTROL            spidrvCsControlAuto

/// Maximum length of one LDMA transfer.
#define DMA_MAX_XFER_LEN (((_LDMA_CH_CTRL_XFERCNT_MASK >> _LDMA_CH_CTRL_XFERCNT_SHIFT) + 1))

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static SPIDRV_HandleData_t spidrv_handle;

static LDMA_TransferCfg_t rx_config;
static LDMA_TransferCfg_t tx_config;

#if (SLI_CPC_RX_DATA_MAX_LENGTH <= DMA_MAX_XFER_LEN)
static LDMA_Descriptor_t rx_descriptor[2u];
#else
static LDMA_Descriptor_t rx_descriptor[3u];
#endif

static LDMA_Descriptor_t tx_descriptor[4u];

static sl_slist_node_t *rx_free_list_head;
static sl_slist_node_t *rx_free_no_buf_list_head;
static sl_slist_node_t *rx_pending_list_head;

static sl_slist_node_t *tx_free_list_head;
static sl_slist_node_t *tx_submitted_list_head;

static sli_buf_entry_t rx_buf_entries_tbl[SL_CPC_DRV_SPI_RX_QUEUE_SIZE];
static sli_buf_entry_t tx_buf_entries_tbl[SL_CPC_DRV_SPI_TX_QUEUE_SIZE];

static sli_buf_entry_t *current_rx_entry;

static volatile sli_spi_state_t current_spi_state = SPI_STATE_IDLE;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

static void sli_cpc_hw_cs_interrupt(uint8_t intNo);
static void sli_cpc_spi_read_data_idle(void);
static bool write_callback(unsigned int channel, unsigned int sequenceNo, void *userParam);
static sl_status_t prepare_next_tx(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initiate SPI Hardware.
 ******************************************************************************/
sl_status_t sli_cpc_drv_init(void)
{
  uint32_t buf_cnt;
  SPIDRV_Init_t init_data;

  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_LDMA, true);

  NVIC_EnableIRQ(SL_CPC_TX_IRQn(SL_CPC_DRV_SPI_PERIPHERAL_NO));

  sl_slist_init(&rx_free_list_head);
  sl_slist_init(&rx_free_no_buf_list_head);
  sl_slist_init(&rx_pending_list_head);
  sl_slist_init(&tx_free_list_head);
  sl_slist_init(&tx_submitted_list_head);

  current_rx_entry = NULL;

  for (buf_cnt = 0; buf_cnt < SL_CPC_DRV_SPI_RX_QUEUE_SIZE; buf_cnt++) {
    sli_buf_entry_t *entry = &rx_buf_entries_tbl[buf_cnt];
    EFM_ASSERT(sli_cpc_get_rx_buffer(&entry->handle) == SL_STATUS_OK);
    sli_cpc_push_buffer_handle(&rx_free_list_head, &entry->node, entry->handle);
  }

  for (buf_cnt = 0; buf_cnt < SL_CPC_DRV_SPI_TX_QUEUE_SIZE; buf_cnt++) {
    sli_buf_entry_t *entry = &tx_buf_entries_tbl[buf_cnt];
    sl_slist_push(&tx_free_list_head, &entry->node);
  }

  init_data = (SPIDRV_Init_t) {
    SL_CPC_DRV_SPI_PERIPHERAL,             ///< The EUSART used for SPI.
#if defined(_EUSART_ROUTELOC0_MASK)
    SL_CPC_DRV_SPI_TX_LOC,   ///< A location number for the SPI Tx pin.
    SL_CPC_DRV_SPI_RX_LOC,   ///< A location number for the SPI Rx pin.
    SL_CPC_DRV_SPI_CLK_LOC,  ///< A location number for the SPI Clk pin.
    SL_CPC_DRV_SPI_CS_LOC,   ///< A location number for the SPI Cs pin.
#elif defined(_GPIO_EUSART_ROUTEEN_MASK)
    SL_CPC_DRV_SPI_TX_PORT,           ///< Tx port.
    SL_CPC_DRV_SPI_RX_PORT,           ///< Rx port.
    SL_CPC_DRV_SPI_CLK_PORT,          ///< Clock port.
    SL_CPC_DRV_SPI_CS_PORT,           ///< Chip select port.
    SL_CPC_DRV_SPI_TX_PIN,            ///< Tx pin.
    SL_CPC_DRV_SPI_RX_PIN,            ///< Rx pin.
    SL_CPC_DRV_SPI_CLK_PIN,           ///< Clock pin.
    SL_CPC_DRV_SPI_CS_PIN,            ///< Chip select pin.
#endif
    SL_CPC_DRV_SPI_BITRATE,          ///< An SPI bitrate.
    SL_CPC_DRV_SPI_FRAME_LENGTH,     ///< An SPI framelength, valid numbers are 4..16
    0,                               ///< The value to transmit when using SPI receive API functions.
    SLI_CPC_DRV_SPI_TYPE,            ///< An SPI type, master or slave.
    SL_CPC_DRV_SPI_BIT_ORDER,        ///< A bit order on the SPI bus, MSB or LSB first.
    SL_CPC_DRV_SPI_CLOCK_MODE,       ///< SPI mode, CLKPOL/CLKPHASE setting.
    SLI_CPC_DRV_SPI_CS_CONTROL,      ///< A select master mode chip select (CS) control scheme.
    spidrvSlaveStartImmediate,       ///< A slave mode transfer start scheme.
  };

  // Configure GPIO mode for CS since it is controlled by the application (this driver)
  GPIO_PinModeSet(SL_CPC_DRV_SPI_CS_PORT, SL_CPC_DRV_SPI_CS_PIN, gpioModeInputPullFilter, 1);

  // RCP RX IRQ line
  GPIO_PinModeSet(SL_CPC_DRV_SPI_RX_IRQ_PORT, SL_CPC_DRV_SPI_RX_IRQ_PIN, gpioModePushPull, 1);

  // Initialize an SPI driver instance with DMA channels.
  SPIDRV_Init(&spidrv_handle, &init_data);

  // Configure DMA transfer
  rx_config = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(spidrv_handle.rxDMASignal);
  tx_config = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(spidrv_handle.txDMASignal);

  // Initialize GPIO interrupt driver
  GPIOINT_Init();

  // Enabled GPIO IRQ
  GPIO_ExtIntConfig(SL_CPC_DRV_SPI_CS_PORT, SL_CPC_DRV_SPI_CS_PIN, SL_CPC_DRV_SPI_CS_RISING_EDGE_INT_NO, true, false, true);
  GPIOINT_CallbackRegister(SL_CPC_DRV_SPI_CS_RISING_EDGE_INT_NO, sli_cpc_hw_cs_interrupt);

  GPIO_ExtIntConfig(SL_CPC_DRV_SPI_CS_PORT, SL_CPC_DRV_SPI_CS_PIN, SL_CPC_DRV_SPI_CS_FALLING_EDGE_INT_NO, false, true, true);
  GPIOINT_CallbackRegister(SL_CPC_DRV_SPI_CS_FALLING_EDGE_INT_NO, sli_cpc_hw_cs_interrupt);

  // Enabled TX complete interrupt
  EUSART_IntEnable(SL_CPC_DRV_SPI_PERIPHERAL, EUSART_IF_TXC);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  // Start read channel
  sli_cpc_spi_read_data_idle();

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * Gets CPC driver capabilities.
 ******************************************************************************/
void sli_cpc_drv_get_capabilities(sli_cpc_drv_capabilities_t *capabilities)
{
  if (capabilities == NULL) {
    return;
  }

  *capabilities = (sli_cpc_drv_capabilities_t){0 };
}

/***************************************************************************/ /**
 * Read bytes from SPI.
 ******************************************************************************/
sl_status_t sli_cpc_drv_read_data(sl_cpc_buffer_handle_t **buffer_handle, uint16_t *payload_rx_len)
{
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  sli_buf_entry_t *entry = (sli_buf_entry_t *)SLI_CPC_POP_BUFFER_HANDLE_LIST(&rx_pending_list_head, sli_buf_entry_t);
  if (entry == NULL) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_EMPTY;
  }
  CORE_EXIT_ATOMIC();

  *buffer_handle = entry->handle;
  *payload_rx_len = SLI_CPC_RX_DATA_MAX_LENGTH;

  CORE_ENTER_ATOMIC();
  status = sli_cpc_get_rx_buffer(&entry->handle);
  if (status == SL_STATUS_OK) {
    sli_cpc_push_buffer_handle(&rx_free_list_head, &entry->node, entry->handle);
  } else {
    sl_slist_push(&rx_free_no_buf_list_head, &entry->node);
  }
  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * Write bytes to SPI.
 ******************************************************************************/
sl_status_t sli_cpc_drv_transmit_data(sl_cpc_buffer_handle_t *buffer_handle, uint16_t payload_tx_len)
{
  sl_status_t status;
  sli_buf_entry_t *entry;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  entry = (sli_buf_entry_t *)sl_slist_pop(&tx_free_list_head);
  CORE_EXIT_ATOMIC();

  if (entry == NULL) {
    return SL_STATUS_NOT_READY;
  }

  entry->handle = buffer_handle;
  entry->payload_len = payload_tx_len;

  CORE_ENTER_ATOMIC();
  sli_cpc_push_back_buffer_handle(&tx_submitted_list_head, &entry->node, entry->handle);

  if (current_spi_state != SPI_STATE_IDLE) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_OK;
  }

  status = prepare_next_tx();
  if (status == SL_STATUS_RECEIVE) {
    status = SL_STATUS_OK;
  }
  CORE_EXIT_ATOMIC();

  return status;
}

/***************************************************************************/ /**
 * Checks if driver is ready to transmit.
 ******************************************************************************/
bool sli_cpc_drv_is_transmit_ready(void)
{
  sl_slist_node_t *head;

  sl_atomic_load(head, tx_free_list_head);

  return (head != NULL);
}

/***************************************************************************/ /**
 * Prepare for reception when entering idle.
 ******************************************************************************/
static void sli_cpc_spi_read_data_idle(void)
{
  Ecode_t ecode;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (current_spi_state != SPI_STATE_IDLE) {
    CORE_EXIT_ATOMIC();
    return;
  }

  current_rx_entry = (sli_buf_entry_t *)SLI_CPC_POP_BUFFER_HANDLE_LIST(&rx_free_list_head, sli_buf_entry_t);

  if (current_rx_entry != NULL) {
#if (SLI_CPC_RX_DATA_MAX_LENGTH <= DMA_MAX_XFER_LEN)
    rx_descriptor[0u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(SL_CPC_DRV_SPI_PERIPHERAL->RXDATA), current_rx_entry->handle->hdlc_header, SLI_CPC_HDLC_HEADER_RAW_SIZE, 1);
    rx_descriptor[1u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(SL_CPC_DRV_SPI_PERIPHERAL->RXDATA), current_rx_entry->handle->data, SLI_CPC_RX_DATA_MAX_LENGTH);
    rx_descriptor[0].xfer.doneIfs = 0u;
    rx_descriptor[1].xfer.doneIfs = 0u;
#else
    rx_descriptor[0u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(SL_CPC_DRV_SPI_PERIPHERAL->RXDATA), current_rx_entry->handle->hdlc_header, SLI_CPC_HDLC_HEADER_RAW_SIZE, 1);
    rx_descriptor[1u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(SL_CPC_DRV_SPI_PERIPHERAL->RXDATA), current_rx_entry->handle->data, DMADRV_MAX_XFER_COUNT, 1);
    rx_descriptor[2u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(SL_CPC_DRV_SPI_PERIPHERAL->RXDATA), &((uint8_t *)current_rx_entry->handle->data)[DMADRV_MAX_XFER_COUNT], SLI_CPC_RX_DATA_MAX_LENGTH - DMADRV_MAX_XFER_COUNT);
    rx_descriptor[0].xfer.doneIfs = 0u;
    rx_descriptor[1].xfer.doneIfs = 0u;
    rx_descriptor[2].xfer.doneIfs = 0u;
#endif

    SL_CPC_DRV_SPI_PERIPHERAL->CMD = EUSART_CMD_CLEARTX;

    while ((SL_CPC_DRV_SPI_PERIPHERAL->STATUS & EUSART_STATUS_CLEARTXBUSY) != 0U) {
    }

    // Read data until FIFO is emptied
    // but taking care not to underflow the receiver
    while (SL_CPC_DRV_SPI_PERIPHERAL->STATUS & EUSART_STATUS_RXFL) {
      SL_CPC_DRV_SPI_PERIPHERAL->RXDATA;
    }

    ecode = DMADRV_LdmaStartTransfer(spidrv_handle.rxDMACh,
                                     &rx_config,
                                     rx_descriptor,
                                     NULL,
                                     NULL);
    EFM_ASSERT(ecode == ECODE_OK);
  }
  CORE_EXIT_ATOMIC();
}

/***************************************************************************/ /**
 * Chip select interrupt handler.
 ******************************************************************************/
static void sli_cpc_hw_cs_interrupt(uint8_t intNo)
{
  CORE_DECLARE_IRQ_STATE;

  if (GPIO_PinOutGet(SL_CPC_DRV_SPI_RX_IRQ_PORT, SL_CPC_DRV_SPI_RX_IRQ_PIN) == 0) {
    return;
  }

  if (intNo == SL_CPC_DRV_SPI_CS_RISING_EDGE_INT_NO) {
    // End of transfer
    if (current_spi_state == SPI_STATE_RX) {
      DMADRV_StopTransfer(spidrv_handle.rxDMACh);

      CORE_ENTER_ATOMIC();
      if (current_rx_entry != NULL) {
        sli_cpc_push_back_buffer_handle(&rx_pending_list_head, &current_rx_entry->node, current_rx_entry->handle);
        current_rx_entry = NULL;

        // Notify core
        sli_cpc_drv_notify_rx_data();
      } else {
        SLI_CPC_DEBUG_TRACE_CORE_DRIVER_PACKET_DROPPED();
      }
      CORE_EXIT_ATOMIC();
    } else if (current_spi_state == SPI_STATE_TX) {
      sli_buf_entry_t *entry;

      CORE_ENTER_ATOMIC();
      entry = (sli_buf_entry_t *)SLI_CPC_POP_BUFFER_HANDLE_LIST(&tx_submitted_list_head, sli_buf_entry_t);
      CORE_EXIT_ATOMIC();

      // Notify core
      sli_cpc_drv_notify_tx_complete(entry->handle);

      CORE_ENTER_ATOMIC();
      sl_slist_push(&tx_free_list_head, &entry->node);
      CORE_EXIT_ATOMIC();
    }

    current_spi_state = SPI_STATE_IDLE;

    sli_cpc_spi_read_data_idle();

    if (tx_submitted_list_head) {
      (void)prepare_next_tx();
    }
  } else if (intNo == SL_CPC_DRV_SPI_CS_FALLING_EDGE_INT_NO) {
    if (current_spi_state == SPI_STATE_IDLE) {
      current_spi_state = SPI_STATE_RX;
    }
  }
}

/***************************************************************************/ /**
 * DMA write complete callback.
 ******************************************************************************/
static bool write_callback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  (void)sequenceNo;
  (void)userParam;

  DMADRV_StopTransfer(channel);

  return false;
}

/***************************************************************************/ /**
 * Prepare for transmission of next buffer.
 ******************************************************************************/
static sl_status_t prepare_next_tx(void)
{
  Ecode_t code;
  sli_buf_entry_t *entry;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  entry = (sli_buf_entry_t *)tx_submitted_list_head;
  if (entry == NULL) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_EMPTY;
  }

  if (entry->payload_len <= DMADRV_MAX_XFER_COUNT && entry->payload_len != 0u) {
    tx_descriptor[0u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(entry->handle->hdlc_header, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), SLI_CPC_HDLC_HEADER_RAW_SIZE, 1);
    tx_descriptor[1u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(entry->handle->data, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), entry->payload_len, 1u);
    tx_descriptor[2u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(entry->handle->fcs, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), 2u);
    tx_descriptor[0u].xfer.doneIfs = 0u;
    tx_descriptor[1u].xfer.doneIfs = 0u;
    tx_descriptor[2u].xfer.doneIfs = 1u;
  } else if (entry->payload_len <= (DMADRV_MAX_XFER_COUNT * 2) && entry->payload_len > 0u) {
    tx_descriptor[0u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(entry->handle->hdlc_header, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), SLI_CPC_HDLC_HEADER_RAW_SIZE, 1);
    tx_descriptor[1u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(entry->handle->data, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), DMADRV_MAX_XFER_COUNT, 1u);
    tx_descriptor[2u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&((uint8_t *)entry->handle->data)[DMADRV_MAX_XFER_COUNT], &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), (entry->payload_len - DMADRV_MAX_XFER_COUNT), 1u);
    tx_descriptor[3u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(entry->handle->fcs, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), 2u);
    tx_descriptor[0u].xfer.doneIfs = 0u;
    tx_descriptor[1u].xfer.doneIfs = 0u;
    tx_descriptor[2u].xfer.doneIfs = 0u;
    tx_descriptor[3u].xfer.doneIfs = 1u;
  } else if (entry->payload_len == 0u) {
    tx_descriptor[0u] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(entry->handle->hdlc_header, &(SL_CPC_DRV_SPI_PERIPHERAL->TXDATA), 7u);
    tx_descriptor[0u].xfer.doneIfs = 1u;
  } else {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Did host decided to transmit something?
  if (GPIO_PinInGet(SL_CPC_DRV_SPI_CS_PORT, SL_CPC_DRV_SPI_CS_PIN) == 0u) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_RECEIVE;
  }

  // RX IRQ line
  GPIO_PinOutClear(SL_CPC_DRV_SPI_RX_IRQ_PORT, SL_CPC_DRV_SPI_RX_IRQ_PIN);

  current_spi_state = SPI_STATE_TX;

  // Stop RX
  if (current_rx_entry) {
    DMADRV_StopTransfer(spidrv_handle.rxDMACh);

    sli_cpc_push_buffer_handle(&rx_free_list_head, &current_rx_entry->node, current_rx_entry->handle);
    current_rx_entry = NULL;
  }

  SL_CPC_DRV_SPI_PERIPHERAL->CMD = EUSART_CMD_CLEARTX;

  EUSART_IntClear(SL_CPC_DRV_SPI_PERIPHERAL, EUSART_IF_TXC);

  code = DMADRV_LdmaStartTransfer(spidrv_handle.txDMACh,
                                  &tx_config,
                                  tx_descriptor,
                                  write_callback,
                                  NULL);
  EFM_ASSERT(code == ECODE_OK);

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * Notification when RX buffer becomes free.
 ******************************************************************************/
void sli_cpc_memory_on_rx_buffer_free(void)
{
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (rx_free_no_buf_list_head == NULL) {
    CORE_EXIT_ATOMIC();
    return;
  }

  do {
    sli_buf_entry_t *entry = (sli_buf_entry_t *)rx_free_no_buf_list_head;

    status = sli_cpc_get_rx_buffer(&entry->handle);
    if (status == SL_STATUS_OK) {
      (void)sl_slist_pop(&rx_free_no_buf_list_head);
      sli_cpc_push_buffer_handle(&rx_free_list_head, &entry->node, entry->handle);
    }
  } while (status == SL_STATUS_OK && rx_free_no_buf_list_head != NULL);
  CORE_EXIT_ATOMIC();
}

/***************************************************************************//**
 * TX IRQ handler.
 ******************************************************************************/
void SL_CPC_ISR_TX_RHANDLER(SL_CPC_DRV_SPI_PERIPHERAL_NO)(void)
{
  uint32_t flag = EUSART_IntGet(SL_CPC_DRV_SPI_PERIPHERAL);

  EUSART_IntClear(SL_CPC_DRV_SPI_PERIPHERAL, EUSART_IF_TXC);

  if (flag & EUSART_IF_TXC) {
    // Notify HOST that the transmission is over
    GPIO_PinOutSet(SL_CPC_DRV_SPI_RX_IRQ_PORT, SL_CPC_DRV_SPI_RX_IRQ_PIN);
  }
}
