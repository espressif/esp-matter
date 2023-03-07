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

#include "gpd-components-common.h"

/** @brief Called by framework from the application main enry to inform the user
 * as the first call to the main.
 *
 * @param gpd pointer to gpd instance
 */
SL_WEAK void emberGpdAfPluginMainCallback(EmberGpd_t * gpd)
{
  (void)gpd;
}

/** @brief Called to get the application information payload from application. This callback
 *         helps application to filled up the application specefic application information
 *         that gets transmitted.
 *
 * @param messagePayload The pointer for the user to fillup.
 * @param maxLength maximum length.
 * @param last indicates the present application description packet is the last one.
 * Ver.: always
 *
 * @return the length writen by application.
 */
SL_WEAK uint8_t emberGpdAfPluginGetApplicationDescriptionCallback(uint8_t * messagePayload,
                                                                  uint8_t maxLength,
                                                                  bool * last)
{
  (void)maxLength;
  // This is an example of how to fill up an occupany sensor report as application description
  // The following is a single report descriptor for the occupancy attribute.
  uint8_t appDesc[13] = { 0x01, 0x01, 0x00, 0x00, 0x07, 0x08, 0x06, 0x04, 0x00, 0x00, 0x18, 0x10, 0x00 };
  memcpy(messagePayload, appDesc, sizeof(appDesc));
  *last = true;
  return sizeof(appDesc);
}

/** @brief This is called by framework to receive a contact status in case of generic switch
 *         info during commissioning request.
 *
 * @return a 8bit switch contact status.
 */
SL_WEAK uint8_t emberGpdAfPluginGetSwitchContactStutusCallback(void)
{
  return 0;
}
