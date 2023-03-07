/***************************************************************************//**
 * @file
 * @brief Routines for the Green Power Server GPD command Cluster and Device Id maps.
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

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "green-power-common.h"
#include "green-power-server.h"
#include "green-power-server-config.h"
// Green Power Server configuration for user having the translation table
#if (EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE == 1)
#define USER_HAS_DEFAULT_TRANSLATION_TABLE
#endif //EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE

// The green power server is capable of a pre-defined default translation and
// forwarding of GPDF commands without the support of the translation table
// component. This header is needed to provide some of the data structure
// definitions when in absence of the translation table component. Hence
// added without any condition.
#include "green-power-translation-table.h"
#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#include "green-power-translation-table-config.h"
// Green Power Translation Table configuration for user having the translation table
#if (EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_TRANSLATION_TABLE == 1)
#define USER_HAS_DEFAULT_TRANSLATION_TABLE
#endif // EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_TRANSLATION_TABLE
#define TRANSLATION_TABLE_PRESENT
#endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT

// super/protocol/zigbee/app/framework/util/util.h for the emberAfAppendToExternalBuffer
#include "util.h"

#else // !UC_BUILD

#include EMBER_AF_API_GREEN_POWER_COMMON
#include EMBER_AF_API_GREEN_POWER_SERVER

// If either green power server plugin or translation table plugin sets the user translation
// table option, then define the local flag to be used.
#if (defined(EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE) \
  || defined(EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_USER_HAS_DEFAULT_TRANSLATION_TABLE))
#define USER_HAS_DEFAULT_TRANSLATION_TABLE
#endif //EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE

#ifdef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE
#include "app/framework/plugin/green-power-translation-table/green-power-translation-table.h"
#define TRANSLATION_TABLE_PRESENT
#endif

#endif // UC_BUILD

// This file has three map tables
// 1. Default translation table, maps the gpd command with a zigbee cluster command. This is called as a default or
//    generic translation table. This is used to forward GP operational commands to an application endpoint with or without
//    use of Translation Table plugin.
//    The macro EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE allows the user to define this table
//    if needed.
//
// 2. Cluster mapping from Device Id. In GP spec, the device Id specefies certain clusters to be included mandatorily.
//    During GPD commissioning (with only device Id is present and no other information included in commissioning frame)
//    the functionality matching is done by looking up the device Id, finding the cluster and then matching that with
//    Sink application end point that is in commissioning.
//
// 3. Command mapping from Device Id. In GP spec, the device Id also means certain commands to be included by default.
//    This table is used to look up the commands list from device Id while commissioning.
//
// Example of the sink functionality matching is based on the following flow when just device Id and no other infomation is
// present in the commissioning.
// Device Id -> command Id list -> Cluster list
// Device Id --------------------> Cluster List
//                                             |--> Cluster list matching with Sink application endpoint that
//                                                  is currently commissioning.

#ifndef USER_HAS_DEFAULT_TRANSLATION_TABLE
const EmberAfGreenPowerServerGpdSubTranslationTableEntry emberGpDefaultTranslationTable[] =
{
  { true, EMBER_ZCL_GP_GPDF_IDENTIFY, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_IDENTIFY_CLUSTER_ID, 1, ZCL_IDENTIFY_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x02, 0x00, 0x3C } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE0, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x00 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE1, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x01 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x02 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE3, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x03 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE4, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x04 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE5, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x05 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE6, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x06 } },
  { true, EMBER_ZCL_GP_GPDF_RECALL_SCENE7, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_RECALL_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x07 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE0, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x00 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE1, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x01 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x02 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE3, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x03 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE4, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x04 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE5, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x05 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE6, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x06 } },
  { true, EMBER_ZCL_GP_GPDF_STORE_SCENE7, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_SCENES_CLUSTER_ID, 1, ZCL_STORE_SCENE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x03, 0xFF, 0xFF, 0x07 } },
  { true, EMBER_ZCL_GP_GPDF_OFF, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_ON, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_ON_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_TOGGLE, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_TOGGLE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_RELEASE, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_STOP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_UP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_DOWN, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_STEP_UP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_STEP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_STEP_DOWN, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_STEP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_LEVEL_CONTROL_STOP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_STOP_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_UP_WITH_ON_OFF, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_DOWN_WITH_ON_OFF, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_MOVE_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_STEP_UP_WITH_ON_OFF, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_STEP_DOWN_WITH_ON_OFF, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_LEVEL_CONTROL_CLUSTER_ID, 1, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_STEP_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_HUE_STOP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x02, EMBER_ZCL_HUE_MOVE_MODE_STOP, 0xFF } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_HUE_UP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_MOVE_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_HUE_DOWN, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_MOVE_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_STEP_HUE_UP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_STEP_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_STEP_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_STEP_HUE_DOWN, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_STEP_HUE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_HUE_STEP_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_SATURATION_STOP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED, { 0x01, EMBER_ZCL_SATURATION_MOVE_MODE_STOP } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_SATURATION_UP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_MOVE_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_SATURATION_DOWN, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_MOVE_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_MOVE_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_STEP_SATURATION_UP, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_STEP_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_STEP_MODE_UP } },
  { true, EMBER_ZCL_GP_GPDF_STEP_SATURATION_DOWN, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_STEP_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED | EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0x01, EMBER_ZCL_SATURATION_STEP_MODE_DOWN } },
  { true, EMBER_ZCL_GP_GPDF_MOVE_COLOR, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_STEP_SATURATION_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_STEP_COLOR, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_COLOR_CONTROL_CLUSTER_ID, 1, ZCL_STEP_COLOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_LOCK_DOOR, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_DOOR_LOCK_CLUSTER_ID, 1, ZCL_LOCK_DOOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_UNLOCK_DOOR, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_DOOR_LOCK_CLUSTER_ID, 1, ZCL_UNLOCK_DOOR_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },

  // User configurable as the command does not have a ZCL mapping
  { true, EMBER_ZCL_GP_GPDF_PRESS1_OF1, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_ON_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_RELEASE1_OF1, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_SHORT_PRESS1_OF1, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_TOGGLE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_PRESS1_OF2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_ON_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_RELEASE1_OF2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_PRESS2_OF2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_ON_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_RELEASE2_OF2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_OFF_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_SHORT_PRESS1_OF2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_TOGGLE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },
  { true, EMBER_ZCL_GP_GPDF_SHORT_PRESS2_OF2, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, ZCL_ON_OFF_CLUSTER_ID, 1, ZCL_TOGGLE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA, { 0 } },

  // Gereral ZCL command mapping - for which the cluster should be present in
  // gpd commissioning session to validate if it can be supported by sink
  { true, EMBER_ZCL_GP_GPDF_REQUEST_ATTRIBUTE, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, 0xFFFF, 1, ZCL_READ_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0xFF } },
  { true, EMBER_ZCL_GP_GPDF_READ_ATTR_RESPONSE, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, 0xFFFF, 0, ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0xFF } },
  { true, EMBER_ZCL_GP_GPDF_ZCL_TUNNELING_WITH_PAYLOAD, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, 0xFFFF, 0, 0xFF, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0xFF } },
  { true, EMBER_ZCL_GP_GPDF_ANY_GPD_SENSOR_CMD, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, 0xFFFF, 0, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0xFF } },

// Following command is only available if CAR/MS is implemenetd, which requires translation table support.
#ifdef TRANSLATION_TABLE_PRESENT
  // for compact attribute report, if ZbEndpoint=0xFC then ZbClu + ZbCmd + ZbPayload have No Meaning
  { true, EMBER_ZCL_GP_GPDF_COMPACT_ATTRIBUTE_REPORTING, EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT, HA_PROFILE_ID, 0xFFFF, 0, ZCL_REPORT_ATTRIBUTES_COMMAND_ID, EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD, { 0xFE } },
#endif
};

uint16_t emGpDefaultTableSize = (sizeof(emberGpDefaultTranslationTable) / sizeof(EmberAfGreenPowerServerGpdSubTranslationTableEntry));

static uint16_t defaultTableGpdCmdIndex(uint16_t startIndex, uint8_t gpdCommandId)
{
  for (int i = startIndex; i < emGpDefaultTableSize; i++) {
    if (gpdCommandId == emberGpDefaultTranslationTable[i].gpdCommand) {
      return i;
    }
  }
  return 0xFFFF;
}
// Simply builds a list of all the gpd cluster supported by the standard deviceId
// consumed by formGpdClusterListFromIncommingCommReq
bool emGetClusterListFromCmdIdLookup(uint8_t gpdCommandId,
                                     ZigbeeCluster *gpdCluster)
{
  if (gpdCluster != NULL) {
    uint16_t index = defaultTableGpdCmdIndex(0, gpdCommandId);
    if (0xFFFF != index) {
      gpdCluster->clusterId = emberGpDefaultTranslationTable[index].zigbeeCluster;
      gpdCluster->serverClient = emberGpDefaultTranslationTable[index].serverClient;
      return true;
    }
  }
  return false;
}
#endif

static void fillExternalBufferGpdCommandA6Payload(uint8_t *gpdCommandPayload)
{
  // A6 - GPD ZCL Tunneling gpdCommandPayload format :
  //  -------------------------------------------------------------------------------------------------------------
  // | Total Length(1) | Option(1) | Manf Id (0/2) | ClusterId(2) | ZCommand(1) | Length(1) | Payload(0/Variable) |
  //  -------------------------------------------------------------------------------------------------------------
  // total payload length is given in gpdCommandPayload[0]
  uint8_t frameControl = gpdCommandPayload[1];
  uint8_t *payload = &gpdCommandPayload[2];

  uint16_t manufacturerCode = EMBER_AF_NULL_MANUFACTURER_CODE;
  if (frameControl & ZCL_MANUFACTURER_SPECIFIC_MASK) {
    manufacturerCode = ((*(payload + 1)) << 8) + (*payload);
    payload += 2;
  }
  EmberAfClusterId clusterId = ((*(payload + 1)) << 8) + (*payload);
  payload += 2;
  uint8_t zigbeeCommand = *payload;
  payload += 1;
  emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                clusterId,
                                                manufacturerCode,
                                                zigbeeCommand,
                                                "");
  uint8_t length = *payload;
  payload += 1;
  emberAfAppendToExternalBuffer(payload, length);
}

static void fillExternalBufferGpdCommandA0A1Payload(uint8_t gpdCommandId,
                                                    uint8_t *gpdCommandPayload,
                                                    uint8_t zigbeeCommandId,
                                                    bool direction)
{
  // total payload length is given in gpdCommandPayload[0]
  uint8_t *payload = &gpdCommandPayload[1];
  uint8_t frameControl = (ZCL_GLOBAL_COMMAND                                      \
                          | (direction ? ZCL_FRAME_CONTROL_CLIENT_TO_SERVER       \
                             : ZCL_FRAME_CONTROL_SERVER_TO_CLIENT)                \
                          | ((gpdCommandId == EMBER_ZCL_GP_GPDF_MFR_SP_ATTR_RPTG) \
                             ? ZCL_MANUFACTURER_SPECIFIC_MASK : 0));

  uint16_t manufacturerCode = EMBER_AF_NULL_MANUFACTURER_CODE;
  if (frameControl & ZCL_MANUFACTURER_SPECIFIC_MASK) {
    manufacturerCode = ((*(payload + 1)) << 8) + (*payload);
    payload += 2;
  }
  EmberAfClusterId clusterId = ((*(payload + 1)) << 8) + (*payload);
  payload += 2;
  uint8_t length = (payload - (&(gpdCommandPayload[1])));
  emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                clusterId,
                                                manufacturerCode,
                                                zigbeeCommandId,
                                                "");
  // Remaining payload to copy = gpdCommandPayload[0] - what ever populated
  emberAfAppendToExternalBuffer(payload, (gpdCommandPayload[0] - length));
}

static EmberStatus handleA2A3MultiClusterReportForwarding(uint8_t gpdCommandId,
                                                          uint8_t *gpdCommandPayload,
                                                          uint8_t zigbeeCommandId,
                                                          bool direction,
                                                          uint8_t zbEndpoint)
{
  // To handle the Multi Cluster Attribute reporting, as there is no equivalent ZCL command
  // it needed to be broken down to individual packets as A0/A1 and sent.
  // There are two approaches, break down and send in a loop here OR store it and
  // send it with help of a timer.
  uint8_t *payloadFinger = &gpdCommandPayload[1];
  uint8_t clusterReport[30];
  uint8_t startIndex = 1;
  EmberStatus status;

  if (gpdCommandId == EMBER_ZCL_GP_GPDF_MFR_SP_MULTI_CLUSTER_RPTG) {
    MEMCOPY(&clusterReport[startIndex], &gpdCommandPayload[1], 2); // ManufactureId copied
    payloadFinger += 2; // source pointer incremented
    startIndex += 2;     // Destination pointer incremented
  }
  // Copy cluster Report fields for each cluster in incoming payload
  do {
    uint8_t destIndex = startIndex;
    MEMCOPY(&clusterReport[destIndex], payloadFinger, 4); // ClusterId + Attribute Id copied
    payloadFinger += 4; // source pointer incremented for copied cluster and attribute Id
    destIndex += 4;     // Dest pointer Incremented for copied cluster and attribute Id
    clusterReport[destIndex] = *payloadFinger; //AttributeDataType copied
    uint8_t dataSize = emberAfGetDataSize(*payloadFinger); // get the dataSize with the dataType
    payloadFinger += 1; // source pointer incremented for dataType
    destIndex += 1;     // Dest pointer Incremented for dataType
    MEMCOPY(&clusterReport[destIndex], payloadFinger, dataSize); // Attribuetdata copied
    payloadFinger += dataSize; // source pointer incremented for copied data
    destIndex += dataSize;     // Dest pointer incremented for copied data
    clusterReport[0] = (destIndex - 1);  // finally copy the length in index 0;

    fillExternalBufferGpdCommandA0A1Payload((gpdCommandId - 2), // A0/A1 is exact replica of A2/A3 respectively
                                            clusterReport,
                                            zigbeeCommandId,
                                            direction);
    emberAfSetCommandEndpoints(zbEndpoint,
                               zbEndpoint);
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberAfGetNodeId());
    if (status != EMBER_SUCCESS) {
      return status;
    }
  } while ((payloadFinger - (&gpdCommandPayload[1])) < gpdCommandPayload[0]);
  return status;
}

static void fillExternalBufferGpdCommandScenesPayload(EmberGpAddress *gpdAddr,
                                                      uint8_t gpdCommandId,
                                                      EmberAfGreenPowerServerGpdSubTranslationTableEntry const *genericTranslationTable,
                                                      uint8_t endpoint)
{
  //If the sink implements the Translation Table, it derives the groupId as alias
  uint16_t groupId = ((uint16_t)genericTranslationTable->zclPayloadDefault[2] << 8) + genericTranslationTable->zclPayloadDefault[1];
  if (groupId == 0xFFFF) {
    groupId = emGpdAlias(gpdAddr);
  }
  // General command filling.
  (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND             \
                                    | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),   \
                                   genericTranslationTable->zigbeeCluster,   \
                                   genericTranslationTable->zigbeeCommandId, \
                                   "");
  emberAfAppendToExternalBuffer((uint8_t *)(&groupId), sizeof(groupId));
  emberAfAppendToExternalBuffer((uint8_t *)(&genericTranslationTable->zclPayloadDefault[3]), sizeof(uint8_t));

  if (gpdCommandId & EMBER_ZCL_GP_GPDF_STORE_SCENE0) {
    // Store Scenes commands from 0x18 to 0x1F
    // Add the endpoint to the group in the store
    EmberAfStatus status = emGpAddToApsGroup(endpoint, groupId);
    emberAfCorePrintln("Store Scene : Added endpoint %d to Group %d Status = %d", endpoint, groupId, status);
  } else {
    // Recall Scenes commands from 0x10 to 0x17 - the payload is already ready
  }
}

EmberStatus emGpForwardGpdToMappedEndpoint(EmberGpAddress *addr,
                                           uint8_t gpdCommandId,
                                           uint8_t *gpdCommandPayload,
                                           const EmberAfGreenPowerServerGpdSubTranslationTableEntry *genericTranslationTable,
                                           uint8_t endpoint)
{
  // Start to fill and send the commands
  if (gpdCommandId == EMBER_ZCL_GP_GPDF_MULTI_CLUSTER_RPTG
      || gpdCommandId == EMBER_ZCL_GP_GPDF_MFR_SP_MULTI_CLUSTER_RPTG) {
    // This will fill up and send out multiple UCAST to the dest app endpoint.
    return handleA2A3MultiClusterReportForwarding(gpdCommandId,
                                                  gpdCommandPayload,
                                                  genericTranslationTable->zigbeeCommandId,
                                                  (bool)genericTranslationTable->serverClient,
                                                  endpoint);
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_ZCL_TUNNELING_WITH_PAYLOAD) {
    fillExternalBufferGpdCommandA6Payload(gpdCommandPayload);
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_ATTRIBUTE_REPORTING
             || gpdCommandId == EMBER_ZCL_GP_GPDF_MFR_SP_ATTR_RPTG) {
    fillExternalBufferGpdCommandA0A1Payload(gpdCommandId,
                                            gpdCommandPayload,
                                            genericTranslationTable->zigbeeCommandId,
                                            (bool)genericTranslationTable->serverClient);
  } else if (gpdCommandId >= EMBER_ZCL_GP_GPDF_RECALL_SCENE0
             && gpdCommandId <= EMBER_ZCL_GP_GPDF_STORE_SCENE7) {
    // All scenes commands 0x10 to 0x1F is prepared and handled here
    fillExternalBufferGpdCommandScenesPayload(addr,
                                              gpdCommandId,
                                              genericTranslationTable,
                                              endpoint);
  } else {
    // General command filling.
    (void) emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND             \
                                      | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),   \
                                     genericTranslationTable->zigbeeCluster,   \
                                     genericTranslationTable->zigbeeCommandId, \
                                     "");
    // First copy the pre-configured source
    if (genericTranslationTable->payloadSrc & EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED) {
      emberAfAppendToExternalBuffer(&genericTranslationTable->zclPayloadDefault[1],
                                    genericTranslationTable->zclPayloadDefault[0]);
    }
    // Overwrite the payload from gpd command source - as this is the priority
    if ((genericTranslationTable->payloadSrc & EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD)
        && gpdCommandPayload != NULL) {
      emberAfAppendToExternalBuffer(&gpdCommandPayload[1],
                                    gpdCommandPayload[0]);
    }
  }
  emberAfSetCommandEndpoints(EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_PROXY_ZCL_MESSAGE_SRC_ENDPOINT, endpoint);
  return emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberAfGetNodeId());
}

void emGpForwardGpdCommandDefault(EmberGpAddress *addr,
                                  uint8_t gpdCommandId,
                                  uint8_t *gpdCommandPayload)
{
  uint16_t tableIndex = defaultTableGpdCmdIndex(0, gpdCommandId);
  if (0xFFFF == tableIndex) {
    return; // Can not be forwarded without preconfigured map.
  }
  const EmberAfGreenPowerServerGpdSubTranslationTableEntry *genericTranslationTable = &emberGpDefaultTranslationTable[tableIndex];
  emGpForwardGpdToMappedEndpoint(addr,
                                 gpdCommandId,
                                 gpdCommandPayload,
                                 genericTranslationTable,
                                 EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT); // Application Endpoint
}

const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_SIMPLE_SENSOR_SWITCH[]          = { 0x000F };
const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH[]           = { 0x0400 };
const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_OCCUPANCY_SENSOR_SWITCH[]       = { 0x0406 };
const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_TEMPERATURE_SENSOR_SWITCH[]     = { 0x0402 };
const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_PRESSURE_SENSOR_SWITCH[]        = { 0x0403 };
const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_FLOW_SENSOR_SWITCH[]            = { 0x0404 };
const uint16_t deviceIdCluster_EMBER_GP_DEVICE_ID_GPD_INDOOR_ENVIRONMENT_SENSOR[]     = { 0x0402, 0x0405, 0x0400, 0x040D };

#define DEVICEID_CLUSTER_LOOKUP(num)  { num, (sizeof(deviceIdCluster_##num) / sizeof(uint16_t)), deviceIdCluster_##num }
#define DEVICE_ID_MAP_CLUSTER_TABLE_SIZE (sizeof(gpdDeviceClusterMap) / sizeof(GpDeviceIdAndClusterMap))
const GpDeviceIdAndClusterMap gpdDeviceClusterMap[] = {
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_SIMPLE_SENSOR_SWITCH),             //4
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH),              //11
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_OCCUPANCY_SENSOR_SWITCH),          //12
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_TEMPERATURE_SENSOR_SWITCH),        //30
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_PRESSURE_SENSOR_SWITCH),           //31
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_FLOW_SENSOR_SWITCH),               //32
  DEVICEID_CLUSTER_LOOKUP(EMBER_GP_DEVICE_ID_GPD_INDOOR_ENVIRONMENT_SENSOR),        //33
};

// Simply builds a list of all the gpd cluster supported by the standard deviceId
// consumed by formGpdClusterListFromIncommingCommReq
uint8_t emGetClusterListFromDeviceIdLookup(uint8_t gpdDeviceId,
                                           ZigbeeCluster *gpdClusterList)
{
  for (uint8_t mapTableIndex = 0; mapTableIndex < DEVICE_ID_MAP_CLUSTER_TABLE_SIZE; mapTableIndex++) {
    if (gpdDeviceClusterMap[mapTableIndex].deviceId == gpdDeviceId) {
      for (uint8_t clusterIndex = 0; clusterIndex < gpdDeviceClusterMap[mapTableIndex].numberOfClusters; clusterIndex++) {
        // cluster supported
        gpdClusterList[clusterIndex].clusterId = gpdDeviceClusterMap[mapTableIndex].cluster[clusterIndex];
        // GPD announce this cluster as server,
        // this cluster is recorded as client side to evaluate it presence on the sink
        gpdClusterList[clusterIndex].serverClient = 0; // reverse for the match
      }
      return gpdDeviceClusterMap[mapTableIndex].numberOfClusters;
    }
  }
  return 0;
}

// all define to map CmdID with DeviceID
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_SIMPLE_GENERIC_ONE_STATE_SWITCH[]   = { 2, 0x60, 0x61 };                                                                         //0
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_SIMPLE_GENERIC_TWO_STATE_SWITCH[]   = { 4, 0x62, 0x63, 0x64, 0x65 };                                                             //1
#ifdef EMBER_TEST
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_ON_OFF_SWITCH[]                     = { 8, 0x00, 0x20, 0x21, 0x22, 0x13, 0x1B, 0x40, 0x30 };                                     //2
#else
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_ON_OFF_SWITCH[]                     = { 3, 0x20, 0x21, 0x22 };                                                                   //2
#endif
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LEVEL_CONTROL_SWITCH[]              = { 9, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38 };                               //3
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_ADVANCED_GENERIC_ONE_STATE_SWITCH[] = { 3, 0x60, 0x61, 0x66 };                                                                   //5
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_ADVANCED_GENERIC_TWO_STATE_SWITCH[] = { 7, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68 };                                           //6
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_GENERIC_SWITCH[]                    = { 2, 0x69, 0x6A };                                                                         //7
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_COLOR_DIMMER_SWITCH[]               = { 12, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B };            //10
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH[]               = { 2, 0xAF, 0xA6 }; // 0xAF for any command from 0xA0 to 0xA3 to compact TT                 //11,12,30,31,32,33
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_DOOR_LOCK_CONTROLLER_SWITCH[]       = { 2, 0x50, 0x51 };                                                                         //20
const uint8_t deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_SCENCES[]                           = { 16, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F }; //unspecified

#define deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_TEMPERATURE_SENSOR_SWITCH       deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH
#define deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_PRESSURE_SENSOR_SWITCH          deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH
#define deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_FLOW_SENSOR_SWITCH              deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH
#define deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_INDOOR_ENVIRONMENT_SENSOR       deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH
#define deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_OCCUPANCY_SENSOR_SWITCH         deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH
#define deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_SIMPLE_SENSOR_SWITCH            deviceIdCmds_EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH

#define DEVICEID_CMDS_LOOKUP(num)  { num, deviceIdCmds_##num }
#define DEVICE_ID_MAP_TABLE_SIZE (sizeof(gpdDeviceCmdMap) / sizeof(GpDeviceIdAndCommandMap))
const GpDeviceIdAndCommandMap gpdDeviceCmdMap[] = {
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_SIMPLE_GENERIC_ONE_STATE_SWITCH),  //0
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_SIMPLE_GENERIC_TWO_STATE_SWITCH),  //1
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_ON_OFF_SWITCH),                    //2
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_LEVEL_CONTROL_SWITCH),             //3
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_SIMPLE_SENSOR_SWITCH),             //4
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_ADVANCED_GENERIC_ONE_STATE_SWITCH),//5
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_ADVANCED_GENERIC_TWO_STATE_SWITCH),//6
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_GENERIC_SWITCH),                   //7
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_COLOR_DIMMER_SWITCH),              //10
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_LIGHT_SENSOR_SWITCH),              //11
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_OCCUPANCY_SENSOR_SWITCH),          //12
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_DOOR_LOCK_CONTROLLER_SWITCH),      //20
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_TEMPERATURE_SENSOR_SWITCH),        //30
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_PRESSURE_SENSOR_SWITCH),           //31
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_FLOW_SENSOR_SWITCH),               //32
  DEVICEID_CMDS_LOOKUP(EMBER_GP_DEVICE_ID_GPD_INDOOR_ENVIRONMENT_SENSOR),        //33
};

// Simply builds a list of all the gpd commands supported by the standard deviceId
// consumed by formGpdCommandListFromIncommingCommReq
uint8_t emGetCommandListFromDeviceIdLookup(uint8_t gpdDeviceId,
                                           uint8_t *gpdCommandList)
{
  for (uint8_t index = 0; index < DEVICE_ID_MAP_TABLE_SIZE; index++) {
    if (gpdDeviceCmdMap[index].deviceId == gpdDeviceId) {
      MEMCOPY(gpdCommandList,
              &gpdDeviceCmdMap[index].cmd[1], // commands supported
              gpdDeviceCmdMap[index].cmd[0]); // length
      return gpdDeviceCmdMap[index].cmd[0];
    }
  }
  return 0;
}
