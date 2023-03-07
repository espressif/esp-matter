/***************************************************************************//**
 * @brief Zigbee Trust Center Backup component configuration header.
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

// <h>Zigbee Trust Center Backup Configuration

// <o EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_MAX_CLI_BACKUP_SIZE> Maximum CLI backup size <1-255>
// <i> Default: 6
// <i> The maximum number of import/export entries that can be manipulated by the CLI.  This should be set larger than the link key table.
#define EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_MAX_CLI_BACKUP_SIZE   6

// <q EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT> Enable back up to POSIX file system
// <i> Default: FALSE
// <i> This enables APIs and CLI commands for exporting/importing Trust Center backup data to/from a POSIX file system.
#define EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT   0

// </h>

// <<< end of configuration section >>>
