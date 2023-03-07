/***************************************************************************//**
 * @file
 * @brief Throughput test application - platform interface implementation
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

#include <stdint.h>

#include "app_log.h"
#include "throughput_central_interface.h"

#if ((_WIN32 == 1) || (__CYGWIN__ == 1))

#include <windows.h>

LARGE_INTEGER starting_time;
LARGE_INTEGER starting_rssi_time;

/**************************************************************************//**
 * Start timer
 *****************************************************************************/
void timer_start()
{
  QueryPerformanceCounter(&starting_time);
}

/**************************************************************************//**
 * Timer end. The return value of this function shall be the time passed
 * form the timer_start() call in seconds.
 *****************************************************************************/
float timer_end()
{
  LARGE_INTEGER elapsed_microseconds;
  LARGE_INTEGER ending_time;
  LARGE_INTEGER frequency;

  QueryPerformanceCounter(&ending_time);
  QueryPerformanceFrequency(&frequency);
  elapsed_microseconds.QuadPart = ending_time.QuadPart - starting_time.QuadPart;
  elapsed_microseconds.QuadPart *= 1000000;

  float time = (float)(elapsed_microseconds.QuadPart / frequency.QuadPart) / 1e6;
  return time;
}

/**************************************************************************//**
 * Start RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_start(void)
{
  QueryPerformanceCounter(&starting_rssi_time);
}
/**************************************************************************//**
 * Stop RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_stop(void)
{
  starting_rssi_time.QuadPart = 0;
}

/**************************************************************************//**
 * RSSI refresh timer step
 *****************************************************************************/
void timer_step_rssi(void)
{
  LARGE_INTEGER rssi_current_time;
  LARGE_INTEGER frequency;
  LARGE_INTEGER elapsed_milliseconds;

  if (starting_rssi_time.QuadPart != 0 ) {
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&rssi_current_time);
    elapsed_milliseconds.QuadPart = 1000 * (rssi_current_time.QuadPart - starting_rssi_time.QuadPart)
                                    / frequency.QuadPart;
    if (elapsed_milliseconds.QuadPart > THROUGHPUT_CENTRAL_REFRESH_TIMER_PERIOD) {
      timer_on_refresh_rssi();
      starting_rssi_time = rssi_current_time;
    }
  }
}

#else

#include <unistd.h>
#include <time.h>
struct timespec starting_time;
struct timespec starting_rssi_time;

/**************************************************************************//**
 * Start timer
 *****************************************************************************/
void timer_start()
{
  clock_gettime(CLOCK_REALTIME, &starting_time);
}

/**************************************************************************//**
 * Timer end. The return value of this function shall be the time passed
 * form the timer_start() call in seconds.
 *****************************************************************************/
float timer_end()
{
  struct timespec ending_time;
  clock_gettime(CLOCK_REALTIME, &ending_time);
  float time = (float)((ending_time.tv_sec - starting_time.tv_sec)
                       + (ending_time.tv_nsec - starting_time.tv_nsec) * 1e-9);
  return time;
}

/**************************************************************************//**
 * Start RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_start(void)
{
  clock_gettime(CLOCK_REALTIME, &starting_rssi_time);
}

/**************************************************************************//**
 * Stop RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_stop(void)
{
  starting_rssi_time.tv_sec = 0;
  starting_rssi_time.tv_nsec = 0;
}

/**************************************************************************//**
 * RSSI refresh timer step
 *****************************************************************************/
void timer_step_rssi(void)
{
  struct timespec rssi_current_time;
  uint64_t elapsed_milliseconds;

  if (starting_rssi_time.tv_sec != 0 || starting_rssi_time.tv_nsec != 0 ) {
    clock_gettime(CLOCK_REALTIME, &rssi_current_time);
    elapsed_milliseconds = ((rssi_current_time.tv_sec - starting_rssi_time.tv_sec)
                            + ((rssi_current_time.tv_nsec - starting_rssi_time.tv_nsec) * 1e-9)) * 1000;
    if (elapsed_milliseconds > THROUGHPUT_CENTRAL_REFRESH_TIMER_PERIOD) {
      timer_on_refresh_rssi();
      starting_rssi_time.tv_sec = rssi_current_time.tv_sec;
      starting_rssi_time.tv_nsec = rssi_current_time.tv_nsec;
    }
  }
}

#endif

/**************************************************************************//**
 * ASCII graphics for indicating wait status
 * Make a loading animation while waiting for scan response match.
 *****************************************************************************/
void waiting_indication(void)
{
  static uint32_t cnt = 0;
  const char *fan[] = { "-", "\\", "|", "/" };
  app_log("(%s)", fan[((cnt++) % 4)]);
  app_log("\b\b\b");     // Move cursor back with backspace character '\b'.
}
