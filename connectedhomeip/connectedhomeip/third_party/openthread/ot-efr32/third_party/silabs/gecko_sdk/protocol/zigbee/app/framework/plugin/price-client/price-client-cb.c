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

#include "af.h"

/** @brief Price Started
 *
 * This function is called by the Price client plugin whenever a price starts.
 *
 * @param price The price that has started. Ver.: always
 */
WEAK(void emberAfPluginPriceClientPriceStartedCallback(EmberAfPluginPriceClientPrice *price))
{
}

/** @brief Price Expired
 *
 * This function is called by the Price client plugin whenever a price expires.
 *
 * @param price The price that has expired. Ver.: always
 */
WEAK(void emberAfPluginPriceClientPriceExpiredCallback(EmberAfPluginPriceClientPrice *price))
{
}

/** @brief Pending Cpp Event
 *
 * This function is called by the Price client plugin when a CPP event is
 * received with pending status.
 *
 * @param cppAuth The CPP Auth status. Ver.: always
 */
WEAK(uint8_t emberAfPluginPriceClientPendingCppEventCallback(uint8_t cppAuth))
{
  extern uint8_t emberAfPriceClusterDefaultCppEventAuthorization;

  /* This callback should decide which CPP Auth status to send in response to the pending status.
     It may return EMBER_AF_PLUGIN_PRICE_CPP_AUTH_ACCEPTED or EMBER_AF_PLUGIN_PRICE_CPP_AUTH_REJECTED. */
  if ( cppAuth != EMBER_AF_PLUGIN_PRICE_CPP_AUTH_FORCED ) {
    return emberAfPriceClusterDefaultCppEventAuthorization;
  } else {
    return EMBER_AF_PLUGIN_PRICE_CPP_AUTH_ACCEPTED;
  }
}
