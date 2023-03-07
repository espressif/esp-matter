/***************************************************************************//**
 * @brief Zigbee NCP Application Framework Callbacks.
 * This file provides the WEAK definitions of the NCP framework callbacks.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "stack/include/ember-types.h"
#include "stack/include/gp-types.h"
#include "app/util/ezsp/ezsp-enum.h"

/** @brief Called when emberTrustCenterJoinHandler is called from the stack.
 *
 * The NCP will perform initial processing based on the current
 * ::EZSP_TRUST_CENTER_POLICY value and will pass this callback to the
 * NCP application. Implementations of this callback should return the
 * ::EmberJoinDecision value that will dictate the behavior of the trust center
 * in response to the new device joining or rejoining to the network.
 *
 * See documentation on the stack emberTrustCenterJoinHandler callback
 * for more information.
 *
 * @param newNodeId The node ID of the device wishing to join.
 * @param newNodeEui64 The EUI64 of the device wishing to join.
 * @param status The ::EmberUpdateDevice status indicating whether the
 * device is joining/rejoining or leaving, and whether the join/rejoin is
 * secure (using network encryption) or a trust center rejoin.
 * @param parentOfNewNode The node ID of the parent of device wishing to join.
 * @param decisionId The ::EzspDecisionId that the NCP is currently using
 * as set by the HOST application.
 * @param joinDecision The current ::EmberJoinDecision that the NCP has chosen
 * based on the ::EzspDecisionId for the ::EZSP_TRUST_CENTER_POLICY as set by the
 * HOST application. Implementations of this callback may want to return a
 * different value based on their desired security.
 *
 * @return An ::EmberJoinDecision value that will dictate the behavior of the
 * trust center in response to the new device joining or rejoining to
 * the network.
 */
WEAK(EmberJoinDecision emberAfPluginEzspSecurityTrustCenterJoinCallback(EmberNodeId newNodeId,
                                                                        const EmberEUI64 newNodeEui64,
                                                                        EmberDeviceUpdate status,
                                                                        EmberNodeId parentOfNewNode,
                                                                        EzspDecisionId decisionId,
                                                                        EmberJoinDecision joinDecision))
{
  return joinDecision;
}

/** @brief NCP Application hook callback for green power messages.
 *
 * This function is called by the application hook plugin to allow
 * user to take an action gpd message is received. The return code
 * true indicate the user handled the message and false allows the
 * stack to handle it.
 *
 * @param status                     Ver.: always
 * @param gpdLink                    Ver.: always
 * @param sequenceNumber             Ver.: always
 * @param *addr                      Ver.: always
 * @param gpdfSecurityLevel          Ver.: always
 * @param gpdfSecurityKeyType        Ver.: always
 * @param autoCommissioning          Ver.: always
 * @param rxAfterTx                  Ver.: always
 * @param gpdSecurityFrameCounter    Ver.: always
 * @param gpdCommandId               Ver.: always
 * @param mic                        Ver.: always
 * @param proxyTableIndex            Ver.: always
 * @param gpdCommandPayloadLength    Ver.: always
 * @param *gpdCommandPayload         Ver.: always
 */
WEAK(bool emberAfNcpGpepIncomingMessageHandler(GP_PARAMS))
{
  // return false so that stack will handle it, return true
  // if the message has been handled by user.
  return false;
}

/** @brief NCP Application hook callback for green power messages.
 *
 * This function is called by the application hook plugin to allow
 * user to take an action gpd message is sent. The return code true
 * indicate the user handled the message and false allows the stack
 * to handle it.
 *
 * @param status                     Ver.: always
 * @param gpepHandle                 Ver.: always
 */
WEAK(bool emberAfNcpDGpSentHandler(EmberStatus status, uint8_t gpepHandle))
{
  // return false so that stack will handle it, return true
  // if the message has been handled by user.
  return false;
}
