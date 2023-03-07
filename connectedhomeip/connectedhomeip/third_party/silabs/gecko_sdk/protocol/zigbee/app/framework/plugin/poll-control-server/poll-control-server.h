/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Poll Control Server plugin.
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
 * @defgroup poll-control-server  Poll Control Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Poll Control Cluster Server Component
 *
 * Silicon Labs implementation of the Poll Control server cluster.
 * The component periodically checks in with Poll Control clients
 * to see if any client wants to manage the poll period of the server.
 * Any device listed in the binding table for the Poll Control cluster
 * is assumed to be a client. At startup, this component sets the
 * long and short poll intervals of the device to the values of the
 * long and short poll interval attributes.
 *
 */

/**
 * @addtogroup poll-control-server
 * @{
 */

/**
 * @name API
 * @{
 */

/**
 * @brief Set whether the Poll Control server uses non TC clients.
 *
 * Sets the behavior of the Poll Control server when determining whether or not
 * to use non Trust Center Poll Control clients.
 *
 * @param IgnoreNonTc a bool determining whether the server should ignore any
 *        non Trust Center Poll Control clients
 */
void emberAfPluginPollControlServerSetIgnoreNonTrustCenter(bool ignoreNonTc);

/**
 * @brief Return the current value of ignoreNonTrustCenter for the Poll Control server.
 */
bool emberAfPluginPollControlServerGetIgnoreNonTrustCenter(void);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup poll_control_server_cb Poll Control
 * @ingroup af_callback
 * @brief Callbacks for Poll Control Component
 *
 */

/**
 * @addtogroup poll_control_server_cb
 * @{
 */

/** @brief CheckInTimeout.
 *
 * This function is called by the Poll Control Server plugin after a threshold value of poll control
 * check in messages are sent to a trust center and no responses are received. This
 * callback provides an opportunity for the application to implement its own rejoin algorithm or logic.
 */
void emberAfPluginPollControlServerCheckInTimeoutCallback(void);

/** @} */ // end of poll_control_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of poll-control-server
