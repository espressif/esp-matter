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
/** @brief Green power server pass frame without translation callback
 *
 * This function is called by the green power server translation table plugin
 * to notify the application of a green power notification without translation
 * (when the endpoint is set as 0xFC).
 *
 * @param addr GPD address in the notification   Ver.: always
 * @param gpdCommandId GPD command id Ver.: always
 * @param gpdCommandPayload GPD command payload Ver.: always
 */
WEAK(void emberAfGreenPowerClusterPassFrameWithoutTranslationCallback(EmberGpAddress * addr,
                                                                      uint8_t gpdCommandId,
                                                                      uint8_t * gpdCommandPayload))
{
}

/** @brief Delete entries from translation table callback
 *
 * This function is called by the green power server translation table plugin
 * to notify the application of a translation table entry deletion for a gpd.
 * Application must respond to the request if it is handling this.
 *
 * @param gpdAddr   Ver.: always
 *
 * @returns Return true if handled by application.
 */
WEAK(bool emberAfGreenPowerClusterGpTransTableDeleteTranslationTableEntryCallback(EmberGpAddress *gpdAddr))
{
  return false;
}

/** @brief Request translation table callback
 *
 * This function is called by the green power server plugin to give a chance to the
 * application to respond to a translation table request.
 * Application must respond to the request if it is handling this.
 *
 * @param startIndex   Ver.: always
 *
 * @returns Return true if handled by application.
 */
WEAK(bool emberAfGreenPowerClusterGpRequestTranslationTableCallback(uint8_t startIndex))
{
  return false;
}

/** @brief Update Translation Table
 *
 * This function is called by the Green Power Sink Plugin to request the
 * application of a Green Power Device to add, remove or replace the requested
 * translation table entries.
 * Application must respond to the request if it is handling this.
 *
 * @param options      Ver.: always
 * @param gpdSrcId     Ver.: always
 * @param gpdIeee      Ver.: always
 * @param gpdEndpoint  Ver.: always
 * @param translations Ver.: always
 *
 * @returns Return true if handled by application.
 */
WEAK(bool emberAfGreenPowerClusterGpUpdateTranslationTableCallback(uint16_t options,
                                                                   uint32_t gpdSrcId,
                                                                   uint8_t* gpdIeee,
                                                                   uint8_t gpdEndpoint,
                                                                   uint8_t* translations))
{
  return false;
}
