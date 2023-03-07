/*******************************************************************************
* @file  rsi_events.c
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

/*
  Includes
 */
#include "rsi_driver.h"

#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
extern rsi_task_handle_t driver_task_handle;
/*
  Global Variables
 */
/** @addtogroup DRIVER8
* @{
*/
/*====================================================*/
/**
 * @fn          void rsi_set_event_non_rom( uint32_t event_num)
 * @brief       Set an event
 * @param[in]   event_num - event number to be set
 * @return      void
 */
///@private
void rsi_set_event_non_rom(uint32_t event_num)
{
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;

#ifdef RSI_M4_INTERFACE
  // mask P2P interrupt
  RSI_MASK_TA_INTERRUPT();
#endif

  // Set the event bit in bitmap
  rsi_driver_cb->scheduler_cb.event_map |= BIT(event_num);

#ifdef RSI_M4_INTERFACE
  // unmask P2P interrupt
  RSI_UNMASK_TA_INTERRUPT();
#endif

  if (global_cb_p->os_enabled == 1) {
    // notify to driver task
    rsi_task_notify(driver_task_handle, BIT(event_num));
  }
}

/*====================================================*/
/**
 * @fn          void rsi_set_event_from_isr_non_rom(uint32_t event_num)
 * @brief       Set an event from ISR
 * @param[in]   uint32_t event_num - event number to be set
 * @return      void
 */
///@private
void rsi_set_event_from_isr_non_rom(uint32_t event_num)
{
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;

  // Set the event bit in scheduler bitmap
  rsi_driver_cb->scheduler_cb.event_map |= BIT(event_num);

  rsi_task_notify_from_isr(driver_task_handle, BIT(event_num), NULL);
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event_from_isr_non_rom(uint32_t event_num)
 * @brief       Unmask an event from ISR
 * @param[in]   uint32_t event_num - event number to unmask
 * @return      void
 */
///@private
void rsi_unmask_event_from_isr_non_rom(uint32_t event_num)
{
  // set event bit in mask bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.mask_map |= BIT(event_num);

  // notify to driver task
  rsi_task_notify_from_isr(driver_task_handle, BIT(event_num), NULL);
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event_non_rom(uint32_t event_num)
 * @brief       Unmask an event
 * @param[in]   event_num - event number to unmask
 * @return      void
 */
///@private
void rsi_unmask_event_non_rom(uint32_t event_num)
{
#ifdef RSI_M4_INTERFACE
  // mask P2P interrupt
  RSI_MASK_TA_INTERRUPT();
#endif

  // set event bit in mask bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.mask_map |= BIT(event_num);

#ifdef RSI_M4_INTERFACE
  // unmask P2P interrupt
  RSI_UNMASK_TA_INTERRUPT();
#endif

  // notify to driver task
  rsi_task_notify(driver_task_handle, BIT(event_num));
}
#endif
/** @} */

/** @addtogroup DRIVER5
* @{
*/
/*====================================================*/
/**
 * @fn          void rsi_set_event(uint32_t event_num)
 * @brief       Set an event. 
 * @param[in]   event_num - Event number to be set 
 * @return      void
 */
///@private
void rsi_set_event(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
  rsi_set_event_non_rom(event_num);
#elif (defined(ROM_WIRELESS))
  ROMAPI_WL->rsi_set_event(global_cb_p, event_num);
#else
  api_wl->rsi_set_event(global_cb_p, event_num);
#endif
}
/*====================================================*/
/**
 * @fn          void rsi_clear_event(uint32_t event_num)
 * @brief       Clear an event.
 * @param[in]   event_num - Event number to clear
 * @return      void
 */
///@private
void rsi_clear_event(uint32_t event_num)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_clear_event(global_cb_p, event_num);
#else
  api_wl->rsi_clear_event(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_mask_event(uint32_t event_num)
 * @brief       Mask an event specified using the event number.
 * @param[in]   event_num - Event number to mask 
 * @return      void
 */
///@private
void rsi_mask_event(uint32_t event_num)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_mask_event(global_cb_p, event_num);
#else
  api_wl->rsi_mask_event(global_cb_p, event_num);
#endif
}
/** @} */

/** @addtogroup DRIVER8
* @{
*/
/*====================================================*/
/**
 * @fn          void rsi_unmask_event(uint32_t event_num)
 * @brief       Unmask an event. 
 * @param[in]   event_num - Event number to unmask
 * @return      void
 */
///@private
void rsi_unmask_event(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
  rsi_unmask_event_non_rom(event_num);
#elif (defined(ROM_WIRELESS))
  ROMAPI_WL->rsi_unmask_event(global_cb_p, event_num);
#else
  api_wl->rsi_unmask_event(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_unmask_event_from_isr(uint32_t event_num)
 * @brief       Unmask an event from ISR context.
 * @param[in]   event_num - event number to unmask
 * @return      void
 */
///@private
void rsi_unmask_event_from_isr(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
  rsi_unmask_event_from_isr_non_rom(event_num);
#elif (defined(ROM_WIRELESS))
  ROMAPI_WL->rsi_unmask_event_from_isr(global_cb_p, event_num);
#else
  api_wl->rsi_unmask_event_from_isr(global_cb_p, event_num);
#endif
}
/** @} */

/** @addtogroup DRIVER5
* @{
*/
/*====================================================*/
/**
 * @fn          uint32_t rsi_find_event(uint32_t event_map)
 * @brief       Find the event which is set from the event map. 
 * @param[in]   event_map - Event map
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */
///@private
uint32_t rsi_find_event(uint32_t event_map)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_find_event(global_cb_p, event_map);
#else
  return api_wl->rsi_find_event(global_cb_p, event_map);
#endif
}

/*====================================================*/
/**
 * @fn          uint16_t rsi_register_event(uint32_t event_id, void (*event_handler_ptr)(void))
 * @brief       Register the event handler for the given event. 
 * @param[in]   event_id -  event number which needs to be registered 
 * @param[in]   event_handler_ptr - event handler which needs to be registered for a given event 
 * @return      0 - Success \n
 *              1 - Error
 *
 */
///@private
uint16_t rsi_register_event(uint32_t event_id, void (*event_handler_ptr)(void))
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_register_event(global_cb_p, event_id, event_handler_ptr);
#else
  return api_wl->rsi_register_event(global_cb_p, event_id, event_handler_ptr);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_set_event_from_isr(uint32_t event_num)
 * @brief       Set the event from isr context. 
 * @param[in]   event_num - Event number to mask 
 * @return      void 
 *                
 */
///@private
void rsi_set_event_from_isr(uint32_t event_num)
{
#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
  rsi_set_event_from_isr_non_rom(event_num);
#elif (defined(ROM_WIRELESS))
  ROMAPI_WL->rsi_set_event_from_isr(global_cb_p, event_num);
#else
  api_wl->rsi_set_event_from_isr(global_cb_p, event_num);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_events_init(void)
 * @brief       Initialize the events. 
 * @param[in]   void 
 * @return      void
 */
///@private
void rsi_events_init(void)
{
  // Initialze RX event
  rsi_register_event(RSI_RX_EVENT, rsi_rx_event_handler);

  // Initialze TX event
  rsi_register_event(RSI_TX_EVENT, rsi_tx_event_handler);

#ifndef RSI_M4_INTERFACE
#ifdef RSI_WLAN_ENABLE
  // Initialze Socket event
  rsi_register_event(RSI_SOCKET_EVENT, rsi_socket_event_handler);
#endif
#endif

#ifdef RSI_ENABLE_DEMOS
#if (BT_A2DP_SOURCE_WIFI_HTTP_S_RX || BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX \
     || BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX)
  // Initialize application event1
  rsi_register_event(RSI_APP_EVENT1, rsi_app_event1_handler);

  // Initialize application event2
  rsi_register_event(RSI_APP_EVENT2, rsi_app_event2_handler);
#endif
#endif
}
/** @} */
