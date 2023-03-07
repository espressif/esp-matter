/*******************************************************************************
* @file  rsi_scheduler.c
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
/*
  Global Variables
 */

#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
volatile uint32_t rsi_driver_eventmap;
#endif
#if defined(FW_LOGGING_ENABLE) && !defined(RSI_WITH_OS)
void sl_fw_log_task(void);
#endif
uint8_t rsi_get_intr_status(void);
/** @addtogroup DRIVER15
* @{
*/
/*====================================================*/
/**
 * @fn          void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Initialize scheduler
 * @param[in]   scheduler_cb - pointer to scheduler control block structure 
 * @return      void 
 */
/// @private
void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_scheduler_init(global_cb_p, scheduler_cb);
#else
  api_wl->rsi_scheduler_init(global_cb_p, scheduler_cb);
#endif
  return;
}
/*====================================================*/
/**
 * @fn          uint32_t rsi_get_event(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Handle events
 * @param[in]   scheduler_cb - pointer to scheduler cb structure 
 * @return      Current event map
 */
/// @private
uint32_t rsi_get_event(rsi_scheduler_cb_t *scheduler_cb)
{
#if ((defined RSI_SPI_INTERFACE) || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
  return rsi_get_event_non_rom(scheduler_cb);
#elif (defined(ROM_WIRELESS))
  return ROMAPI_WL->rsi_get_event(global_cb_p, scheduler_cb);
#else
  return api_wl->rsi_get_event(global_cb_p, scheduler_cb);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Scheduler function handles events
 * @param[in]   scheduler_cb - pointer to scheduler cb structure 
 * @return      void 
 */
/// @private
void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb)
{
#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
  rsi_scheduler_non_rom(scheduler_cb);
#elif (defined(ROM_WIRELESS))
  ROMAPI_WL->rsi_scheduler(global_cb_p, scheduler_cb);
#else
  api_wl->rsi_scheduler(global_cb_p, scheduler_cb);
#if defined(FW_LOGGING_ENABLE) && !defined(RSI_WITH_OS)
  sl_fw_log_task();
#endif
#endif
}
/*====================================================*/
/**
 * @fn          uint32_t rsi_get_event_non_rom(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Handle events on rom
 * @param[in]   scheduler_cb - pointer to scheduler cb structure 
 * @return      Current event map 
 */
/// @private
uint32_t rsi_get_event_non_rom(rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t active_int_event_map;

  rsi_reg_flags_t flags;

  // Disable all the interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  // Get current event map after applying mask
  active_int_event_map = (scheduler_cb->event_map & scheduler_cb->mask_map);
#if ((defined RSI_SPI_INTERFACE) || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))

  if (rsi_get_intr_status() && (!(active_int_event_map & BIT(RSI_RX_EVENT)))) {
    // Disable the interrupt
    rsi_hal_intr_mask();
    // Set event RX pending from device
    rsi_set_event(RSI_RX_EVENT);
  }
#endif

  // Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  return active_int_event_map;
}

#if (defined(RSI_WITH_OS) && (RSI_TASK_NOTIFY))
/*====================================================*/
/**
 * @fn          void rsi_scheduler_non_rom(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Scheduler function handles events
 * @param[in]   scheduler_cb - pointer to scheduler cb structure \n 
 * @return      void  
 */

void rsi_scheduler_non_rom(rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t event_no;
  rsi_event_cb_t *temp_event;

  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;
  uint32_t max_block_time        = 0xFFFFFFFF;
  rsi_base_type_t retval;
  // Wait for events

  retval = rsi_task_notify_wait(0,                                /* Don't clear bits on entry. */
                                max_block_time,                   /* Clear all bits on exit. */
                                (uint32_t *)&rsi_driver_eventmap, /* Stores the notified value. */
                                max_block_time);
  if (retval == 1) {
    while (rsi_get_event(scheduler_cb)) {
      // Find event event

      event_no = rsi_find_event((scheduler_cb->event_map & scheduler_cb->mask_map));

      // Get event handler
      temp_event = &rsi_driver_cb->event_list[event_no];

      if (temp_event->event_handler) {
        // Call event handler
        temp_event->event_handler();
      } else {

        rsi_clear_event(event_no); //Clear unregistered events
        global_cb_p->rsi_driver_cb->unregistered_event_callback(event_no);
      }

      if (global_cb_p->os_enabled != 1) {
        break;
      }
    }
  }
}
#endif
/** @} */
