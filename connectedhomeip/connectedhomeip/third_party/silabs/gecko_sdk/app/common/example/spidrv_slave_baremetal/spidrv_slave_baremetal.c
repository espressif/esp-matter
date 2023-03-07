/***************************************************************************//**
 * @file
 * @brief spidrv slave baremetal examples functions
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
#include <string.h>
#include <stdio.h>
#include "spidrv_slave_baremetal.h"
#include "spidrv.h"
#include "sl_spidrv_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// use SPI handle for EXP header (configured in project settings)
#define SPI_HANDLE sl_spidrv_exp_handle

// size of transmission and reception buffers
#define APP_BUFFER_SIZE             16

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Flag to signal that transfer is complete
static volatile bool transfer_complete = false;

// Data counter
static int counter = 0;

// Transmission and reception buffers
static char rx_buffer[APP_BUFFER_SIZE];
static char tx_buffer[APP_BUFFER_SIZE];

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

// Callback fired when transfer is complete
void transfer_callback(SPIDRV_HandleData_t *handle,
                       Ecode_t transfer_status,
                       int items_transferred)
{
  (void)&handle;
  (void)items_transferred;

  // Post semaphore to signal to application
  // task that transfer is successful
  if (transfer_status == ECODE_EMDRV_SPIDRV_OK) {
    transfer_complete = true;
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * Initialize example.
 ******************************************************************************/
void spidrv_app_init(void)
{
  // stdout is redirected to VCOM in project configuration
  printf("Welcome to the SPIDRV example application, slave mode\r\n");
}

/***************************************************************************//**
 * Ticking function
 ******************************************************************************/
void spidrv_app_process_action(void)
{
  Ecode_t ecode;

  // send a string that includes an incrementing counter
  sprintf(tx_buffer, "pong %03d", counter);
  counter++;
  printf("Sending %s to master...\r\n", tx_buffer);

  transfer_complete = false;

  // Non-blocking data transfer to master. When complete, rx buffer
  // will be filled.
  ecode = SPIDRV_STransfer(SPI_HANDLE, tx_buffer, rx_buffer, APP_BUFFER_SIZE, transfer_callback, 0);
  EFM_ASSERT(ecode == ECODE_OK);

  // wait for transfer to complete
  while (!transfer_complete) ;

  // Data from master is in rx_buffer
  printf("Got message from master: %s\r\n", rx_buffer);
}
