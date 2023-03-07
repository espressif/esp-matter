/***************************************************************************//**
 * @file
 * @brief ZLL specific EZSP command handler declarations.
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

EmberStatus emberAfEzspZllNetworkOpsCommandCallback(EmberZllNetwork* networkInfo,
                                                    EzspZllNetworkOperation op,
                                                    int8_t radioTxPower);

#ifndef UC_BUILD
void emberAfPluginEzspZllPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);
void emberAfPluginEzspZllConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
#endif  // UC_BUILD

// #ezsp command-handler genearted does  not pick up these exceptions
#define emberZllGetTokens emberZllGetTokensStackZll
#define emberZllSetDataToken emberZllSetTokenStackZllData
