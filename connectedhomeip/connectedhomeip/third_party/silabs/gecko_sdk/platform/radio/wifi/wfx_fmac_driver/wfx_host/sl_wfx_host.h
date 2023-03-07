/***************************************************************************//**
 * @file sl_wfx_host.h
 * @brief WFX FMAC driver host implementation
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

#ifndef SL_WFX_HOST_H
#define SL_WFX_HOST_H

#ifndef SL_CATALOG_WFX_BOARD_PRESENT
#warning "WFX Board component (BRD8022A, BRD8023A, or BRD4321A) not installed"
#endif

#include "sl_wfx.h"
#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>
#include <common/include/rtos_err.h>

#ifdef __cplusplus
extern "C" {
#endif
sl_status_t sl_wfx_host_setup_memory_pools(void);
sl_status_t sl_wfx_host_process_event(sl_wfx_generic_message_t *event_payload);
uint8_t     sl_wfx_host_get_waited_event(void);

#ifdef SLEEP_ENABLED
sl_status_t sl_wfx_host_switch_to_wirq (void);
#endif
#ifdef __cplusplus
}
#endif

#define SL_WFX_MAX_STATIONS    8
#define SL_WFX_MAX_SCAN_RESULTS 50

typedef struct __attribute__((__packed__)) scan_result_list_s {
  sl_wfx_ssid_def_t ssid_def;
  uint8_t  mac[SL_WFX_MAC_ADDR_SIZE];
  uint16_t channel;
  sl_wfx_security_mode_bitmask_t security_mode;
  uint16_t rcpi;
} scan_result_list_t;

/* Packet Queue */
typedef struct sl_wfx_packet_queue_item_t{
  struct sl_wfx_packet_queue_item_t *next;
  sl_wfx_interface_t interface;
  uint32_t data_length;
  sl_wfx_send_frame_req_t buffer;
}sl_wfx_packet_queue_item_t;

/* Packet Queue */
typedef struct {
  sl_wfx_packet_queue_item_t *head_ptr;
  sl_wfx_packet_queue_item_t *tail_ptr;
}sl_wfx_packet_queue_t;

extern OS_SEM wfx_wakeup_sem;
extern OS_SEM wfx_init_sem;

#endif
