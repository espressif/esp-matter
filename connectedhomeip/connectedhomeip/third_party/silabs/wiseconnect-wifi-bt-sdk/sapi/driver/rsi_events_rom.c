/*******************************************************************************
* @file  rsi_events_rom.c
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

/** @addtogroup DRIVER5
* @{
*/
/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_set_event(global_cb_t *global_cb_p, uint32_t event_num)
 * @brief       Set an event
 * @param[in]   global_cb_p - pointer to the common buffer
 * @param[in]   event_num   - event number to be set
 * @return      void
 */
///@private
void ROM_WL_rsi_set_event(global_cb_t *global_cb_p, uint32_t event_num)
{
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;

  rsi_reg_flags_t flags;

#ifdef RSI_M4_INTERFACE
  // mask P2P interrupt
  RSI_MASK_TA_INTERRUPT();
#endif

  // Disable all the interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  // Set the event bit in bitmap
  rsi_driver_cb->scheduler_cb.event_map |= BIT(event_num);

  // Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

#ifdef RSI_M4_INTERFACE
  // unmask P2P interrupt
  RSI_UNMASK_TA_INTERRUPT();
#endif

  if (global_cb_p->os_enabled == 1) {
    // Post the semaphore
    RSI_SEMAPHORE_POST(&rsi_driver_cb->scheduler_cb.scheduler_sem);
  }
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_clear_event(global_cb_t *global_cb_p,uint32_t event_num)
 * @brief       Clear an event
 * @param[in]   global_cb_p  - pointer to the common buffer
 * @param[in]   event_num    - event number to clear
 * @return      void
 */
///@private
void ROM_WL_rsi_clear_event(global_cb_t *global_cb_p, uint32_t event_num)
{

  rsi_reg_flags_t flags;

  // Disable all the interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  // Reset the event bit in bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.event_map &= ~BIT(event_num);

  // Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  return;
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_mask_event(global_cb_t *global_cb_p,uint32_t event_num)
 * @brief       Mask an event
 * @param[in]   global_cb_p - pointer to the common buffer
 * @param[in]   event_num   - event number to mask
 * @return      void
 */
///@private
void ROM_WL_rsi_mask_event(global_cb_t *global_cb_p, uint32_t event_num)
{
  rsi_reg_flags_t flags;

  // Disable all the interrupts
  flags = RSI_CRITICAL_SECTION_ENTRY();

  // Reset event bit in mask bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.mask_map &= ~BIT(event_num);

  // Enable all the interrupts
  RSI_CRITICAL_SECTION_EXIT(flags);

  return;
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_unmask_event(global_cb_t *global_cb_p, uint32_t event_num)
 * @brief       Unmask an event
 * @param[in]   global_cb_p - pointer to the common buffer
 * @param[in]   event_num   - event number to unmask
 * @return      void
 */
///@private
void ROM_WL_rsi_unmask_event(global_cb_t *global_cb_p, uint32_t event_num)
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

  if (global_cb_p->os_enabled == 1) {
    // Post the semaphore
    RSI_SEMAPHORE_POST(&global_cb_p->rsi_driver_cb->scheduler_cb.scheduler_sem);
  }
}

/*====================================================*/
/**
 * @fn          uint32_t ROM_WL_rsi_find_event(global_cb_t *global_cb_p, uint32_t event_map)
 * @brief       Find the event which is set from the map
 * @param[in]   global_cb_p - pointer to the common buffer
 * @param[in]   event_map   - event map
 * @return      Event number
 */
///@private
uint32_t ROM_WL_rsi_find_event(global_cb_t *global_cb_p, uint32_t event_map)
{
  uint8_t i;

  for (i = 0; i < global_cb_p->rsi_max_num_events; i++) {
    if (event_map & BIT(i)) {
      break;
    }
  }
  return i;
}

/*====================================================*/
/**
 * @fn          uint16_t ROM_WL_rsi_register_event(global_cb_t *global_cb_p, uint32_t event_id, void (*event_handler_ptr)(void))
 * @brief       Register the event
 * @param[in]   global_cb_p       - pointer to the common buffer
 * @param[in]   event_id          - event number which needs to be registered
 * @param[in]   event_handler_ptr - event handler which needs to be registered for a given event
 * @return      0 - Success \n
 *              1 - Error 
 */
///@private
uint16_t ROM_WL_rsi_register_event(global_cb_t *global_cb_p, uint32_t event_id, void (*event_handler_ptr)(void))
{
  if ((event_id > global_cb_p->rsi_max_num_events)
      || (global_cb_p->rsi_driver_cb->event_list[event_id].event_handler != NULL)) {
    /*
     *Return , if the event number exceeds the RSI_MAX_NUM_EVENTS ,or
     * the event is already registered
     */
    return 1;
  }

  // Register event handler
  global_cb_p->rsi_driver_cb->event_list[event_id].event_handler = event_handler_ptr;

  return 0;
}

#endif
/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_set_event_from_isr(global_cb_t *global_cb_p, uint32_t event_num)
 * @brief       Set an event from ISR
 * @param[in]   global_cb_p - pointer to the common buffer
 * @param[in]   event_num   - event number to be set
 * @return      void
 */
///@private
void ROM_WL_rsi_set_event_from_isr(global_cb_t *global_cb_p, uint32_t event_num)
{
  rsi_driver_cb_t *rsi_driver_cb = global_cb_p->rsi_driver_cb;

  // Set the event bit in scheduler bitmap
  rsi_driver_cb->scheduler_cb.event_map |= BIT(event_num);

  if (global_cb_p->os_enabled == 1) {
    // Post the semaphore
    RSI_SEMAPHORE_POST_FROM_ISR(&rsi_driver_cb->scheduler_cb.scheduler_sem);
  } else {
    // Post the semaphore
    RSI_SEMAPHORE_POST(&rsi_driver_cb->scheduler_cb.scheduler_sem);
  }
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_unmask_event_from_isr(global_cb_t *global_cb_p, uint32_t event_num)
 * @brief       Unmask an event from ISR
 * @param[in]   global_cb_p - pointer to the common buffer 
 * @param[in]   event_num - event number to unmask
 * @return      void
 */
///@private
void ROM_WL_rsi_unmask_event_from_isr(global_cb_t *global_cb_p, uint32_t event_num)
{

  // set event bit in mask bitmap
  global_cb_p->rsi_driver_cb->scheduler_cb.mask_map |= BIT(event_num);

  if (global_cb_p->os_enabled == 1) {
    // Post the semaphore
    RSI_SEMAPHORE_POST_FROM_ISR(&global_cb_p->rsi_driver_cb->scheduler_cb.scheduler_sem);
  } else {
    // Post the semaphore
    RSI_SEMAPHORE_POST(&global_cb_p->rsi_driver_cb->scheduler_cb.scheduler_sem);
  }
}
/** @} */
