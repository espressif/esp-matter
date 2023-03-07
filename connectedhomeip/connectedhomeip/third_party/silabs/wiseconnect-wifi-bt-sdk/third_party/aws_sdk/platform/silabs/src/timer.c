
/*******************************************************************************
* @file timer.c
* @brief Linux implementation of the timer interface

*******************************************************************************/
/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/*******************************************************************************
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

#ifdef __cplusplus
extern "C" {
#endif
#include "rsi_driver.h"
#include "rsi_timer.h"
#include "rsi_hal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "timer_platform.h"

bool has_timer_expired(Timer *timer) 
{
  long left = timer->end_time - rsi_timer_read_counter();
	return (left < 0);
}

void countdown_ms(Timer *timer, uint32_t timeout) 
{
  timer->timeout =timeout;
  timer->end_time = rsi_timer_read_counter() + timeout;
}

uint32_t left_ms(Timer *timer)
{
  long left = timer->end_time -rsi_timer_read_counter();
  return (left < 0) ? 0 : left;
}

void countdown_sec(Timer *timer, uint32_t timeout) 
{
  timer->end_time = rsi_timer_read_counter() + (timeout * 1000);  
}

void init_timer(Timer *timer) 
{
  timer->end_time = 0;  
}

void delay(unsigned milliseconds)
{
  rsi_delay_ms(milliseconds);
}

#ifdef __cplusplus
}
#endif
