/***************************************************************************//**
 * @file
 * @brief Health Thermometer GATT service configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_HEALTH_THERMOMETER_CONFIG_H
#define SL_HEALTH_THERMOMETER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_BT_HT_MEASUREMENT_INTERVAL_SEC> Health Thermometer Measurement Interval (UUID: 2A21) <1-65535>
// <i> Default: 1
// <i> The Measurement Interval is the time interval between two Temperature Measurement indications.
#define SL_BT_HT_MEASUREMENT_INTERVAL_SEC   1

// <o SL_BT_HT_TEMPERATURE_TYPE> Health Thermometer Temperature Type (UUID: 2A1D)
// <SL_BT_HT_TEMPERATURE_TYPE_ARMPIT=> Armpit
// <SL_BT_HT_TEMPERATURE_TYPE_BODY=> Body (general)
// <SL_BT_HT_TEMPERATURE_TYPE_EAR=> Ear (usually ear lobe)
// <SL_BT_HT_TEMPERATURE_TYPE_FINGER=> Finger
// <SL_BT_HT_TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT=> Gastro-intestinal Tract
// <SL_BT_HT_TEMPERATURE_TYPE_MOUTH=> Mouth
// <SL_BT_HT_TEMPERATURE_TYPE_RECTUM=> Rectum
// <SL_BT_HT_TEMPERATURE_TYPE_TOE=> Toe
// <SL_BT_HT_TEMPERATURE_TYPE_TYMPANUM=> Tympanum (ear drum)
// <i> Default: SL_BT_HT_TEMPERATURE_TYPE_BODY
#define SL_BT_HT_TEMPERATURE_TYPE           SL_BT_HT_TEMPERATURE_TYPE_BODY

// <<< end of configuration section >>>

#endif // SL_HEALTH_THERMOMETER_CONFIG_H
