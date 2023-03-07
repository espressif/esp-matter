/***************************************************************************//**
 * @file
 * @brief Example - SNTP Client Application Functions File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_SNTP_CLIENT_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/net_app.h>
#include  <net/include/sntp_client.h>

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef  EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/****************************************************************************************************//**
 *                                          Ex_SNTP_Client_Init()
 *
 * @brief  Initialize the Micrium OS SNTP Client module for the example application.
 *******************************************************************************************************/
void Ex_SNTP_Client_Init(void)
{
  RTOS_ERR err;

  //                                                               ------------- INITIALIZE CLIENT SUITE --------------
  SNTPc_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                     Ex_SNTP_Client_CurTimeDisplay()
 *
 * @brief  Displays current time.
 *
 * @note   (1) The timestamp returned by SNTPc_GetRemoteTime() represents the quantity of seconds
 *             since January 1, 1900. It also gives the fractions of second.
 *******************************************************************************************************/
void Ex_SNTP_Client_CurTimeDisplay(void)
{
  CPU_INT08U sec;
  CPU_INT08U min;
  CPU_INT08U hour;
  CPU_INT16U day;
  CPU_INT16U year;
  CPU_INT16U qty_leap_years;
  CPU_INT32U temp;
  SNTP_PKT   pkt;
  SNTP_TS    ts;
  RTOS_ERR   err;

  SNTPc_ReqRemoteTime("0.pool.ntp.org",                         // Send SNTP request.
                      &pkt,
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  ts = SNTPc_GetRemoteTime(&pkt, &err);                         // Retrieve current time.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  sec = ts.Sec % 60u;                                           // ts.Sec represents qty of seconds since Jan 1, 1900.
  temp = ts.Sec / 60u;

  min = temp % 60u;
  temp /= 60u;

  hour = temp % 24u;
  temp /= 24u;

  //                                                               Assume all years have 365 days for now.
  year = 1900u + (temp / 365u);
  qty_leap_years = (year / 4u) - (year / 100u) + (year / 400u);
  qty_leap_years -= 460u;                                       // 460 leap years between year 0 and 1900.
  day = temp % 365u;

  if (((year % 4u == 0u)
       && (year % 100u != 0u))
      || (year % 400u == 0u)) {
    qty_leap_years--;                                           // Est year is leap, do not consider in date calc.
  }

  if (day < qty_leap_years) {                                   // Date is in year before estimated year.
    CPU_INT16U new_yr_qty_days;

    year -= 1u;
    new_yr_qty_days = (((year % 4u == 0u) && (year % 100u != 0u)) || (year % 400u == 0u)) ? 366u : 365u;
    day = new_yr_qty_days - (qty_leap_years - day);
  } else {                                                      // Date is in estimated year.
    day -= qty_leap_years;
  }

  day++;                                                        // Convert day starting from 0 to starting from 1.

  EX_TRACE("Day %u of year %u. Time: %u:%u:%u(UTC)\r\n", day, year, hour, min, sec);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_SNTP_CLIENT_AVAIL
