/***************************************************************************//**
 * @file
 * @brief See @ref alone_bootload for documentation.
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

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup alone_bootload Standalone Bootloader
 * @brief Definition of the standalone bootloader interface
 *
 * Some functions in this file return an ::EmberStatus value. See
 * error-def.h for definitions of all ::EmberStatus return values.
 *
 * See bootloader-interface-standalone.h for source code.
 *@{
 */

#ifndef __BOOTLOADER_INTERFACE_STANDALONE_H__
#define __BOOTLOADER_INTERFACE_STANDALONE_H__

/** @brief Detects if the standalone bootloader is installed, and if so
 *    returns the installed version.
 *
 *  A returned version of 0x1234 would indicate version 1.2 build 34
 *
 * @return ::BOOTLOADER_INVALID_VERSION if the standalone bootloader is not
 *    present, or the version of the installed standalone bootloader.
 */
uint16_t halGetStandaloneBootloaderVersion(void);

/** @brief Define a numerical value for NO BOOTLOADER mode.  In other words,
 *    the bootloader should not be run.
 */
#define NO_BOOTLOADER_MODE                 0xFF

/** @brief Define a numerical value for the normal bootloader mode.
 */
#define STANDALONE_BOOTLOADER_NORMAL_MODE   1

/** @brief Define a numerical value for the recovery bootloader mode.
 */
#define STANDALONE_BOOTLOADER_RECOVERY_MODE 0

/** @brief Quits the current application and launches the standalone
 * bootloader (if installed). The function returns an error if the standalone
 * bootloader is not present.
 *
 * @param mode  Controls the mode in which the standalone bootloader will run.
 * See the bootloader Application Note for full details.  Options are:
 *   - ::STANDALONE_BOOTLOADER_NORMAL_MODE
 *     Will listen for an over-the-air
 *     image transfer on the current channel with current power settings.
 *   - ::STANDALONE_BOOTLOADER_RECOVERY_MODE
 *     Will listen for an over-the-air
 *     image transfer on the default channel with default power settings.
 *   .
 *   Both modes also allow an image transfer to begin via serial xmodem.
 *
 * @return  An ::EmberStatus error if the standalone bootloader is not present,
 * or ::EMBER_SUCCESS.
 */
EmberStatus halLaunchStandaloneBootloader(uint8_t mode);

#endif //__BOOTLOADER_INTERFACE_STANDALONE_H__

/** @} (end addtogroup app_bootload) */
/** @} (end addtogroup legacyhal) */
