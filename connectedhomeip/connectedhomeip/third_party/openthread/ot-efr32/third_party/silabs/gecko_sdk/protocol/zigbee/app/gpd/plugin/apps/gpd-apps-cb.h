/***************************************************************************//**
 * @file
 * @brief GPD application support callback header.
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

void emberGpdAfPluginMainCallback(EmberGpd_t * gpd);
uint8_t emberGpdAfPluginGetApplicationDescriptionCallback(uint8_t * messagePayload,
                                                          uint8_t maxLength,
                                                          bool * last);
uint8_t emberGpdAfPluginGetSwitchContactStutusCallback(void);
void emberGpdAfPluginCommissioningReplyCallback(uint8_t length,
                                                uint8_t * commReplyPayload);
