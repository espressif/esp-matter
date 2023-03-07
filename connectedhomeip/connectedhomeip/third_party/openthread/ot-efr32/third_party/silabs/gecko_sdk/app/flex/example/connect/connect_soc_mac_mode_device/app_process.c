/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "em_chip.h"
#include "app_log.h"
#include "app_common.h"
#include "app_framework_callback.h"
#include "app_init.h"
#include "app_framework_common.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// TX options set up for the network
EmberMessageOptions tx_options = EMBER_OPTIONS_ACK_REQUESTED;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void emberAfTickCallback(void)
{
  ///////////////////////////////////////////////////////////////////////////
  // Put your application code here!                                       //
  // This is called infinitely.                                            //
  // Do not call blocking functions from here!                             //
  ///////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * This function is called if a child is joining the network
 *****************************************************************************/
void emberAfChildJoinCallback(EmberNodeType nodeType,
                              EmberNodeId nodeId)
{
  app_log_info("Node with short address 0x%04X joined as %s\n", nodeId,
               (nodeType == EMBER_STAR_RANGE_EXTENDER)
               ? "range extender"
               : ((nodeType == EMBER_STAR_END_DEVICE)
                  ? "end device"
                  : ((nodeType == EMBER_STAR_SLEEPY_END_DEVICE)
                     ? "sleepy end device"
                     : ((nodeType == EMBER_MAC_MODE_DEVICE)
                        ? "mac mode device"
                        : "sleepy mac mode device"))));
}

/******************************************************************************
 * This function is called if a message is received (MAC mode)
 *****************************************************************************/
void emberAfIncomingMacMessageCallback(EmberIncomingMacMessage *message)
{
  uint8_t i;
  if (message->options & EMBER_OPTIONS_SECURITY_ENABLED) {
    app_log_info("secured, ");
  } else {
    app_log_info("unsecured, ");
  }
  if (message->options & EMBER_OPTIONS_ACK_REQUESTED) {
    app_log_info("acked\n");
  } else {
    app_log_info("not acked\n");
  }

  if (message->macFrame.srcAddress.mode == EMBER_MAC_ADDRESS_MODE_SHORT) {
    app_log_info("MAC RX: Data from 0x%04X:{", message->macFrame.srcAddress.addr.shortAddress);
  } else if (message->macFrame.srcAddress.mode == EMBER_MAC_ADDRESS_MODE_NONE) {
    app_log_info("MAC RX: Data from unspecified address: {");
  } else {
    // print long address
    app_log_info("MAC RX: Data from ");
    for ( i = 0; i < EUI64_SIZE; i++ ) {
      app_log_info("%2X", message->macFrame.srcAddress.addr.longAddress[i]);
    }
    app_log_info(":{");
  }
  for ( i = 0; i < message->length; i++ ) {
    app_log_info(" %2X", message->payload[i]);
  }
  app_log_info("}\n");
}

/******************************************************************************
 * This function is called if a message is sent (MAC mode)
 *****************************************************************************/
void emberAfMacMessageSentCallback(EmberStatus status,
                                   EmberOutgoingMacMessage *message)
{
  (void) message;
  if ( status != EMBER_SUCCESS ) {
    app_log_info("MAC TX: 0x%02X\n", status);
  }
}

/******************************************************************************
 * This function is called if network status changes. If CLI is added it
 * prints the current status
 *****************************************************************************/
void emberAfStackStatusCallback(EmberStatus status)
{
  switch ( status ) {
    case EMBER_NETWORK_UP:
      app_log_info("Network up\n");
      break;
    case EMBER_NETWORK_DOWN:
      app_log_info("Network down\n");
      break;
    default:
      app_log_info("Stack status: 0x%02X\n", status);
      break;
  }
}

/******************************************************************************
 * This function is called if beacon message is received
 *****************************************************************************/
void emberAfIncomingBeaconCallback(EmberPanId panId,
                                   EmberMacAddress *source,
                                   int8_t rssi,
                                   bool permitJoining,
                                   uint8_t beaconFieldsLength,
                                   uint8_t *beaconFields,
                                   uint8_t beaconPayloadLength,
                                   uint8_t *beaconPayload)
{
  // Eliminate warnings
  (void) rssi;
  (void) permitJoining;
  (void) beaconFieldsLength;
  (void) beaconFields;

  app_log_info("BEACON: panId 0x%04X source ", panId);
  if (source->mode == EMBER_MAC_ADDRESS_MODE_SHORT) {
    app_log_info("0x%04X", source->addr.shortAddress);
  } else if (source->mode == EMBER_MAC_ADDRESS_MODE_LONG) {
    app_log_info("0x%llX\n", SYSTEM_GetUnique());
  } else {
    app_log_info("none");
  }

  app_log_info(" payload {");
  for (uint8_t i = 0; i < beaconPayloadLength; i++) {
    app_log_info("%02X", beaconPayload[i]);
  }
  app_log_info("}\n");
}

/******************************************************************************
 * This function is called if energy scan is completed
 *****************************************************************************/
void emberAfEnergyScanCompleteCallback(int8_t mean,
                                       int8_t min,
                                       int8_t max,
                                       uint16_t variance)
{
  app_log_info("Energy scan complete, mean=%d min=%d max=%d var=%d\n",
               mean, min, max, variance);
}

/******************************************************************************
 * This function is called if scan is completed
 *****************************************************************************/
void emberAfActiveScanCompleteCallback(void)
{
  app_log_info("Active scan complete\n");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
