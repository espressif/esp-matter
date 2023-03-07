/***************************************************************************//**
 * @file app_init.c
 * @brief app_init.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "gpd-components-common.h"
#include "sl_rail_util_init.h"

void app_init(void)
{
  // Initialise NV
  emberGpdNvInit();

  // Initialise timer for rxOffset timing during rxAfterTx
  emberGpdLeTimerInit();

  // Initialise Radio
  emberGpdRadioInit();

  // Provide the rail handle from the configured RAIL instance
  emberGpdRailProvideRailHandle(sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_GPDRAIL));

  //Initialise the Gpd
  (void)emberGpdInit();
}

void emberGpdLeTimerInit(void)
{
  // For Low Energy Timing the sleep timer is used , which uses RTC/RTCC
  // Ensure the clock is enabled.

#if defined _SILICON_LABS_32B_SERIES_2
  // Setting RTCC clock source
  CMU_ClockSelectSet(cmuClock_RTCCCLK, cmuSelect_LFRCO);
#elif defined _SILICON_LABS_32B_SERIES_1
  // Turn on the clock for the RTCC
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#endif
  CMU_ClockEnable(cmuClock_RTCC, true);
}
