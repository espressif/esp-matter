/***************************************************************************//**
 * @brief Zigbee Green Power Translation Table component configuration header.
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

// <h>Zigbee Green Power Translation Table configuration

// <q EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_TRANSLATION_TABLE> User has the default translation table emberGpDefaultTranslationTable.
// <i> Default: FALSE
// <i> The user provided table will be used for the gpd command translation. The table is of type EmberAfGreenPowerServerGpdSubTranslationTableEntry.
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_TRANSLATION_TABLE   0

// <q EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_GENERIC_SWITCH_TRANSLATION_TABLE> User has the default configuration table emberGpSwitchTranslationTable.
// <i> Default: FALSE
// <i> The user provided table will be used for the gpd command translation. The table is of type EmberAfGreenPowerServerDefautGenericSwTranslation.
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_GENERIC_SWITCH_TRANSLATION_TABLE   0

// <o EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE> Customized Gpd Translation Table Size <1-255>
// <i> Default: 1
// <i> Size of the customized translation table. Customized Translation table holds the translation mapping between a custom gpd Command to a ZCL profile/cluster/command. For example, a custom Gpd Command 0xCC mapped to HA(0x0104)/OnOff(0x0006)/Toggle(2) command from a GPD. Where as the default mapping is Gpd Command 0x22 mapped to HA(0x0104)/OnOff(0x0006)/Toggle(2)
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE   1

// <o EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE> Translation Table Size <1-255>
// <i> Default: 30
// <i> Size of the translation table. Translation table holds the translation mapping for  a gpd between a gpdCommand to a zigbee profile/cluster/command. This table can hold either a default/standard mapping( for example a Gpd Toggle command mapped to HA/OnOff/Toggle command) or custom command mapping( example a custom Gpd Command , say, 0xCC mapped to HA(0x0104)/OnOff(0x0006)/Toggle(2) command). The dafault maps are defined number of constant table but a customized table may be set to a application supplied translations.
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE   30

// <q EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS> On SOC platform, store the table in persistent memory
// <i> Default: TRUE
// <i> On an SOC platform, this option enables the persistent storage of the translation table FLASH memory using the tokens.
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USE_TOKENS   1

// </h>

// <<< end of configuration section >>>
