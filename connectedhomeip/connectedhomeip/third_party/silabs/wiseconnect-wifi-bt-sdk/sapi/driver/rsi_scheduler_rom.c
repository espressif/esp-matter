/*******************************************************************************
* @file  rsi_scheduler_rom.c
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
#ifndef ROM_WIRELESS
/*
  Global Variables
 */

/** @addtogroup DRIVER15
* @{
*/
/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_scheduler_init(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Initialize scheduler
 * @param[in]   global_cb_p  - pointer to the global control block
 * @param[in]   scheduler_cb - pointer to scheduler structure
 * @return      void
 */
/// @private
void ROM_WL_rsi_scheduler_init(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
{
  // Set no event
  scheduler_cb->event_map = 0x0;

  // Set no mask
  scheduler_cb->mask_map = 0xFFFFFFFF;

  if (global_cb_p->os_enabled == 1) {
    // create semaphore
    RSI_SEMAPHORE_CREATE(&scheduler_cb->scheduler_sem, 0);
  }

  return;
}
/*====================================================*/
/**
 * @fn          uint32_t ROM_WL_rsi_get_event(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Get an event 
 * @param[in]   global_cb_p  - pointer to the global control block
 * @param[in]   scheduler_cb - pointer to scheduler structure
 * @return      Current event map
 */
/// @private
uint32_t ROM_WL_rsi_get_event(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  uint32_t active_int_event_map;

  rsi_reg_flags_t flags;

  // Disable all the interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  // Get current event map after applying mask
  active_int_event_map = (scheduler_cb->event_map & scheduler_cb->mask_map);

  // Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  return active_int_event_map;
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_scheduler(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Handles events
 * @param[in]   global_cb_p  - pointer to the global control block
 * @param[in]   scheduler_cb - pointer to scheduler cb structure
 * @return      void
 */
/// @private
void ROM_WL_rsi_scheduler(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t event_no;
  rsi_event_cb_t *temp_event;

  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;
#ifdef RSI_M4_INTERFACE
  while (ROM_WL_rsi_get_event(global_cb_p, scheduler_cb))
#else
  while (rsi_get_event(scheduler_cb))
#endif
  {
    // Find event event
    event_no = ROM_WL_rsi_find_event(global_cb_p, (scheduler_cb->event_map & scheduler_cb->mask_map));

    // Get event handler
    temp_event = &rsi_driver_cb->event_list[event_no];

    if (temp_event->event_handler) {
      // Call event handler
      temp_event->event_handler();
    } else {

      ROM_WL_rsi_clear_event(global_cb_p, event_no); //Clear unregistered events
      global_cb_p->rsi_driver_cb->unregistered_event_callback(event_no);
    }

    if (global_cb_p->os_enabled != 1) {
      break;
    }
  }

  if (global_cb_p->os_enabled == 1) {
    RSI_SEMAPHORE_WAIT(&scheduler_cb->scheduler_sem, 0);
  }
}
#endif
/** @} */
