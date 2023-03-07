/***************************************************************************//**
 * @file
 * @brief Definitions for the Delayed Join plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef DELAYED_JOIN_H
#define DELAYED_JOIN_H

/**
 * @defgroup delayed-join Delayed Join
 * @ingroup component
 * @brief API and Callbacks for the Delayed Join Component
 *
 * This component is used both for a trust center and a joining device. It
 * provides a set of APIs for setting the acceptable delay for receiving a
 * network key, as well as sending the network key on demand from the app. The
 * following are the minimum conditions to verify before successfully using this
 * component:
 *  - 1.The trust center must have an acceptable version of a link key for the
 * joining devices.
 *  - 2.The component must be (re-)activated every time a network is opened or
 * after any other event that changes the trust center (re-)join policy.
 *
 */

/**
 * @addtogroup delayed-join
 * @{
 */

#define EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME "Delayed Join"

/**
 * @name API
 * @{
 */

/** @brief Set the network timeout value
 *
 * @param timeout timeout value. Acceptable range 3-255 Ver.: always
 *
 */
void emberAfPluginDelayedJoinSetNetworkKeyTimeout(uint8_t timeout);

/** @} */ // end of name APIs
/** @} */ // end of delayed-join

#endif
