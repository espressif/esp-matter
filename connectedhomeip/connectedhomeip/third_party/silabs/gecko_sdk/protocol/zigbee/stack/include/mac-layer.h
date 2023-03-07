/***************************************************************************//**
 * @file
 * @brief EmberZNet API for accessing and setting MAC configuration.
 * See @ref mac_layer for documentation.
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

#ifndef SILABS_MAC_LAYER_H
#define SILABS_MAC_LAYER_H

/**
 * @addtogroup mac_layer
 *
 * This file defines routines for probing and configuring MAC-related items.
 *
 * See mac-layer.h for source code.
 * @{
 */

/**@brief Get max number of MAC retries
 *
 * Returns the maximum number of no-ack retries that will be attempted
 *
 * @return uint8_t Max MAC retries
 */
uint8_t emberGetMaxMacRetries(void);

/**@brief Disables CCA after a number of consecutive CSMA failures due to CCA.
 *
 * For each packet to be transmitted by the MAC,
 * disables CCA after a number of consecutive CSMA failures
 * due to CCA. The packet is then transmitted using the
 * standard number of mac retries, but with CCA off.
 * Note that each CSMA failure consists of 5 failed and
 * backed off CCA checks, which can take up to 35
 * milliseconds. The maximum allowed value for csmaAttempts
 * is 3. A value of 0 disables the feature.
 *
 * @param csmaAttempts The number of consecutive CSMA failures due to CCA
 *        prior to disabling CCA. The maximum allowed value
 *        is 3. A value of 0 disables the feature.
 *
 * @return EmberStatus EMBER_BAD_ARGUMENT, if csmaAttempts is out of range,
 *         EMBER_SUCCESS otherwise.
 */
EmberStatus emberForceTxAfterFailedCca(uint8_t csmaAttempts);

/** @} END addtogroup */

#endif // SILABS_MAC_LAYER_H
