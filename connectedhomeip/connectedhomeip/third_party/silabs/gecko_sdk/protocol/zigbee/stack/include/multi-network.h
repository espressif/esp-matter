/***************************************************************************//**
 * @file
 * @brief EmberZNet API for multi-network support.
 * See @ref multi_network for documentation.
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

#ifndef SILABS_ZIGBEE_MULTI_NETWORK_H
#define SILABS_ZIGBEE_MULTI_NETWORK_H

/**
 * @addtogroup multi_network
 *
 * EMBER_MULTI_NETWORK_STRIPPED is an optional #define
 * that is used on flash-space constrained chips
 * to conditionally compile out multi-network related source code.
 * @{
 */
#ifndef EMBER_MULTI_NETWORK_STRIPPED
/** @brief Return the current network index.
 */
uint8_t emberGetCurrentNetwork(void);

/** @brief Set the current network.
 *
 * @param index   The network index.
 *
 * @return ::EMBER_INDEX_OUT_OF_RANGE if the index does not correspond to a
 * valid network, and ::EMBER_SUCCESS otherwise.
 */
EmberStatus emberSetCurrentNetwork(uint8_t index);

/** @brief Can only be called inside an application callback.
 *
 * @return the index of the network the callback refers to. If this function
 * is called outside of a callback, it returns 0xFF.
 */
uint8_t emberGetCallbackNetwork(void);

/** @} END addtogroup */
#else // EMBER_MULTI_NETWORK_STRIPPED
#define emberGetCurrentNetwork()  0
// (void)index to avoid compiler warning.
#define emberSetCurrentNetwork(index) EMBER_INVALID_CALL
#define emberGetCallbackNetwork() 0
#endif  // EMBER_MULTI_NETWORK_STRIPPED

#endif // SILABS_ZIGBEE_MULTI_NETWORK_H
