/***************************************************************************//**
 * @file
 * @brief APIs and defines for the EZ-Mode plugin, which implements EZ-mode
 *        commissioning.
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
 * @defgroup ez-mode EZ Mode Commissioning
 * @ingroup component
 * @brief API and Callbacks for the EZ Mode Commissioning Component
 *
 * This component provides a function set for implementing EZ Mode commissioning.
 *
 */

/**
 * @addtogroup ez-mode
 * @{
 */

typedef enum {
  /* Allows the creation of bindings from
   * an attribute server to an attribute client. */
  EMBER_AF_EZMODE_COMMISSIONING_SERVER_TO_CLIENT = 0,

  /* Allows the creation of bindings from
   * an attribute client to an attribute server. */
  EMBER_AF_EZMODE_COMMISSIONING_CLIENT_TO_SERVER = 1,
} EmberAfEzModeCommissioningDirection;

/**
 * @name API
 * @{
 */

/**
 * @brief Start EZ Mode client commissioning.
 *
 * Kicks off the EZ Mode commissioning process by sending out
 * an identify query command to the given endpoint. Endpoints that
 * return an identify query response are interrogated for the given
 * cluster IDs in the given direction (client or server).
 *
 * @param endpoint The endpoint to send the identify query command from
 * @param direction The side of the cluster IDs given either client or server
 * @param clusterIds An array of clusters against which to match.
 *        *NOTE* The API only keeps the pointer to
 *        to the data structure. The data is expected to exist throughout the
 *        EZ-Mode-commissioning calls.
 * @param clusterIdsLength The number of cluster IDs passed for the match.
 */
EmberStatus emberAfEzmodeClientCommission(uint8_t endpoint,
                                          EmberAfEzModeCommissioningDirection direction,
                                          const uint16_t *clusterIds,
                                          uint8_t clusterIdsLength);

/**
 * @brief Begin EZ Mode server commissioning.
 *
 * Kicks of the server side of EZ Mode commissioning by putting the
 * device into identify mode.
 *
 * @param endpoint The endpoint on which to begin identifying.
 */
EmberStatus emberAfEzmodeServerCommission(uint8_t endpoint);

/**
 * @brief Begin EZ Mode server commissioning with a given timeout.
 *
 * Kicks of the server side of EZ Mode commissioning by putting the
 * device into identify mode for a given time.
 *
 * @param endpoint The endpoint on which to begin identifying
 * @param identifyTimeoutSeconds The number of seconds to identify for before
 *                               stopping identify mode.
 */
EmberStatus emberAfEzmodeServerCommissionWithTimeout(uint8_t endpoint,
                                                     uint16_t identifyTimeoutSeconds);

/** @} */ // end of name API
/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup ez_mode_cb EZ Mode
 * @ingroup af_callback
 * @brief Callbacks for EZ Mode Component
 *
 */

/**
 * @addtogroup ez_mode_cb
 * @{
 */

/** @brief Client complete.
 *
 * This function is called by the EZ Mode Commissioning plugin when client
 * commissioning completes.
 *
 * @param bindingIndex The binding index that was created or
 * ::EMBER_NULL_BINDING if an error occurred. Ver.: always
 */
void emberAfPluginEzmodeCommissioningClientCompleteCallback(uint8_t bindingIndex);
/** @} */ // end of ez_mode_cb
/** @} */ // end of name Callbacks
/** @} */ // end of ez-mode
