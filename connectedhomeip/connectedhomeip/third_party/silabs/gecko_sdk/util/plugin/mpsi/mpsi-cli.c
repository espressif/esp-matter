/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi-cli.h"

#ifdef EMBER_AF_API_AF_HEADER
#include EMBER_AF_API_AF_HEADER
#endif

#if defined (EMBER_STACK_ZIGBEE)
#include "app/util/serial/command-interpreter2.h"
EmberCommandEntry emberAfPluginMpsiCommands[] = { MPSI_COMMAND_LIST };
#elif defined (EMBER_STACK_BLE)
#include COMMON_HEADER
#include "command_interpreter.h"
#else
#error "MPSI CLI: Unsupported stack!"
#endif // EMBER_STACK_ZIGBEE

#include "mpsi.h"

void emAfPluginMpsiCliReceiveMessage(CLI_HANDLER_PARAM_LIST)
{
  MpsiMessage_t mpsiMessage;
  uint8_t status;

  mpsiMessage.destinationAppId =
    (uint8_t)emAfPluginMpsiCliUnsignedCommandArgument(0);
  mpsiMessage.messageId =
    (uint16_t)emAfPluginMpsiCliUnsignedCommandArgument(1);
  mpsiMessage.payloadLength =
    (uint8_t)emAfPluginMpsiCliUnsignedCommandArgument(2);
  (void)emAfPluginMpsiCliCopyStringArgument(3,
                                            mpsiMessage.payload,
                                            mpsiMessage.payloadLength,
                                            false);

  status = emberAfPluginMpsiReceiveMessage(&mpsiMessage);

  if (MPSI_SUCCESS != status) {
    mpsiPrintln("MPSI CLI: error receiving MPSI message (0x%x, 0x%2x, %d byte%s"
                "): error %d",
                mpsiMessage.destinationAppId,
                mpsiMessage.messageId,
                mpsiMessage.payloadLength,
                mpsiMessage.payloadLength == 1 ? "" : "s",
                status);
  }
}

void emAfPluginMpsiCliSendMessage(CLI_HANDLER_PARAM_LIST)
{
  MpsiMessage_t mpsiMessage;
  uint8_t status;

  mpsiMessage.destinationAppId =
    (uint8_t)emAfPluginMpsiCliUnsignedCommandArgument(0);
  mpsiMessage.messageId =
    (uint16_t)emAfPluginMpsiCliUnsignedCommandArgument(1);
  mpsiMessage.payloadLength =
    (uint8_t)emAfPluginMpsiCliUnsignedCommandArgument(2);
  (void)emAfPluginMpsiCliCopyStringArgument(3,
                                            mpsiMessage.payload,
                                            mpsiMessage.payloadLength,
                                            false);

  status = emberAfPluginMpsiSendMessage(&mpsiMessage);

  if (MPSI_SUCCESS != status) {
    mpsiPrintln("MPSI CLI: error sending MPSI message (0x%x, 0x%2x, %d byte%s):"
                " error %d",
                mpsiMessage.destinationAppId,
                mpsiMessage.messageId,
                mpsiMessage.payloadLength,
                mpsiMessage.payloadLength == 1 ? "" : "s",
                status);
  }
}
