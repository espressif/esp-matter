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

// Copies uint32_t type to an array
void emberGpdUtilityCopy4Bytes(uint8_t * dst, uint32_t value)
{
  dst[0] = (uint8_t)(value & 0x000000FF);
  dst[1] = (uint8_t)((value >> 8)  & 0x000000FF);
  dst[2] = (uint8_t)((value >> 16) & 0x000000FF);
  dst[3] = (uint8_t)((value >> 24) & 0x000000FF);
}

// GPD address match with considering bcast address
bool emberGpdUtilityAddrMatch(EmberGpdAddr_t * a1, EmberGpdAddr_t * a2)
{
  if (a1->appId == EMBER_GPD_APP_ID_SRC_ID
      && a2->appId == EMBER_GPD_APP_ID_SRC_ID) {
    if (a1->id.srcId == a2->id.srcId) {
      return true;
    }
  } else if (a1->appId == EMBER_GPD_APP_ID_IEEE_ID
             && a2->appId == EMBER_GPD_APP_ID_IEEE_ID) {
    if (!memcmp(a1->id.ieee, a2->id.ieee, 8)) {
      if (a1->gpdEndpoint == a2->gpdEndpoint
          || a1->gpdEndpoint == 0xff
          || a2->gpdEndpoint == 0xff
          || a1->gpdEndpoint == 0x00
          || a2->gpdEndpoint == 0x00) {
        return true;
      }
    }
  }
  return false;
}
