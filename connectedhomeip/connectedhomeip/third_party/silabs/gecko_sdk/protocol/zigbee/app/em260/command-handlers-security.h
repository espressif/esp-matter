/***************************************************************************//**
 * @file
 * @brief Prototypes for ZigBee PRO Security command handler functions.
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

//------------------------------------------------------------------------------
// Ezsp Command Handlers

EmberStatus emberAfPluginEzspSecurityHandleKeyCommandCallback(uint8_t index,
                                                              EmberKeyStruct* keyStruct);

EmberStatus emberAfEzspAesMmoHashCommandCallback(EmberAesMmoHashContext* context,
                                                 bool finalize,
                                                 uint8_t length,
                                                 uint8_t* data,
                                                 EmberAesMmoHashContext* returnContext);

EmberStatus emberAfPluginEzspSecurityHandleKeyCommandCallback(uint8_t index,
                                                              EmberKeyStruct* keyStruct);

EmberStatus emberAfEzspRemoveDeviceCommandCallback(EmberNodeId destShort,
                                                   EmberEUI64 destLong,
                                                   EmberEUI64 targetLong);

EmberStatus emberAfEzspUnicastNwkKeyUpdateCommandCallback(EmberNodeId destShort,
                                                          EmberEUI64 destLong,
                                                          EmberKeyData* key);

EmberStatus emberAfEzspAddTransientLinkKeyCommandCallback(EmberEUI64 partnerEui64,
                                                          EmberKeyData* key);

EmberStatus emberAfEzspGetTransientLinkKeyCommandCallback(const EmberEUI64 eui,
                                                          EmberTransientKeyData *transientKeyData);

EmberStatus emberAfEzspUpdateLinkKeyCommandCallback(void);

#ifndef UC_BUILD
void emberAfPluginEzspSecurityGetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspSecuritySetValueCommandCallback(EmberAfPluginEzspValueCommandContext *context);
void emberAfPluginEzspSecuritySetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspSecurityGetConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspSecuritySetConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspSecurityPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);
#endif  // UC_BUILD

EmberJoinDecision emberAfPluginEzspSecurityTrustCenterJoinCallback(EmberNodeId newNodeId,
                                                                   const EmberEUI64 newNodeEui64,
                                                                   EmberDeviceUpdate status,
                                                                   EmberNodeId parentOfNewNode,
                                                                   EzspDecisionId decisionId,
                                                                   EmberJoinDecision joinDecision);
