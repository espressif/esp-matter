/*******************************************************************************
* @file  rsi_timer.h
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

#ifndef RSI_TIMER_H
#define RSI_TIMER_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_TIMER_NODE_0 0
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
typedef struct rsi_timer_instance_s {
  uint32_t start_time;
  uint32_t timeout;
} rsi_timer_instance_t;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
void rsi_timer_expiry_interrupt_handler(void);
uint32_t rsi_timer_read_counter(void);
uint32_t rsi_hal_gettickcount(void);
void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration);
int32_t rsi_timer_expired(rsi_timer_instance_t *timer);
uint32_t rsi_timer_left(rsi_timer_instance_t *timer);

#endif
