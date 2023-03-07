/***************************************************************************//**
 * @file
 * @brief Definitions for the Simple Metering Client plugin, which implements
 *        the client side of the Simple Metering cluster.
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

#ifndef SIMPLE_METERING_CLIENT_H_
#define SIMPLE_METERING_CLIENT_H_

/**
 * @defgroup simple-metering-client Simple Metering Client
 * @ingroup component cluster
 * @brief API and Callbacks for the Simple Metering Cluster Client Component
 *
 */

/**
 * @addtogroup simple-metering-client
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup simple_metering_client_cb Simple Metering Client
 * @ingroup af_callback
 * @brief Callbacks for Simple Metering Client Component
 *
 */

/**
 * @addtogroup simple_metering_client_cb
 * @{
 */

/** @brief Request Mirror
 *
 * This function is called by the Simple Metering client plugin whenever a
 * Request Mirror command is received. The application should return the
 * endpoint to which the mirror has been assigned. If no mirror could be
 * assigned, the application should return 0xFFFF.
 *
 * @param requestingDeviceIeeeAddress   Ver.: always
 */
uint16_t emberAfPluginSimpleMeteringClientRequestMirrorCallback(EmberEUI64 requestingDeviceIeeeAddress);

/** @brief Remove Mirror
 *
 * This function is called by the Simple Metering client plugin whenever a
 * Remove Mirror command is received. The application should return the
 * endpoint on which the mirror has been removed. If the mirror could not be
 * removed, the application should return 0xFFFF.
 *
 * @param requestingDeviceIeeeAddress   Ver.: always
 */
uint16_t emberAfPluginSimpleMeteringClientRemoveMirrorCallback(EmberEUI64 requestingDeviceIeeeAddress);

/** @} */ // end of simple_metering_client_cb
/** @} */ // end of Callbacks
/** @} */ // end of simple-metering-client

#endif /* SIMPLE_METERING_CLIENT_H_ */
