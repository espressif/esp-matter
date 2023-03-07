/***************************************************************************//**
 * @file
 * @brief Routines for the Scenes Client plugin, which implements the client
 *        side of the Scenes cluster.
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

#include "../../include/af.h"
#include "scenes-client.h"

#ifdef UC_BUILD

#include "zap-cluster-command-parser.h"

bool emberAfScenesClusterAddSceneResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_add_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_add_scene_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return emberAfPluginScenesClientParseAddSceneResponse(cmd,
                                                        cmd_data.status,
                                                        cmd_data.groupId,
                                                        cmd_data.sceneId);
}

bool emberAfScenesClusterViewSceneResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_view_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_view_scene_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return emberAfPluginScenesClientParseViewSceneResponse(cmd,
                                                         cmd_data.status,
                                                         cmd_data.groupId,
                                                         cmd_data.sceneId,
                                                         cmd_data.transitionTime,
                                                         cmd_data.sceneName,
                                                         cmd_data.extensionFieldSets);
}

bool emberAfScenesClusterRemoveSceneResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_remove_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_remove_scene_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfScenesClusterPrintln("RX: RemoveSceneResponse 0x%x, 0x%2x, 0x%x",
                              cmd_data.status,
                              cmd_data.groupId,
                              cmd_data.sceneId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfScenesClusterRemoveAllScenesResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_remove_all_scenes_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_remove_all_scenes_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfScenesClusterPrintln("RX: RemoveAllScenesResponse 0x%x, 0x%2x",
                              cmd_data.status,
                              cmd_data.groupId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfScenesClusterStoreSceneResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_store_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_store_scene_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfScenesClusterPrintln("RX: StoreSceneResponse 0x%x, 0x%2x, 0x%x",
                              cmd_data.status,
                              cmd_data.groupId,
                              cmd_data.sceneId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfScenesClusterGetSceneMembershipResponseCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_get_scene_membership_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_get_scene_membership_response_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfScenesClusterPrint("RX: GetSceneMembershipResponse 0x%x, 0x%x, 0x%2x",
                            cmd_data.status,
                            cmd_data.capacity,
                            cmd_data.groupId);

  // Scene count and the scene list only appear in the payload if the status is
  // SUCCESS.
  if (cmd_data.status == EMBER_ZCL_STATUS_SUCCESS) {
    uint8_t i;
    emberAfScenesClusterPrint(", 0x%x,", cmd_data.sceneCount);
    for (i = 0; i < cmd_data.sceneCount; i++) {
      emberAfScenesClusterPrint(" [0x%x]", cmd_data.sceneList[i]);
    }
  }

  emberAfScenesClusterPrintln("");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // !UC_BUILD

bool emberAfScenesClusterAddSceneResponseCallback(uint8_t status,
                                                  uint16_t groupId,
                                                  uint8_t sceneId)
{
  return emberAfPluginScenesClientParseAddSceneResponse(emberAfCurrentCommand(),
                                                        status,
                                                        groupId,
                                                        sceneId);
}

bool emberAfScenesClusterViewSceneResponseCallback(uint8_t status,
                                                   uint16_t groupId,
                                                   uint8_t sceneId,
                                                   uint16_t transitionTime,
                                                   uint8_t *sceneName,
                                                   uint8_t *extensionFieldSets)
{
  return emberAfPluginScenesClientParseViewSceneResponse(emberAfCurrentCommand(),
                                                         status,
                                                         groupId,
                                                         sceneId,
                                                         transitionTime,
                                                         sceneName,
                                                         extensionFieldSets);
}

bool emberAfScenesClusterRemoveSceneResponseCallback(uint8_t status,
                                                     uint16_t groupId,
                                                     uint8_t sceneId)
{
  emberAfScenesClusterPrintln("RX: RemoveSceneResponse 0x%x, 0x%2x, 0x%x",
                              status,
                              groupId,
                              sceneId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfScenesClusterRemoveAllScenesResponseCallback(uint8_t status,
                                                         uint16_t groupId)
{
  emberAfScenesClusterPrintln("RX: RemoveAllScenesResponse 0x%x, 0x%2x",
                              status,
                              groupId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfScenesClusterStoreSceneResponseCallback(uint8_t status,
                                                    uint16_t groupId,
                                                    uint8_t sceneId)
{
  emberAfScenesClusterPrintln("RX: StoreSceneResponse 0x%x, 0x%2x, 0x%x",
                              status,
                              groupId,
                              sceneId);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfScenesClusterGetSceneMembershipResponseCallback(uint8_t status,
                                                            uint8_t capacity,
                                                            uint16_t groupId,
                                                            uint8_t sceneCount,
                                                            uint8_t *sceneList)
{
  emberAfScenesClusterPrint("RX: GetSceneMembershipResponse 0x%x, 0x%x, 0x%2x",
                            status,
                            capacity,
                            groupId);

  // Scene count and the scene list only appear in the payload if the status is
  // SUCCESS.
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    uint8_t i;
    emberAfScenesClusterPrint(", 0x%x,", sceneCount);
    for (i = 0; i < sceneCount; i++) {
      emberAfScenesClusterPrint(" [0x%x]", sceneList[i]);
    }
  }

  emberAfScenesClusterPrintln("");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#endif // UC_BUILD

bool emberAfPluginScenesClientParseAddSceneResponse(const EmberAfClusterCommand *cmd,
                                                    uint8_t status,
                                                    uint16_t groupId,
                                                    uint8_t sceneId)
{
  bool enhanced = (cmd->commandId == ZCL_ENHANCED_ADD_SCENE_COMMAND_ID);
  emberAfScenesClusterPrintln("RX: %pAddSceneResponse 0x%x, 0x%2x, 0x%x",
                              (enhanced ? "Enhanced" : ""),
                              status,
                              groupId,
                              sceneId);
  emberAfSendDefaultResponse(cmd, EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPluginScenesClientParseViewSceneResponse(const EmberAfClusterCommand *cmd,
                                                     uint8_t status,
                                                     uint16_t groupId,
                                                     uint8_t sceneId,
                                                     uint16_t transitionTime,
                                                     const uint8_t *sceneName,
                                                     const uint8_t *extensionFieldSets)
{
  bool enhanced = (cmd->commandId == ZCL_ENHANCED_VIEW_SCENE_COMMAND_ID);

  emberAfScenesClusterPrint("RX: %pViewSceneResponse 0x%x, 0x%2x, 0x%x",
                            (enhanced ? "Enhanced" : ""),
                            status,
                            groupId,
                            sceneId);

  // Transition time, scene name, and the extension field sets only appear in
  // the payload if the status is SUCCESS.
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    uint16_t extensionFieldSetsLen = (emberAfCurrentCommand()->bufLen
                                      - (emberAfCurrentCommand()->payloadStartIndex
                                         + sizeof(status)
                                         + sizeof(groupId)
                                         + sizeof(sceneId)
                                         + sizeof(transitionTime)
                                         + emberAfStringLength(sceneName) + 1));
    uint16_t extensionFieldSetsIndex = 0;

    emberAfScenesClusterPrint(", 0x%2x, \"", transitionTime);
    emberAfScenesClusterPrintString(sceneName);
    emberAfScenesClusterPrint("\",");

    // Each extension field set contains at least a two-byte cluster id and a
    // one-byte length.
    while (extensionFieldSetsIndex + 3 <= extensionFieldSetsLen) {
      EmberAfClusterId clusterId;
      uint8_t length;
      clusterId = emberAfGetInt16u(extensionFieldSets, extensionFieldSetsIndex, extensionFieldSetsLen);
      extensionFieldSetsIndex += 2;
      length = emberAfGetInt8u(extensionFieldSets, extensionFieldSetsIndex, extensionFieldSetsLen);
      extensionFieldSetsIndex++;
      emberAfScenesClusterPrint(" [0x%2x 0x%x ", clusterId, length);
      if (extensionFieldSetsIndex + length <= extensionFieldSetsLen) {
        emberAfScenesClusterPrintBuffer(extensionFieldSets + extensionFieldSetsIndex, length, false);
      }
      emberAfScenesClusterPrint("]");
      emberAfScenesClusterFlush();
      extensionFieldSetsIndex += length;
    }
  }

  emberAfScenesClusterPrintln("");
  emberAfSendDefaultResponse(cmd, EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD

uint32_t emberAfScenesClusterClientCommandParse(sl_service_opcode_t opcode,
                                                sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_ADD_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterAddSceneResponseCallback(cmd);
        break;
      }
      case ZCL_GET_SCENE_MEMBERSHIP_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterGetSceneMembershipResponseCallback(cmd);
        break;
      }
      case ZCL_REMOVE_ALL_SCENES_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterRemoveAllScenesResponseCallback(cmd);
        break;
      }
      case ZCL_REMOVE_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterRemoveSceneResponseCallback(cmd);
        break;
      }
      case ZCL_STORE_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterStoreSceneResponseCallback(cmd);
        break;
      }
      case ZCL_VIEW_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterViewSceneResponseCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
