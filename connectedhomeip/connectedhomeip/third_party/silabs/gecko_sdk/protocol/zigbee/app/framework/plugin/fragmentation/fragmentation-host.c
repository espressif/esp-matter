/***************************************************************************//**
 * @file
 * @brief Host specific code for fragmentation.
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
#include "app/framework/util/af-main.h"
#include "fragmentation.h"

//-----------------------------------------------------------------------------
// Globals

uint16_t emberMacIndirectTimeout = 0;
uint16_t emberApsAckTimeoutMs    = 0;
uint8_t  emberFragmentWindowSize = EMBER_AF_PLUGIN_FRAGMENTATION_RX_WINDOW_SIZE;

//-----------------------------------------------------------------------------
// Functions

void emberAfPluginFragmentationNcpInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  ezspGetConfigurationValue(EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT,
                            &emberMacIndirectTimeout);
  ezspGetConfigurationValue(EZSP_CONFIG_APS_ACK_TIMEOUT, &emberApsAckTimeoutMs);
  emberAfSetEzspConfigValue(EZSP_CONFIG_FRAGMENT_WINDOW_SIZE,
                            emberFragmentWindowSize,
                            "Fragmentation RX window size");
}

void emAfPluginFragmentationPlatformInitCallback(void)
{
}

EmberStatus emAfPluginFragmentationSend(txFragmentedPacket* txPacket,
                                        uint8_t fragmentNumber,
                                        uint16_t fragmentLen,
                                        uint16_t offset)
{
  EmberStatus status;

  status = ezspSendUnicast(txPacket->messageType,
                           txPacket->indexOrDestination,
                           &(txPacket->apsFrame),
                           fragmentNumber,
                           fragmentLen,
                           txPacket->bufferPtr + offset,
                           &(txPacket->apsFrame.sequence));
  return status;
}

void emAfPluginFragmentationHandleSourceRoute(txFragmentedPacket* txPacket,
                                              uint16_t indexOrDestination)
{
//now this is unnneccessary and similar to SoC
}

void emAfPluginFragmentationSendReply(EmberNodeId sender,
                                      EmberApsFrame* apsFrame,
                                      rxFragmentedPacket* rxPacket)
{
  apsFrame->groupId = HIGH_LOW_TO_INT(rxPacket->fragmentMask, rxPacket->fragmentBase);
  ezspSendReply(sender, apsFrame, 0, NULL);
}
