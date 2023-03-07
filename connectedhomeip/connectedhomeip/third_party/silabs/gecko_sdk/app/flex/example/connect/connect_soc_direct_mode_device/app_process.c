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
#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "hal/hal.h"
#include "em_chip.h"
#include "app_log.h"
#include "app_framework_common.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define DATA_ENDPOINT           1
#define TX_TEST_ENDPOINT        2
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static EmberStatus sendTxTestPacket(void);
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
bool tx_test_print_en = false;

extern EmberMessageOptions tx_options;
extern uint8_t tx_test_packet_size;
extern uint8_t tx_test_inflight_packets;
extern uint8_t tx_test_current_inflights_packets;
extern EmberNodeId tx_test_destination;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application logic (Empty)
 *****************************************************************************/
void emberAfTickCallback(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your application logic code here!                                   //
  // This is called infinitel.                                               //
  /////////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * This callback is called if a message is received. If CLI is added it
 * prints the incoming message.
 *****************************************************************************/
void emberAfIncomingMessageCallback(EmberIncomingMessage *message)
{
  uint8_t i;
  if (message->endpoint == DATA_ENDPOINT) {
    app_log_info("RX: Data from 0x%04X:{", message->source);
    for ( i = 0; i < message->length; i++ ) {
      if ( i == 0) {
        app_log_info("%02X", message->payload[i]);
      } else {
        app_log_info(" %02X", message->payload[i]);
      }
    }
    app_log_info("}\n");
  } else if (message->endpoint == TX_TEST_ENDPOINT && tx_test_print_en) {
    app_log_info("RX: TX test packet from 0x%04X:{", message->source);
    for ( i = 0; i < message->length; i++ ) {
      if ( i == 0) {
        app_log_info(" %02X", message->payload[i]);
      } else {
        app_log_info(" %02X", message->payload[i]);
      }
    }
    app_log_info("}\n");
  }
}

/******************************************************************************
 * This callback is called if a message is transmitted. If CLI is added it
 * prints message success.
 *****************************************************************************/
void emberAfMessageSentCallback(EmberStatus status,
                                EmberOutgoingMessage *message)
{
  if (message->endpoint == DATA_ENDPOINT) {
    if ( status != EMBER_SUCCESS ) {
      app_log_info("TX: 0x%02X\n", status);
    }
  } else if (message->endpoint == TX_TEST_ENDPOINT) {
    //tx_test_current_inflights_packets--;
    if (tx_test_current_inflights_packets < tx_test_inflight_packets) {
      if (sendTxTestPacket() == EMBER_SUCCESS) {
        tx_test_current_inflights_packets++;
      } else {
        tx_test_current_inflights_packets--;
      }
    }
  }
}

/******************************************************************************
 * This callback is called if network status changes. If CLI is added it
 * prints the current status.
 *****************************************************************************/
void emberAfStackStatusCallback(EmberStatus status)
{
  switch (status) {
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

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * This function creates a packet with consecutive data and sends it
 *****************************************************************************/
static EmberStatus sendTxTestPacket(void)
{
  uint8_t tx_test_packet[127];

  for (uint8_t i = 0; i < tx_test_packet_size; i++) {
    tx_test_packet[i] = i;
  }

  return emberMessageSend(tx_test_destination,
                          TX_TEST_ENDPOINT,
                          0, // messageTag
                          tx_test_packet_size,
                          tx_test_packet,
                          tx_options);
}

void emberAfEnergyScanCompleteCallback(int8_t mean,
                                       int8_t min,
                                       int8_t max,
                                       uint16_t variance)
{
  app_log_info("Energy scan complete, mean=%d min=%d max=%d var=%d",
               mean, min, max, variance);
}
