/***************************************************************************//**
 * @file
 * @brief Definitions of reset types compatible with EM2xx usage.
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
#ifndef __EM2XX_RESET_DEFS_H__
#define __EM2XX_RESET_DEFS_H__

/** @addtogroup em2xx_resets
 *@{
 */

/** @brief EM2xx-compatible reset code returned by halGetEm2xxResetInfo()
 *@{
 */
#define EM2XX_RESET_UNKNOWN               0
#define EM2XX_RESET_EXTERNAL              1   // EM2XX reports POWERON instead
#define EM2XX_RESET_POWERON               2
#define EM2XX_RESET_WATCHDOG              3
#define EM2XX_RESET_ASSERT                6
#define EM2XX_RESET_BOOTLOADER            9
#define EM2XX_RESET_SOFTWARE              11

/**@} */

/**@} end of CRC Functions
 */

#endif    //__EM2XX_RESET_DEFS_H__
