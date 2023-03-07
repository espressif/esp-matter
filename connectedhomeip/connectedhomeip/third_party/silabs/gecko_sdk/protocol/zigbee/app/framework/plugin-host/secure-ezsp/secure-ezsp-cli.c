/***************************************************************************//**
 * @file
 * @brief A sample of Secure EZSP protocol.
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
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/util/ezsp/secure-ezsp-host.h"

#ifdef UC_BUILD

EmberStatus emberAfGenerateRandomData(uint8_t* result, uint8_t size);

// plugin secure-ezsp set_security_key [securityType:4]
void emberAfPluginSecureEzspSetSecurityKeyCommand(sl_cli_command_arg_t* arguments)
{
  EzspStatus status;
  EmberKeyData securityKey;
  SecureEzspSecurityType securityType;
  securityType = (SecureEzspSecurityType)sl_cli_get_argument_uint32(arguments, 0);

  status = emberAfGenerateRandomKey(&securityKey);
  if (status != EMBER_SUCCESS) {
    emberAfCorePrint("Random key generation failed, status:0x%x", status);
    assert(0);
  }

  status = emberSecureEzspSetSecurityKey(&securityKey, securityType);

  if (status == EZSP_SUCCESS) {
    emberAfCorePrint("Security Key set { ");
    emberAfCorePrintBuffer(emberKeyContents(&securityKey), EMBER_ENCRYPTION_KEY_SIZE, TRUE);
    emberAfCorePrintln("}");
  } else {
    emberAfCorePrintln("Failed to set Security Key, status:0x%x", status);
  }
}

// plugin secure-ezsp set_security_parameters [securityLevel:4]
void emberAfPluginSecureEzspSetSecurityParametersCommand(sl_cli_command_arg_t* arguments)
{
  EzspStatus status;
  SecureEzspSecurityLevel securityLevel;
  static SecureEzspRandomNumber randomNumber;

  securityLevel = (SecureEzspSecurityLevel)sl_cli_get_argument_uint32(arguments, 0);

  status = emberAfGenerateRandomData(SecureEzspRandomNumberContents(&randomNumber),
                                     SECURE_EZSP_RANDOM_NUMBER_SIZE);

  if (status != EMBER_SUCCESS) {
    emberAfCorePrint("Random data generation failed, status:0x%x", status);
    assert(0);
  }

  status = emberSecureEzspSetSecurityParameters(securityLevel, &randomNumber);

  if (status == EZSP_SUCCESS) {
    emberAfCorePrint("Security Parameters set { ");
    emberAfCorePrintBuffer(randomNumber.contents, SECURE_EZSP_RANDOM_NUMBER_SIZE, TRUE);
    emberAfCorePrintln("}");
  } else {
    emberAfCorePrintln("Failed to set Security Parameters, status:0x%x", status);
  }
}

// plugin secure-ezsp reset_to_factory_defaults
void emberAfPluginSecureEzspResetToFactoryDefaultsCommand(sl_cli_command_arg_t* arguments)
{
  EzspStatus status = emberSecureEzspResetToFactoryDefaults();

  if (status == EZSP_SUCCESS) {
    emberAfCorePrintln("Reset Security");
  } else {
    emberAfCorePrintln("Failed to Reset Security, status:0x%x", status);
  }
}

#else //!UC_BUILD

#ifdef EMBER_AF_LEGACY_CLI
  #error The Secure EZSP plugin is not compatible with the legacy CLI.
#endif // EMBER_AF_LEGACY_CLI

#if defined(EMBER_AF_GENERATE_CLI) || defined(EMBER_AF_API_COMMAND_INTERPRETER2)

// plugin secure-ezsp set_security_key [securityType:4]
void emberAfPluginSecureEzspSetSecurityKeyCommand(void)
{
  EzspStatus status;
  EmberKeyData securityKey;
  SecureEzspSecurityType securityType;
  securityType = (SecureEzspSecurityType)emberUnsignedCommandArgument(0);

  status = emberAfGenerateRandomKey(&securityKey);
  if (status != EMBER_SUCCESS) {
    emberAfCorePrint("Random key generation failed, status:0x%x", status);
    assert(0);
  }

  status = emberSecureEzspSetSecurityKey(&securityKey, securityType);

  if (status == EZSP_SUCCESS) {
    emberAfCorePrint("Security Key set { ");
    emberAfCorePrintBuffer(emberKeyContents(&securityKey), EMBER_ENCRYPTION_KEY_SIZE, TRUE);
    emberAfCorePrintln("}");
  } else {
    emberAfCorePrintln("Failed to set Security Key, status:0x%x", status);
  }
}

// plugin secure-ezsp set_security_parameters [securityLevel:4]
void emberAfPluginSecureEzspSetSecurityParametersCommand(void)
{
  EzspStatus status;
  SecureEzspSecurityLevel securityLevel;
  static SecureEzspRandomNumber randomNumber;

  securityLevel = (SecureEzspSecurityLevel)emberUnsignedCommandArgument(0);

  status = emberAfGenerateRandomData(SecureEzspRandomNumberContents(&randomNumber),
                                     SECURE_EZSP_RANDOM_NUMBER_SIZE);

  if (status != EMBER_SUCCESS) {
    emberAfCorePrint("Random data generation failed, status:0x%x", status);
    assert(0);
  }

  status = emberSecureEzspSetSecurityParameters(securityLevel, &randomNumber);

  if (status == EZSP_SUCCESS) {
    emberAfCorePrint("Security Parameters set { ");
    emberAfCorePrintBuffer(randomNumber.contents, SECURE_EZSP_RANDOM_NUMBER_SIZE, TRUE);
    emberAfCorePrintln("}");
  } else {
    emberAfCorePrintln("Failed to set Security Parameters, status:0x%x", status);
  }
}

// plugin secure-ezsp reset_to_factory_defaults
void emberAfPluginSecureEzspResetToFactoryDefaultsCommand(void)
{
  EzspStatus status = emberSecureEzspResetToFactoryDefaults();

  if (status == EZSP_SUCCESS) {
    emberAfCorePrintln("Reset Security");
  } else {
    emberAfCorePrintln("Failed to Reset Security, status:0x%x", status);
  }
}
#endif // defined(EMBER_AF_GENERATE_CLI) || defined(EMBER_AF_API_COMMAND_INTERPRETER2)
#endif // UC_BUILD

//------------------------------------------------------------------------------
// Callbacks

void emberSecureEzspInitCallback(EzspStatus ncpSecurityKeyStatus,
                                 EzspStatus hostSecurityKeyStatus,
                                 SecureEzspSecurityType ncpSecurityType)
{
  EzspStatus status = EMBER_INVALID_CALL;
  SecureEzspSecurityLevel securityLevel = 5;
  bool newKeySet = false;
  static SecureEzspRandomNumber randomNumber;

  if (ncpSecurityKeyStatus == EZSP_ERROR_SECURITY_KEY_ALREADY_SET
      && hostSecurityKeyStatus == EZSP_ERROR_SECURITY_KEY_NOT_SET) {
    // User should decide what happens in this case
    assert(0);
  }

  if (ncpSecurityKeyStatus == EZSP_ERROR_SECURITY_KEY_NOT_SET) {
#if EMBER_AF_PLUGIN_SECURE_EZSP_SETUP_SECURITY_ON_INIT
    EmberKeyData securityKey;
    status = emberAfGenerateRandomKey(&securityKey);
    if (status != EMBER_SUCCESS) {
      emberAfCorePrint("Random key generation failed, status:0x%x", status);
      assert(0);
    }
  #if EMBER_AF_PLUGIN_SECURE_EZSP_PERMANENT_SECURITY_KEY
    status = emberSecureEzspSetSecurityKey(&securityKey,
                                           SECURE_EZSP_SECURITY_TYPE_PERMANENT);
  #else
    status = emberSecureEzspSetSecurityKey(&securityKey,
                                           SECURE_EZSP_SECURITY_TYPE_TEMPORARY);
  #endif // EMBER_AF_PLUGIN_SECURE_EZSP_PERMANENT_SECURITY_KEY
    if (status == EZSP_SUCCESS) {
      emberAfCorePrint("Security Key set { ");
      emberAfCorePrintBuffer(emberKeyContents(&securityKey), EMBER_ENCRYPTION_KEY_SIZE, TRUE);
      emberAfCorePrintln("}");
      newKeySet = true;
    } else {
      emberAfCorePrintln("Failed to set Security Key, status:0x%x", status);
      return;
    }
#else
    return;
#endif // EMBER_AF_PLUGIN_SECURE_EZSP_SETUP_SECURITY_ON_INIT
  }

  if (ncpSecurityKeyStatus == EZSP_ERROR_SECURITY_KEY_ALREADY_SET
      || newKeySet) {
    emberAfCorePrintln("Security Key Already Set, type:0x%x", ncpSecurityType);
    status = emberAfGenerateRandomData(SecureEzspRandomNumberContents(&randomNumber),
                                       SECURE_EZSP_RANDOM_NUMBER_SIZE);

    if (status != EMBER_SUCCESS) {
      emberAfCorePrint("Random data generation failed, status:0x%x", status);
      assert(0);
    }
    status = emberSecureEzspSetSecurityParameters(securityLevel,
                                                  &randomNumber);
  }

  if (status == EZSP_SUCCESS) {
    emberAfCorePrint("Security Parameters set { ");
    emberAfCorePrintBuffer(randomNumber.contents, SECURE_EZSP_RANDOM_NUMBER_SIZE, TRUE);
    emberAfCorePrintln("}");
  } else {
    emberAfCorePrintln("Failed to set Security Parameters, status:0x%x", status);
  }
}
