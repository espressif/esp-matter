/*******************************************************************************
* @file  rsi_scheduler.h
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

#ifndef RSI_SCHEDULER_H
#define RSI_SCHEDULER_H
/******************************************************
 * *                      Macros
 * ******************************************************/
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// scheduler control block structure
typedef struct rsi_scheduler_cb_s {
  // This bitmap is used to set and clear events
  volatile uint32_t event_map;

  // This bitmap is used to mask and unmask events
  volatile uint32_t mask_map;

  // scheduler semaphore
  rsi_semaphore_handle_t scheduler_sem;
} rsi_scheduler_cb_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb);
uint32_t rsi_get_event(rsi_scheduler_cb_t *scheduler_cb);
void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb);
uint32_t rsi_get_event_non_rom(rsi_scheduler_cb_t *scheduler_cb);

#endif
