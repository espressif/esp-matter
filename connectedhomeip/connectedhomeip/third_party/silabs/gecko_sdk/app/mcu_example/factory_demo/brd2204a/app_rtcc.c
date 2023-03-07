/***************************************************************************//**
 * @file
 * @brief Helper functions for timekeeping using the RTCC
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

#include <stdint.h>

#include "em_rtcc.h"
#include "em_cmu.h"

#include "app_rtcc.h"

/**************************************************************************//**
 * @brief  Sets up the RTCC
 *****************************************************************************/
void setupRTCC(uint32_t resetVal)
{
  // Make sure the RTCC is disabled
  RTCC_Enable(false);

  // RTCC configuration constant table.
  static RTCC_Init_TypeDef initRTCC = RTCC_INIT_DEFAULT;

  // Keep the RTCC running in backup mode
  initRTCC.enaBackupModeSet = true;

  // Set the RTCC frequency to 1024 Hz
  initRTCC.presc = rtccCntPresc_32;

  // Initialize RTCC
  RTCC_Init(&initRTCC);

  // Clear status to enable storing backup mode entering timestamp
  RTCC->CMD = RTCC_CMD_CLRSTATUS;

  /* Set RTCC to the value it had at wakeup, which ensures continuity if recovering
   * from a backup event */
  RTCC_CounterSet(resetVal);

  // Finally enable RTCC
  RTCC_Enable(true);

  // Wait while sync is busy
  while (RTCC->SYNCBUSY) ;
}

/**************************************************************************//**
 * @brief Return the current system run time in milliseconds.
 *****************************************************************************/
uint32_t millis(void)
{
  return (RTCC_CounterGet() * 1000) / 1024;
}

/**************************************************************************//**
 * @brief Halt execution for ms milliseconds.
 *****************************************************************************/
void delay_ms(uint32_t ms)
{
  uint32_t start = millis();
  while ((millis() - start) < ms) ;
}
