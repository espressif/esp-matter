/***************************************************************************//**
 * @file
 * @brief spidrv slave micriumos examples functions
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
#include "spidrv_slave_micriumos.h"
#include "spidrv.h"
#include "sl_spidrv_instances.h"
#include "os.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// MicriumOS task configuration
#define SPIDRV_TASK_STACK_SIZE      256
#define SPIDRV_TASK_PRIO            20

// use SPI handle for EXP header (configured in project settings)
#define SPI_HANDLE sl_spidrv_eusart_exp_handle

// size of transmission and reception buffers
#define APP_BUFFER_SIZE             16

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[SPIDRV_TASK_STACK_SIZE];

// Semaphore to signal that transmission is complete
static OS_SEM  tx_semaphore;

// Transmission and reception buffers
static char rx_buffer[APP_BUFFER_SIZE];
static char tx_buffer[APP_BUFFER_SIZE];

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

// Application task
static void spidrv_app_task(void *arg);

// Callback fired when transfer is complete
void transfer_callback(SPIDRV_HandleData_t *handle,
                       Ecode_t transfer_status,
                       int items_transferred)
{
  RTOS_ERR err;
  (void)&handle;
  (void)items_transferred;

  // Post semaphore to signal to application
  // task that transfer is successful
  if (transfer_status == ECODE_EMDRV_SPIDRV_OK) {
    OSSemPost(&tx_semaphore,
              OS_OPT_POST_1,
              &err);
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
  RTOS_ERR err;

  OSSemCreate(&tx_semaphore, "transfer semaphore", (OS_SEM_CTR)0, &err);

  OSTaskCreate(&tcb,
               "spidrv app task",
               spidrv_app_task,
               DEF_NULL,
               SPIDRV_TASK_PRIO,
               &stack[0],
               (SPIDRV_TASK_STACK_SIZE / 10u),
               SPIDRV_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 * spidrv task.
 ******************************************************************************/
static void spidrv_app_task(void *arg)
{
  RTOS_ERR err;
  CPU_TS  ts;
  Ecode_t ecode;
  (void)&arg;
  int counter = 0;

  // stdout is redirected to VCOM in project configuration
  printf("Welcome to the SPIDRV example application, slave mode\r\n");

  while (1) {
    // send a string that includes an incrementing counter
    sprintf(tx_buffer, "pong %03d", counter);
    counter++;
    printf("Sending %s to master...\r\n", tx_buffer);

    // Non-blocking data transfer to master. When complete, rx buffer
    // will be filled.
    ecode = SPIDRV_STransfer(SPI_HANDLE, tx_buffer, rx_buffer, APP_BUFFER_SIZE, transfer_callback, 0);
    EFM_ASSERT(ecode == ECODE_OK);

    // Wait for semaphore indicating that transfer is complete
    OSSemPend(&tx_semaphore,
              0,
              OS_OPT_PEND_BLOCKING,
              &ts,
              &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    // Data from master is in rx_buffer
    printf("Got message from master: %s\r\n", rx_buffer);
  }
}
