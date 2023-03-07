/***************************************************************************//**
 * @file
 * @brief GPD component callback header.
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

void emberGpdAfPluginNvInitCallback(void);
bool emberGpdAfPluginNvSaveAndLoadCallback(EmberGpd_t * gpd,
                                           uint8_t * nvmData,
                                           uint8_t sizeOfNvmData,
                                           EmebrGpdNvLoadStore_t loadStore);
void emberGpdAfPluginCommissioningChannelConfigCallback(uint8_t channel);
bool emberGpdAfPluginIncomingCommandCallback(uint8_t gpdCommand,
                                             uint8_t length,
                                             uint8_t * commandPayload);
void emberGpdAfPluginSleepCallback(void);
#if defined(EMBER_AF_PLUGIN_APPS_MAC_SEQ) && (EMBER_AF_PLUGIN_APPS_MAC_SEQ == EMBER_GPD_MAC_SEQ_RANDOM)
uint32_t emberGpdAfPluginGetRandomCallback(void);
#endif
void emberGpdAfPluginGetGpdIeeeCallback(uint8_t *eui,
                                        uint8_t *gpdEndpoint);
void emberGpdAfPluginNvInitCallback(void);
void emberGpdAfPluginCommissioningReplyCallback(uint8_t length,
                                                uint8_t * commReplyPayload);
