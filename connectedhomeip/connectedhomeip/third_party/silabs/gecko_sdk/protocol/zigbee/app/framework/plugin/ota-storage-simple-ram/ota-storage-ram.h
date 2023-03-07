/***************************************************************************//**
 * @file
 * @brief APIs and defines for the OTA Storage Simple RAM plugin.
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

/**
 * @defgroup ota-storage-simple-ram  OTA Storage Simple RAM
 * @ingroup component
 * @brief API and Callbacks for the ota-storage-simple-ram Component
 *
 * This is a test implementation of a storage driver for the Over-the-Air
 * simple storage module. It uses RAM to store a single, non-bootable,
 * test image. It is intended for basic demonstration purposes only and
 * is not production-ready. It can be used for either an OTA Client or
 * OTA Server.
 */

/**
 * @addtogroup ota-storage-simple-ram
 * @{
 */

/** @} */ // end of ota-stoarge-simple-ram

void emAfOtaStorageDriverCorruptImage(uint16_t index);
uint16_t emAfOtaStorageDriveGetImageSize(void);
