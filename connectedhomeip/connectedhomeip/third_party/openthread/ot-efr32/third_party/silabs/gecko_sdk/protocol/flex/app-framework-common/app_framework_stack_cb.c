/***************************************************************************//**
 * @brief Connect Application Framework common code.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "callback_dispatcher.h"
#include "app_framework_callback.h"
#include "sl_component_catalog.h"

// If the CMSIS ICP is present, most of the stack callbacks are implemented
// there.
#if !defined(SL_CATALOG_CONNECT_CMSIS_STACK_IPC_PRESENT)

void emberStackStatusHandler(EmberStatus status)
{
  emberAfStackStatus(status);
  emberAfStackStatusCallback(status);
}

void emberChildJoinHandler(EmberNodeType nodeType, EmberNodeId nodeId)
{
  emberAfChildJoin(nodeType, nodeId);
  emberAfChildJoinCallback(nodeType, nodeId);
}

void emberRadioNeedsCalibratingHandler(void)
{
  emberAfRadioNeedsCalibrating();
  emberAfRadioNeedsCalibratingCallback();
}

void emberMessageSentHandler(EmberStatus status,
                             EmberOutgoingMessage *message)
{
  emberAfMessageSent(status, message);
  emberAfMessageSentCallback(status, message);
}

void emberIncomingMessageHandler(EmberIncomingMessage *message)
{
  emberAfIncomingMessage(message);
  emberAfIncomingMessageCallback(message);
}

void emberMacMessageSentHandler(EmberStatus status,
                                EmberOutgoingMacMessage *message)
{
  emberAfMacMessageSent(status, message);
  emberAfMacMessageSentCallback(status, message);
}

void emberIncomingMacMessageHandler(EmberIncomingMacMessage *message)
{
  emberAfIncomingMacMessage(message);
  emberAfIncomingMacMessageCallback(message);
}

void emberIncomingBeaconHandler(EmberPanId panId,
                                EmberMacAddress *source,
                                int8_t rssi,
                                bool permitJoining,
                                uint8_t beaconFieldsLength,
                                uint8_t *beaconFields,
                                uint8_t beaconPayloadLength,
                                uint8_t *beaconPayload)
{
  emberAfIncomingBeacon(panId,
                        source,
                        rssi,
                        permitJoining,
                        beaconFieldsLength,
                        beaconFields,
                        beaconPayloadLength,
                        beaconPayload);
  emberAfIncomingBeaconCallback(panId,
                                source,
                                rssi,
                                permitJoining,
                                beaconFieldsLength,
                                beaconFields,
                                beaconPayloadLength,
                                beaconPayload);
}

void emberActiveScanCompleteHandler(void)
{
  emberAfActiveScanComplete();
  emberAfActiveScanCompleteCallback();
}

void emberEnergyScanCompleteHandler(int8_t mean,
                                    int8_t min,
                                    int8_t max,
                                    uint16_t variance)
{
  emberAfEnergyScanComplete(mean, min, max, variance);
  emberAfEnergyScanCompleteCallback(mean, min, max, variance);
}

void emberFrequencyHoppingStartClientCompleteHandler(EmberStatus status)
{
  emberAfFrequencyHoppingStartClientComplete(status);
  emberAfFrequencyHoppingStartClientCompleteCallback(status);
}

#endif // SL_CATALOG_CONNECT_CMSIS_STACK_IPC_PRESENT

void emberStackIsrHandler(void)
{
  emberAfStackIsr();
  // We do not expose this to the application.
}

void emberMarkApplicationBuffersHandler(void)
{
  emberAfMarkApplicationBuffers();
  emberAfMarkApplicationBuffersCallback();
}
