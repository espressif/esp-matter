/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Prepayment Server plugin.
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

#ifndef SILABS_PREPAYMENT_SERVER_H
#define SILABS_PREPAYMENT_SERVER_H

/**
 * @defgroup prepayment-server Prepayment Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Prepayment Cluster Server Component
 *
 * This component implements the server-side functionality of the
 * Smart Energy 1.2 Prepayment cluster.
 *
 */

/**
 * @addtogroup prepayment-server
 * @{
 */

// Snapshot payload type, D7.2.4.2.2 - Publish Prepay Snapshot Command
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum SnapshotPayloadType
#else
enum
#endif
{
  SNAPSHOT_PAYLOAD_TYPE_DEBT_OR_CREDIT_STATUS = 0x00,
};

#define SNAPSHOT_PAYLOAD_LEN 24

/**
 * @name API
 * @{
 */

/**
 * @brief Send a publish prepay snapshot command sent in response to
 * the get prepay snapshot command.
 * @param nodeId The short address of the destination device.
 * @param srcEndpoint The source endpoint used in the ZigBee transmission.
 * @param dstEndpoint The destination endpoint used in the ZigBee transmission.
 * @param snapshotId A unique identifier allocated by the device that created the snapshot.
 * @param snapshotTime The UTC time when the snapshot was taken.
 * @param totalSnapshotsFound The number of snapshots that matched the criteria in the
 * received Get Prepay Snapshot command.
 * @param commandIndex Indicates a fragment number if the entire payload won't fit into 1 message.
 * @param totalNumberOfCommands The total number of subcommands that will be sent.
 * @param snapshotCause A 32-bit bitmap that indicates the cause of the snapshot.
 * @param snapshotPayloadType An 8-bit enumeration that defines the format of the snapshot payload.
 * @param snapshotPayload Data that was created with the snapshot.
 *
 **/
void emberAfPluginPrepaymentServerPublishPrepaySnapshot(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                        uint32_t snapshotId, uint32_t snapshotTime,
                                                        uint8_t totalSnapshotsFound, uint8_t commandIndex,
                                                        uint8_t totalNumberOfCommands,
                                                        uint32_t snapshotCause,
                                                        uint8_t  snapshotPayloadType,
                                                        uint8_t *snapshotPayload);

/**
 * @brief Send a publish top up log command, sent when a top up is performed,
 * or in response to a get top up log command.
 * @param nodeId  The short address of the destination device.
 * @param srcEndpoint The source endpoint used in the ZigBee transmission.
 * @param dstEndpoint The destination endpoint used in the ZigBee transmission.
 * @param commandIndex Indicates a fragment number if the entire payload won't fit into 1 message.
 * @param totalNumberOfCommands The total number of subcommands that will be sent.
 * @param topUpPayload Information that is sent from each top up log entry.
 *
 **/
void emberAfPluginPrepaymentServerPublishTopUpLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                  uint8_t commandIndex, uint8_t totalNumberOfCommands,
                                                  TopUpPayload *topUpPayload);

/**
 * @brief send a publish debt log command.
 * @param nodeId The short address of the destination device.
 * @param srcEndpoint The source endpoint used in the ZigBee transmission.
 * @param dstEndpoint The destination endpoint used in the ZigBee transmission.
 * @param commandIndex Indicates a fragment number if the entire payload won't fit into 1 message.
 * @param totalNumberOfCommands The total number of subcommands that will be sent.
 * @param debtPayload Includes the contents of a debt record from the log.
 *
 **/
void emberAfPluginPrepaymentServerPublishDebtLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                 uint8_t commandIndex, uint8_t totalNumberOfCommands,
                                                 DebtPayload *debtPayload);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup prepayment_server_cb Prepayment Server
 * @ingroup af_callback
 * @brief Callbacks for Prepayment Server Component
 *
 */

/**
 * @addtogroup prepayment_server_cb
 * @{
 */

/** @brief Prepayment Cluster Server Attribute Changed
 *
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfPrepaymentClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                            EmberAfAttributeId attributeId);
/** @brief Prepayment cluster server default response.
 *
 * This function is called when the server receives the default response from
 * the client.
 *
 * @param endpoint Destination endpoint  Ver.: always
 * @param commandId Command id  Ver.: always
 * @param status Status in default response  Ver.: always
 */
void emberAfPrepaymentClusterServerDefaultResponseCallback(uint8_t endpoint,
                                                           uint8_t commandId,
                                                           EmberAfStatus status);
/** @brief Initialize the prepayment cluster server.
 *
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPrepaymentClusterServerInitCallback(uint8_t endpoint);
/** @brief Prepayment cluster server manufacturer-specific attribute changed.
 *
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 * @param manufacturerCode Manufacturer Code of the attribute that changed
 * Ver.: always
 */
void emberAfPrepaymentClusterServerManufacturerSpecificAttributeChangedCallback(uint8_t endpoint,
                                                                                EmberAfAttributeId attributeId,
                                                                                uint16_t manufacturerCode);
/** @brief Prepayment cluster server message sent.
 *
 *
 * @param type The type of message sent  Ver.: always
 * @param indexOrDestination The destination or address to which the message was
 * sent  Ver.: always
 * @param apsFrame The APS frame for the message  Ver.: always
 * @param msgLen The length of the message  Ver.: always
 * @param message The message that was sent  Ver.: always
 * @param status The status of the sent message  Ver.: always
 */
void emberAfPrepaymentClusterServerMessageSentCallback(EmberOutgoingMessageType type,
                                                       uint16_t indexOrDestination,
                                                       EmberApsFrame *apsFrame,
                                                       uint16_t msgLen,
                                                       uint8_t *message,
                                                       EmberStatus status);
/** @brief Prepayment cluster server pre attribute changed.
 *
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute to be changed  Ver.: always
 * @param attributeType Attribute type  Ver.: always
 * @param size Attribute size  Ver.: always
 * @param value Attribute value  Ver.: always
 */
EmberAfStatus emberAfPrepaymentClusterServerPreAttributeChangedCallback(uint8_t endpoint,
                                                                        EmberAfAttributeId attributeId,
                                                                        EmberAfAttributeType attributeType,
                                                                        uint8_t size,
                                                                        uint8_t *value);

/** @} */ // end of prepayment_server_cb
/** @} */ // end of Callbacks
/** @} */ // end of prepayment-server

#endif  // #ifndef _PREPAYMENT_SERVER_H_
