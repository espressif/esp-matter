/***************************************************************************//**
 * @file
 * @brief Host specific routines for performing service discovery.
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
#include "app/util/zigbee-framework/zigbee-device-host.h"

//------------------------------------------------------------------------------

EmberStatus emAfSendMatchDescriptor(EmberNodeId target,
                                    EmberAfProfileId profileId,
                                    EmberAfClusterId clusterId,
                                    bool serverCluster)
{
  uint8_t inCount = 0;
  uint8_t outCount = 0;
  EmberAfClusterId inCluster, outCluster;

  if (serverCluster) {
    inCount = 1;
    inCluster = clusterId;
    outCluster = 0x0000;
  } else {
    outCount = 1;
    outCluster = clusterId;
    inCluster = 0x0000;
  }

  return ezspMatchDescriptorsRequest(target,
                                     profileId,
                                     inCount,
                                     outCount,
                                     &inCluster,
                                     &outCluster,
                                     EMBER_AF_DEFAULT_APS_OPTIONS);
}
