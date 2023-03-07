/***************************************************************************//**
 * @file sl_wfx_task.c
 * @brief WFX FMAC driver main bus communication task
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "em_gpio.h"

#include "sl_wfx.h"
#include "sl_wfx_host_pinout.h"
#include "sl_wfx_task.h"
#include "sl_wfx_host.h"

/* Bus Task Configurations*/
#define SL_WFX_BUS_TASK_PRIO              15u
#define SL_WFX_BUS_TASK_STK_SIZE         512u
#define SL_WFX_EVENT_TIMEOUT_MS           (0)

/* wfx bus task stack*/
static CPU_STK sl_wfx_bus_task_stk[SL_WFX_BUS_TASK_STK_SIZE];
/* wfx bus task TCB*/
static OS_TCB sl_wfx_bus_task_tcb;

OS_FLAG_GRP           bus_events;
sl_wfx_frame_q_item   bus_tx_frame;
OS_SEM                bus_tx_complete_sem;
OS_MUTEX              sl_wfx_tx_queue_mutex;
sl_wfx_packet_queue_t sl_wfx_tx_queue_context;

static void        sl_wfx_task_entry (void *p_arg);
static sl_status_t sl_wfx_rx_process (uint16_t control_register);
static sl_status_t sl_wfx_tx_process (void);

/**************************************************************************//**
 * Wfx process task entry
 *****************************************************************************/
static void sl_wfx_task_entry(void *p_arg)
{
  RTOS_ERR err;
  uint16_t control_register = 0;
  OS_FLAGS  flags = 0;

  (void)p_arg;

  sl_wfx_host_setup_memory_pools();

  while (1) {
#ifdef SLEEP_ENABLED
#ifdef SL_WFX_USE_SPI
    if (GPIO_PinInGet(SL_WFX_HOST_PINOUT_SPI_WIRQ_PORT, SL_WFX_HOST_PINOUT_SPI_WIRQ_PIN)) //wfx messages pending
#else
    if (GPIO_PinInGet(SL_WFX_HOST_PINOUT_GPIO_WIRQ_PORT, SL_WFX_HOST_PINOUT_GPIO_WIRQ_PIN))
#endif
    {
      OSFlagPost(&bus_events, SL_WFX_BUS_EVENT_FLAG_RX, OS_OPT_POST_FLAG_SET, &err);
    }
#endif
    /*Wait for an event*/
    flags = OSFlagPend(&bus_events, 0xF, SL_WFX_EVENT_TIMEOUT_MS,
                       OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_BLOCKING | OS_OPT_PEND_FLAG_CONSUME,
                       0, &err);
    if (flags & SL_WFX_BUS_EVENT_FLAG_TX) {
      /* Process TX packets */
      sl_wfx_tx_process();
    }
    if (flags & SL_WFX_BUS_EVENT_FLAG_RX) {
      /* Process RX packets */
      sl_wfx_rx_process(control_register);
#ifdef SL_WFX_USE_SDIO
      /* Reenable interrupt (req for sdio)*/
      sl_wfx_host_enable_platform_interrupt();
#endif
    }
  }
}

/***************************************************************************//**
 * Receives frames from the WF200.
 ******************************************************************************/
static sl_status_t sl_wfx_rx_process(uint16_t control_register)
{
  RTOS_ERR err;
  sl_status_t result;

  result = sl_wfx_receive_frame(&control_register);
  if ((control_register & SL_WFX_CONT_NEXT_LEN_MASK) != 0) {
    /* if a packet is still available in the WF200, set an RX event */
    OSFlagPost(&bus_events, SL_WFX_BUS_EVENT_FLAG_RX, OS_OPT_POST_FLAG_SET, &err);
  }

  return result;
}
/**************************************************************************//**
 * Wfx process tx queue
 *****************************************************************************/
static sl_status_t sl_wfx_tx_process(void)
{
  RTOS_ERR err;
  sl_status_t result;
  sl_wfx_packet_queue_item_t *item_to_free;

  if (sl_wfx_tx_queue_context.head_ptr == NULL) {
    return SL_STATUS_EMPTY;
  }

  /* Take TX queue mutex */
  OSMutexPend(&sl_wfx_tx_queue_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);

  /* Send the packet */
  result = sl_wfx_send_ethernet_frame(&sl_wfx_tx_queue_context.head_ptr->buffer,
                                      sl_wfx_tx_queue_context.head_ptr->data_length,
                                      sl_wfx_tx_queue_context.head_ptr->interface,
                                      WFM_PRIORITY_BE0);

  if (result != SL_STATUS_OK) {
    /* If the packet is not successfully sent, set the associated event and return */
    OSFlagPost(&bus_events, SL_WFX_BUS_EVENT_FLAG_TX, OS_OPT_POST_FLAG_SET, &err);
    OSMutexPost(&sl_wfx_tx_queue_mutex, OS_OPT_POST_NONE, &err);
    return SL_STATUS_FULL;
  }

  /* The packet has been successfully sent, free it  */
  item_to_free = sl_wfx_tx_queue_context.head_ptr;

  /* Move the queue pointer to process the next packet */
  sl_wfx_tx_queue_context.head_ptr = sl_wfx_tx_queue_context.head_ptr->next;

  /* The packet has been sent, release the packet  */
  sl_wfx_free_command_buffer((sl_wfx_generic_message_t*) item_to_free,
                             SL_WFX_SEND_FRAME_REQ_ID,
                             SL_WFX_TX_FRAME_BUFFER);

  /* If a packet is available, set the associated event */
  if (sl_wfx_tx_queue_context.head_ptr != NULL) {
    OSFlagPost(&bus_events, SL_WFX_BUS_EVENT_FLAG_TX, OS_OPT_POST_FLAG_SET, &err);
  }

  /* Release TX queue mutex */
  OSMutexPost(&sl_wfx_tx_queue_mutex, OS_OPT_POST_NONE, &err);

  return result;
}
/***************************************************************************//**
 * Creates WF200 bus communication task.
 ******************************************************************************/
void sl_wfx_task_start()
{
  RTOS_ERR err;
  OSFlagCreate(&bus_events, "bus events", 0, &err);
  OSMutexCreate(&sl_wfx_tx_queue_mutex, "tx queue mutex", &err);
  OSSemCreate(&bus_tx_complete_sem, "bus tx comp", 0, &err);

  sl_wfx_tx_queue_context.head_ptr = NULL;
  sl_wfx_tx_queue_context.tail_ptr = NULL;

  OSTaskCreate(&sl_wfx_bus_task_tcb,
               "WFX bus Task",
               sl_wfx_task_entry,
               DEF_NULL,
               SL_WFX_BUS_TASK_PRIO,
               &sl_wfx_bus_task_stk[0],
               (SL_WFX_BUS_TASK_STK_SIZE / 10u),
               SL_WFX_BUS_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  // Check error code.
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}
