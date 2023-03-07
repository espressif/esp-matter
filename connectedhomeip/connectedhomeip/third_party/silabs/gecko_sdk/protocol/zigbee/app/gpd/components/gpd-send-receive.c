/***************************************************************************//**
 * @file
 * @brief GPD commissioning functions.
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
#include "gpd-components-common.h"
#ifndef UC_BUILD
#include "gpd-callbacks.h"
#else
#include "gpd-cb.h"
#endif

static uint8_t txMpdu[EMBER_GPD_TX_MPDU_SIZE];
static uint8_t rxMpdu[EMBER_GPD_RX_MPDU_SIZE];

uint8_t * emberGpdGetRxMpdu(void)
{
  return rxMpdu;
}
// Starts to receive for receiveWindowInUs duration after waiting for
// startDelayInUs. During the startDelayInUs the GPD can sleep to an appropriate
// and possible sleep mode to conserve energy.
int8_t emberAfGpdScheduledReceive(uint32_t startDelayInUs,
                                  uint32_t receiveWindowInUs,
                                  uint8_t channel,
                                  bool sleepInDelay)
{
  // Start LE timer with startDelay
  // Put the micro to low power sleep with above wake up configured
  // Wake Up when start delay time expires
  // Keep Receiver On to receive the data for recieveWindow
  if (startDelayInUs) {
    emberGpdLoadLeTimer(startDelayInUs);
    // If Enters EM0 instead or awaken by other things, made to wait by following
    // code until the LE Timer expires to provide the exact rxOffset before receive.
    // the callback will be responsible to put the micro in the sleep
    while (emberGpdLeTimerRunning()) {
      if (sleepInDelay) {
        emberGpdAfPluginSleepCallback();
      }
    }
  }

  // Load the timer for the receive window
  emberGpdLoadLeTimer(receiveWindowInUs);
  // Start the receiver
  emberGpdRailStartRxWrapper(channel);

  //Code blocker for the entire time of receive window
  while (emberGpdLeTimerRunning()) ;
  return SUCCESS;
}

int8_t emberAfGpdfSend(uint8_t frameType,
                       EmberGpd_t * gpd,
                       uint8_t * payload,
                       uint8_t payloadLength,
                       uint8_t repeatNumber)
{
  // Check packet length
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  payloadLength = SL_MIN(payloadLength, EMBER_GPD_SRC_ID_MAX_PAYLOAD_SIZE);
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  payloadLength = SL_MIN(payloadLength, EMBER_GPD_IEEE_ID_MAX_PAYLOAD_SIZE);
#else
#error "Unsupported GPD Application Id"
#endif

  if (frameType == EMBER_GPD_NWK_FC_FRAME_TYPE_DATA) {
    // update Security frame counter before building packet
#if defined(EMBER_AF_PLUGIN_APPS_MAC_SEQ) && (EMBER_AF_PLUGIN_APPS_MAC_SEQ == EMBER_GPD_MAC_SEQ_INCREMENTAL)
    gpd->securityFrameCounter++;
    emberGpdSaveGpdFrameCounterToShadow(gpd->securityFrameCounter);
    // update NONCE TX & Rx
    emberGpdSetFcIntoTxNonce(gpd->securityFrameCounter);
    emberGpdSetFcIntoRxNonce(gpd->securityFrameCounter);

#elif defined(EMBER_AF_PLUGIN_APPS_MAC_SEQ) && (EMBER_AF_PLUGIN_APPS_MAC_SEQ == EMBER_GPD_MAC_SEQ_RANDOM)
    gpd->securityFrameCounter = (uint8_t)emberGpdAfPluginGetRandomCallback();
#else
#error "Undefined GPD MAC SEQ Mode"
#endif
  }
  uint8_t length = emberBuildOutGoingPdu(frameType,
                                         txMpdu,
                                         payload,
                                         payloadLength,
                                         gpd);
  emberGpdRailWriteTxFifoWrapper(txMpdu, length);
  // local variable
  uint8_t repeat = 0;
  do {
    emberGpdRailIdleWrapper();
    uint32_t preTxRailTime = RAIL_GetTime();
    //
    emberGpdRailStartTxWrapper(gpd->skipCca, gpd->channel);
    emberGpdRailIdleWrapper();
    //
    if (gpd->rxAfterTx) {
      uint32_t txRailDurationUs = RAIL_GetTime() - preTxRailTime;
      emberAfGpdScheduledReceive((((uint32_t)gpd->rxOffset * 1000) - txRailDurationUs),
                                 (uint32_t)(gpd->minRxWindow) * 1000,
                                 gpd->channel,
                                 true);
      emberGpdRailIdleWrapper();
    }
    repeat++;
  } while (repeat < repeatNumber);

  return SUCCESS;
}
#ifndef USER_HAS_GPD_INCOMING_COMMAND_HANDLER
void emberGpdIncomingCommandHandler(uint8_t *gpdCommandBuffer, uint8_t length)
{
  if (gpdCommandBuffer == NULL
      || length == 0) {
    return;
  }
  uint8_t finger = 0;
  uint8_t gpdCommand = gpdCommandBuffer[finger++];
  length--;
  if (emberGpdAfPluginIncomingCommandCallback(gpdCommand,
                                              length,
                                              &gpdCommandBuffer[finger])) {
    // Application handled this call hence no need to handle.
    return;
  }
  switch (gpdCommand) {
    case GP_CMD_CHANNEL_CONFIG:
    {
      uint8_t channel = ((gpdCommandBuffer[finger] & 0x0F) + 11);
      emberGpdAfPluginCommissioningChannelConfigCallback(channel);
      break;
    }
    case GP_CMD_COMMISSIONING_REPLY:
    {
      emberGpdAfPluginCommissioningReplyCallback(length,
                                                 &gpdCommandBuffer[finger]);
      break;
    }
    default:
      break;
  }
  return;
}
#endif
