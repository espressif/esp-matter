/***************************************************************************//**
 * @file
 * @brief This file contains a function that processes cluster-specific
 * ZCL message.
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

// this file contains all the common includes for clusters in the zcl-util
#include "common.h"

// for pulling in defines dealing with EITHER server or client
#include "af-main.h"

// the EM260 host needs to include the config file
#ifdef EZSP_HOST
  #include "config.h"
#endif

//------------------------------------------------------------------------------

#ifdef UC_BUILD

#include "sl_malloc.h"

EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand *cmd)
{
  EmberAfStatus zcl_status = EMBER_ZCL_STATUS_UNSUP_COMMAND;
  sl_service_function_entry_t *service_entry = sl_service_function_get_first_entry();
  sl_service_function_context_t service_context;
  service_context.data = (void*)cmd;

  while (service_entry != NULL) {
    // Call the service function if:
    // - the service function is a ZCL_COMMAND service function
    // - the cluster ID matches
    // - if the manufacturer ID is present, the manufacturer ID matches
    // - the command direction matches
    if (service_entry->type == SL_SERVICE_FUNCTION_TYPE_ZCL_COMMAND
        && service_entry->key == cmd->apsFrame->clusterId
        && (service_entry->subkey & 0xFFFF) == (cmd->mfgSpecific ? cmd->mfgCode : 0xFFFF)
        && (service_entry->subkey >> 16) == cmd->direction) {
      zcl_status = (EmberAfStatus)(service_entry->function)(SL_SERVICE_FUNCTION_TYPE_ZCL_COMMAND, &service_context);

      // Keep going through the list if the service function returned a
      // non-success ZCL status.
      if (zcl_status == EMBER_ZCL_STATUS_SUCCESS) {
        break;
      }
    }
    service_entry = sl_service_function_get_next_entry(service_entry);
  }

  return zcl_status;
}

bool emAfProcessClusterSpecificCommand(EmberAfClusterCommand *cmd)
{
  EmberAfStatus zcl_status = EMBER_ZCL_STATUS_UNSUPPORTED_CLUSTER;

  // if we are disabled then we can only respond to read or write commands
  // or identify cluster (see device enabled attr of basic cluster)
  if (!emberAfIsDeviceEnabled(cmd->apsFrame->destinationEndpoint)
      && cmd->apsFrame->clusterId != ZCL_IDENTIFY_CLUSTER_ID) {
    emberAfCorePrintln("%pd, dropping ep 0x%x clus 0x%2x cmd 0x%x",
                       "disable",
                       cmd->apsFrame->destinationEndpoint,
                       cmd->apsFrame->clusterId,
                       cmd->commandId);
    emberAfSendDefaultResponse(cmd, EMBER_ZCL_STATUS_FAILURE);
    return true;
  }

  if ((cmd->direction == (uint8_t)ZCL_DIRECTION_SERVER_TO_CLIENT
       && emberAfContainsClientWithMfgCode(cmd->apsFrame->destinationEndpoint,
                                           cmd->apsFrame->clusterId,
                                           cmd->mfgCode))
      || (cmd->direction == (uint8_t)ZCL_DIRECTION_CLIENT_TO_SERVER
          && emberAfContainsServerWithMfgCode(cmd->apsFrame->destinationEndpoint,
                                              cmd->apsFrame->clusterId,
                                              cmd->mfgCode))) {
    zcl_status = emberAfClusterSpecificCommandParse(cmd);
  }

  if (zcl_status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendDefaultResponse(cmd, zcl_status);
  }

  return true;
}

sl_status_t sl_zigbee_subscribe_to_zcl_commands(uint16_t cluster_id,
                                                uint16_t manufacturer_id,
                                                uint8_t direction,
                                                sl_service_function_t service_function)
{
  sl_service_function_block_t *block;
  sl_service_function_entry_t *entry;

  if (service_function == NULL
      || (direction != ZCL_DIRECTION_CLIENT_TO_SERVER
          && direction != ZCL_DIRECTION_SERVER_TO_CLIENT)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  block = (sl_service_function_block_t*)sl_malloc(sizeof(sl_service_function_block_t));

  if (block == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  entry = (sl_service_function_entry_t*)sl_malloc(sizeof(sl_service_function_entry_t));

  if (entry == NULL) {
    sl_free(block);
    return SL_STATUS_ALLOCATION_FAILED;
  }

  block->count = 1;
  block->entries = entry;

  entry->type = SL_SERVICE_FUNCTION_TYPE_ZCL_COMMAND;
  entry->function = service_function;
  entry->key = cluster_id;
  // Set the manufacturer ID in the 2 least significant bytes of the subkey.
  entry->subkey = manufacturer_id;

  // Write the direction in the 2 most significant bytes of the subkey
  entry->subkey |= (direction << 16);

  sl_service_function_register_block(block);

  return SL_STATUS_OK;
}

#else // !UC_BUILD

extern EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand *cmd);

bool emAfProcessClusterSpecificCommand(EmberAfClusterCommand *cmd)
{
  EmberAfStatus status;

  // if we are disabled then we can only respond to read or write commands
  // or identify cluster (see device enabled attr of basic cluster)
  if (!emberAfIsDeviceEnabled(cmd->apsFrame->destinationEndpoint)
      && cmd->apsFrame->clusterId != ZCL_IDENTIFY_CLUSTER_ID) {
    emberAfCorePrintln("%pd, dropping ep 0x%x clus 0x%2x cmd 0x%x",
                       "disable",
                       cmd->apsFrame->destinationEndpoint,
                       cmd->apsFrame->clusterId,
                       cmd->commandId);
    emberAfSendDefaultResponse(cmd, EMBER_ZCL_STATUS_FAILURE);
    return true;
  }

#ifdef ZCL_USING_KEY_ESTABLISHMENT_CLUSTER_CLIENT
  if (cmd->apsFrame->clusterId == ZCL_KEY_ESTABLISHMENT_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_SERVER_TO_CLIENT
      && emberAfKeyEstablishmentClusterClientCommandReceivedCallback(cmd)) {
    return true;
  }
#endif
#ifdef ZCL_USING_KEY_ESTABLISHMENT_CLUSTER_SERVER
  if (cmd->apsFrame->clusterId == ZCL_KEY_ESTABLISHMENT_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER
      && emberAfKeyEstablishmentClusterServerCommandReceivedCallback(cmd)) {
    return true;
  }
#endif

#ifdef ZCL_USING_OTA_BOOTLOAD_CLUSTER_CLIENT
  if (cmd->apsFrame->clusterId == ZCL_OTA_BOOTLOAD_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_SERVER_TO_CLIENT
      && emberAfOtaClientIncomingMessageRawCallback(cmd)) {
    return true;
  }
#endif
#ifdef ZCL_USING_OTA_BOOTLOAD_CLUSTER_SERVER
  if (cmd->apsFrame->clusterId == ZCL_OTA_BOOTLOAD_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER
      && emberAfOtaServerIncomingMessageRawCallback(cmd)) {
    return true;
  }
#endif

  // Pass the command to the generated command parser for processing
  status = emberAfClusterSpecificCommandParse(cmd);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendDefaultResponse(cmd, status);
  }

  return true;
}

#endif // UC_BUILD
