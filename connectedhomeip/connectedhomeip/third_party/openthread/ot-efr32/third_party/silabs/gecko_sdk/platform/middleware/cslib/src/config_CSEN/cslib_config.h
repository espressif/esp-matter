/***************************************************************************//**
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef CSLIB_CONFIG_H
#define CSLIB_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>CSLIB settings

// <o DEF_BUTTON_DEBOUNCE> Debounce <1-15>
// <i> Sets the number of samples crossing active/inactive thresh. before touch is qualified/disqualified.
// <i> Default: 2
#define DEF_BUTTON_DEBOUNCE                       2

// <o DEF_ACTIVE_MODE_PERIOD> Scan period in active mode <1-5000>
// <i> Sets the period of scanning in active mode.
// <i> Actual scan frequency is determined by this setting and by DEF_FREE_RUN_SETTING
// <i> Default: 100
#define DEF_ACTIVE_MODE_PERIOD                    100

// <o DEF_SLEEP_MODE_PERIOD> Scan period in sleep mode <1-5000>
// <i> Sets the period of scanning in sleep mode.
// <i> Default: 100
#define DEF_SLEEP_MODE_PERIOD                     100

// <o DEF_COUNTS_BEFORE_SLEEP> Delay before sleep <1-255>
// <i> Number of sequential periods of scanning in active mode with no qualified
// <i> touch detected before system enters sleep.
// <i> Default: 100
#define DEF_COUNTS_BEFORE_SLEEP                   100

// <q DEF_FREE_RUN_SETTING> Free run setting
// <i> Configures whether system does a single scan every DEF_ACTIVE_MODE_PERIOD
// <i> and then enters sleep for the remainder of the period (when cleared to 0),
// <i> or if the system scans continuously (when set to 1).
// <i> Default: 0
#define DEF_FREE_RUN_SETTING                      0

// <q DEF_SLEEP_MODE_ENABLE> Enable sleep mode
// <i> Set this to 1 to enable sleep mode sensing, set this to 0 to always remain in
// <i> active model
// <i> Default: 1
#define DEF_SLEEP_MODE_ENABLE                     1

// </h>

// <<< end of configuration section >>>

#endif // CSLIB_CONFIG_H
