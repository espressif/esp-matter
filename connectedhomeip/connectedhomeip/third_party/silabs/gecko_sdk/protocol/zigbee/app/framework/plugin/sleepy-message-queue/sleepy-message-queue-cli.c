/***************************************************************************//**
 * @file
 * @brief Routines for managing messages for sleepy end devices.
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

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "sleepy-message-queue.h"

// Functions called in this .cli file that are defined elsewhere.
//extern EmberAfSleepyMessageId emberAfPluginSleepyMessageQueueStoreMessage( EmberAfSleepyMessage *pmsg, uint32_t timeoutSec );
//extern EmberAfSleepyMessageId emberAfPluginSleepyMessageQueueGetPendingMessageId( EmberEUI64 dstEui64 );
//extern bool emberAfPluginSleepyMessageQueueGetPendingMessage( EmberAfSleepyMessageId sleepyMsgId, EmberAfSleepyMessage *pmsg );
//extern uint8_t emberAfPluginSleepyMessageQueueGetNumMessages( EmberEUI64 dstEui64 );

#ifdef  UC_BUILD
#include "sl_cli.h"
void emAfSleepyMessageQueueCliGetNextMessageEventTimeoutMs(sl_cli_command_arg_t *arguments)
{
  uint32_t timeoutMs;
  uint32_t msTick;
  uint8_t msgId;

  msgId = sl_cli_get_argument_uint8(arguments, 0);
  timeoutMs = emMessageMSecRemaining(msgId);
  msTick = halCommonGetInt32uMillisecondTick();
  //emberAfAppPrintln("==== REM TIME=%d msec, evtTmout=%d, msTick=%d", timeoutMs, msgTimeoutEvent.timeToExecute, msTick );
  emberAfAppPrintln("Remaining Time=%d msec, msTick=%d", timeoutMs, msTick);
}

// plugin sleepy-message-queue unusedCnt
void emAfSleepyMessageQueueCliGetNumUnusedEntries(sl_cli_command_arg_t *arguments)
{
  uint8_t cnt;
  cnt = emberAfPluginSleepyMessageQueueGetNumUnusedEntries();
  emberAfAppPrintln("Unused Message Queue Entries=%d", cnt);
}

// plugin sleepy-message-queue store <timeoutSec:4> <payload*:4> <payloadLength:2> <payloadId:2> <destEui64:8>
void emAfSleepyMessageQueueCliStore(sl_cli_command_arg_t *arguments)
{
  //uint8_t *payload;
  //uint16_t length;
  //uint16_t payloadId;
  //EmberEUI64 dstEui64;
  EmberAfSleepyMessage msg;
  EmberAfSleepyMessageId msgId;

  uint32_t timeoutSec;

  timeoutSec = sl_cli_get_argument_uint32(arguments, 0);
  msg.payload = (uint8_t *)sl_cli_get_argument_string(arguments, 1);
  msg.length = sl_cli_get_argument_uint16(arguments, 2);
  msg.payloadId = sl_cli_get_argument_uint16(arguments, 3);
  sl_zigbee_copy_hex_arg(arguments, 4, msg.dstEui64, EUI64_SIZE, false);

  msgId = emberAfPluginSleepyMessageQueueStoreMessage(&msg, timeoutSec);
  if ( msgId != EMBER_AF_PLUGIN_SLEEPY_MESSAGE_INVALID_ID ) {
    emberAfAppPrintln("Message Stored, msgID=%d, payloadID=%d", msgId, msg.payloadId);
  } else {
    emberAfAppPrintln("ERROR - Message not stored");
  }
}

// plugin sleepy-message-queue getPendingMsgId <destEui64:8>
void emAfSleepyMessageQueueCliGetPendingMsgId(sl_cli_command_arg_t *arguments)
{
  EmberAfSleepyMessageId msgId;
  EmberEUI64 eui64;

  sl_zigbee_copy_hex_arg(arguments, 0, eui64, EUI64_SIZE, false);
  msgId = emberAfPluginSleepyMessageQueueGetPendingMessageId(eui64);
  emberAfAppPrintln("Message Pending ID=%d", msgId);
}

// plugin sleepy-message-queue getPendingMsg <msgId:1>
void emAfSleepyMessageQueueCliGetPendingMsg(sl_cli_command_arg_t *arguments)
{
  EmberAfSleepyMessage   msg;
  EmberAfSleepyMessageId msgId;
  bool found;

  msgId = sl_cli_get_argument_uint8(arguments, 0);
  found = emberAfPluginSleepyMessageQueueGetPendingMessage(msgId, &msg);
  //emberAfAppPrintln("Pending Msg Found=%d, payload=%d, length=%d, ID=%d",
  //    found, (uint32_t)msg.payload, msg.length, msg.payloadId );
  emberAfAppPrintln("Pending Msg Found=%d", found);
}

// plugin sleepy-message-queue getNumMsg <destEui64:8>
void emAfSleepyMessageQueueCliGetNumPendingMsg(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 eui64;
  uint8_t cnt;

  sl_zigbee_copy_hex_arg(arguments, 0, eui64, EUI64_SIZE, false);
  cnt = emberAfPluginSleepyMessageQueueGetNumMessages(eui64);
  emberAfAppPrintln("Pending Msg Count=%d", cnt);
}

// plugin sleepy-message-queue remove <msgId:1>
void emAfSleepyMessageQueueCliRemoveMsgId(sl_cli_command_arg_t *arguments)
{
  EmberAfSleepyMessageId msgId;
  bool status;

  msgId = sl_cli_get_argument_uint8(arguments, 0);
  status = emberAfPluginSleepyMessageQueueRemoveMessage(msgId);
  if ( status == true ) {
    emberAfAppPrintln("Removed Msg ID=%d", msgId);
  } else {
    emberAfAppPrintln("ERROR - Msg Remove Failed");
  }
}

// plugin sleepy-message-queue removeAll <destEui64:8>
void emAfSleepyMessageQueueCliRemoveAllMsg(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 eui64;
  sl_zigbee_copy_hex_arg(arguments, 0, eui64, EUI64_SIZE, false);
  emberAfPluginSleepyMessageQueueRemoveAllMessages(eui64);
  emberAfAppPrintln("Removed All Msgs from {%x %x %x %x %x %x %x %x}",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7]);
}

void emAfSleepyMessageQueueCliGetCurrentInt32uMillisecondTick()
{
  uint32_t tickMs;

  tickMs = halCommonGetInt32uMillisecondTick();
  emberAfAppPrintln("MS Tick=%d", tickMs);
}

#else

void emAfSleepyMessageQueueCliGetNextMessageEventTimeoutMs(void);
void emAfSleepyMessageQueueCliGetNumUnusedEntries(void);
void emAfSleepyMessageQueueCliStore(void);
void emAfSleepyMessageQueueCliGetPendingMsgId(void);
void emAfSleepyMessageQueueCliGetPendingMsg(void);
void emAfSleepyMessageQueueCliGetNumPendingMsg(void);
void emAfSleepyMessageQueueCliRemoveMsgId(void);
void emAfSleepyMessageQueueCliRemoveAllMsg(void);

void emAfSleepyMessageQueueCliGetNextMessageEventTimeoutMs()
{
  uint32_t timeoutMs;
  uint32_t msTick;
  uint8_t msgId;

  msgId = (uint8_t)emberUnsignedCommandArgument(0);
  timeoutMs = emMessageMSecRemaining(msgId);
  msTick = halCommonGetInt32uMillisecondTick();
  //emberAfAppPrintln("==== REM TIME=%d msec, evtTmout=%d, msTick=%d", timeoutMs, msgTimeoutEvent.timeToExecute, msTick );
  emberAfAppPrintln("Remaining Time=%d msec, msTick=%d", timeoutMs, msTick);
}

// plugin sleepy-message-queue unusedCnt
void emAfSleepyMessageQueueCliGetNumUnusedEntries(void)
{
  uint8_t cnt;
  cnt = emberAfPluginSleepyMessageQueueGetNumUnusedEntries();
  emberAfAppPrintln("Unused Message Queue Entries=%d", cnt);
}

// plugin sleepy-message-queue store <timeoutSec:4> <payload*:4> <payloadLength:2> <payloadId:2> <destEui64:8>
void emAfSleepyMessageQueueCliStore(void)
{
  //uint8_t *payload;
  //uint16_t length;
  //uint16_t payloadId;
  //EmberEUI64 dstEui64;
  EmberAfSleepyMessage msg;
  EmberAfSleepyMessageId msgId;

  uint32_t timeoutSec;

  timeoutSec = (uint32_t)emberUnsignedCommandArgument(0);

  // CLI command is taking a uint32 argument and casting it to a pointer, which breaks while running tests on 64 bit compiler
  // Commenting this line for now as it gives a cast error and takes pointer as a CLI argument which is potentially dangerous (issue raised in JIRA, EMZIGBEE-6972)
  // msg.payload = (uint8_t *)emberUnsignedCommandArgument(1);
  msg.length = (uint16_t)emberUnsignedCommandArgument(2);
  msg.payloadId = (uint16_t)emberUnsignedCommandArgument(3);
  emberCopyStringArgument(4, msg.dstEui64, EUI64_SIZE, false);

  msgId = emberAfPluginSleepyMessageQueueStoreMessage(&msg, timeoutSec);
  if ( msgId != EMBER_AF_PLUGIN_SLEEPY_MESSAGE_INVALID_ID ) {
    emberAfAppPrintln("Message Stored, msgID=%d, payloadID=%d", msgId, msg.payloadId);
  } else {
    emberAfAppPrintln("ERROR - Message not stored");
  }
}

// plugin sleepy-message-queue getPendingMsgId <destEui64:8>
void emAfSleepyMessageQueueCliGetPendingMsgId(void)
{
  EmberAfSleepyMessageId msgId;
  EmberEUI64 eui64;

  emberCopyStringArgument(0, eui64, EUI64_SIZE, false);
  msgId = emberAfPluginSleepyMessageQueueGetPendingMessageId(eui64);
  emberAfAppPrintln("Message Pending ID=%d", msgId);
}

// plugin sleepy-message-queue getPendingMsg <msgId:1>
void emAfSleepyMessageQueueCliGetPendingMsg(void)
{
  EmberAfSleepyMessage   msg;
  EmberAfSleepyMessageId msgId;
  bool found;

  msgId = (uint8_t)emberUnsignedCommandArgument(0);
  found = emberAfPluginSleepyMessageQueueGetPendingMessage(msgId, &msg);
  //emberAfAppPrintln("Pending Msg Found=%d, payload=%d, length=%d, ID=%d",
  //    found, (uint32_t)msg.payload, msg.length, msg.payloadId );
  emberAfAppPrintln("Pending Msg Found=%d", found);
}

// plugin sleepy-message-queue getNumMsg <destEui64:8>
void emAfSleepyMessageQueueCliGetNumPendingMsg(void)
{
  EmberEUI64 eui64;
  uint8_t cnt;

  emberCopyStringArgument(0, eui64, EUI64_SIZE, false);
  cnt = emberAfPluginSleepyMessageQueueGetNumMessages(eui64);
  emberAfAppPrintln("Pending Msg Count=%d", cnt);
}

// plugin sleepy-message-queue remove <msgId:1>
void emAfSleepyMessageQueueCliRemoveMsgId(void)
{
  EmberAfSleepyMessageId msgId;
  bool status;

  msgId = (uint8_t)emberUnsignedCommandArgument(0);
  status = emberAfPluginSleepyMessageQueueRemoveMessage(msgId);
  if ( status == true ) {
    emberAfAppPrintln("Removed Msg ID=%d", msgId);
  } else {
    emberAfAppPrintln("ERROR - Msg Remove Failed");
  }
}

// plugin sleepy-message-queue removeAll <destEui64:8>
void emAfSleepyMessageQueueCliRemoveAllMsg(void)
{
  EmberEUI64 eui64;
  emberCopyStringArgument(0, eui64, EUI64_SIZE, false);
  emberAfPluginSleepyMessageQueueRemoveAllMessages(eui64);
  emberAfAppPrintln("Removed All Msgs from {%x %x %x %x %x %x %x %x}",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7]);
}

void emAfSleepyMessageQueueCliGetCurrentInt32uMillisecondTick()
{
  uint32_t tickMs;

  tickMs = halCommonGetInt32uMillisecondTick();
  emberAfAppPrintln("MS Tick=%d", tickMs);
}
#endif
