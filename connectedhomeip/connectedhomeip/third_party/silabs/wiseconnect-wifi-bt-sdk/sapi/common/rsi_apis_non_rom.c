/*******************************************************************************
* @file  rsi_apis_non_rom.c
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
#include "rsi_wlan_non_rom.h"
#include "rsi_apis_rom.h"

global_cb_t *global_cb;
global_cb_t *global_cb_p;
rsi_wlan_cb_non_rom_t *rsi_wlan_cb_non_rom;
rsi_socket_select_info_t *rsi_socket_select_info;
struct rom_apis_s *rom_apis;
#ifndef ROM_WIRELESS
const struct ROM_WL_API_S api_wl_s = {

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

ROM_WL_API_T *api_wl = NULL;
#endif

extern rsi_socket_info_t *rsi_socket_pool;

void rom_init(void)
{
#ifndef ROM_WIRELESS
  api_wl = &api_wl_s;
#endif

  global_cb_p = global_cb;

  global_cb_p->rsi_driver_cb = rsi_driver_cb;
#ifdef RSI_WLAN_ENABLE
  global_cb_p->rsi_socket_pool = rsi_socket_pool;
#endif
  global_cb_p->rom_apis_p    = rom_apis;
  global_cb_p->no_of_sockets = RSI_NUMBER_OF_SOCKETS;
#ifdef RSI_LITTLE_ENDIAN
  global_cb_p->endian = IS_LITTLE_ENDIAN;
#else
  global_cb_p->endian             = IS_BIG_ENDIAN;
#endif

#ifdef RX_BUFFER_MEM_COPY
  global_cb_p->rx_buffer_mem_copy = 1;
#else
  global_cb_p->rx_buffer_mem_copy = 0;
#endif

  global_cb_p->rsi_tcp_listen_min_backlog = RSI_TCP_LISTEN_MIN_BACKLOG;

#ifdef RSI_WITH_OS
  global_cb_p->os_enabled = 1;
#else
  global_cb_p->os_enabled         = 0;
#endif

  global_cb_p->rsi_max_num_events = RSI_MAX_NUM_EVENTS;

#ifdef RSI_M4_INTERFACE
  global_cb_p->tx_desc = &tx_desc[0];
  global_cb_p->rx_desc = &rx_desc[0];
#endif

  global_cb_p->rom_apis_p->rsi_mutex_create           = rsi_mutex_create;
  global_cb_p->rom_apis_p->rsi_mutex_lock             = rsi_mutex_lock;
  global_cb_p->rom_apis_p->rsi_mutex_unlock           = rsi_mutex_unlock;
  global_cb_p->rom_apis_p->rsi_semaphore_create       = rsi_semaphore_create;
  global_cb_p->rom_apis_p->rsi_critical_section_entry = rsi_critical_section_entry;
  global_cb_p->rom_apis_p->rsi_critical_section_exit  = rsi_critical_section_exit;
  global_cb_p->rom_apis_p->rsi_semaphore_wait         = rsi_semaphore_wait;
  global_cb_p->rom_apis_p->rsi_semaphore_post         = rsi_semaphore_post;
#ifdef RSI_M4_INTERFACE
  global_cb_p->rom_apis_p->rsi_semaphore_post_from_isr = rsi_semaphore_post_from_isr;
  global_cb_p->rom_apis_p->rsi_mask_ta_interrupt       = rsi_mask_ta_interrupt;
  global_cb_p->rom_apis_p->rsi_unmask_ta_interrupt     = rsi_unmask_ta_interrupt;
#endif
#ifdef RSI_WLAN_ENABLE
  global_cb_p->rom_apis_p->rsi_driver_wlan_send_cmd = rsi_driver_wlan_send_cmd;
  global_cb_p->rom_apis_p->rsi_driver_send_data     = rsi_driver_send_data;
#endif
  global_cb_p->rom_apis_p->rsi_delay_ms = rsi_delay_ms;
#ifdef RSI_M4_INTERFACE
  global_cb_p->rom_apis_p->rsi_assertion = rsi_assertion;
#endif

#ifdef RSI_WLAN_ENABLE
#ifdef ROM_WIRELESS
  global_cb_p->rom_apis_p->ROM_WL_calculate_length_to_send = ROMAPI_WL->calculate_length_to_send;
#else
  global_cb_p->rom_apis_p->ROM_WL_calculate_length_to_send = api_wl->calculate_length_to_send;
#endif
#endif
}
