/***************************************************************************//**
 * @brief Weakly defined callbacks for mailbox client.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "stack/include/ember.h"
#include "mailbox-client.h"

//------------------------------------------------------------------------------
// Weak callbacks definitions

WEAK(void emberAfPluginMailboxClientMessageSubmitCallback(EmberAfMailboxStatus status,
                                                          EmberNodeId mailboxServer,
                                                          EmberNodeId messageDestination,
                                                          uint8_t tag))
{
  (void)status;
  (void)mailboxServer;
  (void)messageDestination;
  (void)tag;
}

WEAK(void emberAfPluginMailboxClientMessageDeliveredCallback(EmberAfMailboxStatus status,
                                                             EmberNodeId mailboxServer,
                                                             EmberNodeId messageDestination,
                                                             uint8_t tag))
{
  (void)status;
  (void)mailboxServer;
  (void)messageDestination;
  (void)tag;
}

WEAK(void emberAfPluginMailboxClientCheckInboxCallback(EmberAfMailboxStatus status,
                                                       EmberNodeId mailboxServer,
                                                       EmberNodeId messageSource,
                                                       uint8_t *message,
                                                       EmberMessageLength messageLength,
                                                       uint8_t tag,
                                                       bool moreMessages))
{
  (void)status;
  (void)mailboxServer;
  (void)messageSource;
  (void)message;
  (void)messageLength;
  (void)tag;
  (void)moreMessages;
}
