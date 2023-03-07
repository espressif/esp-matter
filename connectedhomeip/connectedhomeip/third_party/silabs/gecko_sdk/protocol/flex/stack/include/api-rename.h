/***************************************************************************//**
 * @brief Ember Connect API renaming (to be used for bare metal SoC
 * applications).
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of  Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software  is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __API_RENAME_H__
#define __API_RENAME_H__

// Core
#define emberGetSecurityKey                                emApiGetSecurityKey
#define emberRemovePsaSecurityKey                          emApiRemovePsaSecurityKey
#define emberApplyIrCalibration                            emApiApplyIrCalibration
#define emberCalibrateCurrentChannelExtended               emApiCalibrateCurrentChannelExtended
#define emberCurrentStackTasks                             emApiCurrentStackTasks
#define emberGetCalType                                    emApiGetCalType
#define emberGetCounter                                    emApiGetCounter
#define emberGetMaximumPayloadLength                       emApiGetMaximumPayloadLength
#define emberGetRadioChannel                               emApiGetRadioChannel
#define emberGetRadioPower                                 emApiGetRadioPower
#define emberInit                                          emApiInit
#define emberMacGetParentAddress                           emApiMacGetParentAddress
#define emberNetworkState                                  emApiNetworkState
#define emberSetIndirectQueueTimeout                       emApiSetIndirectQueueTimeout
#define emberSetMacParams                                  emApiSetMacParams
#define emberSetPsaSecurityKey                             emApiSetPsaSecurityKey
#define emberSetRadioChannelExtended                       emApiSetRadioChannelExtended
#define emberSetRadioPower                                 emApiSetRadioPower
#define emberSetRadioPowerMode                             emApiSetRadioPowerMode
#define emberSetSecurityKey                                emApiSetSecurityKey
#define emberStackIdleTimeMs                               emApiStackIdleTimeMs
#define emberStackIsUp                                     emApiStackIsUp
#define emberTempCalibration                               emApiTempCalibration
#define emberTick                                          emApiTick

// Messaging
#define emberGetChildFlags                                 emApiGetChildFlags
#define emberGetChildInfo                                  emApiGetChildInfo
#define emberMacAddShortToLongAddressMapping               emApiMacAddShortToLongAddressMapping
#define emberMacClearShortToLongAddressMappings            emApiMacClearShortToLongAddressMappings
#define emberMacMessageSend                                emApiMacMessageSend
#define emberMacSetPanCoordinator                          emApiMacSetPanCoordinator
#define emberMessageSend                                   emApiMessageSend
#define emberNetworkLeave                                  emApiNetworkLeave
#define emberPollForData                                   emApiPollForData
#define emberPurgeIndirectMessages                         emApiPurgeIndirectMessages
#define emberRemoveChild                                   emApiRemoveChild
#define emberSetPollDestinationAddress                     emApiSetPollDestinationAddress

// Network Management
#define emberClearSelectiveJoinPayload                     emApiClearSelectiveJoinPayload
#define emberFormNetwork                                   emApiFormNetwork
#define emberFrequencyHoppingSetChannelMask                emApiFrequencyHoppingSetChannelMask
#define emberFrequencyHoppingStartClient                   emApiFrequencyHoppingStartClient
#define emberFrequencyHoppingStartServer                   emApiFrequencyHoppingStartServer
#define emberFrequencyHoppingStop                          emApiFrequencyHoppingStop
#define emberGetActiveScanDuration                         emApiGetActiveScanDuration
#define emberGetAuxiliaryAddressFilteringEntry             emApiGetAuxiliaryAddressFilteringEntry
#define emberGetDefaultChannel                             emApiGetDefaultChannel
#define emberJoinCommissioned                              emApiJoinCommissioned
#define emberJoinNetworkExtended                           emApiJoinNetworkExtended
#define emberNetworkInit                                   emApiNetworkInit
#define emberPermitJoining                                 emApiPermitJoining
#define emberResetNetworkState                             emApiResetNetworkState
#define emberSetActiveScanDuration                         emApiSetActiveScanDuration
#define emberSetApplicationBeaconPayload                   emApiSetApplicationBeaconPayload
#define emberSetAuxiliaryAddressFilteringEntry             emApiSetAuxiliaryAddressFilteringEntry
#define emberSetSelectiveJoinPayload                       emApiSetSelectiveJoinPayload
#define emberStartActiveScan                               emApiStartActiveScan
#define emberStartEnergyScan                               emApiStartEnergyScan
#define emberStartTxStream                                 emApiStartTxStream
#define emberStopTxStream                                  emApiStopTxStream

#endif // __API_RENAME_H__
