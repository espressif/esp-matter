/***************************************************************************//**
 * @file
 * @brief Routines for the ZLL Scenes Server plugin.
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
#include "../scenes/scenes.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

#define ZCL_SCENES_CLUSTER_MODE_COPY_ALL_SCENES_MASK BIT(0)

#ifdef UC_BUILD

bool emberAfScenesClusterEnhancedAddSceneCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_enhanced_add_scene_command_t cmd_data;

  if (zcl_decode_scenes_cluster_enhanced_add_scene_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return emberAfPluginScenesServerParseAddScene(cmd,
                                                cmd_data.groupId,
                                                cmd_data.sceneId,
                                                cmd_data.transitionTime,
                                                cmd_data.sceneName,
                                                cmd_data.extensionFieldSets);
}

bool emberAfScenesClusterEnhancedViewSceneCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_enhanced_view_scene_command_t cmd_data;

  if (zcl_decode_scenes_cluster_enhanced_view_scene_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return emberAfPluginScenesServerParseViewScene(cmd,
                                                 cmd_data.groupId,
                                                 cmd_data.sceneId);
}

bool emberAfScenesClusterCopySceneCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_scenes_cluster_copy_scene_command_t cmd_data;
  EmberStatus sendStatus;
  EmberAfStatus status = EMBER_ZCL_STATUS_INVALID_FIELD;
  bool copyAllScenes;
  uint8_t i;

  if (zcl_decode_scenes_cluster_copy_scene_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  copyAllScenes = (cmd_data.mode & ZCL_SCENES_CLUSTER_MODE_COPY_ALL_SCENES_MASK);

  emberAfScenesClusterPrintln("RX: CopyScene 0x%x, 0x%2x, 0x%x, 0x%2x, 0x%x",
                              cmd_data.mode,
                              cmd_data.groupIdFrom,
                              cmd_data.sceneIdFrom,
                              cmd_data.groupIdTo,
                              cmd_data.sceneIdTo);

  // If a group id is specified but this endpoint isn't in it, take no action.
  if ((cmd_data.groupIdFrom != ZCL_SCENES_GLOBAL_SCENE_GROUP_ID
       && !emberAfGroupsClusterEndpointInGroupCallback(emberAfCurrentEndpoint(),
                                                       cmd_data.groupIdFrom))
      || (cmd_data.groupIdTo != ZCL_SCENES_GLOBAL_SCENE_GROUP_ID
          && !emberAfGroupsClusterEndpointInGroupCallback(emberAfCurrentEndpoint(),
                                                          cmd_data.groupIdTo))) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
    goto kickout;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_SCENES_TABLE_SIZE; i++) {
    EmberAfSceneTableEntry from;
    emberAfPluginScenesServerRetrieveSceneEntry(from, i);
    if (from.endpoint == emberAfCurrentEndpoint()
        && from.groupId == cmd_data.groupIdFrom
        && (copyAllScenes || from.sceneId == cmd_data.sceneIdFrom)) {
      uint8_t j, index = EMBER_AF_SCENE_TABLE_NULL_INDEX;
      for (j = 0; j < EMBER_AF_PLUGIN_SCENES_TABLE_SIZE; j++) {
        EmberAfSceneTableEntry to;
        if (i == j) {
          continue;
        }
        emberAfPluginScenesServerRetrieveSceneEntry(to, j);
        if (to.endpoint == emberAfCurrentEndpoint()
            && to.groupId == cmd_data.groupIdTo
            && to.sceneId == (copyAllScenes ? from.sceneId : cmd_data.sceneIdTo)) {
          index = j;
          break;
        } else if (index == EMBER_AF_SCENE_TABLE_NULL_INDEX
                   && to.endpoint == EMBER_AF_SCENE_TABLE_UNUSED_ENDPOINT_ID) {
          index = j;
        }
      }

      // If the target index is still zero, the table is full.
      if (index == EMBER_AF_SCENE_TABLE_NULL_INDEX) {
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
        goto kickout;
      }

      // Save the "from" entry to the "to" index.  This makes a copy of "from"
      // with the correct group and scene ids and leaves the original in tact.
      from.groupId = cmd_data.groupIdTo;
      if (!copyAllScenes) {
        from.sceneId = cmd_data.sceneIdTo;
      }
      emberAfPluginScenesServerSaveSceneEntry(from, index);

      if (j != index) {
        emberAfPluginScenesServerIncrNumSceneEntriesInUse();
        emberAfScenesSetSceneCountAttribute(emberAfCurrentEndpoint(),
                                            emberAfPluginScenesServerNumSceneEntriesInUse());
      }

      // If we aren't copying all scenes, we can stop here.
      status = EMBER_ZCL_STATUS_SUCCESS;
      if (!copyAllScenes) {
        goto kickout;
      }
    }
  }

  kickout:
  // Copy Scene commands are only responded to when they are addressed to a
  // single device.
  if (emberAfCurrentCommand()->type == EMBER_INCOMING_UNICAST
      || emberAfCurrentCommand()->type == EMBER_INCOMING_UNICAST_REPLY) {
    emberAfFillCommandScenesClusterCopySceneResponse(status,
                                                     cmd_data.groupIdFrom,
                                                     cmd_data.sceneIdFrom);
    sendStatus = emberAfSendResponse();
    if (EMBER_SUCCESS != sendStatus) {
      emberAfScenesClusterPrintln("Scenes: failed to send %s response: 0x%x",
                                  "copy_scene",
                                  sendStatus);
    }
  }
  return true;
}

#else // !UC_BUILD

bool emberAfScenesClusterEnhancedAddSceneCallback(uint16_t groupId,
                                                  uint8_t sceneId,
                                                  uint16_t transitionTime,
                                                  uint8_t *sceneName,
                                                  uint8_t *extensionFieldSets)
{
  return emberAfPluginScenesServerParseAddScene(emberAfCurrentCommand(),
                                                groupId,
                                                sceneId,
                                                transitionTime,
                                                sceneName,
                                                extensionFieldSets);
}

bool emberAfScenesClusterEnhancedViewSceneCallback(uint16_t groupId,
                                                   uint8_t sceneId)
{
  return emberAfPluginScenesServerParseViewScene(emberAfCurrentCommand(),
                                                 groupId,
                                                 sceneId);
}

bool emberAfScenesClusterCopySceneCallback(uint8_t mode,
                                           uint16_t groupIdFrom,
                                           uint8_t sceneIdFrom,
                                           uint16_t groupIdTo,
                                           uint8_t sceneIdTo)
{
  EmberStatus sendStatus;
  EmberAfStatus status = EMBER_ZCL_STATUS_INVALID_FIELD;
  bool copyAllScenes = (mode & ZCL_SCENES_CLUSTER_MODE_COPY_ALL_SCENES_MASK);
  uint8_t i;

  emberAfScenesClusterPrintln("RX: CopyScene 0x%x, 0x%2x, 0x%x, 0x%2x, 0x%x",
                              mode,
                              groupIdFrom,
                              sceneIdFrom,
                              groupIdTo,
                              sceneIdTo);

  // If a group id is specified but this endpoint isn't in it, take no action.
  if ((groupIdFrom != ZCL_SCENES_GLOBAL_SCENE_GROUP_ID
       && !emberAfGroupsClusterEndpointInGroupCallback(emberAfCurrentEndpoint(),
                                                       groupIdFrom))
      || (groupIdTo != ZCL_SCENES_GLOBAL_SCENE_GROUP_ID
          && !emberAfGroupsClusterEndpointInGroupCallback(emberAfCurrentEndpoint(),
                                                          groupIdTo))) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
    goto kickout;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_SCENES_TABLE_SIZE; i++) {
    EmberAfSceneTableEntry from;
    emberAfPluginScenesServerRetrieveSceneEntry(from, i);
    if (from.endpoint == emberAfCurrentEndpoint()
        && from.groupId == groupIdFrom
        && (copyAllScenes || from.sceneId == sceneIdFrom)) {
      uint8_t j, index = EMBER_AF_SCENE_TABLE_NULL_INDEX;
      for (j = 0; j < EMBER_AF_PLUGIN_SCENES_TABLE_SIZE; j++) {
        EmberAfSceneTableEntry to;
        if (i == j) {
          continue;
        }
        emberAfPluginScenesServerRetrieveSceneEntry(to, j);
        if (to.endpoint == emberAfCurrentEndpoint()
            && to.groupId == groupIdTo
            && to.sceneId == (copyAllScenes ? from.sceneId : sceneIdTo)) {
          index = j;
          break;
        } else if (index == EMBER_AF_SCENE_TABLE_NULL_INDEX
                   && to.endpoint == EMBER_AF_SCENE_TABLE_UNUSED_ENDPOINT_ID) {
          index = j;
        }
      }

      // If the target index is still zero, the table is full.
      if (index == EMBER_AF_SCENE_TABLE_NULL_INDEX) {
        status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
        goto kickout;
      }

      // Save the "from" entry to the "to" index.  This makes a copy of "from"
      // with the correct group and scene ids and leaves the original in tact.
      from.groupId = groupIdTo;
      if (!copyAllScenes) {
        from.sceneId = sceneIdTo;
      }
      emberAfPluginScenesServerSaveSceneEntry(from, index);

      if (j != index) {
        emberAfPluginScenesServerIncrNumSceneEntriesInUse();
        emberAfScenesSetSceneCountAttribute(emberAfCurrentEndpoint(),
                                            emberAfPluginScenesServerNumSceneEntriesInUse());
      }

      // If we aren't copying all scenes, we can stop here.
      status = EMBER_ZCL_STATUS_SUCCESS;
      if (!copyAllScenes) {
        goto kickout;
      }
    }
  }

  kickout:
  // Copy Scene commands are only responded to when they are addressed to a
  // single device.
  if (emberAfCurrentCommand()->type == EMBER_INCOMING_UNICAST
      || emberAfCurrentCommand()->type == EMBER_INCOMING_UNICAST_REPLY) {
    emberAfFillCommandScenesClusterCopySceneResponse(status,
                                                     groupIdFrom,
                                                     sceneIdFrom);
    sendStatus = emberAfSendResponse();
    if (EMBER_SUCCESS != sendStatus) {
      emberAfScenesClusterPrintln("Scenes: failed to send %s response: 0x%x",
                                  "copy_scene",
                                  sendStatus);
    }
  }
  return true;
}

#endif // UC_BUILD

EmberAfStatus emberAfPluginZllScenesServerRecallSceneZllExtensions(uint8_t endpoint)
{
  bool globalSceneControl = true;
  EmberAfStatus status = emberAfWriteServerAttribute(endpoint,
                                                     ZCL_ON_OFF_CLUSTER_ID,
                                                     ZCL_GLOBAL_SCENE_CONTROL_ATTRIBUTE_ID,
                                                     (uint8_t *)&globalSceneControl,
                                                     ZCL_BOOLEAN_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfScenesClusterPrintln("ERR: writing global scene control %x", status);
  }
  return status;
}

#ifdef UC_BUILD

uint32_t emberAfZllScenesClusterServerCommandParse(sl_service_opcode_t opcode,
                                                   sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_COPY_SCENE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterCopySceneCallback(cmd);
        break;
      }
      case ZCL_ENHANCED_ADD_SCENE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterEnhancedAddSceneCallback(cmd);
        break;
      }
      case ZCL_ENHANCED_VIEW_SCENE_COMMAND_ID:
      {
        wasHandled = emberAfScenesClusterEnhancedViewSceneCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
