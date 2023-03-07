/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef __MPSI_CLI_H__
#define __MPSI_CLI_H__

#if defined (EMBER_STACK_ZIGBEE)
#define emAfPluginMpsiCliEntryAction(command, function, paramList, help) \
  emberCommandEntryAction(command, function, paramList, help)

#define emAfPluginMpsiCliEntryTerminator() \
  emberCommandEntryTerminator()

#define emAfPluginMpsiCliSignedCommandArgument(arg) \
  emberSignedCommandArgument(arg)

#define emAfPluginMpsiCliUnsignedCommandArgument(arg) \
  emberUnsignedCommandArgument(arg)

#define emAfPluginMpsiCliCopyStringArgument(arg, buffer, length, leftPad) \
  emberCopyStringArgument(arg, buffer, length, leftPad)

#define CLI_HANDLER_PARAM_LIST void

#elif defined (EMBER_STACK_BLE)
#define emAfPluginMpsiCliEntryAction(command, function, paramList, help) \
  COMMAND_ENTRY(command, paramList, function, help)

#define emAfPluginMpsiCliEntryTerminator() \
  COMMAND_SEPARATOR("")

#define emAfPluginMpsiCliSignedCommandArgument(arg) \
  ciGetSigned(buf[arg + 1])

#define emAfPluginMpsiCliUnsignedCommandArgument(arg) \
  ciGetUnsigned(buf[arg + 1])

uint8_t emAfPluginMpsiCliCopyStringArgument(uint8_t arg,
                                            uint8_t *buffer,
                                            uint8_t length,
                                            bool    leftPad)
{
  MEMMOVE(buffer, buf[arg], length);
  (void)leftPad;
  return length;
}

#define CLI_HANDLER_PARAM_LIST int abc, char **buf

#else
#error "MPSI CLI: Unsupported stack!"
#endif

#define MPSI_COMMAND_LIST                                               \
  emAfPluginMpsiCliEntryAction("receive-message",                       \
                               emAfPluginMpsiCliReceiveMessage, "uvub", \
                               "Simulate receiving an MPSI message"),   \
  emAfPluginMpsiCliEntryAction("send-message",                          \
                               emAfPluginMpsiCliSendMessage, "uvub",    \
                               "Simulate sending an MPSI message"),     \
  emAfPluginMpsiCliEntryTerminator()

void emAfPluginMpsiCliReceiveMessage(CLI_HANDLER_PARAM_LIST);
void emAfPluginMpsiCliSendMessage(CLI_HANDLER_PARAM_LIST);

#endif // __MPSI_CLI_H__
