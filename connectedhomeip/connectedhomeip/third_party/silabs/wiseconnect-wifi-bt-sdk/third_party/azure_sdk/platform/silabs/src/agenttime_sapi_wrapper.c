/*******************************************************************************
* @file  agenttime_sapi_wrapper.c
* @brief
*******************************************************************************
* Copyright (c) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE file in the project root for full license information.
*
*******************************************************************************
*
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

/**
 * Includes
 */
#ifdef AZURE_ENABLE
#include <time.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/agenttime.h"
#include <stdint.h>
#include "rsi_common_apis.h"
#include "rsi_hal.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_rtc.h"
#define YEAR 4
#define MONTH 12
unsigned short days[YEAR][MONTH] = {
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
    };
#elif STM32F411xE
//Nothing to include here
#elif EFR32MG21A020F1024IM32
//Nothing to include here
#else
	/* Device specific, needs implementation if not the above devices */
#endif


int32_t set_time(uint32_t Epoch)
{
  int32_t ret_val = RSI_SUCCESS;
#if defined(STM32F411xE)|| defined(EFR32MG21A020F1024IM32)
  ret_val = rsi_rtc_set_time(Epoch);
#elif RSI_M4_INTERFACE
	uint32_t years 	  = 0;
	uint8_t year	  = 0;
	uint8_t month	  = 0;
	uint8_t dayOfWeek = 4; //starting day of Week 1/1/1970 is Thursday
	RTC_TIME_CONFIG_T rtcConfig = {0};

	RSI_RTC_Init(RTC);
	rtcConfig.Second = Epoch%60;
	Epoch /= 60;
	rtcConfig.Minute = Epoch%60;
	Epoch /= 60;
	rtcConfig.Hour   = Epoch%24;
	Epoch /= 24;

	years = Epoch/(365*4+1)*4; Epoch %= 365*4+1;

	for (year=3; year>0; year--)
	{
		if (Epoch >= days[year][0])
			break;
	}

	for (month=11; month>0; month--)
	{
		if (Epoch >= days[year][month])
			break;
	}
	rtcConfig.Year  				= years+year;
	rtcConfig.Month  				= month+1;
	rtcConfig.Day   				= Epoch-days[year][month]+1;
	rtcConfig.DayOfWeek    	= (dayOfWeek += month < 3 ? year-- : year - 2, 23*month/9 + dayOfWeek + 4 + year/4- year/100 + year/400)%7;
	rtcConfig.Century      	= 0;
	rtcConfig.MilliSeconds 	= 0;
	RSI_RTC_SetDateTime(RTC, &rtcConfig);
#else
	ret_val = RSI_FAILURE;
#endif
	if(ret_val != RSI_SUCCESS)
	{
		LOG_PRINT("Set time Failed");
	}
	return ret_val;
}

time_t get_time(time_t *p)
{
	uint32_t epoch1 = 0;
#if defined(STM32F411xE)|| defined(EFR32MG21A020F1024IM32)
	epoch1 = rsi_rtc_get_time();
#elif RSI_M4_INTERFACE
	RTC_TIME_CONFIG_T value = {0};
	RSI_RTC_GetDateTime(RTC, &value);
	epoch1 = (((value.Year/4*(365*4+1)+days[value.Year%4][value.Month -1]+(value.Day -1))*24+value.Hour)*60+value.Minute)*60+value.Second;
#else
	epoch1 = 0;
#endif
		return epoch1; 
}

struct tm *get_gmtime(time_t *currentTime)
{
  return gmtime(currentTime);
}

time_t get_mktime(struct tm *cal_time)
{
  return mktime(cal_time);
}

char *get_ctime(time_t *timeToGet)
{
  return ctime(timeToGet);
}

double get_difftime(time_t stopTime, time_t startTime)
{
  return difftime(stopTime, startTime);
}

time_t time(time_t *p)
{
  return get_time(p);
}
#endif
