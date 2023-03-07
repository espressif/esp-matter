/***************************************************************************//**
 * @file
 * @brief SOC specific routines for performing service discovery.
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

#include "app/framework/include/af.h"
#include "app/util/zigbee-framework/zigbee-device-library.h"

//------------------------------------------------------------------------------

EmberStatus emAfSendMatchDescriptor(EmberNodeId target,
                                    EmberAfProfileId profileId,
                                    EmberAfClusterId clusterId,
                                    bool serverCluster)
{
  EmberMessageBuffer clusterList = emAllocateBuffer(1 * 32);
  EmberMessageBuffer inClusters = EMBER_NULL_MESSAGE_BUFFER;
  EmberMessageBuffer outClusters = EMBER_NULL_MESSAGE_BUFFER;
  EmberStatus status = EMBER_NO_BUFFERS;

  if (clusterList != EMBER_NULL_MESSAGE_BUFFER) {
    emberSetMessageBufferLength(clusterList, 2);
    emberSetLinkedBuffersLowHighInt16u(clusterList, 0, clusterId);

    if (serverCluster) {
      inClusters = clusterList;
    } else {
      outClusters = clusterList;
    }

    status = emberMatchDescriptorsRequest(target,
                                          profileId,
                                          inClusters,
                                          outClusters,
                                          EMBER_AF_DEFAULT_APS_OPTIONS);
    emberReleaseMessageBuffer(clusterList);
  }
  return status;
}
