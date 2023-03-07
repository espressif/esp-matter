/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Manufacturing Library CLI plugin.
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
 * @defgroup manufacturing-library-cli Manufacturing Library CLI
 * @ingroup component
 * @brief API and Callbacks for the Manufacturing Library CLI Component
 *
 */

/**
 * @addtogroup manufacturing-library-cli
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Return true if the manufacturing library is currently running.
 *
 * The function returns whether the manufacturing library is currently running.
 * Code that initiates scan/join behavior should not do so if the manufacturing
 * library is currently running as this will cause a conflict and may result
 * in a fatal error.
 *
 * @return A ::bool value that is true if the manufacturing library is
 * running and false if it is not.
 */
bool emberAfMfglibRunning(void);

/** @brief Return true if the manufacturing library token has been set.
 *
 * The function returns whether the manufacturing library token has currently
 * been set. Reference designs are programmed to initiate scan/join
 * behavior as soon as the device has been powered up. Certain sleepy devices,
 * such as security sensors, may also use the UART for manufacturing, which
 * becomes inactive during normal operation. Setting this token allows
 * the device to stay awake or hold off on normal joining behavior for a few
 * seconds to allow manufacturing mode to be enabled. The last step in the
 * manufacturing process is to disable this token.
 *
 * Note:  This token is disabled by default. To enable it
 * in your application, edit the file
 * app/framework/plugin/manufacturing-library-cli/manufacturing-library-cli-tokens.h.
 *
 * @return A ::bool value that is true if the manufacturing library token
 * has been set and false if it has not been set.
 */
bool emberAfMfglibEnabled(void);

/** @brief Start the manufacturing test mode.
 *
 * @param wantCallback Indicates whether to enable the receive callback and tabulate
 * statistics.
 *
 */
void emberAfMfglibStart(bool wantCallback);

/** @brief Stop the manufacturing test mode.
 *
 */
void emberAfMfglibStop(void);

/** @brief Return the saved RX test statistics.
 *
 * @param packetsReceived Total number of packets received during the test.
 *
 * @param savedRssiReturn RSSI from the first received packet.
 *
 * @param savedLqiReturn LQI from the first received packet.
 *
 */
void emberAfMfglibRxStatistics(uint16_t* packetsReceived,
                               int8_t* savedRssiReturn,
                               uint8_t* savedLqiReturn);

/** @} */ // end of name API
/** @} */ // end of manufacturing-library-cli

#endif
