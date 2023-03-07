/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"
#include "green-power-server.h"

/** @brief Green power commissioning notification callback
 *
 * This function is called by the green power server plugin to notify the
 * application about gp commissioning notification received by the sink.
 * If the this function returns false indicating application does not handle
 * the notification, the plugin will handle. If this returns true, the plugin
 * will skip processing of the commissioning notification.
 *
 * @param commandId                    Ver.: always
 * @param commNotificationOptions      Ver.: always
 * @param gpdAddr                      Ver.: always
 * @param gpdSecurityFrameCounte       Ver.: always
 * @param gpdCommandId                 Ver.: always
 * @param gpdCommandPayload            Ver.: always
 * @param gppShortAddress              Ver.: always
 * @param rssi                         Ver.: always
 * @param linkQuality                  Ver.: always
 * @param gppDistance                  Ver.: always
 * @param commissioningNotificationMic Ver.: always
 *
 * @returns true if application handles it and plugin will not process it anymore.
 * else return false to process the notification by the plugin  Ver.: always
 */
WEAK(bool emberAfPluginGreenPowerServerGpdCommissioningNotificationCallback(uint8_t commandId,
                                                                            uint16_t commNotificationOptions,
                                                                            EmberGpAddress *gpdAddr,
                                                                            uint32_t gpdSecurityFrameCounter,
                                                                            uint8_t gpdCommandId,
                                                                            uint8_t* gpdCommandPayload,
                                                                            uint16_t gppShortAddress,
                                                                            int8_t rssi,
                                                                            uint8_t linkQuality,
                                                                            uint8_t gppDistance,
                                                                            uint32_t commissioningNotificationMic))
{
  return false;
}

/** @brief Green power server commissioning callback
 *
 * This function is called by the green power server plugin to notify the
 * application of a Green Power Device that has requested commissioning with
 * this sink. Returns false if callback is not handled, true if callback is handled.
 * When the callback is handled, it must set the matchFound argument appropriately
 * to indicate if the matching functionality is found on the sink or not.
 *
 * @param appInfo   Application information of the commissioning GPD. Ver.: always
 * @param matchFound Output flag to notify matching functionality. Ver.: always
 *
 * @returns true if application handled it Ver.: always
 */
WEAK(bool emberAfPluginGreenPowerServerGpdCommissioningCallback(EmberGpApplicationInfo * appInfo,
                                                                bool* matchFound))
{
  bool handled = false;
  // When this is implemented by the user :
  // iterate through the sink commissioning endpoints to process the matching of the
  // cluster functionality based on the gpd application information and set the supplied
  // matchFound flag.
  *matchFound = false;
  return handled;
}

/** @brief Green power server security failure callback
 *
 * This function is called by the green power server plugin to notify the
 * application of a Green Power Security Processing failed for an incoming notification.
 *
 * @param gpdAddr   Ver.: always
 */
WEAK(void emberAfPluginGreenPowerServerGpdSecurityFailureCallback(EmberGpAddress *gpdAddr))
{
}

/** @brief Green power server notification forward callback
 *
 * This function is called by the green power server plugin to notify the
 * application of a Green Power Gp Notification of an incoming gpd command.
 * Return true to handle in application.
 *
 * @param options from the incoming Gp Notification Command Ver.: always
 * @param addr GPD address        Ver.: always
 * @param gpdSecurityFrameCounter Ver.: always
 * @param gpdCommandId            Ver.: always
 * @param gpdCommandPayload first byte is length of the payload Ver.: always
 * @param gppShortAddress         Ver.: always
 * @param gppDistance             Ver.: always
 *
 * @returns true if application handled it and plugin will not process it anymore.
 * else return false to process the notification by the plugin  Ver.: always
 */
WEAK(bool emberAfGreenPowerClusterGpNotificationForwardCallback(uint16_t options,
                                                                EmberGpAddress * addr,
                                                                uint32_t gpdSecurityFrameCounter,
                                                                uint8_t gpdCommandId,
                                                                uint8_t * gpdCommandPayload,
                                                                uint16_t gppShortAddress,
                                                                uint8_t  gppDistance))
{
  return false;
}

/** @brief Green power server pairing complete callback
 *
 * This function is called by the Green Power Server upon the completion of the pairing
 * and to indicate the closure of the pairing session.
 *
 * @param numberOfEndpoints number of sink endpoints participated in the pairing Ver.: always
 * @param endpoints list of sink endpoints Ver.: always
 */
WEAK(void emberAfGreenPowerServerPairingCompleteCallback(uint8_t numberOfEndpoints,
                                                         uint8_t * endpoints))
{
}

/** @brief Green power server commissioning timeout callback
 *
 * This function is called by the Green Power Server upon expiry of any of the
 * commissioning timer , those are server commissioning window expiry, generic
 * switch commissioning or multi-sensor commissioning timer expiry.
 *
 * @param commissioningTimeoutType one of the types
 *                     COMMISSIONING_TIMEOUT_TYPE_COMMISSIONING_WINDOW_TIMEOUT,
 *                     COMMISSIONING_TIMEOUT_TYPE_GENERIC_SWITCH
 *                     or COMMISSIONING_TIMEOUT_TYPE_MULTI_SENSOR Ver.: always
 * @param numberOfEndpoints Number of sink endpoints participated in the commissioning Ver.: always
 * @param endpoints list of sink endpoints Ver.: always
 */
WEAK(void emberAfGreenPowerServerCommissioningTimeoutCallback(uint8_t commissioningTimeoutType,
                                                              uint8_t numberOfEndpoints,
                                                              uint8_t * endpoints))
{
}

/** @brief Green power server update involved TC callback
 *
 * This function is called by the Green Power Server to proceed with updating the InvolveTC bit
 * of the security level attribute.
 *
 * @param status   Ver.: always
 *
 * @returns true if application handled it and plugin will not process it  Ver.: always
 */
WEAK(bool emberAfGreenPowerServerUpdateInvolveTCCallback(EmberStatus status))
{
  bool handled = false;
  return handled;
}

/** @brief Green power cluster GP Pairing Search callback
 *
 * This function is called to allow application to respond to the GP pairing search
 * request.
 *
 * @param options   Ver.: since gp-1.0-09-5499-24
 * @param gpdSrcId   Ver.: since gp-1.0-09-5499-24
 * @param gpdIeee   Ver.: since gp-1.0-09-5499-24
 * @param endpoint   Ver.: always
 *
 * @returns Return true if the response to the command is handled.
 */
WEAK(bool emberAfGreenPowerClusterGpPairingSearchCallback(uint16_t options,
                                                          uint32_t gpdSrcId,
                                                          uint8_t* gpdIeee,
                                                          uint8_t endpoint))
{
  // Returns true if handled else return false for a default response
  // to be generated by application framework cluster command handler
  return false;
}

/** @brief Sink table access notification callback
 *
 * This function is called by the green power server plugin to notify the
 * application about Green Power Device addition or removal by the green power
 * server to the Sink Table. If returned false, the sink table remains un-accessed.
 *
 * @param data void pointer to the data Ver.: always
 * @param accessType accessType         Ver.: always
 *
 * @returns true if the access is granted.
 */
WEAK(bool  emberAfPluginGreenPowerServerSinkTableAccessNotificationCallback(void* data,
                                                                            EmberAfGpServerSinkTableAccessType accessType))
{
  // The data is a void pointer must be typecast to required type based on the access type for correct access.
  // Example :
  // if accessType is GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_REMOVE_GPD
  //                   or GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_ADD_GPD
  // The data must be typecast to (EmberGpAddress *) e.g. and access
  //      EmberGpAddress * addr = (EmberGpAddress *) data;
  return true;
}

/** @brief Sink commissioning enter call status notification callback
 *
 * This function is called by the green power server plugin from the sink
 * commissioning enter command to notify the application of the status of the
 * proxy commissioning enter message submission to network layer.
 *
 * @param commissioningState Sink commissioning state Ver.: always
 * @param apsFrame aps frame header        Ver.: always
 * @param messageType unicast or broadcast Ver.: always
 * @param destination destination node     Ver.: always
 * @param status status of the network submission Ver.: always
 *
 */
WEAK(void emberAfGreenPowerClusterCommissioningMessageStatusNotificationCallback(EmberAfGreenPowerServerCommissioningState *commissioningState,
                                                                                 EmberApsFrame *apsFrame,
                                                                                 EmberOutgoingMessageType messageType,
                                                                                 uint16_t destination,
                                                                                 EmberStatus status))
{
}
