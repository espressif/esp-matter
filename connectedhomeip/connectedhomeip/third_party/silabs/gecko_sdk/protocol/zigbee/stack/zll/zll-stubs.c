/***************************************************************************//**
 * @file
 * @brief ZigBee Light Link function stubs.
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/zll-types.h"
#include "stack/include/library.h"
#include "event_queue/event-queue.h"
#include "mac-phy.h"

//------------------------------------------------------------------------------
// Globals

EmberEvent emZllTouchLinkEvent;

const EmberLibraryStatus emZllLibraryStatus = EMBER_LIBRARY_IS_STUB;

//------------------------------------------------------------------------------
// Forward Declarations

//------------------------------------------------------------------------------

EmberStatus emZllSendScanRequest(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

uint8_t emGetNextScanChannel(uint8_t currentChannel)
{
  return currentChannel + 1;
}

bool emGoToNextScanChannel(uint8_t currentChannel)
{
  (void)currentChannel;
  return true;
}

bool emZllScanComplete(void)
{
  return false;
}

void emberZllClearTokens(void)
{
}

bool emProcessZllInterpanStackMessage(const EmberMacFilterMatchStruct* macFilterMatchStruct)
{
  (void)macFilterMatchStruct;
  return false;
}

bool emZllStackStatusHandler(EmberStatus status)
{
  (void)status;
  return false;
}

bool emZllWaitingForInterpanResponse(void)
{
  return false;
}

void emZllDeviceAnnounceHandler(EmberNodeId source, EmberNodeId nodeId, uint8_t capabilities)
{
  (void)source;
  (void)nodeId;
  (void)capabilities;
}

bool emberIsZllNetwork(void)
{
  return false;
}

void emZllInit(void)
{
}

EmberStatus emberZllFormNetwork(EmberZllNetwork* networkInfo,
                                int8_t radioTxPower)
{
  (void)networkInfo;
  (void)radioTxPower;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberZllJoinTarget(const EmberZllNetwork* targetNetworkInfo)
{
  (void)targetNetworkInfo;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberZllSetSecurityStateWithoutKey(const EmberZllInitialSecurityState *securityState)
{
  (void)securityState;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberZllSetInitialSecurityState(const EmberKeyData *networkKey,
                                            const EmberZllInitialSecurityState *securityState)
{
  (void)networkKey;
  (void)securityState;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberZllStartScan(uint32_t channelMask,
                              int8_t radioPowerForScan,
                              EmberNodeType nodeType)
{
  (void)channelMask;
  (void)radioPowerForScan;
  (void)nodeType;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus emberZllSetRxOnWhenIdle(uint32_t durationMs)
{
  (void)durationMs;
  return EMBER_LIBRARY_NOT_PRESENT;
}

void emberZllSetRadioIdleMode(EmberRadioPowerMode mode)
{
  (void)mode;
}

uint8_t emberZllGetRadioIdleMode(void)
{
  // ZLL does stuff when the radio mode is power off. To stop that, the stub
  // reports the radio power on always
  return EMBER_RADIO_POWER_MODE_RX_ON;
}

void emberSetZllNodeType(EmberNodeType nodeType)
{
  (void)nodeType;
}

void emberSetZllAdditionalState(uint16_t mask)
{
  (void)mask;
}

void emberZllGetTokensStackZll(EmberTokTypeStackZllData *data,
                               EmberTokTypeStackZllSecurity *security)
{
  (void)data;
  (void)security;
}

void emberZllSetTokenStackZllData(EmberTokTypeStackZllData *token)
{
  (void)token;
}

void emberZllSetNonZllNetwork(void)
{
}

EmberZllPolicy emberZllGetPolicy(void)
{
  return EMBER_ZLL_POLICY_DISABLED;
}

EmberStatus emberZllSetPolicy(EmberZllPolicy policy)
{
  (void)policy;
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberNodeType emZllGetNodeType(void)
{
  return EMBER_UNKNOWN_DEVICE;
}

bool emberZllOperationInProgress(void)
{
  return false;
}

bool emberZllRxOnWhenIdleGetActive(void)
{
  return false;
}

void emZllSuspendRxOnWhenIdle(bool mode)
{
  (void)mode;
}

uint32_t emberGetZllPrimaryChannelMask(void)
{
  return 0;
}

uint32_t emberGetZllSecondaryChannelMask(void)
{
  return 0;
}

void emberSetZllPrimaryChannelMask(uint32_t mask)
{
  (void)mask;
}

void emberSetZllSecondaryChannelMask(uint32_t mask)
{
  (void)mask;
}

void emMarkZllBuffers(void)
{
}

Buffer sli_build_zll_scan_request_payload(void)
{
  return NULL_BUFFER;
}
void emberZllScanningComplete(void)
{
}
