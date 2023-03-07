/***************************************************************************//**
 * @file
 * @brief Routines for the ZLL Level Control Server plugin.
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
#include "zll-level-control-server.h"

EmberAfStatus emberAfPluginZllLevelControlServerMoveToLevelWithOnOffZllExtensions(const EmberAfClusterCommand *cmd)
{
  bool globalSceneControl = true;
  EmberAfStatus status = emberAfWriteServerAttribute(cmd->apsFrame->destinationEndpoint,
                                                     ZCL_ON_OFF_CLUSTER_ID,
                                                     ZCL_GLOBAL_SCENE_CONTROL_ATTRIBUTE_ID,
                                                     (uint8_t *)&globalSceneControl,
                                                     ZCL_BOOLEAN_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfLevelControlClusterPrintln("ERR: writing global scene control %x", status);
  }
  return status;
}

bool emberAfPluginZllLevelControlServerIgnoreMoveToLevelMoveStepStop(uint8_t endpoint, uint8_t commandId)
{
  // If a MoveToLevel, Move, Step, or Stop command is received while the device
  // is in its off state (i.e., the OnOff attribute of the On/Off cluster is
  // equal to false), the command shall be ignored.
  bool onOff = true;
#ifdef ZCL_USING_ON_OFF_CLUSTER_SERVER
  if ((commandId == ZCL_MOVE_TO_LEVEL_COMMAND_ID
       || commandId == ZCL_MOVE_COMMAND_ID
       || commandId == ZCL_STEP_COMMAND_ID
       || commandId == ZCL_STOP_COMMAND_ID)
      && emberAfContainsServer(endpoint, ZCL_ON_OFF_CLUSTER_ID)) {
    EmberAfStatus status = emberAfReadServerAttribute(endpoint,
                                                      ZCL_ON_OFF_CLUSTER_ID,
                                                      ZCL_ON_OFF_ATTRIBUTE_ID,
                                                      (uint8_t *)&onOff,
                                                      sizeof(onOff));
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfLevelControlClusterPrintln("ERR: reading on/off %x", status);
    }
  }
#endif
  return !onOff;
}
