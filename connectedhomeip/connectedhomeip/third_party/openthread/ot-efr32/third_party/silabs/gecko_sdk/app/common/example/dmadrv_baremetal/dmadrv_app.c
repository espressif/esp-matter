/***************************************************************************//**
 * @file
 * @brief DMADRV examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "sl_sleeptimer.h"
#include "dmadrv.h"
#include "em_assert.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Max length of one DMA transfer is defined by DMADRV_MAX_XFER_COUNT
#define RX_BUFFER_SIZE (8)
//
#define TX_BUFFER_SIZE (RX_BUFFER_SIZE + 64)

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
//
static unsigned int tx_channel, rx_channel;

// Transfer and reception buffers
static char tx_buffer[TX_BUFFER_SIZE];
static char rx_buffer[RX_BUFFER_SIZE];

// Flag indicating that the DMA data transfer is completed on reception channel
static volatile bool rx_transfer_complete;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
// Callback triggered when DMA transfer on reception channel is complete
static bool rx_callback(unsigned int channel,
                        unsigned int sequence_no,
                        void *user_param)
{
  (void)channel;
  (void)sequence_no;
  (void)&user_param;
  rx_transfer_complete = true;
  // return value is not used for simple (non ping-pong) transfers
  return true;
}

// Function to transfer transmission buffer to USART via DMA
static void transmit_data(void)
{
  bool active;

  DMADRV_TransferActive(tx_channel, &active);

  // wait for any active transfers to finish
  while (active) {
    sl_sleeptimer_delay_millisecond(1);
    DMADRV_TransferActive(tx_channel, &active);
  }

  // Transfer data from tx buffer to USART peripheral
  DMADRV_MemoryPeripheral(tx_channel,
                          dmadrvPeripheralSignal_USART0_TXBL,
                          (void*)&(USART0->TXDATA),
                          tx_buffer,
                          true,
                          strlen(tx_buffer),
                          dmadrvDataSize1,
                          NULL,
                          NULL);
}
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void dmadrv_app_init(void)
{
  DMADRV_Init();
  uint32_t status;

  // Allocate channels for transmission and reception
  status = DMADRV_AllocateChannel(&tx_channel, NULL);
  EFM_ASSERT(status == ECODE_EMDRV_DMADRV_OK);
  status = DMADRV_AllocateChannel(&rx_channel, NULL);
  EFM_ASSERT(status == ECODE_EMDRV_DMADRV_OK);

  // Initialise transfer complete flag
  rx_transfer_complete = false;

  snprintf(tx_buffer, sizeof(tx_buffer), "Welcome to the DMADRV sample application\r\nEnter data\r\n");
  transmit_data();
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void  dmadrv_app_process_action(void)
{
  bool active;

  if (rx_transfer_complete) {
    rx_transfer_complete = false;
    snprintf(tx_buffer, sizeof(tx_buffer), "You wrote: %s\r\nEnter data\r\n", rx_buffer);
    transmit_data();
  }

  DMADRV_TransferActive(rx_channel, &active);

  if (!active) {
    // Start data transfer from USART peripheral to rx buffer
    DMADRV_PeripheralMemory(rx_channel,
                            dmadrvPeripheralSignal_USART0_RXDATAV,
                            rx_buffer,
                            (void*)&(USART0->RXDATA),
                            true,
                            RX_BUFFER_SIZE,
                            dmadrvDataSize1,
                            rx_callback,
                            NULL);
  }
}
