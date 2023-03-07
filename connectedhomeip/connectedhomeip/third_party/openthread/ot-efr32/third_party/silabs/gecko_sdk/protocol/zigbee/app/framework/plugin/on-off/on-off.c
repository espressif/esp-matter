/***************************************************************************//**
 * @file
 * @brief Routines for the On-Off plugin, which implements the On-Off server
 *        cluster.
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
#include "on-off.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_REPORTING_PRESENT
#include "reporting.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_SCENES_PRESENT
#include "scenes.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT
#include "zll-on-off-server.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_ZLL_LEVEL_CONTROL_SERVER_PRESENT
  #include "zll-level-control-server.h"
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_REPORTING
  #include "app/framework/plugin/reporting/reporting.h"
#endif
#ifdef EMBER_AF_PLUGIN_SCENES
  #include "../scenes/scenes.h"
#endif //EMBER_AF_PLUGIN_SCENES
#ifdef EMBER_AF_PLUGIN_ZLL_ON_OFF_SERVER
  #include "../zll-on-off-server/zll-on-off-server.h"
  #define SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER
  #include "../zll-level-control-server/zll-level-control-server.h"
#endif
#endif // UC_BUILD

#ifdef ZCL_USING_ON_OFF_CLUSTER_START_UP_ON_OFF_ATTRIBUTE
static bool areStartUpOnOffServerAttributesTokenized(uint8_t endpoint);
#endif

EmberAfStatus emberAfOnOffClusterSetValueCallback(uint8_t endpoint,
                                                  uint8_t command,
                                                  bool initiatedByLevelChange)
{
  EmberAfStatus status;
  bool currentValue, newValue;

  emberAfOnOffClusterPrintln("On/Off set value: %x %x", endpoint, command);

  // read current on/off value
  status = emberAfReadAttribute(endpoint,
                                ZCL_ON_OFF_CLUSTER_ID,
                                ZCL_ON_OFF_ATTRIBUTE_ID,
                                CLUSTER_MASK_SERVER,
                                (uint8_t *)&currentValue,
                                sizeof(currentValue),
                                NULL); // data type
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfOnOffClusterPrintln("ERR: reading on/off %x", status);
    return status;
  }

  // if the value is already what we want to set it to then do nothing
  if ((!currentValue && command == ZCL_OFF_COMMAND_ID)
      || (currentValue && command == ZCL_ON_COMMAND_ID)) {
    emberAfOnOffClusterPrintln("On/off already set to new value");
    return EMBER_ZCL_STATUS_SUCCESS;
  }

  // we either got a toggle, or an on when off, or an off when on,
  // so we need to swap the value
  newValue = !currentValue;
  emberAfOnOffClusterPrintln("Toggle on/off from %x to %x", currentValue, newValue);

  // the sequence of updating on/off attribute and kick off level change effect should
  // be depend on whether we are turning on or off. If we are turning on the light, we
  // should update the on/off attribute before kicking off level change, if we are
  // turning off the light, we should do the opposite,cmd_data that is kick off level change
  // before updating the on/off attribute.
  if (newValue) {
    // write the new on/off value
    status = emberAfWriteAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   CLUSTER_MASK_SERVER,
                                   (uint8_t *)&newValue,
                                   ZCL_BOOLEAN_ATTRIBUTE_TYPE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfOnOffClusterPrintln("ERR: writing on/off %x", status);
      return status;
    }

    // If initiatedByLevelChange is false, then we assume that the level change
    // ZCL stuff has not happened and we do it here
    if (!initiatedByLevelChange
        && emberAfContainsServer(endpoint, ZCL_LEVEL_CONTROL_CLUSTER_ID)) {
      emberAfOnOffClusterLevelControlEffectCallback(endpoint,
                                                    newValue);
    }
  } else {
    // If initiatedByLevelChange is false, then we assume that the level change
    // ZCL stuff has not happened and we do it here
    if (!initiatedByLevelChange
        && emberAfContainsServer(endpoint, ZCL_LEVEL_CONTROL_CLUSTER_ID)) {
      emberAfOnOffClusterLevelControlEffectCallback(endpoint,
                                                    newValue);
    }

    // write the new on/off value
    status = emberAfWriteAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   CLUSTER_MASK_SERVER,
                                   (uint8_t *)&newValue,
                                   ZCL_BOOLEAN_ATTRIBUTE_TYPE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfOnOffClusterPrintln("ERR: writing on/off %x", status);
      return status;
    }
  }

#ifdef SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT
  if (initiatedByLevelChange) {
    emberAfPluginZllOnOffServerLevelControlZllExtensions(endpoint);
  }
#endif // SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT

  // the scene has been changed (the value of on/off has changed) so
  // the current scene as descibed in the attribute table is invalid,
  // so mark it as invalid (just writes the valid/invalid attribute)
  if (emberAfContainsServer(endpoint, ZCL_SCENES_CLUSTER_ID)) {
    emberAfScenesClusterMakeInvalidCallback(endpoint);
  }

  // The returned status is based solely on the On/Off cluster.  Errors in the
  // Level Control and/or Scenes cluster are ignored.
  return EMBER_ZCL_STATUS_SUCCESS;
}

bool emberAfOnOffClusterOffCallback(void)
{
  EmberAfStatus status = emberAfOnOffClusterSetValueCallback(emberAfCurrentEndpoint(),
                                                             ZCL_OFF_COMMAND_ID,
                                                             false);
#ifdef SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfPluginZllOnOffServerOffZllExtensions(emberAfCurrentCommand());
  }
#endif
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfOnOffClusterOnCallback(void)
{
  EmberAfStatus status = emberAfOnOffClusterSetValueCallback(emberAfCurrentEndpoint(),
                                                             ZCL_ON_COMMAND_ID,
                                                             false);
#ifdef SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfPluginZllOnOffServerOnZllExtensions(emberAfCurrentCommand());
  }
#endif
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfOnOffClusterToggleCallback(void)
{
  EmberAfStatus status = emberAfOnOffClusterSetValueCallback(emberAfCurrentEndpoint(),
                                                             ZCL_TOGGLE_COMMAND_ID,
                                                             false);
#ifdef SL_CATALOG_ZIGBEE_ZLL_ON_OFF_SERVER_PRESENT
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    emberAfPluginZllOnOffServerToggleZllExtensions(emberAfCurrentCommand());
  }
#endif
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

void emberAfOnOffClusterServerInitCallback(uint8_t endpoint)
{
#ifdef ZCL_USING_ON_OFF_CLUSTER_START_UP_ON_OFF_ATTRIBUTE
  // StartUp behavior relies on OnOff and StartUpOnOff attributes being tokenized.
  if (areStartUpOnOffServerAttributesTokenized(endpoint)) {
    // Read the StartUpOnOff attribute and set the OnOff attribute as per
    // following from zcl 7 14-0127-20i-zcl-ch-3-general.doc.
    // 3.8.2.2.5	StartUpOnOff Attribute
    // The StartUpOnOff attribute SHALL define the desired startup behavior of a
    // lamp device when it is supplied with power and this state SHALL be
    // reflected in the OnOff attribute.  The values of the StartUpOnOff
    // attribute are listed below.
    // Table 3 46. Values of the StartUpOnOff Attribute
    // Value      Action on power up
    // 0x00       Set the OnOff attribute to 0 (off).
    // 0x01       Set the OnOff attribute to 1 (on).
    // 0x02       If the previous value of the OnOff attribute is equal to 0,
    //            set the OnOff attribute to 1.If the previous value of the OnOff
    //            attribute is equal to 1, set the OnOff attribute to 0 (toggle).
    // 0x03-0xfe  These values are reserved.  No action.
    // 0xff       Set the OnOff attribute to its previous value.

    // Initialize startUpOnOff to No action value 0xFE
    uint8_t startUpOnOff = 0xFE;
    EmberAfStatus status = emberAfReadAttribute(endpoint,
                                                ZCL_ON_OFF_CLUSTER_ID,
                                                ZCL_START_UP_ON_OFF_ATTRIBUTE_ID,
                                                CLUSTER_MASK_SERVER,
                                                (uint8_t *)&startUpOnOff,
                                                sizeof(startUpOnOff),
                                                NULL);
    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      // Initialise updated value to 0
      bool updatedOnOff = 0;
      status = emberAfReadAttribute(endpoint,
                                    ZCL_ON_OFF_CLUSTER_ID,
                                    ZCL_ON_OFF_ATTRIBUTE_ID,
                                    CLUSTER_MASK_SERVER,
                                    (uint8_t *)&updatedOnOff,
                                    sizeof(updatedOnOff),
                                    NULL);
      if (status == EMBER_ZCL_STATUS_SUCCESS) {
        switch (startUpOnOff) {
          case EMBER_ZCL_START_UP_ON_OFF_VALUE_SET_TO_OFF:
            updatedOnOff = 0; // Off
            break;
          case EMBER_ZCL_START_UP_ON_OFF_VALUE_SET_TO_ON:
            updatedOnOff = 1; //On
            break;
          case EMBER_ZCL_START_UP_ON_OFF_VALUE_SET_TO_TOGGLE:
            updatedOnOff = !updatedOnOff;
            break;
          case EMBER_ZCL_START_UP_ON_OFF_VALUE_SET_TO_PREVIOUS:
          default:
            // All other values 0x03- 0xFE are reserved - no action.
            // When value is 0xFF - update with last value - that is as good as
            // no action.
            break;
        }
        status = emberAfWriteAttribute(endpoint,
                                       ZCL_ON_OFF_CLUSTER_ID,
                                       ZCL_ON_OFF_ATTRIBUTE_ID,
                                       CLUSTER_MASK_SERVER,
                                       (uint8_t *)&updatedOnOff,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);
      }
    }
  }
#endif
  emberAfPluginOnOffClusterServerPostInitCallback(endpoint);
}

#ifdef ZCL_USING_ON_OFF_CLUSTER_START_UP_ON_OFF_ATTRIBUTE
static bool areStartUpOnOffServerAttributesTokenized(uint8_t endpoint)
{
  EmberAfAttributeMetadata *metadata;

  metadata = emberAfLocateAttributeMetadata(endpoint,
                                            ZCL_ON_OFF_CLUSTER_ID,
                                            ZCL_ON_OFF_ATTRIBUTE_ID,
                                            CLUSTER_MASK_SERVER,
                                            EMBER_AF_NULL_MANUFACTURER_CODE);
  if (!emberAfAttributeIsTokenized(metadata)) {
    return false;
  }

  metadata = emberAfLocateAttributeMetadata(endpoint,
                                            ZCL_ON_OFF_CLUSTER_ID,
                                            ZCL_START_UP_ON_OFF_ATTRIBUTE_ID,
                                            CLUSTER_MASK_SERVER,
                                            EMBER_AF_NULL_MANUFACTURER_CODE);
  if (!emberAfAttributeIsTokenized(metadata)) {
    return false;
  }

  return true;
}
#endif

#ifdef UC_BUILD

uint32_t emberAfOnOffClusterServerCommandParse(sl_service_opcode_t opcode,
                                               sl_service_function_context_t *context)
{
  (void)opcode;

  bool wasHandled = false;
  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_OFF_COMMAND_ID:
      {
        wasHandled = emberAfOnOffClusterOffCallback();
        break;
      }
      case ZCL_ON_COMMAND_ID:
      {
        wasHandled = emberAfOnOffClusterOnCallback();
        break;
      }
      case ZCL_TOGGLE_COMMAND_ID:
      {
        wasHandled = emberAfOnOffClusterToggleCallback();
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
