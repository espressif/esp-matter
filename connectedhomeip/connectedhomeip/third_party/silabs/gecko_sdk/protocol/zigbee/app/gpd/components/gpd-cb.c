/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif  // SL_COMPONENT_CATALOG_PRESENT

#include "gpd-components-common.h"
#ifdef SL_CATALOG_PSA_CRYPTO_PRESENT
#include "psa/crypto.h"
#endif

/** @brief This is called by framework to initialise the NVM system
 *
 */
SL_WEAK void emberGpdAfPluginNvInitCallback(void)
{
}

/** @brief Called to the application to give a chance to load or store the GPD Context
 *.        in a non volatile context. Thsi can help the application to use any other
 *         non volatile storage.
 *
 * @param gpd pointer to gpd instance
 * @param nvmData The pointer to the data that needs saving or retrieving to or from
 *                the non-volatile memory.
 * @param sizeOfNvmData The size of the data non-volatile data.
 * @param loadStore indication wether to load or store the context.
 * Ver.: always
 *
 * @return true if application handled it.
 */
SL_WEAK bool emberGpdAfPluginNvSaveAndLoadCallback(EmberGpd_t * gpd,
                                                   uint8_t * nvmData,
                                                   uint8_t sizeOfNvmData,
                                                   EmebrGpdNvLoadStore_t loadStore)
{
  (void)gpd;
  (void)nvmData;
  (void)sizeOfNvmData;
  (void)loadStore;
  return false;
}

/** @brief Called from the incomming command handler context to let the application
 *         handle the channel configuration.
 *
 * @param channel channel.
 * Ver.: always
 *
 */
SL_WEAK void emberGpdAfPluginCommissioningChannelConfigCallback(uint8_t channel)
{
  (void)channel;
}

/** @brief Called from the imcomming command handler context for all the incoming
 *         command before the command handler handles it. based on the return code
 *         of this callback the internal command handler skips the processing.
 *
 * @param gpdCommand CommandId.
 * @param length length of the command
 * @param commandPayload The pointer to the commissioning reply payload.
 * Ver.: always
 *
 * @return true if application handled it.
 */
SL_WEAK bool emberGpdAfPluginIncomingCommandCallback(uint8_t gpdCommand,
                                                     uint8_t length,
                                                     uint8_t * commandPayload)
{
  (void)gpdCommand;
  (void)length;
  (void)commandPayload;
  return false;
}

/** @brief This is called by the application state machine to indicate when the GPD is going to sleep.
 *         Typically this will be called from rx-after-tx state machines and application state machine.
 *
 */
SL_WEAK void emberGpdAfPluginSleepCallback(void)
{
  // If Enters EM0 instead or awaken by other things, made to wait by following
  // code until the LE Timer expires to provide the exact rxOffset before receive.
  EMU_EnterEM2(true);
}

/** @brief This is called by framework to get a random number. User should implement it appropriately
 * depending on the random number generation in the project.
 *
 * @return a 32bit random number.
 */
#if defined(EMBER_AF_PLUGIN_APPS_MAC_SEQ) && (EMBER_AF_PLUGIN_APPS_MAC_SEQ == EMBER_GPD_MAC_SEQ_RANDOM)
SL_WEAK uint32_t emberGpdAfPluginGetRandomCallback(void)
{
  // randomValue is not initialised intentionally.
  // The reason is, if the call returns 0 (i.e entropy is not supported), whatever randomness stack has that will be taken.
  volatile uint8_t randomValue;
#ifdef SL_CATALOG_PSA_CRYPTO_PRESENT
  (void) psa_generate_random( (uint8_t *)(&randomValue), sizeof(randomValue));
#else
  (void)emberGpdRailGetRadioEntropyWrapper((uint8_t*)(&randomValue), sizeof(randomValue));
#endif //SL_CATALOG_PSA_CRYPTO_PRESENT
  return (uint32_t)randomValue;
}
#endif

/** @brief This is called by framework to get the IEEE (EUI64) address of the GPD.
 * Typically GPD application may have the IEEE address for each of the GPD programmed at a different
 * location or storage, so the framework obtains the address through this callback.
 *
 * @param eui pointer to collect the ieee
 * @param gpdEndpoint pointer to collect gpd endpoint
 */
SL_WEAK void emberGpdAfPluginGetGpdIeeeCallback(uint8_t *eui,
                                                uint8_t *gpdEndpoint)
{
#if defined EMBER_AF_PLUGIN_APPS_IEEE && defined EMBER_AF_PLUGIN_APPS_GPD_ENDPOINT
  uint8_t ieee[] = EMBER_AF_PLUGIN_APPS_IEEE;
  memcpy(eui, ieee, EMBER_GPD_EUI64_SIZE);
  *gpdEndpoint = EMBER_AF_PLUGIN_APPS_GPD_ENDPOINT;
#else
  (void)eui;
  (void)gpdEndpoint;
#endif
}

/** @brief Called from the imcomming command handler context to let the application
 *         handle the commissioning reply.
 *
 * @param length Length of the commissioning reply payload.
 * @param commReplyPayload The pointer to the commissioning reply payload.
 * Ver.: always
 *
 */
SL_WEAK void emberGpdAfPluginCommissioningReplyCallback(uint8_t length,
                                                        uint8_t * commReplyPayload)
{
  (void)length;
  (void)commReplyPayload;
}
