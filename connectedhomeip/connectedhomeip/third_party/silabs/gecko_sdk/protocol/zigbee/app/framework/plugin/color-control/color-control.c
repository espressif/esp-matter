/***************************************************************************//**
 * @file
 * @brief Routines for the Color Control plugin.
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
#include "app/framework/util/attribute-storage.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

/** @brief Is Color Supported
 *
 * This function will be called to determine whether a color is supported by a
 * device. The color will be specified by hue and saturation.
 *
 * @param hue   Ver.: always
 * @param saturation   Ver.: always
 */
bool emberAfPluginColorControlIsColorSupportedCallback(uint8_t hue,
                                                       uint8_t saturation);

typedef struct {
  bool active;
  uint8_t commandId;
  bool hueMoveDirection; // true for up
  bool satMoveDirection; // true for up
  bool acceleratedHue; // Used for HueAndSat; true if Hue move > Sat move
  uint8_t acceleratedMoveRate;
  uint8_t hueMoveToLevel;
  uint8_t saturationMoveToLevel;
  uint32_t eventDuration; // All time fields in milliseconds
  uint32_t elapsedTime;
  uint32_t transitionTime;
} ColorControlState;

static ColorControlState stateTable[EMBER_AF_COLOR_CONTROL_CLUSTER_SERVER_ENDPOINT_COUNT];

static ColorControlState *getColorControlState(uint8_t endpoint);

#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
static void colorControlClearRemainingTime(uint8_t endpoint);
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE

static void colorControlSetHue(uint8_t endpoint, uint8_t hue);
static void colorControlSetSaturation(uint8_t endpoint, uint8_t saturation);
static void colorControlSetColorModeToZero(void);
static EmberAfStatus colorControlReadCurrentHue(uint8_t endpoint, uint8_t* hue);
static EmberAfStatus colorControlReadCurrentSaturation(uint8_t endpoint,
                                                       uint8_t* saturation);
static bool colorControlHueAndSaturationHandler(uint8_t endpoint);

static ColorControlState *getColorControlState(uint8_t endpoint)
{
  uint8_t index = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_COLOR_CONTROL_CLUSTER_ID);
  return (index == 0xFF ? NULL : &stateTable[index]);
}

void emberAfColorControlClusterServerInitCallback(uint8_t endpoint)
{
  ColorControlState *state = getColorControlState(endpoint);
  if (state == NULL) {
    return;
  }

  state->active = false;
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
  colorControlClearRemainingTime(endpoint);
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
}

void emberAfColorControlClusterServerTickCallback(uint8_t endpoint)
{
  ColorControlState *state = getColorControlState(endpoint);
  EmberAfStatus status;
  uint8_t hue, sat;
  bool hueUp = state->hueMoveDirection;
  bool satUp = state->satMoveDirection;

  if (state == NULL) {
    return;
  }

  status = colorControlReadCurrentHue(endpoint, &hue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: could not read current hue %x",
                                      status);
    return;
  }
  status = colorControlReadCurrentSaturation(endpoint, &sat);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: could not read current saturation %x",
                                      status);
    return;
  }

  switch (state->commandId) {
    case ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID:
      state->active = colorControlHueAndSaturationHandler(endpoint);
      break;
    case ZCL_STEP_HUE_COMMAND_ID:
    case ZCL_MOVE_TO_HUE_COMMAND_ID:
    case ZCL_MOVE_HUE_COMMAND_ID:
      hue = hueUp ? hue + 1 : hue - 1;
      if (hue == 0xFF) {
        hue = hueUp ? 0x00 : 0xFE;
      }
      colorControlSetHue(endpoint, hue);
      if (state->commandId != ZCL_MOVE_HUE_COMMAND_ID) {
        state->active = (hue != state->hueMoveToLevel);
      }
      break;
    case ZCL_STEP_SATURATION_COMMAND_ID:
    case ZCL_MOVE_TO_SATURATION_COMMAND_ID:
    case ZCL_MOVE_SATURATION_COMMAND_ID:
      sat = satUp ? sat + 1 : sat - 1;
      if (sat == 0xFF) {
        sat = satUp ? 0xFE : 0x00;
      }
      colorControlSetSaturation(endpoint, sat);
      if (state->commandId != ZCL_MOVE_SATURATION_COMMAND_ID) {
        state->active = (sat != state->saturationMoveToLevel
                         || sat == 0x00
                         || sat == 0xFE);
      }
      break;
    default:
      emberAfColorControlClusterPrintln("ERR: unknown color control command.");
      return;
  }

#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
  {
    uint16_t remainingTime;

    state->elapsedTime = (state->active
                          ? state->elapsedTime + state->eventDuration
                          : state->transitionTime);

    // If we're done, we should clear the remaining time
    if (state->elapsedTime == state->transitionTime) {
      colorControlClearRemainingTime(endpoint);
      return;
    }

    // The remainingTime attribute expects values in tenths of seconds;
    // we maintain in milliseconds
    remainingTime = state->elapsedTime / 100;
    status = emberAfWriteServerAttribute(endpoint,
                                         ZCL_COLOR_CONTROL_CLUSTER_ID,
                                         ZCL_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE_ID,
                                         (uint8_t *)&remainingTime,
                                         ZCL_INT16U_ATTRIBUTE_TYPE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfColorControlClusterPrintln("ERR: writing remaining time %x", status);
      return;
    }
  }
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE

  if (!state->active) {
    return;
  }

  //schedule the next tick
  slxu_zigbee_zcl_schedule_server_tick(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       state->eventDuration);
}

// Clear remaining time
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
static void colorControlClearRemainingTime(uint8_t endpoint)
{
  uint16_t data = 0xFFFF;
  EmberAfStatus status = emberAfWriteServerAttribute(endpoint,
                                                     ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                     ZCL_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE_ID,
                                                     (uint8_t *)&data,
                                                     ZCL_INT16U_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: writing remaining time %x", status);
    return;
  }
}
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE

// Sets the hue attribute
static void colorControlSetHue(uint8_t endpoint, uint8_t hue)
{
  EmberAfStatus status = emberAfWriteServerAttribute(endpoint,
                                                     ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                     ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                                     (uint8_t *)&hue,
                                                     ZCL_INT8U_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: writing current hue %x", status); emberAfColorControlClusterFlush();
    return;
  }
  emberAfDebugPrintln("hue=%x", hue);
}

// Sets the saturation attribute
static void colorControlSetSaturation(uint8_t endpoint, uint8_t saturation)
{
  EmberAfStatus status = emberAfWriteServerAttribute(endpoint,
                                                     ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                     ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                                     (uint8_t *)&saturation,
                                                     ZCL_INT8U_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: writing current saturation %x",
                                      status);
    return;
  }
  emberAfDebugPrintln("saturation=%x", saturation);
}

static void colorControlSetColorModeToZero(void)
{
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE
  // Set the optional Color Mode attribute to zero as per the spec.  If an
  // error occurs, log it, but ignore it.
  uint8_t colorMode = 0;
  EmberAfStatus status = emberAfWriteServerAttribute(emberAfCurrentEndpoint(),
                                                     ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                     ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID,
                                                     &colorMode,
                                                     ZCL_ENUM8_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: writing color mode%x", status);
  }
  emberAfDebugPrintln("colorMode=%x", colorMode); emberAfColorControlClusterFlush();
#else
  emberAfDebugPrintln("no color mode attribute"); emberAfColorControlClusterFlush();
#endif //ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE
}

// returns a ZCL status, EMBER_ZCL_STATUS_SUCCESS when it works. Sets newHue to the
// current Hue attribute value
static EmberAfStatus colorControlReadCurrentHue(uint8_t endpoint, uint8_t* hue)
{
  EmberAfStatus status = emberAfReadServerAttribute(endpoint,
                                                    ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                    ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                                    hue,
                                                    sizeof(uint8_t));
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: reading current hue %x", status);
  }
  return status;
}

// returns a ZCL status, EMBER_ZCL_STATUS_SUCCESS when it works. Sets newHue to the
// current Hue attribute value
static EmberAfStatus colorControlReadCurrentSaturation(uint8_t endpoint,
                                                       uint8_t* saturation)
{
  EmberAfStatus status = emberAfReadServerAttribute(endpoint,
                                                    ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                    ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                                    saturation,
                                                    sizeof(uint8_t));
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: reading current saturation %x",
                                      status);
  }
  return status;
}

/****************** COMMAND HANDLERS **********************/

#ifdef UC_BUILD

static bool moveToHueCommandHandler(uint8_t hue,
                                    uint8_t direction,
                                    uint16_t transitionTime,
                                    uint8_t optionsMask,
                                    uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  emberAfColorControlClusterPrintln("ColorControl: MoveToHue (%x, %x, %2x)",
                                    hue,
                                    direction,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }
  if (!emberAfPluginColorControlIsColorSupportedCallback(hue, currentSaturation)) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(), &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Nothing to do, prevents divide-by-zero
  if ( hue == currentHue ) {
    status = EMBER_ZCL_STATUS_SUCCESS;
    goto send_default_response;
  }

  // As hue is effectively measured on a circle, the new hue may be moved to in
  // either direction.  The direction of hue change is given by the Direction
  // field. If Directionis "Shortest distance," the direction is taken that
  // involves the shortest path round the circle.  This case corresponds to
  // expected normal usage.  If Direction is "Longest distance," the direction
  // is taken that involves the longest path round the circle.  This case can be
  // used for "rainbow effects."  In both cases, if both distances are the same,
  // the Up direction shall be taken.
  switch (direction) {
    case EMBER_ZCL_HUE_DIRECTION_SHORTEST_DISTANCE:
      state->hueMoveDirection = ((uint8_t) (hue - currentHue) <= 127 ? true : false);
      break;
    case EMBER_ZCL_HUE_DIRECTION_LONGEST_DISTANCE:
      state->hueMoveDirection = ((uint8_t) (hue - currentHue) >= 127 ? true : false);
      break;
    case EMBER_ZCL_HUE_DIRECTION_UP:
      state->hueMoveDirection = true;
      break;
    case EMBER_ZCL_HUE_DIRECTION_DOWN:
      state->hueMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  state->commandId = ZCL_MOVE_TO_HUE_COMMAND_ID;
  state->elapsedTime = 0;
  state->hueMoveToLevel = hue;
  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->eventDuration = state->transitionTime / ((state->hueMoveDirection)
                                                  ? (hue - currentHue)
                                                  : (currentHue - hue));
  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  // Schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  state->active = true;
  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

// Move hue to a given hue, taking transitionTime until completed.
bool emberAfColorControlClusterMoveToHueCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_move_to_hue_command_t cmd_data;

  if (zcl_decode_color_control_cluster_move_to_hue_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return moveToHueCommandHandler(cmd_data.hue,
                                 cmd_data.direction,
                                 cmd_data.transitionTime,
                                 cmd_data.optionsMask,
                                 cmd_data.optionsOverride);
}

// Move hue continuously at the given rate. If mode is stop, then stop.
bool emberAfColorControlClusterMoveHueCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_move_hue_command_t cmd_data;
  uint8_t moveMode;
  uint8_t rate;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;

  if (zcl_decode_color_control_cluster_move_hue_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  moveMode = cmd_data.moveMode;
  rate = cmd_data.rate;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;

  emberAfColorControlClusterPrintln("ColorControl: MoveHue (%x, %x)",
                                    moveMode,
                                    rate);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the rate is set to 0, then the command shall have no effect
  // and a ZCL default response command shall be generated with status
  // code equal to INVALID_VALUE.

  if (rate == 0) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
    goto send_default_response;
  }

  state->active = true;

  switch (moveMode) {
    case EMBER_ZCL_HUE_MOVE_MODE_STOP:
      state->active = false;
      slxu_zigbee_zcl_deactivate_server_tick(emberAfCurrentEndpoint(),
                                             ZCL_COLOR_CONTROL_CLUSTER_ID);
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      colorControlClearRemainingTime(emberAfCurrentEndpoint());
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      status = EMBER_ZCL_STATUS_SUCCESS;
      goto send_default_response;
    case EMBER_ZCL_HUE_MOVE_MODE_UP:
      state->hueMoveDirection = true;
      break;
    case EMBER_ZCL_HUE_MOVE_MODE_DOWN:
      state->hueMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  state->commandId = ZCL_MOVE_HUE_COMMAND_ID;
  if (state->active) {
    state->eventDuration = MILLISECOND_TICKS_PER_SECOND / rate;
  }

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Step hue by one step, taking time as specified.
bool emberAfColorControlClusterStepHueCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_step_hue_command_t cmd_data;
  uint8_t stepMode;
  uint8_t stepSize;
  uint8_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  if (zcl_decode_color_control_cluster_step_hue_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  stepMode = cmd_data.stepMode;
  stepSize = cmd_data.stepSize;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;

  emberAfColorControlClusterPrintln("ColorControl: StepHue (%x, %x, %x)",
                                    stepMode,
                                    stepSize,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(), &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Add or subtract the step size to/from the current hue to get the final hue.
  // However, the range for hues is 0x00 to 0xFE, so if we land on 0xFF or if we
  // roll past it, we have to add or subtract one to the final hue, effectively
  // skipping over the invalid 0xFF.
  switch (stepMode) {
    case EMBER_ZCL_HUE_STEP_MODE_UP:
      state->hueMoveToLevel = currentHue + stepSize;
      if (stepSize == 0xFF - currentHue) {
        state->hueMoveToLevel++;
      }
      state->hueMoveDirection = true;
      break;
    case EMBER_ZCL_HUE_STEP_MODE_DOWN:
      state->hueMoveToLevel = currentHue - stepSize;
      if (stepSize == currentHue - 1) {
        state->hueMoveToLevel--;
      }
      state->hueMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  if (!emberAfPluginColorControlIsColorSupportedCallback(state->hueMoveToLevel,
                                                         currentSaturation)
      || stepSize == 0 ) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  state->commandId = ZCL_STEP_HUE_COMMAND_ID;
  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->elapsedTime = 0;
  state->eventDuration = state->transitionTime / stepSize;

  state->active = true;

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Move saturation to a given saturation, taking transitionTime until completed.
static bool moveToSaturationCommandHandler(uint8_t saturation,
                                           uint16_t transitionTime,
                                           uint8_t optionsMask,
                                           uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  emberAfColorControlClusterPrintln("ColorControl: MoveToSaturation (%x, %2x)",
                                    saturation,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(),
                                      &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }
  if (!emberAfPluginColorControlIsColorSupportedCallback(currentHue, saturation)) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Nothing to do, prevent divide-by-zero
  if ( saturation == currentSaturation ) {
    status = EMBER_ZCL_STATUS_SUCCESS;
    goto send_default_response;
  }

  state->commandId = ZCL_MOVE_TO_SATURATION_COMMAND_ID;
  state->elapsedTime = 0;
  state->saturationMoveToLevel = saturation;
  state->satMoveDirection =
    (saturation > currentSaturation ? true : false);
  state->eventDuration = state->transitionTime / ((state->satMoveDirection)
                                                  ? (saturation - currentSaturation)
                                                  : (currentSaturation - saturation));

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

bool emberAfColorControlClusterMoveToSaturationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_move_to_saturation_command_t cmd_data;

  if (zcl_decode_color_control_cluster_move_to_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return moveToSaturationCommandHandler(cmd_data.saturation,
                                        cmd_data.transitionTime,
                                        cmd_data.optionsMask,
                                        cmd_data.optionsOverride);
}

// Move sat continuously at the given rate. If mode is stop, then stop.
bool emberAfColorControlClusterMoveSaturationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_move_saturation_command_t cmd_data;
  uint8_t moveMode;
  uint8_t rate;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;

  if (zcl_decode_color_control_cluster_move_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  moveMode = cmd_data.moveMode;
  rate = cmd_data.rate;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;

  emberAfColorControlClusterPrintln("ColorControl: MoveSaturation (%x, %x)",
                                    moveMode,
                                    rate);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the rate is set to 0, then the command shall have no effect
  // and a ZCL default response command shall be generated with status
  // code equal to INVALID_VALUE.

  if (rate == 0) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
    goto send_default_response;
  }

  state->active = true;

  switch (moveMode) {
    case EMBER_ZCL_SATURATION_MOVE_MODE_STOP:
      state->active = false;
      slxu_zigbee_zcl_deactivate_server_tick(emberAfCurrentEndpoint(),
                                             ZCL_COLOR_CONTROL_CLUSTER_ID);
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      colorControlClearRemainingTime(emberAfCurrentEndpoint());
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      status = EMBER_ZCL_STATUS_SUCCESS;
      goto send_default_response;
    case EMBER_ZCL_SATURATION_MOVE_MODE_UP:
      state->satMoveDirection = true;
      break;
    case EMBER_ZCL_SATURATION_MOVE_MODE_DOWN:
      state->satMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  state->commandId = ZCL_MOVE_SATURATION_COMMAND_ID;
  if (state->active) {
    state->eventDuration = MILLISECOND_TICKS_PER_SECOND / rate;
  }

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }
  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Step sat by one step, taking time as specified.
bool emberAfColorControlClusterStepSaturationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_step_saturation_command_t cmd_data;
  uint8_t stepMode;
  uint8_t stepSize;
  uint8_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  if (zcl_decode_color_control_cluster_step_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  stepMode = cmd_data.stepMode;
  stepSize = cmd_data.stepSize;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;

  emberAfColorControlClusterPrintln("ColorControl: StepSaturation (%x, %x, %x)",
                                    stepMode,
                                    stepSize,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Add or subtract the step size to/from the current saturation to get the
  // final saturation.  However, the range for saturations is 0x00 to 0xFE, so
  // don't increment or decrement past those bounds.
  switch (stepMode) {
    case EMBER_ZCL_SATURATION_STEP_MODE_UP:
      state->saturationMoveToLevel = (stepSize > 0xFE - currentSaturation
                                      ? 0xFE
                                      : currentSaturation + stepSize);
      state->satMoveDirection = true;
      break;
    case EMBER_ZCL_SATURATION_STEP_MODE_DOWN:
      state->saturationMoveToLevel = (stepSize > currentSaturation
                                      ? 0x00
                                      : currentSaturation - stepSize);
      state->satMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(),
                                      &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  if (!emberAfPluginColorControlIsColorSupportedCallback(currentHue,
                                                         state->saturationMoveToLevel)
      || stepSize == 0 ) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  state->commandId = ZCL_STEP_SATURATION_COMMAND_ID;
  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->elapsedTime = 0;
  state->eventDuration = state->transitionTime / stepSize;

  state->active = true;

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Move hue and saturation to a given values, taking time as specified.
bool emberAfColorControlClusterMoveToHueAndSaturationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_color_control_cluster_move_to_hue_and_saturation_command_t cmd_data;
  uint8_t hue;
  uint8_t saturation;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentSaturation, currentHue, hueDiff, satDiff;

  if (zcl_decode_color_control_cluster_move_to_hue_and_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  hue = cmd_data.hue;
  saturation = cmd_data.saturation;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;

  emberAfColorControlClusterPrintln("ColorControl: MoveToHueAndSaturation (%x, %x, %2x)",
                                    hue,
                                    saturation,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  if (!emberAfPluginColorControlIsColorSupportedCallback(hue, saturation)) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(), &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  state->commandId = ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID;
  state->hueMoveToLevel = hue;
  state->hueMoveDirection = (hue - currentHue <= 127 ? true : false);
  hueDiff = (state->hueMoveDirection ? hue - currentHue : currentHue - hue);

  state->saturationMoveToLevel = saturation;
  state->satMoveDirection = (saturation - currentSaturation <= 127 ? true : false);
  satDiff = (state->satMoveDirection
             ? saturation - currentSaturation
             : currentSaturation - saturation);

  if (hueDiff == 0) {
    return moveToSaturationCommandHandler(saturation,
                                          transitionTime,
                                          0x00,  // optionsMask
                                          0x00); // optionsOverride
  } else if (satDiff == 0) {
    return moveToHueCommandHandler(hue,
                                   EMBER_ZCL_HUE_DIRECTION_SHORTEST_DISTANCE,
                                   transitionTime,
                                   0x00,  // optionsMask
                                   0x00); // optionsOverride
  }

  state->elapsedTime = 0;
  state->acceleratedHue = hueDiff > satDiff;
  state->acceleratedMoveRate = (state->acceleratedHue
                                ? hueDiff / satDiff
                                : satDiff / hueDiff);

  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->eventDuration = state->transitionTime / ((state->acceleratedHue)
                                                  ? hueDiff
                                                  : satDiff);

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  state->active = true;

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

#else // !UC_BUILD

// Move hue to a given hue, taking transitionTime until completed.
bool emberAfColorControlClusterMoveToHueCallback(uint8_t hue,
                                                 uint8_t direction,
                                                 uint16_t transitionTime,
                                                 uint8_t optionsMask,
                                                 uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  emberAfColorControlClusterPrintln("ColorControl: MoveToHue (%x, %x, %2x)",
                                    hue,
                                    direction,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }
  if (!emberAfPluginColorControlIsColorSupportedCallback(hue, currentSaturation)) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(), &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Nothing to do, prevents divide-by-zero
  if ( hue == currentHue ) {
    status = EMBER_ZCL_STATUS_SUCCESS;
    goto send_default_response;
  }

  // As hue is effectively measured on a circle, the new hue may be moved to in
  // either direction.  The direction of hue change is given by the Direction
  // field. If Directionis "Shortest distance," the direction is taken that
  // involves the shortest path round the circle.  This case corresponds to
  // expected normal usage.  If Direction is "Longest distance," the direction
  // is taken that involves the longest path round the circle.  This case can be
  // used for "rainbow effects."  In both cases, if both distances are the same,
  // the Up direction shall be taken.
  switch (direction) {
    case EMBER_ZCL_HUE_DIRECTION_SHORTEST_DISTANCE:
      state->hueMoveDirection = ((uint8_t) (hue - currentHue) <= 127 ? true : false);
      break;
    case EMBER_ZCL_HUE_DIRECTION_LONGEST_DISTANCE:
      state->hueMoveDirection = ((uint8_t) (hue - currentHue) >= 127 ? true : false);
      break;
    case EMBER_ZCL_HUE_DIRECTION_UP:
      state->hueMoveDirection = true;
      break;
    case EMBER_ZCL_HUE_DIRECTION_DOWN:
      state->hueMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  state->commandId = ZCL_MOVE_TO_HUE_COMMAND_ID;
  state->elapsedTime = 0;
  state->hueMoveToLevel = hue;
  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->eventDuration = state->transitionTime / ((state->hueMoveDirection)
                                                  ? (hue - currentHue)
                                                  : (currentHue - hue));
  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  // Schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  state->active = true;
  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

// Move hue continuously at the given rate. If mode is stop, then stop.
bool emberAfColorControlClusterMoveHueCallback(uint8_t moveMode,
                                               uint8_t rate,
                                               uint8_t optionsMask,
                                               uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;

  emberAfColorControlClusterPrintln("ColorControl: MoveHue (%x, %x)",
                                    moveMode,
                                    rate);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the rate is set to 0, then the command shall have no effect
  // and a ZCL default response command shall be generated with status
  // code equal to INVALID_VALUE.

  if (rate == 0) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
    goto send_default_response;
  }

  state->active = true;

  switch (moveMode) {
    case EMBER_ZCL_HUE_MOVE_MODE_STOP:
      state->active = false;
      slxu_zigbee_zcl_deactivate_server_tick(emberAfCurrentEndpoint(),
                                             ZCL_COLOR_CONTROL_CLUSTER_ID);
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      colorControlClearRemainingTime(emberAfCurrentEndpoint());
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      status = EMBER_ZCL_STATUS_SUCCESS;
      goto send_default_response;
    case EMBER_ZCL_HUE_MOVE_MODE_UP:
      state->hueMoveDirection = true;
      break;
    case EMBER_ZCL_HUE_MOVE_MODE_DOWN:
      state->hueMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  state->commandId = ZCL_MOVE_HUE_COMMAND_ID;
  if (state->active) {
    state->eventDuration = MILLISECOND_TICKS_PER_SECOND / rate;
  }

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Step hue by one step, taking time as specified.
bool emberAfColorControlClusterStepHueCallback(uint8_t stepMode,
                                               uint8_t stepSize,
                                               uint8_t transitionTime,
                                               uint8_t optionsMask,
                                               uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  emberAfColorControlClusterPrintln("ColorControl: StepHue (%x, %x, %x)",
                                    stepMode,
                                    stepSize,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(), &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Add or subtract the step size to/from the current hue to get the final hue.
  // However, the range for hues is 0x00 to 0xFE, so if we land on 0xFF or if we
  // roll past it, we have to add or subtract one to the final hue, effectively
  // skipping over the invalid 0xFF.
  switch (stepMode) {
    case EMBER_ZCL_HUE_STEP_MODE_UP:
      state->hueMoveToLevel = currentHue + stepSize;
      if (stepSize == 0xFF - currentHue) {
        state->hueMoveToLevel++;
      }
      state->hueMoveDirection = true;
      break;
    case EMBER_ZCL_HUE_STEP_MODE_DOWN:
      state->hueMoveToLevel = currentHue - stepSize;
      if (stepSize == currentHue - 1) {
        state->hueMoveToLevel--;
      }
      state->hueMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  if (!emberAfPluginColorControlIsColorSupportedCallback(state->hueMoveToLevel,
                                                         currentSaturation)
      || stepSize == 0 ) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  state->commandId = ZCL_STEP_HUE_COMMAND_ID;
  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->elapsedTime = 0;
  state->eventDuration = state->transitionTime / stepSize;

  state->active = true;

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Move saturation to a given saturation, taking transitionTime until completed.
bool emberAfColorControlClusterMoveToSaturationCallback(uint8_t saturation,
                                                        uint16_t transitionTime,
                                                        uint8_t optionsMask,
                                                        uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  emberAfColorControlClusterPrintln("ColorControl: MoveToSaturation (%x, %2x)",
                                    saturation,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(),
                                      &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }
  if (!emberAfPluginColorControlIsColorSupportedCallback(currentHue, saturation)) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Nothing to do, prevent divide-by-zero
  if ( saturation == currentSaturation ) {
    status = EMBER_ZCL_STATUS_SUCCESS;
    goto send_default_response;
  }

  state->commandId = ZCL_MOVE_TO_SATURATION_COMMAND_ID;
  state->elapsedTime = 0;
  state->saturationMoveToLevel = saturation;
  state->satMoveDirection =
    (saturation > currentSaturation ? true : false);
  state->eventDuration = state->transitionTime / ((state->satMoveDirection)
                                                  ? (saturation - currentSaturation)
                                                  : (currentSaturation - saturation));

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Move sat continuously at the given rate. If mode is stop, then stop.
bool emberAfColorControlClusterMoveSaturationCallback(uint8_t moveMode,
                                                      uint8_t rate,
                                                      uint8_t optionsMask,
                                                      uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;

  emberAfColorControlClusterPrintln("ColorControl: MoveSaturation (%x, %x)",
                                    moveMode,
                                    rate);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the rate is set to 0, then the command shall have no effect
  // and a ZCL default response command shall be generated with status
  // code equal to INVALID_VALUE.

  if (rate == 0) {
    status = EMBER_ZCL_STATUS_INVALID_FIELD;
    goto send_default_response;
  }

  state->active = true;

  switch (moveMode) {
    case EMBER_ZCL_SATURATION_MOVE_MODE_STOP:
      state->active = false;
      slxu_zigbee_zcl_deactivate_server_tick(emberAfCurrentEndpoint(),
                                             ZCL_COLOR_CONTROL_CLUSTER_ID);
#ifdef ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      colorControlClearRemainingTime(emberAfCurrentEndpoint());
#endif // ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE
      status = EMBER_ZCL_STATUS_SUCCESS;
      goto send_default_response;
    case EMBER_ZCL_SATURATION_MOVE_MODE_UP:
      state->satMoveDirection = true;
      break;
    case EMBER_ZCL_SATURATION_MOVE_MODE_DOWN:
      state->satMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  state->commandId = ZCL_MOVE_SATURATION_COMMAND_ID;
  if (state->active) {
    state->eventDuration = MILLISECOND_TICKS_PER_SECOND / rate;
  }

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }
  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Step sat by one step, taking time as specified.
bool emberAfColorControlClusterStepSaturationCallback(uint8_t stepMode,
                                                      uint8_t stepSize,
                                                      uint8_t transitionTime,
                                                      uint8_t optionsMask,
                                                      uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentHue, currentSaturation;

  emberAfColorControlClusterPrintln("ColorControl: StepSaturation (%x, %x, %x)",
                                    stepMode,
                                    stepSize,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  // Add or subtract the step size to/from the current saturation to get the
  // final saturation.  However, the range for saturations is 0x00 to 0xFE, so
  // don't increment or decrement past those bounds.
  switch (stepMode) {
    case EMBER_ZCL_SATURATION_STEP_MODE_UP:
      state->saturationMoveToLevel = (stepSize > 0xFE - currentSaturation
                                      ? 0xFE
                                      : currentSaturation + stepSize);
      state->satMoveDirection = true;
      break;
    case EMBER_ZCL_SATURATION_STEP_MODE_DOWN:
      state->saturationMoveToLevel = (stepSize > currentSaturation
                                      ? 0x00
                                      : currentSaturation - stepSize);
      state->satMoveDirection = false;
      break;
    default:
      status = EMBER_ZCL_STATUS_INVALID_FIELD;
      goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(),
                                      &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  if (!emberAfPluginColorControlIsColorSupportedCallback(currentHue,
                                                         state->saturationMoveToLevel)
      || stepSize == 0 ) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  state->commandId = ZCL_STEP_SATURATION_COMMAND_ID;
  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->elapsedTime = 0;
  state->eventDuration = state->transitionTime / stepSize;

  state->active = true;

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

// Move hue and saturation to a given values, taking time as specified.
bool emberAfColorControlClusterMoveToHueAndSaturationCallback(uint8_t hue,
                                                              uint8_t saturation,
                                                              uint16_t transitionTime,
                                                              uint8_t optionsMask,
                                                              uint8_t optionsOverride)
{
  ColorControlState *state = getColorControlState(emberAfCurrentEndpoint());
  EmberAfStatus status;
  uint8_t currentSaturation, currentHue, hueDiff, satDiff;

  emberAfColorControlClusterPrintln("ColorControl: MoveToHueAndSaturation (%x, %x, %2x)",
                                    hue,
                                    saturation,
                                    transitionTime);

  if (state == NULL) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto send_default_response;
  }

  // If the color specified is not achievable by the hardware, then the
  // color shall not be set and a ZCL default response command shall be
  // generated with status code equal to INVALID_VALUE.
  if (!emberAfPluginColorControlIsColorSupportedCallback(hue, saturation)) {
    status = EMBER_ZCL_STATUS_INVALID_VALUE;
    goto send_default_response;
  }

  status = colorControlReadCurrentSaturation(emberAfCurrentEndpoint(),
                                             &currentSaturation);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  status = colorControlReadCurrentHue(emberAfCurrentEndpoint(), &currentHue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    goto send_default_response;
  }

  state->commandId = ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID;
  state->hueMoveToLevel = hue;
  state->hueMoveDirection = (hue - currentHue <= 127 ? true : false);
  hueDiff = (state->hueMoveDirection ? hue - currentHue : currentHue - hue);

  state->saturationMoveToLevel = saturation;
  state->satMoveDirection = (saturation - currentSaturation <= 127 ? true : false);
  satDiff = (state->satMoveDirection
             ? saturation - currentSaturation
             : currentSaturation - saturation);

  if (hueDiff == 0) {
    return emberAfColorControlClusterMoveToSaturationCallback(saturation,
                                                              transitionTime,
                                                              0x00,  // optionsMask
                                                              0x00); // optionsOverride
  } else if (satDiff == 0) {
    return emberAfColorControlClusterMoveToHueCallback(hue,
                                                       EMBER_ZCL_HUE_DIRECTION_SHORTEST_DISTANCE,
                                                       transitionTime,
                                                       0x00,  // optionsMask
                                                       0x00); // optionsOverride
  }

  state->elapsedTime = 0;
  state->acceleratedHue = hueDiff > satDiff;
  state->acceleratedMoveRate = (state->acceleratedHue
                                ? hueDiff / satDiff
                                : satDiff / hueDiff);

  state->transitionTime = transitionTime * MILLISECOND_TICKS_PER_SECOND / 10;
  state->eventDuration = state->transitionTime / ((state->acceleratedHue)
                                                  ? hueDiff
                                                  : satDiff);

  // Set the Color Mode attribute to zero as per the spec, ignoring any errors.
  colorControlSetColorModeToZero();

  state->active = true;

  //schedule the next tick
  if (slxu_zigbee_zcl_schedule_server_tick(emberAfCurrentEndpoint(),
                                           ZCL_COLOR_CONTROL_CLUSTER_ID,
                                           state->eventDuration)
      != EMBER_SUCCESS) {
    status = EMBER_ZCL_STATUS_FAILURE;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  goto send_default_response;

  send_default_response:
  emberAfSendImmediateDefaultResponse(status);

  return true;
}

#endif // UC_BUILD

static bool colorControlHueAndSaturationHandler(uint8_t endpoint)
{
  EmberAfStatus status;
  ColorControlState *state = getColorControlState(endpoint);
  uint8_t hue, sat, hueDiff, satDiff;
  bool hueUp = state->hueMoveDirection;
  bool satUp = state->satMoveDirection;
  bool proceed = true;

  if (state == NULL) {
    return false;
  }

  status = colorControlReadCurrentHue(endpoint, &hue);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: could not read current hue %x",
                                      status);
    return false;
  }
  status = colorControlReadCurrentSaturation(endpoint, &sat);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("ERR: could not read current saturation %x",
                                      status);
    return false;
  }

  // Calculate the hue and saturation differentials
  hueDiff = (hue > state->hueMoveToLevel)
            ? (0xFF - (hue - state->hueMoveToLevel))
            : (state->hueMoveToLevel - hue);
  satDiff = (sat > state->saturationMoveToLevel)
            ? (0xFF - (sat - state->saturationMoveToLevel))
            : (state->saturationMoveToLevel - sat);

  // See where we are relative to our expected correction threshold
  if (state->acceleratedHue) {
    if ( satDiff == 0 ) {
      return false;
    }
    hueDiff = hueDiff / satDiff;
    if (hueDiff > state->acceleratedMoveRate) {
      hueDiff = state->acceleratedMoveRate - 1;
    } else {
      hueDiff = state->acceleratedMoveRate + 1;
    }

    sat = satUp ? sat + 1 : sat - 1;
    if (sat == 0xFF) {
      sat = satUp ? 0x00 : 0xFE;
    }
    // Are we finished?
    if (sat == state->saturationMoveToLevel) {
      hue = state->hueMoveToLevel;
      proceed = false;
      goto set_values;
    }
    hue = hueUp ? hue + hueDiff : hue - hueDiff;
    if (hue == 0xFF) {
      hue = hueUp ? 0x00 : 0xFE;
    }
  } else {
    if ( hueDiff == 0 ) {
      return false;
    }
    satDiff = satDiff / hueDiff;
    if (satDiff > state->acceleratedMoveRate) {
      satDiff = state->acceleratedMoveRate - 1;
    } else {
      satDiff = state->acceleratedMoveRate + 1;
    }

    hue = hueUp ? hue + 1 : hue - 1;
    if (hue == 0xFF) {
      hue = hueUp ? 0x00 : 0xFE;
    }
    // Are we finished?
    if (hue == state->hueMoveToLevel) {
      sat = state->saturationMoveToLevel;
      proceed = false;
      goto set_values;
    }
    sat = satUp ? sat + satDiff : sat - satDiff;
    if (sat == 0xFF) {
      sat = satUp ? 0x00 : 0xFE;
    }
  }

  set_values:
  colorControlSetHue(endpoint, hue);
  colorControlSetSaturation(endpoint, sat);
  return proceed;
}

#ifdef UC_BUILD

uint32_t emberAfColorControlClusterServerCommandParse(sl_service_opcode_t opcode,
                                                      sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_MOVE_TO_HUE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToHueCallback(cmd);
        break;
      }
      case ZCL_MOVE_HUE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveHueCallback(cmd);
        break;
      }
      case ZCL_STEP_HUE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterStepHueCallback(cmd);
        break;
      }
      case ZCL_MOVE_TO_SATURATION_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToSaturationCallback(cmd);
        break;
      }
      case ZCL_MOVE_SATURATION_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveSaturationCallback(cmd);
        break;
      }
      case ZCL_STEP_SATURATION_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterStepSaturationCallback(cmd);
        break;
      }
      case ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToHueAndSaturationCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
