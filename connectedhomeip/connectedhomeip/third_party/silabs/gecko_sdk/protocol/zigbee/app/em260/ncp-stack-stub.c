/***************************************************************************//**
 * @file
 * @brief NCP stack stubs.
 * This is a generic file which provides the STACK stub definitions for NCP
 * if the respective component is not present in application.
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

#include "sl_component_catalog.h"
#include "stack/include/ember-types.h"

#ifndef SL_CATALOG_ZIGBEE_BINDING_TABLE_PRESENT
bool emAfProcessEzspCommandBinding(uint16_t commandId)
{
  (void)commandId;
  return false;
}
#endif  // SL_CATALOG_ZIGBEE_BINDING_TABLE_PRESENT

#ifndef SL_CATALOG_ZIGBEE_LIGHT_LINK_PRESENT
bool emAfProcessEzspCommandZll(uint16_t commandId)
{
  (void)commandId;
  return false;
}
#endif  // SL_CATALOG_ZIGBEE_LIGHT_LINK_PRESENT

#ifndef SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT
bool emAfProcessEzspCommandGreenPower(uint16_t commandId)
{
  (void)commandId;
  return false;
}
#endif

#ifndef SL_CATALOG_ZIGBEE_MFGLIB_PRESENT
bool emAfProcessEzspCommandMfglib(uint16_t commandId)
{
  (void)commandId;
  return false;
}
#endif

#ifndef SL_CATALOG_ZIGBEE_CBKE_CORE_PRESENT
bool emAfProcessEzspCommandCertificateBasedKeyExchangeCbke(uint16_t commandId)
{
  (void)commandId;
  return false;
}
#endif  // SL_CATALOG_ZIGBEE_CBKE_CORE_PRESENT

// ToDo: Change this catalog option to stack_common after EMZIGBEE-7033
// gets merged to migration branch.
#ifndef SL_CATALOG_ZIGBEE_STACK_COMMON_PRESENT
bool emAfProcessEzspCommandMessaging(uint16_t commandId)
{
  (void)commandId;
  return false;
}

bool emAfProcessEzspCommandNetworking(uint16_t commandId)
{
  (void)commandId;
  return false;
}

bool emAfProcessEzspCommandSecurity(uint16_t commandId)
{
  (void)commandId;
  return false;
}

bool emAfProcessEzspCommandTrustCenter(uint16_t commandId)
{
  (void)commandId;
  return false;
}
#endif  // SL_CATALOG_ZIGBEE_STACK_COMMON_PRESENT

// The weak stub for the command-handler when the token interface
// component is not present.
WEAK(bool emAfProcessEzspTokenInterfaceCommands(uint16_t commandId))
{
  (void)commandId;
  return false;
}
