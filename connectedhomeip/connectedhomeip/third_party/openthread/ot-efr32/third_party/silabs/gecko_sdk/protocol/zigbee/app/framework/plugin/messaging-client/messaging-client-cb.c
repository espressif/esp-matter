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

/** @brief Pre Display Message
 *
 * This function is called by the Messaging client plugin when a DisplayMessage
 * command is received. If callback returns true, the plugin assumes the
 * message have been handled and will not do anything with the message.
 * Otherwise, the plugin will go through with its own implementation.
 *
 * @param messageId   Ver.: always
 * @param messageControl   Ver.: always
 * @param startTime   Ver.: always
 * @param durationInMinutes   Ver.: always
 * @param message   Ver.: always
 * @param optionalExtendedMessageControl   Ver.: always
 */
WEAK(bool emberAfPluginMessagingClientPreDisplayMessageCallback(uint32_t messageId,
                                                                uint8_t messageControl,
                                                                uint32_t startTime,
                                                                uint16_t durationInMinutes,
                                                                uint8_t *message,
                                                                uint8_t optionalExtendedMessageControl))
{
  return false;
}

/** @brief Display Message
 *
 * This function is called by the Messaging client plugin whenever the
 * application should display a message.
 *
 * @param message The message that should be displayed. Ver.: always
 */
WEAK(void emberAfPluginMessagingClientDisplayMessageCallback(EmberAfPluginMessagingClientMessage *message))
{
}

/** @brief Cancel Message
 *
 * This function is called by the Messaging client plugin whenever the
 * application should stop displaying a message.
 *
 * @param message The message that should no longer be displayed. Ver.: always
 */
WEAK(void emberAfPluginMessagingClientCancelMessageCallback(EmberAfPluginMessagingClientMessage *message))
{
}
