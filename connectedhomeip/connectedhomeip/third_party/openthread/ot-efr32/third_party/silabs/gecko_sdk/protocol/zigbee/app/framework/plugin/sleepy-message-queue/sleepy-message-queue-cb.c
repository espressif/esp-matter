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

/** @brief Message Timed Out
 *
 * This function is called by the sleepy message queue when a message times
 * out. The plugin will invalidate the entry in the queue after giving the
 * application a chance to perform any actions on the timed-out message.
 *
 * @param sleepyMsgId   Ver.: always
 */
WEAK(void emberAfPluginSleepyMessageQueueMessageTimedOutCallback(uint8_t sleepyMsgId))
{
  // The application may call emberAfPluginSleepyMessageGetPendingMessage() to retrieve the
  // emberAfSleepyMessage in this callback before it is invalidated by the plugin.
}
