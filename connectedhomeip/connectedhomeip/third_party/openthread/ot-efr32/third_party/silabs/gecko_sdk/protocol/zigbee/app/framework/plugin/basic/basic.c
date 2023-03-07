/***************************************************************************//**
 * @file
 * @brief Implementation for the Basic Server Cluster plugin.
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

#include "af.h"
#include "basic.h"
#include "app/framework/util/attribute-storage.h"
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_REPORTING_PRESENT
#include "reporting.h"
#endif // SL_CATALOG_ZIGBEE_REPORTING_PRESENT
#else // !SL_COMPONENT_CATALOG_PRESENT
#ifdef EMBER_AF_PLUGIN_REPORTING
#include "app/framework/plugin/reporting/reporting.h"
#define SL_CATALOG_ZIGBEE_REPORTING_PRESENT
#endif // EMBER_AF_PLUGIN_REPORTING
#endif // SL_COMPONENT_CATALOG_PRESENT

bool emberAfBasicClusterResetToFactoryDefaultsCallback(void)
{
  emberAfBasicClusterPrintln("RX: ResetToFactoryDefaultsCallback");
  emberAfResetAttributes(emberAfCurrentEndpoint());
  emberAfPluginBasicResetToFactoryDefaultsCallback(emberAfCurrentEndpoint());
#ifdef SL_CATALOG_ZIGBEE_REPORTING_PRESENT
  emAfPluginReportingGetLastValueAll();
#endif // SL_CATALOG_ZIGBEE_REPORTING_PRESENT
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD

uint32_t emberAfBasicClusterServerCommandParse(sl_service_opcode_t opcode,
                                               sl_service_function_context_t *context)
{
  (void)opcode;
  bool wasHandled = false;
  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  if (!cmd->mfgSpecific && cmd->commandId == ZCL_RESET_TO_FACTORY_DEFAULTS_COMMAND_ID) {
    wasHandled = emberAfBasicClusterResetToFactoryDefaultsCallback();
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
