/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Prepayment Client plugin.
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

#ifndef SILABS_PREPAYMENT_CLIENT_H
#define SILABS_PREPAYMENT_CLIENT_H

/**
 * @defgroup prepayment-client Prepayment Client
 * @ingroup component cluster
 * @brief API and Callbacks for the Prepayment Cluster Client Component
 *
 * This component implements the client-side functionality of the Smart Energy
 * 1.2 Prepayment cluster.
 *
 */

/**
 * @addtogroup prepayment-client
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief change prepayment client payment mode
 *
 * @param nodeId Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 * @param providerId Ver.: always
 * @param issuerEventId Ver.: always
 * @param implementationDateTime Ver.: always
 * @param proposedPaymentControlConfiguration Ver.: always
 * @param cutOffValue Ver.: always
 *
 */
void emberAfPluginPrepaymentClientChangePaymentMode(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t implementationDateTime, uint16_t proposedPaymentControlConfiguration, uint32_t cutOffValue);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup prepayment_client_cb Prepayment Client
 * @ingroup af_callback
 * @brief Callbacks for Prepayment Client Component
 *
 */

/**
 * @addtogroup prepayment_client_cb
 * @{
 */

/** @brief Payment mode change callback
 *
 * @param friendlyCredit Ver.: always
 * @param friendlyCreditCalenderId Ver.: always
 * @param emergencyCreditLimit Ver.: always
 * @param emergencyCreditThreshold Ver.: always
 *
 * @return bool
 *
 */
bool emberAfPluginPrepaymentClusterChangePaymentModeResponseCallback(uint8_t friendlyCredit, uint32_t friendlyCreditCalendarId, uint32_t emergencyCreditLimit, uint32_t emergencyCreditThreshold);

/** @brief Called when the prepayment cluster client attribute has changed.
 *
 * Client attribute has changed.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfPrepaymentClusterClientAttributeChangedCallback(uint8_t endpoint,
                                                            EmberAfAttributeId attributeId);
/** @brief Called when the client receives the default response from
 * the server.
 *
 * This function is called when the client receives the default response from
 * the server.
 *
 * @param endpoint Destination endpoint  Ver.: always
 * @param commandId Command id  Ver.: always
 * @param status Status in default response  Ver.: always
 */
void emberAfPrepaymentClusterClientDefaultResponseCallback(uint8_t endpoint,
                                                           uint8_t commandId,
                                                           EmberAfStatus status);
/** @brief Initialize prepayment cluster client.
 *
 * Client initialization.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPrepaymentClusterClientInitCallback(uint8_t endpoint);
/** @brief Called when the prepayment cluster client
 * manufacturer -specific attribute has changed.
 *
 * Called when the prepayment cluster client
 * manufacturer -specific attribute has changed.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 * @param manufacturerCode Manufacturer Code of the attribute that changed
 * Ver.: always
 */
void emberAfPrepaymentClusterClientManufacturerSpecificAttributeChangedCallback(uint8_t endpoint,
                                                                                EmberAfAttributeId attributeId,
                                                                                uint16_t manufacturerCode);
/** @brief Called when the prepayment cluster client message was sent.
 *
 * Client message  was sent.
 *
 * @param type The type of message sent  Ver.: always
 * @param indexOrDestination The destination or address to which the message was
 * sent  Ver.: always
 * @param apsFrame The APS frame for the message  Ver.: always
 * @param msgLen The length of the message  Ver.: always
 * @param message The message that was sent  Ver.: always
 * @param status The status of the sent message  Ver.: always
 */
void emberAfPrepaymentClusterClientMessageSentCallback(EmberOutgoingMessageType type,
                                                       uint16_t indexOrDestination,
                                                       EmberApsFrame *apsFrame,
                                                       uint16_t msgLen,
                                                       uint8_t *message,
                                                       EmberStatus status);
/** @brief Called when the prepayment cluster client pre attribute
 * has changed.
 *
 * Client pre attribute has changed.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute to be changed  Ver.: always
 * @param attributeType Attribute type  Ver.: always
 * @param size Attribute size  Ver.: always
 * @param value Attribute value  Ver.: always
 */
EmberAfStatus emberAfPrepaymentClusterClientPreAttributeChangedCallback(uint8_t endpoint,
                                                                        EmberAfAttributeId attributeId,
                                                                        EmberAfAttributeType attributeType,
                                                                        uint8_t size,
                                                                        uint8_t *value);
/** @brief Called upon the prepayment cluster client tick.
 *
 * Client tick.
 *
 * @param endpoint Endpoint that is being served  Ver.: always
 */
void emberAfPrepaymentClusterClientTickCallback(uint8_t endpoint);
/** @} */ // end of prepayment_client_cb
/** @} */ // end of Callbacks
/** @} */ // end of prepayment-client

#endif  // #ifndef _PREPAYMENT_CLIENT_H_
