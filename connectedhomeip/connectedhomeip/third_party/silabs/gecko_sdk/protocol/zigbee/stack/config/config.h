/***************************************************************************//**
 * @file
 * @brief See @ref stack_info for documentation.
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

/** @addtogroup stack_info
 * See also config.h.
 *
 * This documentation was produced from the following software release and build.
 * <table border=1>
 * <tr><td>SOFTWARE_VERSION</td><td>0x4700</td><td>High byte = release number,
 * low byte = patch number</td></tr>
 * </table>
 *
 * @{
 */

#ifndef SILABS_CONFIG_CONFIG_H
#define SILABS_CONFIG_CONFIG_H

// The 4 digit version: A.B.C.D
#define EMBER_MAJOR_VERSION  7
#define EMBER_MINOR_VERSION  1
#define EMBER_PATCH_VERSION  1
#define EMBER_SPECIAL_VERSION  0

// 2 bytes
#define EMBER_BUILD_NUMBER 0
#define EMBER_FULL_VERSION (  ((uint16_t)EMBER_MAJOR_VERSION << 12)   \
                              | ((uint16_t)EMBER_MINOR_VERSION <<  8) \
                              | ((uint16_t)EMBER_PATCH_VERSION <<  4) \
                              | ((uint16_t)EMBER_SPECIAL_VERSION))

#define EMBER_VERSION_TYPE EMBER_VERSION_TYPE_GA

/**
 * Software version. High byte = release number, low byte = patch number.
 */
#define SOFTWARE_VERSION EMBER_FULL_VERSION

/** @} End group
 */

#endif // SILABS_CONFIG_CONFIG_H
