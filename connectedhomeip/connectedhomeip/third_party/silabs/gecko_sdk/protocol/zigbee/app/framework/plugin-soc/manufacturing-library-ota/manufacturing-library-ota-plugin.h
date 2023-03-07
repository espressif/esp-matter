/***************************************************************************//**
 * @file
 * @brief Definitions for the Manufacturing Library OTA plugin.
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

#ifndef SILABS_MANUFACTURING_LIBRARY_CLI_PLUGIN_H
#define SILABS_MANUFACTURING_LIBRARY_CLI_PLUGIN_H

/**
 * @defgroup manufacturing-library-ota Manufacturing Library OTA
 * @ingroup component soc
 * @brief API and Callbacks for the Manufacturing Library OTA Component
 *
 */

/**
 * @addtogroup manufacturing-library-ota
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Indicate whether the manufacturing library is currently running.
 *
 * Used to check whether the manufacturing library is currently running.
 * Do not initiate scan/join behavior when the manufacturing
 * library is currently running as this will cause a conflict and may result
 * in a fatal error.
 *
 * @return A ::bool value that is true if the manufacturing library is
 * running, or false if it is not.
 */
bool emberAfMfglibRunning(void);

/** @brief Indicate whether the manufacturing library token has been set.
 *
 * Returns whether the manufacturing library token has currently
 * been set. Reference designs are programmed to initiate off scan/join
 * behavior as soon as the device has been powered up. Certain sleepy devices,
 * such as security sensors, may also use the UART for manufacturing, which
 * becomes inactive during normal operation. Setting this token allows
 * the device to stay awake or hold off on normal joining behavior for a few
 * seconds to allow manufacturing mode to be enabled. The last step in the
 * manufacturing process is to disable this token.
 *
 * Note: This token is disabled by default. To enable this token
 * in the application, edit the file
 * app/framework/plugin/mfg-lib/mfg-lib-tokens.h.
 *
 * @return A ::bool value that is true if the manufacturing library token
 * has been set or false if it has not been set.
 */
bool emberAfMfglibEnabled(void);

/** @} */ // end of name API
/** @} */ // end of manufacturing-library-ota

#endif
