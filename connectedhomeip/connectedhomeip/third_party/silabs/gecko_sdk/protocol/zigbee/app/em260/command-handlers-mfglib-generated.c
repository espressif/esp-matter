/*****************************************************************************/
/**
 * Copyright 2021 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//
// Description: Handlers for the EZSP frames that directly correspond to Ember
// API calls.

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "ezsp-enum.h"
#include "secure-ezsp-types.h"
#include "app/em260/command-context.h"
#include "stack/include/cbke-crypto-engine.h"
#include "stack/include/mfglib.h"
#include "stack/include/binding-table.h"
#include "stack/include/message.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "app/em260/command-handlers-cbke.h"
#include "app/em260/command-handlers-binding.h"
#include "app/em260/command-handlers-mfglib.h"
#include "app/em260/command-handlers-security.h"
#include "app/em260/command-handlers-zll.h"
#include "app/em260/command-handlers-zigbee-pro.h"
#include "child.h"
#include "message.h"
#include "zll-api.h"
#include "security.h"
#include "stack-info.h"
#include "network-formation.h"
#include "zigbee-device-stack.h"
#include "ember-duty-cycle.h"
#include "multi-phy.h"

bool emAfProcessEzspCommandMfglib(uint16_t commandId)
{
  switch (commandId) {
//------------------------------------------------------------------------------

    case EZSP_MFGLIB_START: {
      EmberStatus status;
      bool rxCallback;
      rxCallback = fetchInt8u();
      status = emberAfEzspMfglibStartCommandCallback(rxCallback);
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_END: {
      EmberStatus status;
      status = mfglibEnd();
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_START_TONE: {
      EmberStatus status;
      status = mfglibStartTone();
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_STOP_TONE: {
      EmberStatus status;
      status = mfglibStopTone();
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_START_STREAM: {
      EmberStatus status;
      status = mfglibStartStream();
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_STOP_STREAM: {
      EmberStatus status;
      status = mfglibStopStream();
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_SEND_PACKET: {
      EmberStatus status;
      uint8_t packetLength;
      uint8_t *packetContents;
      packetLength = fetchInt8u();
      packetContents = (uint8_t *)fetchInt8uPointer(packetLength);
      status = emberAfEzspMfglibSendPacketCommandCallback(packetLength, packetContents);
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_SET_CHANNEL: {
      EmberStatus status;
      uint8_t channel;
      channel = fetchInt8u();
      status = mfglibSetChannel(channel);
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_GET_CHANNEL: {
      uint8_t channel;
      channel = mfglibGetChannel();
      appendInt8u(channel);
      break;
    }

    case EZSP_MFGLIB_SET_POWER: {
      EmberStatus status;
      uint16_t txPowerMode;
      int8_t power;
      txPowerMode = fetchInt16u();
      power = fetchInt8u();
      status = mfglibSetPower(txPowerMode, power);
      appendInt8u(status);
      break;
    }

    case EZSP_MFGLIB_GET_POWER: {
      int8_t power;
      power = mfglibGetPower();
      appendInt8u(power);
      break;
    }

//------------------------------------------------------------------------------

    default: {
      return false;
    }
  }

  return true;
}
