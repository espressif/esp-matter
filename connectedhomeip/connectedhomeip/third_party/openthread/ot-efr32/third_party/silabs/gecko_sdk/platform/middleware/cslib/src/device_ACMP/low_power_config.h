/***************************************************************************//**
 * @file
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

#ifndef LOW_POWER_CONFIG_H
#define LOW_POWER_CONFIG_H

// Functions which must be defined with implementation-specific
// responsibilities.  These are called by LowPowerRoutines.c
void CSLIB_configureSensorForSleepModeCB(void);
void CSLIB_configureTimerForSleepModeCB(void);
void CSLIB_configureTimerForActiveModeCB(void);
void CSLIB_enterLowPowerStateCB(void);
void CSLIB_checkTimerCB(void);
extern uint8_t timerTick;

#endif // LOW_POWER_CONFIG_H
