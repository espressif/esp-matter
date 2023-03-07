/***************************************************************************//**
 * @file
 * @brief Concentrator configuration and source route table management
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

#ifndef SILABS_SOURCE_ROUTE_H
#define SILABS_SOURCE_ROUTE_H

/**
 * @addtogroup source_route
 *
 * This file provides routines pertaining to source routing. Source routing is
 * only available for routers and coordinators that have the source routing
 * library included.
 *
 * See source-route.h for more information.
 * @{
 */

/** @brief initializing the node to act as a concentrator with:
 *
 * @params discovery mode on/off
 *  concentratorTypeParam
 *  min time between MTORRs
 *  max time between MTORRs
 *  threshodl to react to route error message
 *  threshodl to react to delivery failures
 *  max number of hops for MTORR
 */
EmberStatus emberSetConcentrator(bool on,
                                 uint16_t concentratorTypeParam,
                                 uint16_t minTime,
                                 uint16_t maxTime,
                                 uint8_t routeErrorThresholdParam,
                                 uint8_t deliveryFailureThresholdParam,
                                 uint8_t maxHopsParam);

/** @brief Returns the number of used entries in the source route table.
 */
uint8_t emberGetSourceRouteTableFilledSize(void);

/** @brief Returns the size of the source route table.
 */
uint8_t emberGetSourceRouteTableTotalSize(void);

/** @brief Returns the number of bytes needed in a packet for source routing.
 * Since each hop consumes 2 bytes in the packet, this routine calculates the
 * total number of bytes needed based on number of hops to reach the destination.
 * @param destination The destination's short address.
 * @return uint8_t The number of bytes needed for source routing in a packet.
 */
uint8_t emberGetSourceRouteOverhead(EmberNodeId destination);

/** @brief sets source route discovery mode(MTORR scheduling mode)
 *   to on/off/reschedule
 *
 *  @params:  mode:
 *  EMBER_SOURCE_ROUTE_DISCOVERY_OFF = 0x00,
 *  EMBER_SOURCE_ROUTE_DISCOVERY_ON = 0x01,
 *  EMBER_SOURCE_ROUTE_DISCOVERY_RESCHEDULE = 0x02,
 */
uint32_t emberSetSourceRouteDiscoveryMode(EmberSourceRouteDiscoveryMode mode);

//--------------------legacy APIs, with no direct ezsp tranlation, will be deprecated

/** @brief Legacy API  Starts periodic many-to-one route discovery.
 * Periodic discovery is started by default on bootup,
 * but this function may be used if discovery has been
 * stopped by a call to ::emberConcentratorStopDiscovery().
 */
void emberConcentratorStartDiscovery(void);

/** @brief Legacy API. Stops periodic many-to-one route discovery. */
void emberConcentratorStopDiscovery(void);

/** @brief Legacy API. Notes when a packet fails to deliver.
 */
void emberConcentratorNoteDeliveryFailure(EmberOutgoingMessageType type, EmberStatus status);

/** @brief Legacy API. Notes when a route error has occurred.
 */
void emberConcentratorNoteRouteError(EmberStatus status,
                                     EmberNodeId nodeId);

/** @brief To configure non trust center node to assume a concentrator type
 * of the trust center it join to, until it receive many-to-one route request
 * from the trust center. For the trust center node, concentrator type is
 * configured from the concentrator plugin.
 * The stack by default assumes trust center be a low RAM concentrator that make
 * other devices send route record to the trust center even without receiving
 * a many-to-one route request. The assumed concentrator type can be changed by
 * setting appropriate config value to emberAssumedTrustCenterConcentratorType.
 */
extern EmberAssumeTrustCenterConcentratorType emberAssumedTrustCenterConcentratorType;

/** @} END addtogroup */

#endif // SILABS_SOURCE_ROUTE_H
