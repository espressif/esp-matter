/***************************************************************************//**
 * @brief Zigbee Color Control Cluster Server component configuration header.
 *\n*******************************************************************************
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Color Control Cluster Server configuration

// <q EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_XY> CIE 1931
// <i> Default: TRUE
// <i> Support CIE 1931 Commands.  Note:  selecting this option requires the current X and current Y attributes be selected.  Also, a default report table entries will be created for these attributes.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_XY   1

// <q EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_TEMP> Color Temperature
// <i> Default: TRUE
// <i> Support Color Temperature Commands.  Note:  selecting this option requires the color temperature attribute be selected.  Also, a default report table entries will be created for this attribute.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_TEMP   1

// <q EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_HSV> Hue Saturation
// <i> Default: TRUE
// <i> Support Hue Saturation Commands.  Note:  selecting this option requires the current hue and current saturation attributes be selected.  Also, a default report table entry will be created for these attributes.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_HSV   1

// <o EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_MIN_REPORT_INTERVAL> Min Reporting Interval <1-65535>
// <i> Default: 1
// <i> Default minimum Reporting Interval in Seconds for the color control server.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_MIN_REPORT_INTERVAL   1

// <o EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_MAX_REPORT_INTERVAL> Max Reporting Interval <1-65535>
// <i> Default: 65535
// <i> Default maximum Reporting Interval in Seconds for the color control server.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_MAX_REPORT_INTERVAL   65535

// <o EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_COLOR_XY_CHANGE> Color XY Reportable Change <1-65535>
// <i> Default: 1
// <i> Default minimum reportable change for the colorX and colorY attributes.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_COLOR_XY_CHANGE   1

// <o EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_COLOR_TEMP_CHANGE> Color Temperature Reportable Change <1-65535>
// <i> Default: 1
// <i> Default minimum reportable change for the color temperature attribute.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_COLOR_TEMP_CHANGE   1

// <o EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_HUE_CHANGE> Hue Reportable Change <1-255>
// <i> Default: 1
// <i> Default minimum reportable change for the hue attribute.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_HUE_CHANGE   1

// <o EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_SATURATION_CHANGE> Saturation Reportable Change <1-255>
// <i> Default: 1
// <i> Default minimum reportable change for the saturation attribute.
#define EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_SATURATION_CHANGE   1

// </h>

// <<< end of configuration section >>>
