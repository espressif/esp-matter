/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Messaging Client plugin.
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

// ----------------------------------------------------------------------------
// Message Control byte
// ----------------------------------------------------------------------------

#define ZCL_MESSAGING_CLUSTER_TRANSMISSION_MASK (BIT(1) | BIT(0))
#define ZCL_MESSAGING_CLUSTER_IMPORTANCE_MASK   (BIT(3) | BIT(2))
#define ZCL_MESSAGING_CLUSTER_CONFIRMATION_MASK BIT(7)

#define ZCL_MESSAGING_CLUSTER_START_TIME_NOW         0x00000000UL
#define ZCL_MESSAGING_CLUSTER_END_TIME_NEVER         0xFFFFFFFFUL
#define ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED 0xFFFF

/**
 * @defgroup messaging-client Messaging Client
 * @ingroup component cluster
 * @brief API and Callbacks for the Messaging Cluster Client Component
 *
 * Silicon Labs implementation of the Messaging client cluster.
 * This component requires extending to display the message on
 * the hardware display.
 *
 */

/**
 * @addtogroup messaging-client
 * @{
 */

/**
 * @name API
 * @{
 */

/**
 * @brief Clear the message.
 *
 * This function is used to manually inactivate or clear the message.
 *
 * @param endpoint The relevant endpoint.
 *
 **/
void emAfPluginMessagingClientClearMessage(uint8_t endpoint);

/**
 * @brief Print information about the message.
 *
 * @param endpoint The relevant endpoint.
 *
 **/
void emAfPluginMessagingClientPrintInfo(uint8_t endpoint);

/**
 * @brief Confirm a message.
 *
 * This function is used to confirm a message. The messaging client plugin
 * sends the confirmation command to the endpoint on the node that sent the
 * message.
 *
 * @return ::EMBER_ZCL_STATUS_SUCCESS if the confirmation was sent,
 * ::EMBER_ZCL_STATUS_FAILURE if an error occurred, or
 * ::EMBER_ZCL_STATUS_NOT_FOUND if the message does not exist.
 */
EmberAfStatus emberAfPluginMessagingClientConfirmMessage(uint8_t endpoint);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup messaging_client_cb Messaging Client
 * @ingroup af_callback
 * @brief Callbacks for Messaging Client Component
 *
 */

/**
 * @addtogroup messaging_client_cb
 * @{
 */

/** @brief Pre Display Message
 *
 * This function is called by the Messaging client plugin when a DisplayMessage
 * command is received. If callback returns true, the plugin assumes the
 * message have been handled and will not do anything with the message.
 * Otherwise, the plugin will go through with its own implementation.
 *
 * @param messageId   Ver.: always
 * @param messageControl   Ver.: always
 * @param startTime   Ver.: always
 * @param durationInMinutes   Ver.: always
 * @param message   Ver.: always
 * @param optionalExtendedMessageControl   Ver.: always
 */
bool emberAfPluginMessagingClientPreDisplayMessageCallback(uint32_t messageId,
                                                           uint8_t messageControl,
                                                           uint32_t startTime,
                                                           uint16_t durationInMinutes,
                                                           uint8_t *message,
                                                           uint8_t optionalExtendedMessageControl);
/** @brief Display a message.
 *
 * This function is called by the messaging client plugin whenever the
 * application should display a message.
 *
 * @param message The message that should be displayed. Ver.: always
 */
void emberAfPluginMessagingClientDisplayMessageCallback(EmberAfPluginMessagingClientMessage *message);
/** @brief Cancel a message.
 *
 * This function is called by the messaging client plugin whenever the
 * application should stop displaying a message.
 *
 * @param message The message that should no longer be displayed. Ver.: always
 */
void emberAfPluginMessagingClientCancelMessageCallback(EmberAfPluginMessagingClientMessage *message);
/** @} */ // end of messaging_client_cb
/** @} */ // end of Callbacks
/** @} */ // end of messaging client
