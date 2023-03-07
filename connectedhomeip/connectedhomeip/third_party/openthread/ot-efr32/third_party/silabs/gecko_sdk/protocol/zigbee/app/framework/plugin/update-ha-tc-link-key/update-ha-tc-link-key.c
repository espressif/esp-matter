/***************************************************************************//**
 * @file
 * @brief Routines for the Update HA TC Link Key plugin, which provides a way
 *        for trust centers to preemptively update the link key for HA devices.
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

#define R21_STACK_REVISION 21

void emberAfPluginDeviceDatabaseDiscoveryCompleteCallback(const EmberAfDeviceInfo *info)
{
  if (info->stackRevision < R21_STACK_REVISION) {
    EmberNodeId nodeId = emberLookupNodeIdByEui64(((EmberAfDeviceInfo *)info)->eui64);
    EmberStatus status = (nodeId == EMBER_NULL_NODE_ID
                          ? EMBER_ERR_FATAL
                          : emberSendTrustCenterLinkKey(nodeId, ((EmberAfDeviceInfo *)info)->eui64));
    emberAfCorePrint("Updating trust center link key of joining device ");
    emberAfPrintBigEndianEui64(info->eui64);
    emberAfCorePrintln(": 0x%X", status);
  }
}
