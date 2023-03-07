/***************************************************************************//**
 * @file
 * @brief Throughput test application - platform interface
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef THROUGHPUT_CENTRAL_INTERFACE_H
#define THROUGHPUT_CENTRAL_INTERFACE_H

#include "throughput_types.h"

#define THROUGHPUT_CENTRAL_REFRESH_TIMER_PERIOD       1000

/**************************************************************************//**
 * ASCII graphics for indicating wait status
 *****************************************************************************/
void waiting_indication(void);

/**************************************************************************//**
 * Start timer
 *****************************************************************************/
void timer_start();

/**************************************************************************//**
 * Timer end. The return value of this function shall be the time passed
 * form the timer_start() call in seconds.
 *****************************************************************************/
float timer_end();

/**************************************************************************//**
 * Start RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_start(void);

/**************************************************************************//**
 * Stop RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_stop(void);

/**************************************************************************//**
 * Event handler
 *****************************************************************************/
void timer_on_refresh_rssi(void);

#endif
