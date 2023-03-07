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

/** @brief Query Response
 *
 * Allows the application to control whether the plugin can respond to incoming
 * bootload messages.
 *
 * @param queryWasBroadcast Boolean indicating whether the query was broadcast
 * or unicast Ver.: always
 * @param queryData A data structure providing the details about the local
 * device's bootloader. Ver.: always
 */
WEAK(void emberAfPluginStandaloneBootloaderServerQueryResponseCallback(bool queryWasBroadcast,
                                                                       const EmberAfStandaloneBootloaderQueryResponseData*queryData))
{
}

/** @brief Finished
 *
 * Notifies the application when the standalone bootload has completed,
 * successfully or otherwise.
 *
 * @param success Boolean indicating the success or failure of the bootload.
 * Ver.: always
 */
WEAK(void emberAfPluginStandaloneBootloaderServerFinishedCallback(bool success))
{
}
