/***************************************************************************//**
 * @file
 * @brief Application main entry and initialisations.
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

// GPD Instance
static EmberGpd_t gpdNode = { 0 };

EmberGpd_t * emberGpdGetGpd(void)
{
  return &gpdNode;
}

void emberGpdLoadGpdDefaultConfiguration(EmberGpd_t * gpd)
{
  gpd->addr.appId = EMBER_AF_PLUGIN_APPS_APPLICATION_ID;
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_SRC_ID)
  gpd->addr.id.srcId = EMBER_AF_PLUGIN_APPS_SRC_ID;
#elif defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  emberGpdAfPluginGetGpdIeeeCallback(gpd->addr.id.ieee,
                                     &gpd->addr.gpdEndpoint);
#else
#error "Unsupported GPD Application Id"
#endif

  uint8_t channelSet[] = EMBER_AF_PLUGIN_APPS_APP_CHANNEL_SET;
  // Initialise the first member of the channel set as default channel
  gpd->channel = channelSet[0];
  gpd->minRxWindow = EMBER_AF_PLUGIN_APPS_RX_WINDOW;
  //gpd->radioState = GPD_RADIO_STATE_IDLE;
  gpd->rxAfterTx = EMBER_AF_PLUGIN_APPS_RX_CAPABILITY;
  gpd->rxOffset = EMBER_AF_PLUGIN_APPS_RX_OFFSET;
  gpd->skipCca = true;

  uint8_t defaultKey[EMBER_AF_PLUGIN_APPS_KEY_LENGTH] = EMBER_AF_PLUGIN_APPS_KEY;
  (void) memcpy(gpd->securityKey, defaultKey, EMBER_AF_PLUGIN_APPS_KEY_LENGTH);
  gpd->securityKeyType = EMBER_AF_PLUGIN_APPS_SECURITY_KEY_TYPE;
  gpd->securityLevel = EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL;

  gpd->gpdState = EMBER_GPD_APP_STATE_NOT_COMMISSIONED;
  //gpd->securityFrameCounter = 0; // Default FC , no need to initialise
}

// Initialises the Gpd device data structure with configured default values.
// Upon power up, the members that are saved to the non volatile memory to be
// retrieved and loaded.
EmberGpd_t * emberGpdInit(void)
{
  emberGpdLoadGpdDefaultConfiguration(&gpdNode);
  emberGpdLoadSecDataFromNV(&gpdNode);
  emberGpdSecurityInit(&(gpdNode.addr),
                       gpdNode.securityKey,
                       gpdNode.securityFrameCounter);
  return &gpdNode;
}

void emberGpdSetChannel(uint8_t channel)
{
  gpdNode.channel = channel;
}

void emberGpdSetState(uint8_t gpdState)
{
  gpdNode.gpdState = gpdState;
}

EmberGpdAddr_t * emberGpdGetAddr(void)
{
  return &(gpdNode.addr);
}
