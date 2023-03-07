/***************************************************************************//**
 * @file sl_apploader_util.h
 * @brief Bluetooth AppLoader Utility
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_APPLOADER_UTIL_H
#define SL_APPLOADER_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup apploader_util
 * @{
 *
 * @brief AppLoader Utility
 *
 * The Apploader utility provides the convenience functionality related to OTA
 * DFU.
 *
 ******************************************************************************/

/** @} end apploader_util */

#include <stdint.h>

/**
 * Reset the device to OTA DFU mode.
 */
void sl_apploader_util_reset_to_ota_dfu();

#ifdef __cplusplus
}
#endif

#endif // SL_APPLOADER_UTIL_H
