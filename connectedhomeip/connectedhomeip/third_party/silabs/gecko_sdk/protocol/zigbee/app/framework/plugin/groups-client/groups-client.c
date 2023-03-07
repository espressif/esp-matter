/***************************************************************************//**
 * @file
 * @brief Routines for the Groups Client plugin, the client implementation of
 *        the Groups cluster.
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

// *******************************************************************
// * groups-client.c
// *
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "../../include/af.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"

bool emberAfGroupsClusterAddGroupResponseCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_groups_cluster_add_group_response_command_t cmd_data;

  if (zcl_decode_groups_cluster_add_group_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS ) {
    return false;
  }

  emberAfGroupsClusterPrintln("RX: AddGroupResponse 0x%x, 0x%2x",
                              cmd_data.status,
                              cmd_data.groupId);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterViewGroupResponseCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_groups_cluster_view_group_response_command_t cmd_data;

  if (zcl_decode_groups_cluster_view_group_response_command(cmd, &cmd_data) != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfGroupsClusterPrint("RX: ViewGroupResponse 0x%x, 0x%2x, \"",
                            cmd_data.status,
                            cmd_data.groupId);
  emberAfGroupsClusterPrintString(cmd_data.groupName);
  emberAfGroupsClusterPrintln("\"");
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterGetGroupMembershipResponseCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_groups_cluster_get_group_membership_response_command_t cmd_data;
  uint8_t i;

  if (zcl_decode_groups_cluster_get_group_membership_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfGroupsClusterPrint("RX: GetGroupMembershipResponse 0x%x, 0x%x,",
                            cmd_data.capacity,
                            cmd_data.groupCount);
  for (i = 0; i < cmd_data.groupCount; i++) {
    emberAfGroupsClusterPrint(" [0x%2x]",
                              emberAfGetInt16u(cmd_data.groupList + (i << 1), 0, 2));
  }
  emberAfGroupsClusterPrintln("");
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterRemoveGroupResponseCallback(EmberAfClusterCommand *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_groups_cluster_remove_group_response_command_t cmd_data;

  if (zcl_decode_groups_cluster_remove_group_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfGroupsClusterPrintln("RX: RemoveGroupResponse 0x%x, 0x%2x",
                              cmd_data.status,
                              cmd_data.groupId);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // !UC_BUILD

bool emberAfGroupsClusterAddGroupResponseCallback(uint8_t status,
                                                  uint16_t groupId)
{
  emberAfGroupsClusterPrintln("RX: AddGroupResponse 0x%x, 0x%2x",
                              status,
                              groupId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterViewGroupResponseCallback(uint8_t status,
                                                   uint16_t groupId,
                                                   uint8_t* groupName)
{
  emberAfGroupsClusterPrint("RX: ViewGroupResponse 0x%x, 0x%2x, \"",
                            status,
                            groupId);
  emberAfGroupsClusterPrintString(groupName);
  emberAfGroupsClusterPrintln("\"");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterGetGroupMembershipResponseCallback(uint8_t capacity,
                                                            uint8_t groupCount,
                                                            uint8_t* groupList)
{
  uint8_t i;
  emberAfGroupsClusterPrint("RX: GetGroupMembershipResponse 0x%x, 0x%x,",
                            capacity,
                            groupCount);
  for (i = 0; i < groupCount; i++) {
    emberAfGroupsClusterPrint(" [0x%2x]",
                              emberAfGetInt16u(groupList + (i << 1), 0, 2));
  }
  emberAfGroupsClusterPrintln("");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfGroupsClusterRemoveGroupResponseCallback(uint8_t status,
                                                     uint16_t groupId)
{
  emberAfGroupsClusterPrintln("RX: RemoveGroupResponse 0x%x, 0x%2x",
                              status,
                              groupId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#endif // UC_BUILD

#ifdef UC_BUILD

uint32_t emberAfGroupsClusterClientCommandParse(sl_service_opcode_t opcode,
                                                sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_ADD_GROUP_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfGroupsClusterAddGroupResponseCallback(cmd);
        break;
      }
      case ZCL_GET_GROUP_MEMBERSHIP_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfGroupsClusterGetGroupMembershipResponseCallback(cmd);
        break;
      }
      case ZCL_REMOVE_GROUP_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfGroupsClusterRemoveGroupResponseCallback(cmd);
        break;
      }
      case ZCL_VIEW_GROUP_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfGroupsClusterViewGroupResponseCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
