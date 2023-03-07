/***************************************************************************//**
 * @file
 * @brief CLI for the Network Creator Security plugin.
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
#ifdef UC_BUILD

#include "app/framework/include/af.h"
#include "network-creator-security.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "network-creator-security-config.h"

// plugin network_creator_security [open|close]
void emAfPluginNetworkCreatorSecurityOpenOrCloseNetworkCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  bool open = memcmp(sl_cli_get_command_string(arguments, position), "open", strlen("open")) == 0;

  status = (open
            ? emberAfPluginNetworkCreatorSecurityOpenNetwork()
            : emberAfPluginNetworkCreatorSecurityCloseNetwork());

  sl_zigbee_core_debug_print("%s: %s network: 0x%02X\n",
                             EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                             (open ? "Open" : "Close"),
                             status);
}

extern EmberKeyData distributedKey;

void emAfPluginNetworkCreatorSecuritySetJoiningLinkKeyCommand(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;
  size_t len = 16;
  sl_zigbee_copy_eui64_arg(arguments, 0, eui64, true);
  //emberCopyKeyArgument(1, &keyData);
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 1, &len);
  MEMSET(keyData.contents, 0, EMBER_ENCRYPTION_KEY_SIZE);
  MEMMOVE(keyData.contents, ptr_string, EMBER_ENCRYPTION_KEY_SIZE); // Is the padding correct?

  status = emberAddTransientLinkKey(eui64, &keyData);

  emberAfCorePrintln("%s: %s: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Set joining link key",
                     status);
}

void emAfPluginNetworkCreatorSecurityClearJoiningLinkKeyCommand(sl_cli_command_arg_t *arguments)
{
  emberClearTransientLinkKeys();

  emberAfCorePrintln("%s: %s: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Clear joining link keys",
                     EMBER_SUCCESS);
}

void emAfPluginNetworkCreatorSecurityOpenNetworkWithKeyCommand(sl_cli_command_arg_t *arguments)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;
  size_t len = 16;
  sl_zigbee_copy_eui64_arg(arguments, 0, eui64, true);
  //emberCopyKeyArgument(1, &keyData);
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 1, &len);
  MEMSET(keyData.contents, 0, EMBER_ENCRYPTION_KEY_SIZE);
  MEMMOVE(keyData.contents, ptr_string, EMBER_ENCRYPTION_KEY_SIZE);  // Is the padding correct?

  status = emberAfPluginNetworkCreatorSecurityOpenNetworkWithKeyPair(eui64, keyData);

  emberAfCorePrintln("%s: Open network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     status);
}

void emAfPluginNetworkCreatorSecurityConfigureDistributedKey(sl_cli_command_arg_t *arguments)
{
  size_t len = 16;
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 0, &len);
  MEMSET(distributedKey.contents, 0, EMBER_ENCRYPTION_KEY_SIZE);
  MEMMOVE(distributedKey.contents, ptr_string, EMBER_ENCRYPTION_KEY_SIZE); // Is the padding correct?
}

#else // !UC_BUILD

#include "app/framework/include/af.h"

#include "network-creator-security.h"

extern EmberKeyData distributedKey;

void emAfPluginNetworkCreatorSecuritySetJoiningLinkKeyCommand(void)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;

  emberCopyBigEndianEui64Argument(0, eui64);
  emberCopyKeyArgument(1, &keyData);

  status = emberAddTransientLinkKey(eui64, &keyData);

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Set joining link key",
                     status);
}

void emAfPluginNetworkCreatorSecurityClearJoiningLinkKeyCommand(void)
{
  emberClearTransientLinkKeys();

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Clear joining link keys",
                     EMBER_SUCCESS);
}

void emAfPluginNetworkCreatorSecurityOpenOrCloseNetworkCommand(void)
{
  EmberStatus status;
  bool open = (emberStringCommandArgument(-1, NULL)[0] == 'o');

  status = (open
            ? emberAfPluginNetworkCreatorSecurityOpenNetwork()
            : emberAfPluginNetworkCreatorSecurityCloseNetwork());

  emberAfCorePrintln("%p: %p network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     (open ? "Open" : "Close"),
                     status);
}

void emAfPluginNetworkCreatorSecurityOpenNetworkWithKeyCommand(void)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;

  emberCopyBigEndianEui64Argument(0, eui64);
  emberCopyKeyArgument(1, &keyData);
  status = emberAfPluginNetworkCreatorSecurityOpenNetworkWithKeyPair(eui64, keyData);

  emberAfCorePrintln("%p: Open network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     status);
}

void emAfPluginNetworkCreatorSecurityConfigureDistributedKey(void)
{
  emberCopyKeyArgument(0, &distributedKey);
}

#endif // !UC_BUILD
