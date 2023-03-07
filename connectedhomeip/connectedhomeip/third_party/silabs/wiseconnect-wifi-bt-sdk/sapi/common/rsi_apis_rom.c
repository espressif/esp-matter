/*******************************************************************************
* @file  rsi_apis_rom.c
* @brief 
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

#include "rsi_apis_rom.h"
#include "rsi_driver.h"
#include "rsi_socket.h"

#ifndef ROM_WIRELESS

const struct ROM_WL_API_S wl_api = {
#ifdef RSI_WLAN_ENABLE
  // Socket APIs
  .rsi_get_application_socket_descriptor = &ROM_WL_rsi_get_application_socket_descriptor,
  .calculate_buffers_required            = &ROM_WL_calculate_buffers_required,
  .calculate_length_to_send              = &ROM_WL_calculate_length_to_send,
#endif

  // Scheduler
  .rsi_scheduler_init = &ROM_WL_rsi_scheduler_init,
  .rsi_get_event      = &ROM_WL_rsi_get_event,
  .rsi_scheduler      = &ROM_WL_rsi_scheduler,

  // Packet Allocation Management
  .rsi_pkt_pool_init    = &ROM_WL_rsi_pkt_pool_init,
  .rsi_pkt_alloc        = &ROM_WL_rsi_pkt_alloc,
  .rsi_pkt_free         = &ROM_WL_rsi_pkt_free,
  .rsi_is_pkt_available = &ROM_WL_rsi_is_pkt_available,

  // Queue Management
  .rsi_queues_init          = &ROM_WL_rsi_queues_init,
  .rsi_enqueue_pkt          = &ROM_WL_rsi_enqueue_pkt,
  .rsi_dequeue_pkt          = &ROM_WL_rsi_dequeue_pkt,
  .rsi_check_queue_status   = &ROM_WL_rsi_check_queue_status,
  .rsi_block_queue          = &ROM_WL_rsi_block_queue,
  .rsi_unblock_queue        = &ROM_WL_rsi_unblock_queue,
  .rsi_enqueue_pkt_from_isr = &ROM_WL_rsi_enqueue_pkt_from_isr,

  // Events
  .rsi_set_event             = &ROM_WL_rsi_set_event,
  .rsi_clear_event           = &ROM_WL_rsi_clear_event,
  .rsi_mask_event            = &ROM_WL_rsi_mask_event,
  .rsi_unmask_event          = &ROM_WL_rsi_unmask_event,
  .rsi_find_event            = &ROM_WL_rsi_find_event,
  .rsi_register_event        = &ROM_WL_rsi_register_event,
  .rsi_set_event_from_isr    = &ROM_WL_rsi_set_event_from_isr,
  .rsi_unmask_event_from_isr = &ROM_WL_rsi_unmask_event_from_isr,

  // Utils
  .rsi_uint16_to_2bytes  = &ROM_WL_rsi_uint16_to_2bytes,
  .rsi_uint32_to_4bytes  = &ROM_WL_rsi_uint32_to_4bytes,
  .rsi_bytes2R_to_uint16 = &ROM_WL_rsi_bytes2R_to_uint16,
  .rsi_bytes4R_to_uint32 = &ROM_WL_rsi_bytes4R_to_uint32,

#ifdef RSI_M4_INTERFACE
  // M4 HAL
  .rsi_m4_interrupt_isr = &ROM_WL_rsi_m4_interrupt_isr,
  .mask_ta_interrupt    = &ROM_WL_mask_ta_interrupt,
  .unmask_ta_interrupt  = &ROM_WL_unmask_ta_interrupt,
  .rsi_frame_read       = &ROM_WL_rsi_frame_read,
  .rsi_frame_write      = &ROM_WL_rsi_frame_write,
  .rsi_submit_rx_pkt    = &ROM_WL_rsi_submit_rx_pkt
#endif
};
#endif
