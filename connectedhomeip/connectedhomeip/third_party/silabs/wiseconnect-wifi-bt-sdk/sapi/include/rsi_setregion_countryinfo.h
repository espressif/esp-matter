/*******************************************************************************
* @file  rsi_setregion_countryinfo.h
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

#ifndef _SETREGION_COUNTRYINFO_H_
#define _SETREGION_COUNTRYINFO_H_

#include <rsi_driver.h>

#define REGION_US        1
#define REGION_EU        2
#define REGION_JP        3
#define REGION_INDIA     4
#define REGION_SINGAPORE 5
#define REGION_KOREA     6

void extract_setregionap_country_info(rsi_req_set_region_ap_t *);
#endif
