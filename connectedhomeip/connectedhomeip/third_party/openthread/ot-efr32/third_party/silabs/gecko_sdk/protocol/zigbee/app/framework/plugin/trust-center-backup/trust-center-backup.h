/***************************************************************************//**
 * @file trust-center-backup.h
 * @brief Definitions for the Trust Center Backup plugin.
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

#ifdef UC_BUILD
#include "trust-center-backup-config.h"
#if (EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT == 1)
#define POSIX_FILE_BACKUP_SUPPORT
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
#define POSIX_FILE_BACKUP_SUPPORT
#endif
#endif // UC_BUILD

/**
 * @defgroup trust-center-backup Trust Center Backup
 * @ingroup component
 * @brief API and Callbacks for the Trust Center Backup Component
 *
 * This component is only for a trust center.  It provides a set of APIs
 * for importing and exporting the backup info for a Smart Energy trust center.
 * It requires extending to hook up import/export routines into an external
 * storage device or network, where the data may be saved to and restored from.
 *
 */

/**
 * @addtogroup trust-center-backup
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief export backup data
 *
 * @param backup Ver.: always
 *
 * @return EmberStatus status code
 *
 */
EmberStatus emberTrustCenterExportBackupData(EmberAfTrustCenterBackupData* backup);

/** @brief Import backup and start network
 *
 * @param nbackup Ver.: always
 *
 * @return EmberStatus status code
 *
 */
EmberStatus emberTrustCenterImportBackupAndStartNetwork(const EmberAfTrustCenterBackupData* backup);

/** @brief Import backup from file
 *
 * @param filepath Ver.: always
 *
 * @return EmberStatus status code
 *
 * @note Available only for EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
 *
 */
EmberStatus emberAfTrustCenterImportBackupFromFile(const char* filepath);

/** @brief export backup to a file
 *
 * @param filepath Ver.: always
 *
 * @return EmberStatus status code
 *
 * @note Available only for EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
 *
 */
EmberStatus emberAfTrustCenterExportBackupToFile(const char* filepath);

/** @brief saves tokens to a file
 *
 * @param filepath Ver.: always
 *
 * @return EmberStatus status code
 *
 * @note Available only for EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
 *
 */
EmberStatus emberAfTrustCenterBackupSaveTokensToFile(const char* filepath);

/** @brief restores tokens from a file
 *
 * @param filepath Ver.: always
 *
 * @return EmberStatus status code
 *
 * @note Available only for EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
 *
 */
EmberStatus emberAfTrustCenterBackupRestoreTokensFromFile(const char* filepath);

/** @brief Updates zigbeed tokens from a file that stores NCP tokens.
 *
 * @param filepath Ver.: always
 *
 * @return EmberStatus status code
 *
 * @note Available only for EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
 *
 */
EmberStatus emberAfTrustCenterBackupWriteNcpTokenToZigbeedTokens(const char* filepath);

/** @} */ // end of name API
/** @} */ // end of trust-center-backup
#ifdef UC_BUILD
void emAfTcExportCommand(sl_cli_command_arg_t *arguments);
void emAfTcImportCommand(sl_cli_command_arg_t *arguments);
void emAfTrustCenterBackupSaveTokensToFileCli(sl_cli_command_arg_t *arguments);
void emAfTrustCenterBackupRestoreTokensFromFileCli(sl_cli_command_arg_t *arguments);
#else
void emAfTcExportCommand(void);
void emAfTcImportCommand(void);
#endif
