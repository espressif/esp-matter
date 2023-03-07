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

/** @brief Get Password
 *
 * This callback will be implemented by an esi to provide device specific
 * password information.
 *
 * @param senderNodeId   Ver.: always
 * @param passwordType   Ver.: always
 * @param passwordInfo   Ver.: always
 */
WEAK(void emberAfPluginDeviceManagementServerGetPasswordCallback(EmberNodeId senderNodeId,
                                                                 EmberAfDeviceManagementPasswordType passwordType,
                                                                 EmberAfDeviceManagementPassword passwordInfo))
{
}
