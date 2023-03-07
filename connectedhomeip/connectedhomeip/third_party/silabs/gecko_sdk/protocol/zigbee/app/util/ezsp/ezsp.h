/***************************************************************************//**
 * @file
 * @brief Host EZSP layer. Provides functions that allow the Host
 * application to send every EZSP command to the NCP. The command and response
 * parameters are defined in the datasheet.
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

#ifndef SILABS_EZSP_H
#define SILABS_EZSP_H

// Reset the NCP and initialize the serial protocol (SPI or UART). After this
// function returns EZSP_SUCCESS, the NCP has finished rebooting and is ready
// to accept a command.
EzspStatus ezspInit(void);

// For ezsp-uart, must be called before setting sleep mode and enabling
// synchronous callbacks (read via ezspCallback()).
void ezspEnableNcpSleep(bool enable);

// Returns true if there are one or more callbacks queued up on the NCP
// awaiting collection.
bool ezspCallbackPending(void);

// The sleep mode to use in the frame control of every command sent. The Host
// application can set this to the desired NCP sleep mode. Subsequent commands
// will pass this value to the NCP.
extern uint8_t ezspSleepMode;

// Wakes the NCP up from deep sleep.
void ezspWakeUp(void);

// For ezsp-uart, indicate whether the Host can sleep without disrupting the
// uart protocol. In particular the Host may not sleep unless the NCP is
// already asleep.
bool ezspOkToSleep(void);

// The Host application must call this function periodically to allow the EZSP
// layer to handle asynchronous events.
void ezspTick(void);

// The EZSP layer calls this function after sending a command while waiting for
// the response. The Host application can use this function to perform any tasks
// that do not involve the NCP.
void ezspWaitingForResponse(void);

// Callback from the EZSP layer indicating that the transaction with the NCP
// could not be completed due to a serial protocol error or that the response
// received from the NCP reported an error. The status parameter provides more
// information about the error. This function must be provided by the Host
// application.
void ezspErrorHandler(EzspStatus status);

// Cleanly close down the serial protocol (SPI or UART). After this function has
// been called, ezspInit() must be called to resume communication with the
// NCP.
void ezspClose(void);

//----------------------------------------------------------------
// Functions with special handling

EzspStatus ezspGetVersionStruct(EmberVersion* returnData);

EzspStatus ezspSetEndpointFlags(uint8_t endpoint,
                                EzspEndpointFlags flags);
EzspStatus ezspGetEndpointFlags(uint8_t endpoint,
                                EzspEndpointFlags* returnFlags);

uint8_t ezspGetSourceRouteOverhead(EmberNodeId destination);

#define ezspEnableEndpoint(endpoint) \
  ezspSetEndpointFlags((endpoint), EZSP_ENDPOINT_ENABLED)

bool emberStackIsPerformingRejoin(void);
EmberStatus emberSetMfgSecurityConfig(uint32_t magicNumber,
                                      const EmberMfgSecurityStruct* settings);
EmberStatus emberGetMfgSecurityConfig(EmberMfgSecurityStruct* settings);
EmberStatus emberSetChildTimeoutOptionMask(uint16_t mask);
EmberStatus emberSetKeepAliveMode(uint8_t mode);
EmberKeepAliveMode emberGetKeepAliveMode(void);
uint8_t emberGetNetworkKeyTimeout(void);
EmberStatus emberStartWritingStackTokens(void);
EmberStatus emberStopWritingStackTokens(void);
EmberStatus emberSetExtendedSecurityBitmask(EmberExtendedSecurityBitmask mask);
EmberStatus emberGetExtendedSecurityBitmask(EmberExtendedSecurityBitmask* mask);
EmberStatus emberSetNodeId(EmberNodeId nodeId);
uint8_t emberGetLastStackZigDevRequestSequence(void);
#ifndef UC_BUILD
void emberSetMaximumIncomingTransferSize(uint16_t size);
void emberSetMaximumOutgoingTransferSize(uint16_t size);
void emberSetDescriptorCapability(uint8_t capability);
#endif
EmberStatus emberSendUnicastNetworkKeyUpdate(EmberNodeId targetShort,
                                             EmberEUI64  targetLong,
                                             EmberKeyData* newKey);
EmberStatus emberAesHashSimple(uint8_t totalLength,
                               const uint8_t* data,
                               uint8_t* result);
uint8_t emberGetCurrentNetwork(void);
EmberStatus emberSetCurrentNetwork(uint8_t index);
uint8_t emberGetCallbackNetwork(void);

EmberStatus emberFindAndRejoinNetworkWithReason(bool haveCurrentNetworkKey,
                                                uint32_t channelMask,
                                                EmberRejoinReason reason);
EmberStatus emberFindAndRejoinNetworkWithNodeType(bool haveCurrentNetworkKey,
                                                  uint32_t channelMask,
                                                  EmberNodeType nodeType);

EmberStatus emberFindAndRejoinNetwork(bool haveCurrentNetworkKey,
                                      uint32_t channelMask);

EmberRejoinReason emberGetLastRejoinReason(void);
EmberLeaveReason emberGetLastLeaveReason(EmberNodeId* id);
uint8_t getSourceRouteOverhead(uint8_t messageLength);
EmberStatus emberSetPreinstalledCbkeData283k1(EmberPublicKey283k1Data *caPublic,
                                              EmberCertificate283k1Data *myCert,
                                              EmberPrivateKey283k1Data *myKey);
#define EZSP_MAXIMIZE_PACKET_BUFFER_COUNT 0xFF

//----------------------------------------------------------------
// ZLL methods

void emberZllGetTokenStackZllData(EmberTokTypeStackZllData *token);
void emberZllGetTokenStackZllSecurity(EmberTokTypeStackZllSecurity *token);
EmberStatus emberZllFormNetwork(EmberZllNetwork* networkInfo,
                                int8_t radioTxPower);
EmberStatus emberZllJoinTarget(const EmberZllNetwork* targetNetworkInfo);

//----------------------------------------------------------------
#include "rename-ezsp-functions.h"
#include "command-prototypes.h"
#endif // __EZSP_H__
