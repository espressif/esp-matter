/***************************************************************************//**
 * @brief Zigbee OTA Bootload Cluster Storage Common Code component configuration header.
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

// <h>Zigbee OTA Bootload Cluster Storage Common Code configuration

// <o EMBER_AF_PLUGIN_OTA_STORAGE_COMMON_MAX_TAGS_IN_OTA_FILE> Max Tags In OTA File <1-16>
// <i> Default: 6
// <i> Maximum amount of tags embedded within an OTA file. Most images will include an Upgrade Image tag, a certificate tag and a signature tag. The default max should be more than enough.
#define EMBER_AF_PLUGIN_OTA_STORAGE_COMMON_MAX_TAGS_IN_OTA_FILE   6

// </h>

// <<< end of configuration section >>>
