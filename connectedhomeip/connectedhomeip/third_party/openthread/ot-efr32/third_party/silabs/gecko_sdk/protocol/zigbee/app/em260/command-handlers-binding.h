/***************************************************************************//**
 * @file
 * @brief Declarations for binding table related command handlers.
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

// Ezsp Command Handlers

EmberStatus emberAfEzspSetBindingCommandCallback(uint8_t index,
                                                 EmberBindingTableEntry *value);

EmberStatus emberAfEzspDeleteBindingCommandCallback(uint8_t index);

#ifndef UC_BUILD
void emberAfPluginEzspBindingPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);
void emberAfPluginEzspBindingConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspBindingPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);
void emberAfPluginEzspBindingConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspBindingPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);
// -----------------------------------------------------------------------------
// Framework callbacks

void emberAfPluginEzspBindingPermitHostToNcpFrameCallback(bool *permit);
#endif  // UC_BUILD
