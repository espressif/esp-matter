/***************************************************************************//**
 * @file
 * @brief Routines for the ZLL Commissioning Server plugin, which implement the
 *        server side of the ZLL protocol.
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

#include "af.h"
#include "zll-commissioning-server.h"

/** @brief Group Identifier Count
 *
 * This function is called by the ZLL Commissining Server plugin to determine the
 * number of group identifiers in use by a specific endpoint on the device. The
 * total number of group identifiers on the device, which are shared by all
 * endpoints, is defined by ::EMBER_ZLL_GROUP_ADDRESSES.
 *
 * @param endpoint The endpoint for which the group identifier count is
 * requested. Ver.: always
 */
WEAK(uint8_t emberAfPluginZllCommissioningServerGroupIdentifierCountCallback(uint8_t endpoint))
{
  return 0x00;
}

/** @brief Group Identifier
 *
 * This function is called by the ZLL Commissining Server plugin to obtain
 * information about the group identifiers in use by a specific endpoint on the
 * device. The application should populate the record with information about
 * the group identifier and return true. If no information is available for the
 * given endpoint and index, the application should return false.
 *
 * @param endpoint The endpoint for which the group identifier is requested.
 * Ver.: always
 * @param index The index of the group on the endpoint. Ver.: always
 * @param record The group information record. Ver.: always
 */
WEAK(bool emberAfPluginZllCommissioningServerGroupIdentifierCallback(uint8_t endpoint,
                                                                     uint8_t index,
                                                                     EmberAfPluginZllCommissioningGroupInformationRecord *record))
{
  return false;
}

/** @brief Endpoint Information Count
 *
 * This function is called by the ZLL Commissining Server plugin to determine the
 * number of remote endpoints controlled by a specific endpoint on the local
 * device.
 *
 * @param endpoint The local endpoint for which the remote endpoint information
 * count is requested. Ver.: always
 */
WEAK(uint8_t emberAfPluginZllCommissioningServerEndpointInformationCountCallback(uint8_t endpoint))
{
  return 0x00;
}

/** @brief Endpoint Information
 *
 * This function is called by the ZLL Commissining Server plugin to obtain
 * information about the remote endpoints controlled by a specific endpoint on
 * the local device. The application should populate the record with
 * information about the remote endpoint and return true. If no information is
 * available for the given endpoint and index, the application should return
 * false.
 *
 * @param endpoint The local endpoint for which the remote endpoint information
 * is requested. Ver.: always
 * @param index The index of the remote endpoint information on the local
 * endpoint. Ver.: always
 * @param record The endpoint information record. Ver.: always
 */
WEAK(bool emberAfPluginZllCommissioningServerEndpointInformationCallback(uint8_t endpoint,
                                                                         uint8_t index,
                                                                         EmberAfPluginZllCommissioningEndpointInformationRecord *record))
{
  return false;
}

/** @brief Identify
 *
 * This function is called by the ZLL Commissining Server plugin to notify the
 * application that it should take an action to identify itself. This typically
 * occurs when an Identify Request is received via inter-PAN messaging.
 *
 * @param durationS If the duration is zero, the device should exit identify
 * mode. If the duration is 0xFFFF, the device should remain in identify mode
 * for the default time. Otherwise, the duration specifies the length of time
 * in seconds that the device should remain in identify mode. Ver.: always
 */
WEAK(void emberAfPluginZllCommissioningServerIdentifyCallback(uint16_t durationS))
{
}
