/***************************************************************************//**
 * @file sl_wfx_task.h
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

#ifndef SL_WFX_TASK_H
#define SL_WFX_TASK_H

#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>
#include <common/include/rtos_err.h>
#include "sl_wfx_constants.h"
#include "sl_wfx_host.h"

#define SL_WFX_BUS_EVENT_FLAG_RX      1
#define SL_WFX_BUS_EVENT_FLAG_TX      2
#define SL_WFX_BUS_EVENT_WAKE         4
#define SL_WFX_BUS_QUEUE_SIZE        10

typedef struct {
  sl_wfx_send_frame_req_t *frame;
  uint32_t                 data_length;
  sl_wfx_interface_t       interface;
  uint8_t                  priority;
} sl_wfx_frame_q_item;

extern sl_wfx_frame_q_item   bus_tx_frame;
extern OS_SEM                bus_tx_complete_sem;
extern OS_FLAG_GRP           bus_events;
extern OS_MUTEX              sl_wfx_tx_queue_mutex;
extern sl_wfx_packet_queue_t sl_wfx_tx_queue_context;
#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************//**
 * Start wfx bus communication task.
 *****************************************************************************/
void sl_wfx_task_start (void);

/**************************************************************************//**
 * Returns status of wfx receive frames.
 *****************************************************************************/
bool wfx_bus_is_receive_processing (void);
#ifdef __cplusplus
}
#endif

#endif
