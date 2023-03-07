/***************************************************************************//**
 * @file
 * @brief System on chip specific code for fragmentation.
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

//-----------------------------------------------------------------------------
// Functions

void emAfPluginFragmentationPlatformInitCallback(void)
{
  emberFragmentWindowSize = EMBER_AF_PLUGIN_FRAGMENTATION_RX_WINDOW_SIZE;
}

EmberStatus emAfPluginFragmentationSend(txFragmentedPacket* txPacket,
                                        uint8_t fragmentNumber,
                                        uint16_t fragmentLen,
                                        uint16_t offset)
{
  EmberStatus status;
  EmberMessageBuffer message;
  message = emberFillLinkedBuffers(txPacket->bufferPtr + offset,
                                   fragmentLen);
  if (message == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  }
  status = emberSendUnicast(txPacket->messageType,
                            txPacket->indexOrDestination,
                            &txPacket->apsFrame,
                            message);

  emberReleaseMessageBuffer(message);
  return status;
}

void emAfPluginFragmentationHandleSourceRoute(txFragmentedPacket* txPacket,
                                              uint16_t indexOrDestination)
{
  // Nothing to do on SOC.  This will be handled later by the NWK code
  // calling into the application to ask for a source route.
}

void emAfPluginFragmentationSendReply(EmberNodeId sender,
                                      EmberApsFrame* apsFrame,
                                      rxFragmentedPacket* rxPacket)
{
  emberSetReplyFragmentData(HIGH_LOW_TO_INT(rxPacket->fragmentMask,
                                            rxPacket->fragmentBase));
  emberSendReply(apsFrame->clusterId, EMBER_NULL_MESSAGE_BUFFER);
}
