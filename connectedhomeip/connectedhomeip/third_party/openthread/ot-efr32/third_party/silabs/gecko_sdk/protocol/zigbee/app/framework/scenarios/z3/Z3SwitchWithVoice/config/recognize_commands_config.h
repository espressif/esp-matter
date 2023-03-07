/***************************************************************************//**
 * @file
 * @brief Command Recognition Config
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

#ifndef RECOGNIZE_COMMANDS_CONFIG_H
#define RECOGNIZE_COMMANDS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Keyword recognition configuration
// <i> Settings for further recognition of the instantaneous model outputs

// <o> Smoothing window duration [ms] <300-1000>
// <i> Sets the duration of a time window from which the network results will be
// <i> averaged together to determine a keyword detection. Longer durations
// <i> reduce the chance of misdetections, but lowers the confidence score which
// <i> may result in missed commands.
// <i> Default: 1000
#define SMOOTHING_WINDOW_DURATION_MS 1000

// <o> Minimum detection count <0-50>
// <i> Sets the minimum number of results required in the smoothing window for
// <i> the keyword detection to be considered a reliable result.
// <i> Default: 2
#define MINIMUM_DETECTION_COUNT 3

// <o> Detection Threshold <0-255>
// <i> Sets a threshold for determining the confidence score required in order
// <i> to classify a keyword as detected. This can be increased to avoid
// <i> misclassifications.
// <i> The confidence scores are in the range <0-255>
// <i> Default: 160
#define DETECTION_THRESHOLD 160

// <o> Suppresion time after detection [ms] <0-2000>
// <i> Sets a time window to wait after a detected keyword before triggering
// <i> a new detection.
// <i> Default: 750
#define SUPPRESION_TIME_MS 750

// <<< end of configuration section >>>

#endif // RECOGNIZE_COMMANDS_CONFIG_H
