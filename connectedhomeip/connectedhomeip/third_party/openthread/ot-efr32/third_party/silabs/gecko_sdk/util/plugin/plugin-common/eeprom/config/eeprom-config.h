/***************************************************************************//**
 * @brief Eeprom driver component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

// <h>Eeprom configuration

// <o EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT> Max Devices <1-10>
// <i> Default: 2
// <i> The number of partial words that are stored by this plugin for writing later.  If a subsequent write to the following address of the partial word is made, or a call to emberAfPluginEepromFlushSavedPartialWrites() is made, then the partial words are written out.
#define EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT   2

// </h>

// <<< end of configuration section >>>
