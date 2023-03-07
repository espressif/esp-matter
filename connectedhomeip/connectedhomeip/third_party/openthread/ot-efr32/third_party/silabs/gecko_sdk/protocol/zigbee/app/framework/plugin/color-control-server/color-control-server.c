/***************************************************************************//**
 * @file
 * @brief Routines for the Color Control Server plugin.
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
#include "color-control-server.h"

// move mode
enum {
  MOVE_MODE_STOP     = 0x00,
  MOVE_MODE_UP       = 0x01,
  MOVE_MODE_DOWN     = 0x03
};

enum {
  COLOR_MODE_HSV         = 0x00,
  COLOR_MODE_CIE_XY      = 0x01,
  COLOR_MODE_TEMPERATURE = 0x02
};

enum {
  HSV_TO_HSV         = 0x00,
  HSV_TO_CIE_XY      = 0x01,
  HSV_TO_TEMPERATURE = 0x02,
  CIE_XY_TO_HSV         = 0x10,
  CIE_XY_TO_CIE_XY      = 0x11,
  CIE_XY_TO_TEMPERATURE = 0x12,
  TEMPERATURE_TO_HSV         = 0x20,
  TEMPERATURE_TO_CIE_XY      = 0x21,
  TEMPERATURE_TO_TEMPERATURE = 0x22
};

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#include "color-control-server-config.h"
#if (EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_XY == 1)
#define SUPPORT_CIE_1931
#endif
#if (EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_TEMP == 1)
#define SUPPORT_COLOR_TEMPERATURE
#endif
#if (EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_HSV == 1)
#define SUPPORT_HUE_SATURATION
#endif
sl_zigbee_event_t emberAfPluginColorControlServerTempTransitionEvent;
sl_zigbee_event_t emberAfPluginColorControlServerXyTransitionEvent;
sl_zigbee_event_t emberAfPluginColorControlServerHueSatTransitionEvent;

#define COLOR_TEMP_CONTROL (&emberAfPluginColorControlServerTempTransitionEvent)
#define COLOR_XY_CONTROL   (&emberAfPluginColorControlServerXyTransitionEvent)
#define COLOR_HSV_CONTROL  (&emberAfPluginColorControlServerHueSatTransitionEvent)

void emberAfPluginColorControlServerTempTransitionEventHandler(SLXU_UC_EVENT);
void emberAfPluginColorControlServerXyTransitionEventHandler(SLXU_UC_EVENT);
void emberAfPluginColorControlServerHueSatTransitionEventHandler(SLXU_UC_EVENT);
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_XY
#define SUPPORT_CIE_1931
#endif
#ifdef EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_TEMP
#define SUPPORT_COLOR_TEMPERATURE
#endif
#ifdef EMBER_AF_PLUGIN_COLOR_CONTROL_SERVER_HSV
#define SUPPORT_HUE_SATURATION
#endif
EmberEventControl emberAfPluginColorControlServerTempTransitionEventControl;
EmberEventControl emberAfPluginColorControlServerXyTransitionEventControl;
EmberEventControl emberAfPluginColorControlServerHueSatTransitionEventControl;

#define COLOR_TEMP_CONTROL emberAfPluginColorControlServerTempTransitionEventControl
#define COLOR_XY_CONTROL   emberAfPluginColorControlServerXyTransitionEventControl
#define COLOR_HSV_CONTROL  emberAfPluginColorControlServerHueSatTransitionEventControl
#endif // UC_BUILD

#define UPDATE_TIME_MS 100
#define TRANSITION_TIME_1S 10
#define MIN_CIE_XY_VALUE 0
// this value comes directly from the ZCL specification table 5.3
#define MAX_CIE_XY_VALUE 0xfeff
#define MIN_TEMPERATURE_VALUE 0
#define MAX_TEMPERATURE_VALUE 0xfeff
#define MIN_HUE_VALUE 0
#define MAX_HUE_VALUE 254
#define MIN_SATURATION_VALUE 0
#define MAX_SATURATION_VALUE 254
#define HALF_MAX_UINT8T 127

#define MIN_CURRENT_LEVEL 0x01
#define MAX_CURRENT_LEVEL 0xFE

#define REPORT_FAILED 0xFF

typedef struct {
  uint8_t  initialHue;
  uint8_t  currentHue;
  uint8_t  finalHue;
  uint16_t stepsRemaining;
  uint16_t stepsTotal;
  uint8_t  endpoint;
  bool     up;
  bool     repeat;
} ColorHueTransitionState;

static ColorHueTransitionState colorHueTransitionState;

typedef struct {
  uint16_t initialValue;
  uint16_t currentValue;
  uint16_t finalValue;
  uint16_t stepsRemaining;
  uint16_t stepsTotal;
  uint16_t lowLimit;
  uint16_t highLimit;
  uint8_t  endpoint;
} Color16uTransitionState;

static Color16uTransitionState colorXTransitionState;
static Color16uTransitionState colorYTransitionState;

static Color16uTransitionState colorTempTransitionState;

static Color16uTransitionState colorSaturationTransitionState;

// Forward declarations:
static bool computeNewColor16uValue(Color16uTransitionState *p);
static void stopAllColorTransitions(void);
static void handleModeSwitch(uint8_t endpoint,
                             uint8_t newColorMode);
static bool shouldExecuteIfOff(uint8_t endpoint,
                               uint8_t optionMask,
                               uint8_t optionOverride);

#ifdef SUPPORT_HUE_SATURATION
static uint8_t addHue(uint8_t hue1, uint8_t hue2);
static uint8_t subtractHue(uint8_t hue1, uint8_t hue2);
static uint8_t addSaturation(uint8_t saturation1, uint8_t saturation2);
static uint8_t subtractSaturation(uint8_t saturation1, uint8_t saturation2);
static void initHueSat(uint8_t endpoint);
static uint8_t readHue(uint8_t endpoint);
static uint8_t readSaturation(uint8_t endpoint);
#endif

#ifdef SUPPORT_CIE_1931
static uint16_t findNewColorValueFromStep(uint16_t oldValue, int16_t step);
static uint16_t readColorX(uint8_t endpoint);
static uint16_t readColorY(uint8_t endpoint);
#endif

static uint16_t computeTransitionTimeFromStateAndRate(Color16uTransitionState *p,
                                                      uint16_t rate);

// convenient token handling functions
static uint8_t readColorMode(uint8_t endpoint)
{
  uint8_t colorMode;

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfReadServerAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID,
                                       (uint8_t *)&colorMode,
                                       sizeof(uint8_t)));

  return colorMode;
}

static uint16_t readColorTemperature(uint8_t endpoint)
{
  uint16_t colorTemperature;

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfReadServerAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_ATTRIBUTE_ID,
                                       (uint8_t *)&colorTemperature,
                                       sizeof(uint16_t)));

  return colorTemperature;
}

static uint16_t readColorTemperatureMin(uint8_t endpoint)
{
  uint16_t colorTemperatureMin;
  EmberStatus status;

  status =
    emberAfReadServerAttribute(endpoint,
                               ZCL_COLOR_CONTROL_CLUSTER_ID,
                               ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_ATTRIBUTE_ID,
                               (uint8_t *)&colorTemperatureMin,
                               sizeof(uint16_t));

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    colorTemperatureMin = MIN_TEMPERATURE_VALUE;
  }

  return colorTemperatureMin;
}

static uint16_t readColorTemperatureMax(uint8_t endpoint)
{
  uint16_t colorTemperatureMax;
  EmberStatus status;

  status =
    emberAfReadServerAttribute(endpoint,
                               ZCL_COLOR_CONTROL_CLUSTER_ID,
                               ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_ATTRIBUTE_ID,
                               (uint8_t *)&colorTemperatureMax,
                               sizeof(uint16_t));

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    colorTemperatureMax = MAX_TEMPERATURE_VALUE;
  }

  return colorTemperatureMax;
}

static uint16_t readColorTemperatureCoupleToLevelMin(uint8_t endpoint)
{
  uint16_t colorTemperatureCoupleToLevelMin;
  EmberStatus status;

  status =
    emberAfReadServerAttribute(endpoint,
                               ZCL_COLOR_CONTROL_CLUSTER_ID,
                               ZCL_COLOR_CONTROL_TEMPERATURE_LEVEL_MIN_MIREDS_ATTRIBUTE_ID,
                               (uint8_t *)&colorTemperatureCoupleToLevelMin,
                               sizeof(uint16_t));

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    // Not less than the physical min.
    colorTemperatureCoupleToLevelMin = readColorTemperatureMin(endpoint);
  }

  return colorTemperatureCoupleToLevelMin;
}

static uint8_t readLevelControlCurrentLevel(uint8_t endpoint)
{
  uint8_t currentLevel;
  EmberStatus status;

  status =
    emberAfReadServerAttribute(endpoint,
                               ZCL_LEVEL_CONTROL_CLUSTER_ID,
                               ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
                               (uint8_t *)&currentLevel,
                               sizeof(uint8_t));

  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    currentLevel = 0x7F; // midpoint of range 0x01-0xFE
  }

  return currentLevel;
}

static void writeRemainingTime(uint8_t endpoint, uint16_t remainingTime)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE_ID,
                                        (uint8_t *)&remainingTime,
                                        ZCL_INT16U_ATTRIBUTE_TYPE));
}

static void writeColorMode(uint8_t endpoint, uint8_t colorMode)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE_ID,
                                        (uint8_t *)&colorMode,
                                        ZCL_INT8U_ATTRIBUTE_TYPE));

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID,
                                        (uint8_t *)&colorMode,
                                        ZCL_INT8U_ATTRIBUTE_TYPE));
}

static void writeHue(uint8_t endpoint, uint8_t hue)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                        (uint8_t *)&hue,
                                        ZCL_INT8U_ATTRIBUTE_TYPE));
}

static void writeSaturation(uint8_t endpoint, uint8_t saturation)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                        (uint8_t *)&saturation,
                                        ZCL_INT8U_ATTRIBUTE_TYPE));
}

static void writeColorX(uint8_t endpoint, uint16_t colorX)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_CURRENT_X_ATTRIBUTE_ID,
                                        (uint8_t *)&colorX,
                                        ZCL_INT16U_ATTRIBUTE_TYPE));
}

static void writeColorY(uint8_t endpoint, uint16_t colorY)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_CURRENT_Y_ATTRIBUTE_ID,
                                        (uint8_t *)&colorY,
                                        ZCL_INT16U_ATTRIBUTE_TYPE));
}

static void writeColorTemperature(uint8_t endpoint, uint16_t colorTemperature)
{
  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfWriteServerAttribute(endpoint,
                                        ZCL_COLOR_CONTROL_CLUSTER_ID,
                                        ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_ATTRIBUTE_ID,
                                        (uint8_t *)&colorTemperature,
                                        ZCL_INT16U_ATTRIBUTE_TYPE));
}

// -------------------------------------------------------------------------
// ****** callback section *******

// Templated to the event_init context.
void emAfPluginColorControlServerInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  slxu_zigbee_event_init(&emberAfPluginColorControlServerTempTransitionEvent,
                         emberAfPluginColorControlServerTempTransitionEventHandler);
  slxu_zigbee_event_init(&emberAfPluginColorControlServerXyTransitionEvent,
                         emberAfPluginColorControlServerXyTransitionEventHandler);
  slxu_zigbee_event_init(&emberAfPluginColorControlServerHueSatTransitionEvent,
                         emberAfPluginColorControlServerHueSatTransitionEventHandler);
}

#ifdef SUPPORT_HUE_SATURATION

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveToHueAndSaturationCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveToHueAndSaturationCallback(uint8_t hue,
                                                              uint8_t saturation,
                                                              uint16_t transitionTime,
                                                              uint8_t optionsMask,
                                                              uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t currentHue = readHue(endpoint);
  bool moveUp;

#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_to_hue_and_saturation_command_t cmd_data;
  uint8_t hue;
  uint8_t saturation;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_to_hue_and_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  hue = cmd_data.hue;
  saturation = cmd_data.saturation;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (transitionTime == 0) {
    transitionTime++;
  }

  // limit checking:  hue and saturation are 0..254.  Spec dictates we ignore
  // this and report a malformed packet.
  if (hue > MAX_HUE_VALUE || saturation > MAX_SATURATION_VALUE) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // compute shortest direction
  if (hue > currentHue) {
    moveUp = (hue - currentHue) < HALF_MAX_UINT8T;
  } else {
    moveUp = (currentHue - hue) > HALF_MAX_UINT8T;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.initialHue = currentHue;
  colorHueTransitionState.currentHue = currentHue;
  colorHueTransitionState.finalHue   = hue;
  colorHueTransitionState.stepsRemaining = transitionTime;
  colorHueTransitionState.stepsTotal     = transitionTime;
  colorHueTransitionState.endpoint = endpoint;
  colorHueTransitionState.up = moveUp;
  colorHueTransitionState.repeat = false;

  colorSaturationTransitionState.initialValue = readSaturation(endpoint);
  colorSaturationTransitionState.currentValue = readSaturation(endpoint);
  colorSaturationTransitionState.finalValue = saturation;
  colorSaturationTransitionState.stepsRemaining = transitionTime;
  colorSaturationTransitionState.stepsTotal     = transitionTime;
  colorSaturationTransitionState.endpoint       = endpoint;
  colorSaturationTransitionState.lowLimit  = MIN_SATURATION_VALUE;
  colorSaturationTransitionState.highLimit = MAX_SATURATION_VALUE;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveHueCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveHueCallback(uint8_t moveMode,
                                               uint8_t rate,
                                               uint8_t optionsMask,
                                               uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_hue_command_t cmd_data;
  uint8_t moveMode;
  uint8_t rate;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_hue_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  moveMode = cmd_data.moveMode;
  rate = cmd_data.rate;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

#ifdef EMBER_TEST
  emberAfColorControlClusterPrintln("ColorControl: MoveHue (%x, %x)",
                                    moveMode,
                                    rate);
#endif

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (moveMode == EMBER_ZCL_HUE_MOVE_MODE_STOP) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.initialHue = readHue(endpoint);
  colorHueTransitionState.currentHue = readHue(endpoint);
  if (moveMode == EMBER_ZCL_HUE_MOVE_MODE_UP) {
    colorHueTransitionState.finalHue = addHue(readHue(endpoint),
                                              rate);
    colorHueTransitionState.up = true;
  } else if (moveMode == EMBER_ZCL_HUE_MOVE_MODE_DOWN) {
    colorHueTransitionState.finalHue = subtractHue(readHue(endpoint),
                                                   rate);
    colorHueTransitionState.up = false;
  } else {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }
  colorHueTransitionState.stepsRemaining = TRANSITION_TIME_1S;
  colorHueTransitionState.stepsTotal     = TRANSITION_TIME_1S;
  colorHueTransitionState.endpoint = endpoint;
  colorHueTransitionState.repeat = true;
  // hue movement can last forever.  Indicate this with a remaining time of
  // maxint.
  writeRemainingTime(endpoint, MAX_INT16U_VALUE);

  colorSaturationTransitionState.stepsRemaining = 0;

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveSaturationCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveSaturationCallback(uint8_t moveMode,
                                                      uint8_t rate,
                                                      uint8_t optionsMask,
                                                      uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_saturation_command_t cmd_data;
  uint8_t moveMode;
  uint8_t rate;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  moveMode = cmd_data.moveMode;
  rate = cmd_data.rate;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint16_t transitionTime;

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (moveMode == EMBER_ZCL_SATURATION_MOVE_MODE_STOP
      || rate == 0) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.stepsRemaining = 0;

  colorSaturationTransitionState.initialValue = readSaturation(endpoint);
  colorSaturationTransitionState.currentValue = readSaturation(endpoint);
  if (moveMode == EMBER_ZCL_SATURATION_MOVE_MODE_UP) {
    colorSaturationTransitionState.finalValue = MAX_SATURATION_VALUE;
  } else {
    colorSaturationTransitionState.finalValue = MIN_SATURATION_VALUE;
  }

  transitionTime =
    computeTransitionTimeFromStateAndRate(&colorSaturationTransitionState,
                                          rate);

  colorSaturationTransitionState.stepsRemaining = transitionTime;
  colorSaturationTransitionState.stepsTotal     = transitionTime;
  colorSaturationTransitionState.endpoint       = endpoint;
  colorSaturationTransitionState.lowLimit  = MIN_SATURATION_VALUE;
  colorSaturationTransitionState.highLimit = MAX_SATURATION_VALUE;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveToHueCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveToHueCallback(uint8_t hue,
                                                 uint8_t hueMoveMode,
                                                 uint16_t transitionTime,
                                                 uint8_t optionsMask,
                                                 uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_to_hue_command_t cmd_data;
  uint8_t hue;
  uint8_t hueMoveMode;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_to_hue_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  hue = cmd_data.hue;
  hueMoveMode = cmd_data.direction;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint8_t currentHue = readHue(endpoint);
  uint8_t direction;

  if (transitionTime == 0) {
    transitionTime++;
  }

  // limit checking:  hue and saturation are 0..254.  Spec dictates we ignore
  // this and report a malformed packet.
  if (hue > MAX_HUE_VALUE) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  // For move to hue, the move modes are different from the other move commands.
  // Need to translate from the move to hue transitions to the internal
  // representation.
  switch (hueMoveMode) {
    case EMBER_ZCL_HUE_DIRECTION_SHORTEST_DISTANCE:
      if ( ((uint8_t) (currentHue - hue)) > HALF_MAX_UINT8T ) {
        direction = MOVE_MODE_UP;
      } else {
        direction = MOVE_MODE_DOWN;
      }
      break;
    case EMBER_ZCL_HUE_DIRECTION_LONGEST_DISTANCE:
      if ( ((uint8_t) (currentHue - hue)) > HALF_MAX_UINT8T ) {
        direction = MOVE_MODE_DOWN;
      } else {
        direction = MOVE_MODE_UP;
      }
      break;
      break;
    case EMBER_ZCL_HUE_DIRECTION_UP:
      direction = MOVE_MODE_UP;
      break;
    case EMBER_ZCL_HUE_DIRECTION_DOWN:
      direction = MOVE_MODE_DOWN;
      break;
    default:
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
      return true;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.initialHue = readHue(endpoint);
  colorHueTransitionState.currentHue = readHue(endpoint);
  colorHueTransitionState.finalHue   = hue;
  colorHueTransitionState.stepsRemaining = transitionTime;
  colorHueTransitionState.stepsTotal     = transitionTime;
  colorHueTransitionState.endpoint = endpoint;
  colorHueTransitionState.up = (direction == MOVE_MODE_UP);
  colorHueTransitionState.repeat = false;

  colorSaturationTransitionState.stepsRemaining = 0;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveToSaturationCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveToSaturationCallback(uint8_t saturation,
                                                        uint16_t transitionTime,
                                                        uint8_t optionsMask,
                                                        uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_to_saturation_command_t cmd_data;
  uint8_t saturation;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_to_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  saturation = cmd_data.saturation;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  if (transitionTime == 0) {
    transitionTime++;
  }

  // limit checking:  hue and saturation are 0..254.  Spec dictates we ignore
  // this and report a malformed packet.
  if (saturation > MAX_SATURATION_VALUE) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.stepsRemaining = 0;

  colorSaturationTransitionState.initialValue = readSaturation(endpoint);
  colorSaturationTransitionState.currentValue = readSaturation(endpoint);
  colorSaturationTransitionState.finalValue = saturation;
  colorSaturationTransitionState.stepsRemaining = transitionTime;
  colorSaturationTransitionState.stepsTotal     = transitionTime;
  colorSaturationTransitionState.endpoint       = endpoint;
  colorSaturationTransitionState.lowLimit  = MIN_SATURATION_VALUE;
  colorSaturationTransitionState.highLimit = MAX_SATURATION_VALUE;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterStepHueCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterStepHueCallback(uint8_t stepMode,
                                               uint8_t stepSize,
                                               uint8_t transitionTime,
                                               uint8_t optionsMask,
                                               uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_step_hue_command_t cmd_data;
  uint8_t stepMode;
  uint8_t stepSize;
  uint8_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_step_hue_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  stepMode = cmd_data.stepMode;
  stepSize = cmd_data.stepSize;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint8_t currentHue = readHue(endpoint);

  if (transitionTime == 0) {
    transitionTime++;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (stepMode == MOVE_MODE_STOP) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.initialHue = currentHue;
  colorHueTransitionState.currentHue = currentHue;

  if (stepMode == MOVE_MODE_UP) {
    colorHueTransitionState.finalHue = addHue(currentHue, stepSize);
    colorHueTransitionState.up = true;
  } else {
    colorHueTransitionState.finalHue = subtractHue(currentHue, stepSize);
    colorHueTransitionState.up = false;
  }
  colorHueTransitionState.stepsRemaining = transitionTime;
  colorHueTransitionState.stepsTotal     = transitionTime;
  colorHueTransitionState.endpoint = endpoint;
  colorHueTransitionState.repeat = false;

  colorSaturationTransitionState.stepsRemaining = 0;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterStepSaturationCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterStepSaturationCallback(uint8_t stepMode,
                                                      uint8_t stepSize,
                                                      uint8_t transitionTime,
                                                      uint8_t optionsMask,
                                                      uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_step_saturation_command_t cmd_data;
  uint8_t stepMode;
  uint8_t stepSize;
  uint8_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_step_saturation_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  stepMode = cmd_data.stepMode;
  stepSize = cmd_data.stepSize;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.transitionTime;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint8_t currentSaturation = readSaturation(endpoint);

  if (transitionTime == 0) {
    transitionTime++;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (stepMode == MOVE_MODE_STOP) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_HSV);

  // now, kick off the state machine.
  initHueSat(endpoint);

  colorHueTransitionState.stepsRemaining = 0;

  colorSaturationTransitionState.initialValue = currentSaturation;
  colorSaturationTransitionState.currentValue = currentSaturation;

  if (stepMode == MOVE_MODE_UP) {
    colorSaturationTransitionState.finalValue = addSaturation(currentSaturation,
                                                              stepSize);
  } else {
    colorSaturationTransitionState.finalValue =
      subtractSaturation(currentSaturation,
                         stepSize);
  }
  colorSaturationTransitionState.stepsRemaining = transitionTime;
  colorSaturationTransitionState.stepsTotal     = transitionTime;
  colorSaturationTransitionState.endpoint       = endpoint;
  colorSaturationTransitionState.lowLimit  = MIN_SATURATION_VALUE;
  colorSaturationTransitionState.highLimit = MAX_SATURATION_VALUE;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

static uint8_t addSaturation(uint8_t saturation1, uint8_t saturation2)
{
  uint16_t saturation16;

  saturation16 =  ((uint16_t) saturation1);
  saturation16 += ((uint16_t) saturation2);

  if (saturation16 > MAX_SATURATION_VALUE) {
    saturation16 = MAX_SATURATION_VALUE;
  }

  return ((uint8_t) saturation16);
}

static uint8_t subtractSaturation(uint8_t saturation1, uint8_t saturation2)
{
  if (saturation2 > saturation1) {
    return 0;
  }

  return saturation1 - saturation2;
}

// any time we call a hue or saturation transition, we need to assume certain
// things about the hue and saturation data structures.  This function will
// properly initialize them.
static void initHueSat(uint8_t endpoint)
{
  colorHueTransitionState.stepsRemaining = 0;
  colorHueTransitionState.currentHue = readHue(endpoint);
  colorHueTransitionState.endpoint   = endpoint;

  colorSaturationTransitionState.stepsRemaining = 0;
  colorSaturationTransitionState.currentValue = readSaturation(endpoint);
  colorSaturationTransitionState.endpoint     = endpoint;
}

static uint8_t readHue(uint8_t endpoint)
{
  uint8_t hue;

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfReadServerAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE_ID,
                                       (uint8_t *)&hue,
                                       sizeof(uint8_t)));

  return hue;
}

static uint8_t readSaturation(uint8_t endpoint)
{
  uint8_t saturation;

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfReadServerAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE_ID,
                                       (uint8_t *)&saturation,
                                       sizeof(uint8_t)));

  return saturation;
}

#endif  // SUPPORT_HUE_SATURATION

#ifdef SUPPORT_CIE_1931

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveToColorCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveToColorCallback(uint16_t colorX,
                                                   uint16_t colorY,
                                                   uint16_t transitionTime,
                                                   uint8_t optionsMask,
                                                   uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_to_color_command_t cmd_data;
  uint16_t colorX;
  uint16_t colorY;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_to_color_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  colorX = cmd_data.colorX;
  colorY = cmd_data.colorY;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  if (transitionTime == 0) {
    transitionTime++;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_CIE_XY);

  // now, kick off the state machine.
  colorXTransitionState.initialValue = readColorX(endpoint);
  colorXTransitionState.currentValue = readColorX(endpoint);
  colorXTransitionState.finalValue = colorX;
  colorXTransitionState.stepsRemaining = transitionTime;
  colorXTransitionState.stepsTotal = transitionTime;
  colorXTransitionState.endpoint = endpoint;
  colorXTransitionState.lowLimit  = MIN_CIE_XY_VALUE;
  colorXTransitionState.highLimit = MAX_CIE_XY_VALUE;

  colorYTransitionState.initialValue = readColorY(endpoint);
  colorYTransitionState.currentValue = readColorY(endpoint);
  colorYTransitionState.finalValue = colorY;
  colorYTransitionState.stepsRemaining = transitionTime;
  colorYTransitionState.stepsTotal = transitionTime;
  colorYTransitionState.endpoint = endpoint;
  colorYTransitionState.lowLimit  = MIN_CIE_XY_VALUE;
  colorYTransitionState.highLimit = MAX_CIE_XY_VALUE;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_XY_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveColorCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveColorCallback(int16_t rateX,
                                                 int16_t rateY,
                                                 uint8_t optionsMask,
                                                 uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_color_command_t cmd_data;
  uint16_t rateX;
  uint16_t rateY;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_color_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  rateX = cmd_data.rateX;
  rateY = cmd_data.rateY;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint16_t transitionTimeX, transitionTimeY;
  uint16_t unsignedRate;

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (rateX == 0 && rateY == 0) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_CIE_XY);

  // now, kick off the state machine.
  colorXTransitionState.initialValue = readColorX(endpoint);
  colorXTransitionState.currentValue = colorXTransitionState.initialValue;
  if (rateX > 0) {
    colorXTransitionState.finalValue = MAX_CIE_XY_VALUE;
    unsignedRate = (uint16_t) rateX;
  } else {
    colorXTransitionState.finalValue = MIN_CIE_XY_VALUE;
    unsignedRate = (uint16_t) (rateX * -1);
  }
  transitionTimeX =
    computeTransitionTimeFromStateAndRate(&colorXTransitionState,
                                          unsignedRate);
  colorXTransitionState.stepsRemaining = transitionTimeX;
  colorXTransitionState.stepsTotal = transitionTimeX;
  colorXTransitionState.endpoint = endpoint;
  colorXTransitionState.lowLimit  = MIN_CIE_XY_VALUE;
  colorXTransitionState.highLimit = MAX_CIE_XY_VALUE;

  colorYTransitionState.initialValue = readColorY(endpoint);
  colorYTransitionState.currentValue = colorYTransitionState.initialValue;
  if (rateY > 0) {
    colorYTransitionState.finalValue = MAX_CIE_XY_VALUE;
    unsignedRate = (uint16_t) rateY;
  } else {
    colorYTransitionState.finalValue = MIN_CIE_XY_VALUE;
    unsignedRate = (uint16_t) (rateY * -1);
  }
  transitionTimeY =
    computeTransitionTimeFromStateAndRate(&colorYTransitionState,
                                          unsignedRate);
  colorYTransitionState.stepsRemaining = transitionTimeY;
  colorYTransitionState.stepsTotal = transitionTimeY;
  colorYTransitionState.endpoint = endpoint;
  colorYTransitionState.lowLimit  = MIN_CIE_XY_VALUE;
  colorYTransitionState.highLimit = MAX_CIE_XY_VALUE;

  if (transitionTimeX < transitionTimeY) {
    writeRemainingTime(endpoint, transitionTimeX);
  } else {
    writeRemainingTime(endpoint, transitionTimeY);
  }

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_XY_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterStepColorCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterStepColorCallback(int16_t stepX,
                                                 int16_t stepY,
                                                 uint16_t transitionTime,
                                                 uint8_t optionsMask,
                                                 uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_step_color_command_t cmd_data;
  uint16_t stepX;
  uint16_t stepY;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_step_color_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  stepX = cmd_data.stepX;
  stepY = cmd_data.stepY;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint16_t colorX =
    findNewColorValueFromStep(readColorX(endpoint), stepX);
  uint16_t colorY =
    findNewColorValueFromStep(readColorY(endpoint), stepY);

  if (transitionTime == 0) {
    transitionTime++;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_CIE_XY);

  // now, kick off the state machine.
  colorXTransitionState.initialValue = readColorX(endpoint);
  colorXTransitionState.currentValue = readColorX(endpoint);
  colorXTransitionState.finalValue = colorX;
  colorXTransitionState.stepsRemaining = transitionTime;
  colorXTransitionState.stepsTotal = transitionTime;
  colorXTransitionState.endpoint = endpoint;
  colorXTransitionState.lowLimit  = MIN_CIE_XY_VALUE;
  colorXTransitionState.highLimit = MAX_CIE_XY_VALUE;

  colorYTransitionState.initialValue = readColorY(endpoint);
  colorYTransitionState.currentValue = readColorY(endpoint);
  colorYTransitionState.finalValue = colorY;
  colorYTransitionState.stepsRemaining = transitionTime;
  colorYTransitionState.stepsTotal = transitionTime;
  colorYTransitionState.endpoint = endpoint;
  colorYTransitionState.lowLimit  = MIN_CIE_XY_VALUE;
  colorYTransitionState.highLimit = MAX_CIE_XY_VALUE;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_XY_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

static uint16_t findNewColorValueFromStep(uint16_t oldValue, int16_t step)
{
  uint16_t newValue;
  int32_t  newValueSigned;

  newValueSigned = ((int32_t) oldValue) + ((int32_t) step);

  if (newValueSigned < 0) {
    newValue = 0;
  } else if (newValueSigned > MAX_CIE_XY_VALUE) {
    newValue = MAX_CIE_XY_VALUE;
  } else {
    newValue = (uint16_t) newValueSigned;
  }

  return newValue;
}

static uint16_t readColorX(uint8_t endpoint)
{
  uint16_t colorX;

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfReadServerAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_CURRENT_X_ATTRIBUTE_ID,
                                       (uint8_t *)&colorX,
                                       sizeof(uint16_t)));

  return colorX;
}

static uint16_t readColorY(uint8_t endpoint)
{
  uint16_t colorY;

  assert(EMBER_ZCL_STATUS_SUCCESS
         == emberAfReadServerAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_CURRENT_Y_ATTRIBUTE_ID,
                                       (uint8_t *)&colorY,
                                       sizeof(uint16_t)));

  return colorY;
}

#endif  // SUPPORT_CIE_1931

#ifdef SUPPORT_COLOR_TEMPERATURE

static void moveToColorTemp(uint8_t endpoint,
                            uint16_t colorTemperature,
                            uint16_t transitionTime)
{
  uint16_t temperatureMin = readColorTemperatureMin(endpoint);
  uint16_t temperatureMax = readColorTemperatureMax(endpoint);

  if (transitionTime == 0) {
    transitionTime++;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_TEMPERATURE);

  if (colorTemperature < temperatureMin) {
    colorTemperature = temperatureMin;
  }

  if (colorTemperature > temperatureMax) {
    colorTemperature = temperatureMax;
  }

  // now, kick off the state machine.
  colorTempTransitionState.initialValue = readColorTemperature(endpoint);
  colorTempTransitionState.currentValue = readColorTemperature(endpoint);
  colorTempTransitionState.finalValue   = colorTemperature;
  colorTempTransitionState.stepsRemaining = transitionTime;
  colorTempTransitionState.stepsTotal     = transitionTime;
  colorTempTransitionState.endpoint  = endpoint;
  colorTempTransitionState.lowLimit  = temperatureMin;
  colorTempTransitionState.highLimit = temperatureMax;

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_TEMP_CONTROL, UPDATE_TIME_MS);
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveToColorTemperatureCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterMoveToColorTemperatureCallback(uint16_t colorTemperature,
                                                              uint16_t transitionTime,
                                                              uint8_t optionsMask,
                                                              uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_to_color_temperature_command_t cmd_data;
  uint16_t colorTemperature;
  uint16_t transitionTime;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_to_color_temperature_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  colorTemperature = cmd_data.colorTemperature;
  transitionTime = cmd_data.transitionTime;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  moveToColorTemp(endpoint, colorTemperature, transitionTime);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterMoveColorTemperatureCallback(EmberAfClusterCommand *cmd)
#else // UC_BUILD
bool emberAfColorControlClusterMoveColorTemperatureCallback(uint8_t moveMode,
                                                            uint16_t rate,
                                                            uint16_t colorTemperatureMinimum,
                                                            uint16_t colorTemperatureMaximum,
                                                            uint8_t optionsMask,
                                                            uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_move_color_temperature_command_t cmd_data;
  uint8_t moveMode;
  uint16_t rate;
  uint16_t colorTemperatureMinimum;
  uint16_t colorTemperatureMaximum;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_move_color_temperature_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  moveMode = cmd_data.moveMode;
  rate = cmd_data.rate;
  colorTemperatureMinimum = cmd_data.colorTemperatureMinimum;
  colorTemperatureMaximum = cmd_data.colorTemperatureMaximum;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint16_t tempPhysicalMin = readColorTemperatureMin(endpoint);
  uint16_t tempPhysicalMax = readColorTemperatureMax(endpoint);
  uint16_t transitionTime;

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (moveMode == MOVE_MODE_STOP) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  if (rate == 0) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_FIELD);
    return true;
  }

  if (colorTemperatureMinimum < tempPhysicalMin) {
    colorTemperatureMinimum = tempPhysicalMin;
  }
  if (colorTemperatureMaximum > tempPhysicalMax) {
    colorTemperatureMaximum = tempPhysicalMax;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_TEMPERATURE);

  // now, kick off the state machine.
  colorTempTransitionState.initialValue = readColorTemperature(endpoint);
  colorTempTransitionState.currentValue = readColorTemperature(endpoint);
  if (moveMode == MOVE_MODE_UP) {
    if (tempPhysicalMax > colorTemperatureMaximum) {
      colorTempTransitionState.finalValue = colorTemperatureMaximum;
    } else {
      colorTempTransitionState.finalValue = tempPhysicalMax;
    }
  } else {
    if (tempPhysicalMin < colorTemperatureMinimum) {
      colorTempTransitionState.finalValue = colorTemperatureMinimum;
    } else {
      colorTempTransitionState.finalValue = tempPhysicalMin;
    }
  }
  transitionTime =
    computeTransitionTimeFromStateAndRate(&colorTempTransitionState,
                                          rate);
  colorTempTransitionState.stepsRemaining = transitionTime;
  colorTempTransitionState.stepsTotal     = transitionTime;
  colorTempTransitionState.endpoint  = endpoint;
  colorTempTransitionState.lowLimit  = colorTemperatureMinimum;
  colorTempTransitionState.highLimit = colorTemperatureMaximum;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_TEMP_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#ifdef UC_BUILD
bool emberAfColorControlClusterStepColorTemperatureCallback(EmberAfClusterCommand *cmd)
#else // !UC_BUILD
bool emberAfColorControlClusterStepColorTemperatureCallback(uint8_t stepMode,
                                                            uint16_t stepSize,
                                                            uint16_t transitionTime,
                                                            uint16_t colorTemperatureMinimum,
                                                            uint16_t colorTemperatureMaximum,
                                                            uint8_t optionsMask,
                                                            uint8_t optionsOverride)
#endif // UC_BUILD
{
  uint8_t endpoint = emberAfCurrentEndpoint();
#ifdef UC_BUILD
  sl_zcl_color_control_cluster_step_color_temperature_command_t cmd_data;
  uint8_t stepMode;
  uint16_t stepSize;
  uint16_t transitionTime;
  uint16_t colorTemperatureMinimum;
  uint16_t colorTemperatureMaximum;
  uint8_t optionsMask;
  uint8_t optionsOverride;

  if (zcl_decode_color_control_cluster_step_color_temperature_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_MALFORMED_COMMAND);
    return true;
  }

  stepMode = cmd_data.stepMode;
  stepSize = cmd_data.stepSize;
  transitionTime = cmd_data.transitionTime;
  colorTemperatureMinimum = cmd_data.colorTemperatureMinimum;
  colorTemperatureMaximum = cmd_data.colorTemperatureMaximum;
  optionsMask = cmd_data.optionsMask;
  optionsOverride = cmd_data.optionsOverride;
#endif // UC_BUILD

  if (!shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  uint16_t tempPhysicalMin = readColorTemperatureMin(endpoint);
  uint16_t tempPhysicalMax = readColorTemperatureMax(endpoint);

  if (transitionTime == 0) {
    transitionTime++;
  }

  // New command.  Need to stop any active transitions.
  stopAllColorTransitions();

  if (stepMode == MOVE_MODE_STOP) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    return true;
  }

  if (colorTemperatureMinimum < tempPhysicalMin) {
    colorTemperatureMinimum = tempPhysicalMin;
  }
  if (colorTemperatureMaximum > tempPhysicalMax) {
    colorTemperatureMaximum = tempPhysicalMax;
  }

  // Handle color mode transition, if necessary.
  handleModeSwitch(endpoint, COLOR_MODE_TEMPERATURE);

  // now, kick off the state machine.
  colorTempTransitionState.initialValue = readColorTemperature(endpoint);
  colorTempTransitionState.currentValue = readColorTemperature(endpoint);
  if (stepMode == MOVE_MODE_UP) {
    colorTempTransitionState.finalValue
      = readColorTemperature(endpoint) + stepSize;
  } else {
    colorTempTransitionState.finalValue
      = readColorTemperature(endpoint) - stepSize;
  }
  colorTempTransitionState.stepsRemaining = transitionTime;
  colorTempTransitionState.stepsTotal     = transitionTime;
  colorTempTransitionState.endpoint  = endpoint;
  colorTempTransitionState.lowLimit  = colorTemperatureMinimum;
  colorTempTransitionState.highLimit = colorTemperatureMaximum;

  writeRemainingTime(endpoint, transitionTime);

  // kick off the state machine:
  slxu_zigbee_event_set_delay_ms(COLOR_TEMP_CONTROL, UPDATE_TIME_MS);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

void emberAfPluginLevelControlCoupledColorTempChangeCallback(uint8_t endpoint)
{
  // ZCL 5.2.2.1.1 Coupling color temperature to Level Control
  //
  // If the Level Control for Lighting cluster identifier 0x0008 is supported
  // on the same endpoint as the Color Control cluster and color temperature is
  // supported, it is possible to couple changes in the current level to the
  // color temperature.
  //
  // The CoupleColorTempToLevel bit of the Options attribute of the Level
  // Control cluster indicates whether the color temperature is to be linked
  // with the CurrentLevel attribute in the Level Control cluster.
  //
  // If the CoupleColorTempToLevel bit of the Options attribute of the Level
  // Control cluster is equal to 1 and the ColorMode or EnhancedColorMode
  // attribute is set to 0x02 (color temperature) then a change in the
  // CurrentLevel attribute SHALL affect the ColorTemperatureMireds attribute.
  // This relationship is manufacturer specific, with the qualification that
  // the maximum value of the CurrentLevel attribute SHALL correspond to a
  // ColorTemperatureMired attribute value equal to the
  // CoupleColorTempToLevelMinMireds attribute. This relationship is one-way so
  // a change to the ColorTemperatureMireds attribute SHALL NOT have any effect
  // on the CurrentLevel attribute.
  //
  // In order to simulate the behavior of an incandescent bulb, a low value of
  // the CurrentLevel attribute SHALL be associated with a high value of the
  // ColorTemperatureMireds attribute (i.e., a low value of color temperature
  // in kelvins).
  //
  // If the CoupleColorTempToLevel bit of the Options attribute of the Level
  // Control cluster is equal to 0, there SHALL be no link between color
  // temperature and current level.

  if (!emberAfContainsServer(endpoint, ZCL_COLOR_CONTROL_CLUSTER_ID)) {
    return;
  }

  if (readColorMode(endpoint) == COLOR_MODE_TEMPERATURE) {
    uint16_t tempCoupleMin = readColorTemperatureCoupleToLevelMin(endpoint);
    uint16_t tempPhysMax = readColorTemperatureMax(endpoint);
    uint8_t currentLevel = readLevelControlCurrentLevel(endpoint);

    // Scale color temp setting between the coupling min and the physical max.
    // Note that mireds varies inversely with level: low level -> high mireds.
    // Peg min/MAX level to MAX/min mireds, otherwise interpolate.
    uint16_t newColorTemp;
    if (currentLevel <= MIN_CURRENT_LEVEL) {
      newColorTemp = tempPhysMax;
    } else if (currentLevel >= MAX_CURRENT_LEVEL) {
      newColorTemp = tempCoupleMin;
    } else {
      uint32_t tempDelta = (((uint32_t)tempPhysMax - (uint32_t)tempCoupleMin)
                            * currentLevel)
                           / (uint32_t)(MAX_CURRENT_LEVEL - MIN_CURRENT_LEVEL + 1);
      newColorTemp = (uint16_t)((uint32_t)tempPhysMax - tempDelta);
    }

    // Apply new color temp.
    moveToColorTemp(endpoint, newColorTemp, 0);
  }
}

#endif // SUPPORT_COLOR_TEMPERATURE

bool emberAfColorControlClusterStopMoveStepCallback(uint8_t optionsMask,
                                                    uint8_t optionsOverride)
{
  // Received a stop command.  This is all we need to do.
  uint8_t endpoint = emberAfCurrentEndpoint();

  if (shouldExecuteIfOff(endpoint, optionsMask, optionsOverride)) {
    stopAllColorTransitions();
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

// **************** transition state machines ***********

static void stopAllColorTransitions(void)
{
  slxu_zigbee_event_set_inactive(COLOR_TEMP_CONTROL);
  slxu_zigbee_event_set_inactive(COLOR_XY_CONTROL);
  slxu_zigbee_event_set_inactive(COLOR_HSV_CONTROL);
}

void emberAfPluginColorControlServerStopTransition(void)
{
  stopAllColorTransitions();
}

// The specification says that if we are transitioning from one color mode
// into another, we need to compute the new mode's attribute values from the
// old mode.  However, it also says that if the old mode doesn't translate into
// the new mode, this must be avoided.
// I am putting in this function to compute the new attributes based on the old
// color mode.
static void handleModeSwitch(uint8_t endpoint,
                             uint8_t newColorMode)
{
  uint8_t oldColorMode = readColorMode(endpoint);
  uint8_t colorModeTransition;

  if (oldColorMode == newColorMode) {
    return;
  } else {
    writeColorMode(endpoint, newColorMode);
  }

  colorModeTransition = (newColorMode << 4) + oldColorMode;

  // Note:  It may be OK to not do anything here.
  switch (colorModeTransition) {
    case HSV_TO_CIE_XY:
      emberAfPluginColorControlServerComputePwmFromXyCallback(endpoint);
      break;
    case TEMPERATURE_TO_CIE_XY:
      emberAfPluginColorControlServerComputePwmFromXyCallback(endpoint);
      break;
    case CIE_XY_TO_HSV:
      emberAfPluginColorControlServerComputePwmFromHsvCallback(endpoint);
      break;
    case TEMPERATURE_TO_HSV:
      emberAfPluginColorControlServerComputePwmFromHsvCallback(endpoint);
      break;
    case HSV_TO_TEMPERATURE:
      emberAfPluginColorControlServerComputePwmFromTempCallback(endpoint);
      break;
    case CIE_XY_TO_TEMPERATURE:
      emberAfPluginColorControlServerComputePwmFromTempCallback(endpoint);
      break;

    // for the following cases, there is no transition.
    case HSV_TO_HSV:
    case CIE_XY_TO_CIE_XY:
    case TEMPERATURE_TO_TEMPERATURE:
    default:
      return;
  }
}

static uint8_t addHue(uint8_t hue1, uint8_t hue2)
{
  uint16_t hue16;

  hue16 =  ((uint16_t) hue1);
  hue16 += ((uint16_t) hue2);

  if (hue16 > MAX_HUE_VALUE) {
    hue16 -= MAX_HUE_VALUE;
  }

  return ((uint8_t) hue16);
}

static uint8_t subtractHue(uint8_t hue1, uint8_t hue2)
{
  uint16_t hue16;

  hue16 =  ((uint16_t) hue1);
  if (hue2 > hue1) {
    hue16 += MAX_HUE_VALUE;
  }

  hue16 -= ((uint16_t) hue2);

  return ((uint8_t) hue16);
}

static bool computeNewHueValue(ColorHueTransitionState *p)
{
  uint32_t newHue32;
  uint8_t newHue;

  // exit with a false if hue is not currently moving
  if (p->stepsRemaining == 0) {
    return false;
  }

  (p->stepsRemaining)--;

  if (p->repeat == false) {
    writeRemainingTime(p->endpoint, p->stepsRemaining);
  }

  // are we going up or down?
  if (p->finalHue == p->currentHue) {
    // do nothing
  } else if (p->up) {
    newHue32 = (uint32_t) subtractHue(p->finalHue, p->initialHue);
    newHue32 *= ((uint32_t) (p->stepsRemaining));
    newHue32 /= ((uint32_t) (p->stepsTotal));
    p->currentHue = subtractHue((uint8_t) p->finalHue,
                                (uint8_t) newHue32);
  } else {
    newHue32 = (uint32_t) subtractHue(p->initialHue, p->finalHue);
    newHue32 *= ((uint32_t) (p->stepsRemaining));
    newHue32 /= ((uint32_t) (p->stepsTotal));

    p->currentHue = addHue((uint8_t) p->finalHue,
                           (uint8_t) newHue32);
  }

  if (p->stepsRemaining == 0) {
    if (p->repeat == false) {
      // we are performing a move to and not a move.
      return true;
    } else {
      // we are performing a Hue move.  Need to compute the new values for the
      // next move period.
      if (p->up) {
        newHue = subtractHue(p->finalHue, p->initialHue);
        newHue = addHue(p->finalHue, newHue);

        p->initialHue = p->finalHue;
        p->finalHue = newHue;
      } else {
        newHue = subtractHue(p->initialHue, p->finalHue);
        newHue = subtractHue(p->finalHue, newHue);

        p->initialHue = p->finalHue;
        p->finalHue = newHue;
      }
      p->stepsRemaining = TRANSITION_TIME_1S;
    }
  }
  return false;
}

void emberAfPluginColorControlServerHueSatTransitionEventHandler(SLXU_UC_EVENT)
{
  uint8_t endpoint = colorHueTransitionState.endpoint;
  bool limitReached1, limitReached2;

  limitReached1 = computeNewHueValue(&colorHueTransitionState);
  limitReached2 = computeNewColor16uValue(&colorSaturationTransitionState);

  if (limitReached1 || limitReached2) {
    stopAllColorTransitions();
  } else {
    slxu_zigbee_event_set_delay_ms(COLOR_HSV_CONTROL, UPDATE_TIME_MS);
  }

  writeHue(colorHueTransitionState.endpoint,
           colorHueTransitionState.currentHue);
  writeSaturation(colorSaturationTransitionState.endpoint,
                  (uint8_t) colorSaturationTransitionState.currentValue);

  emberAfColorControlClusterPrintln("Hue %d Saturation %d endpoint %d",
                                    colorHueTransitionState.currentHue,
                                    colorSaturationTransitionState.currentValue,
                                    endpoint);

  emberAfPluginColorControlServerComputePwmFromHsvCallback(endpoint);
}

// Return value of true means we need to stop.
static bool computeNewColor16uValue(Color16uTransitionState *p)
{
  uint32_t newValue32u;

  if (p->stepsRemaining == 0) {
    return false;
  }

  (p->stepsRemaining)--;

  writeRemainingTime(p->endpoint, p->stepsRemaining);

  // handle sign
  if (p->finalValue == p->currentValue) {
    // do nothing
  } else if (p->finalValue > p->initialValue) {
    newValue32u = ((uint32_t) (p->finalValue - p->initialValue));
    newValue32u *= ((uint32_t) (p->stepsRemaining));
    newValue32u /= ((uint32_t) (p->stepsTotal));
    p->currentValue = p->finalValue - ((uint16_t) (newValue32u));
  } else {
    newValue32u = ((uint32_t) (p->initialValue - p->finalValue));
    newValue32u *= ((uint32_t) (p->stepsRemaining));
    newValue32u /= ((uint32_t) (p->stepsTotal));
    p->currentValue = p->finalValue + ((uint16_t) (newValue32u));
  }

  if (p->stepsRemaining == 0) {
    // we have completed our move.
    return true;
  }

  return false;
}

static uint16_t computeTransitionTimeFromStateAndRate(Color16uTransitionState *p,
                                                      uint16_t rate)
{
  uint32_t transitionTime;
  uint16_t max, min;

  if (rate == 0) {
    return MAX_INT16U_VALUE;
  }

  if (p->currentValue > p->finalValue) {
    max = p->currentValue;
    min = p->finalValue;
  } else {
    max = p->finalValue;
    min = p->currentValue;
  }

  transitionTime = max - min;
  transitionTime *= 10;
  transitionTime /= rate;

  if (transitionTime > MAX_INT16U_VALUE) {
    return MAX_INT16U_VALUE;
  }

  return (uint16_t) transitionTime;
}

void emberAfPluginColorControlServerXyTransitionEventHandler(SLXU_UC_EVENT)
{
  uint8_t endpoint = colorXTransitionState.endpoint;
  bool limitReachedX, limitReachedY;

  // compute new values for X and Y.
  limitReachedX = computeNewColor16uValue(&colorXTransitionState);

  limitReachedY = computeNewColor16uValue(&colorYTransitionState);

  if (limitReachedX || limitReachedY) {
    stopAllColorTransitions();
  } else {
    slxu_zigbee_event_set_delay_ms(COLOR_XY_CONTROL, UPDATE_TIME_MS);
  }

  // update the attributes
  writeColorX(colorXTransitionState.endpoint,
              colorXTransitionState.currentValue);
  writeColorY(colorXTransitionState.endpoint,
              colorYTransitionState.currentValue);

  emberAfColorControlClusterPrintln("Color X %d Color Y %d",
                                    colorXTransitionState.currentValue,
                                    colorYTransitionState.currentValue);

  emberAfPluginColorControlServerComputePwmFromXyCallback(endpoint);
}

void emberAfPluginColorControlServerTempTransitionEventHandler(SLXU_UC_EVENT)
{
  uint8_t endpoint = colorTempTransitionState.endpoint;
  bool limitReached;

  limitReached = computeNewColor16uValue(&colorTempTransitionState);

  if (limitReached) {
    stopAllColorTransitions();
  } else {
    slxu_zigbee_event_set_delay_ms(COLOR_TEMP_CONTROL, UPDATE_TIME_MS);
  }

  writeColorTemperature(colorTempTransitionState.endpoint,
                        colorTempTransitionState.currentValue);

  emberAfColorControlClusterPrintln("Color Temperature %d",
                                    colorTempTransitionState.currentValue);

  emberAfPluginColorControlServerComputePwmFromTempCallback(endpoint);
}

static bool shouldExecuteIfOff(uint8_t endpoint,
                               uint8_t optionMask,
                               uint8_t optionOverride)
{
  // From 5.2.2.2.1.10 of ZCL7 document 14-0129-15f-zcl-ch-5-lighting.docx:
  //   "Command execution SHALL NOT continue beyond the Options processing if
  //    all of these criteria are true:
  //      - The On/Off cluster exists on the same endpoint as this cluster.
  //      - The OnOff attribute of the On/Off cluster, on this endpoint, is 0x00
  //        (FALSE).
  //      - The value of the ExecuteIfOff bit is 0."

  if (!emberAfContainsServer(endpoint, ZCL_ON_OFF_CLUSTER_ID)) {
    return true;
  }

  uint8_t options;
  EmberAfStatus status = emberAfReadServerAttribute(endpoint,
                                                    ZCL_COLOR_CONTROL_CLUSTER_ID,
                                                    ZCL_OPTIONS_ATTRIBUTE_ID,
                                                    &options,
                                                    sizeof(options));
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("Unable to read Options attribute: 0x%X",
                                      status);
    // If we can't read the attribute, then we should just assume that it has
    // its default value.
    options = 0x00;
  }

  bool on;
  status = emberAfReadServerAttribute(endpoint,
                                      ZCL_ON_OFF_CLUSTER_ID,
                                      ZCL_ON_OFF_ATTRIBUTE_ID,
                                      (uint8_t *)&on,
                                      sizeof(on));
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfColorControlClusterPrintln("Unable to read OnOff attribute: 0x%X",
                                      status);
    return true;
  }
  // The device is on - hence ExecuteIfOff does not matter
  if (on) {
    return true;
  }
  // The OptionsMask & OptionsOverride fields SHALL both be present or both
  // omitted in the command. A temporary Options bitmap SHALL be created from
  // the Options attribute, using the OptionsMask & OptionsOverride fields, if
  // present. Each bit of the temporary Options bitmap SHALL be determined as
  // follows:
  // Each bit in the Options attribute SHALL determine the corresponding bit in
  // the temporary Options bitmap, unless the OptionsMask field is present and
  // has the corresponding bit set to 1, in which case the corresponding bit in
  // the OptionsOverride field SHALL determine the corresponding bit in the
  // temporary Options bitmap.
  //The resulting temporary Options bitmap SHALL then be processed as defined
  // in section 5.2.2.2.1.10.

  // ---------- The following order is important in decision making -------
  // -----------more readable ----------
  //
  if (optionMask == 0xFF && optionOverride == 0xFF) {
    // 0xFF are the default values passed to the command handler when
    // the payload is not present - in that case there is use of option
    // attribute to decide execution of the command
    return READBITS(options, EMBER_ZCL_COLOR_CONTROL_OPTIONS_EXECUTE_IF_OFF);
  }
  // ---------- The above is to distinguish if the payload is present or not

  if (READBITS(optionMask, EMBER_ZCL_COLOR_CONTROL_OPTIONS_EXECUTE_IF_OFF)) {
    // Mask is present and set in the command payload, this indicates
    // use the override as temporary option
    return READBITS(optionOverride, EMBER_ZCL_COLOR_CONTROL_OPTIONS_EXECUTE_IF_OFF);
  }
  // if we are here - use the option attribute bits
  return (READBITS(options, EMBER_ZCL_COLOR_CONTROL_OPTIONS_EXECUTE_IF_OFF));
}

void emberAfColorControlClusterServerInitCallback(uint8_t endpoint)
{
#ifdef SUPPORT_COLOR_TEMPERATURE
  // 07-5123-07 (i.e. ZCL 7) 5.2.2.2.1.22 StartUpColorTemperatureMireds Attribute
  // The StartUpColorTemperatureMireds attribute SHALL define the desired startup color
  // temperature values a lamp SHAL use when it is supplied with power and this value SHALL
  // be reflected in the ColorTemperatureMireds attribute. In addition, the ColorMode and
  // EnhancedColorMode attributes SHALL be set to 0x02 (color temperature). The values of
  // the StartUpColorTemperatureMireds attribute are listed in the table below.
  // Value                Action on power up
  // 0x0000-0xffef        Set the ColorTemperatureMireds attribute to this value.
  // 0xffff               Set the ColorTemperatureMireds attribue to its previous value.

  // Initialize startUpColorTempMireds to "maintain previous value" value 0xFFFF
  uint16_t startUpColorTemp = 0xFFFF;
  EmberAfStatus status = emberAfReadAttribute(endpoint,
                                              ZCL_COLOR_CONTROL_CLUSTER_ID,
                                              ZCL_START_UP_COLOR_TEMPERATURE_MIREDS_ATTRIBUTE_ID,
                                              CLUSTER_MASK_SERVER,
                                              (uint8_t *)&startUpColorTemp,
                                              sizeof(startUpColorTemp),
                                              NULL);
  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    uint16_t updatedColorTemp = MAX_TEMPERATURE_VALUE;
    status = emberAfReadAttribute(endpoint,
                                  ZCL_COLOR_CONTROL_CLUSTER_ID,
                                  ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_ATTRIBUTE_ID,
                                  CLUSTER_MASK_SERVER,
                                  (uint8_t *)&updatedColorTemp,
                                  sizeof(updatedColorTemp),
                                  NULL);
    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      uint16_t tempPhysicalMin = readColorTemperatureMin(endpoint);
      uint16_t tempPhysicalMax = readColorTemperatureMax(endpoint);
      if (tempPhysicalMin <= startUpColorTemp && startUpColorTemp <= tempPhysicalMax) {
        // Apply valid startup color temp value that is within physical limits of device.
        // Otherwise, the startup value is outside the device's supported range, and the
        // existing setting of ColorTemp attribute will be left unchanged (i.e., treated as
        // if startup color temp was set to 0xFFFF).
        updatedColorTemp = startUpColorTemp;
        status = emberAfWriteAttribute(endpoint,
                                       ZCL_COLOR_CONTROL_CLUSTER_ID,
                                       ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_ATTRIBUTE_ID,
                                       CLUSTER_MASK_SERVER,
                                       (uint8_t *)&updatedColorTemp,
                                       ZCL_INT16U_ATTRIBUTE_TYPE);
        if (status == EMBER_ZCL_STATUS_SUCCESS) {
          // Set ColorMode attributes to reflect ColorTemperature.
          uint8_t updateColorMode = EMBER_ZCL_COLOR_MODE_COLOR_TEMPERATURE;
          status = emberAfWriteAttribute(endpoint,
                                         ZCL_COLOR_CONTROL_CLUSTER_ID,
                                         ZCL_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE_ID,
                                         CLUSTER_MASK_SERVER,
                                         &updateColorMode,
                                         ZCL_ENUM8_ATTRIBUTE_TYPE);
          updateColorMode = EMBER_ZCL_ENHANCED_COLOR_MODE_COLOR_TEMPERATURE;
          status = emberAfWriteAttribute(endpoint,
                                         ZCL_COLOR_CONTROL_CLUSTER_ID,
                                         ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE_ID,
                                         CLUSTER_MASK_SERVER,
                                         &updateColorMode,
                                         ZCL_ENUM8_ATTRIBUTE_TYPE);
        }
      }
    }
  }
#endif
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
  #ifdef SUPPORT_HUE_SATURATION
      case ZCL_STEP_HUE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterStepHueCallback(cmd);
        break;
      }
      case ZCL_MOVE_TO_HUE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToHueCallback(cmd);
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
      case ZCL_MOVE_HUE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveHueCallback(cmd);
        break;
      }
      case ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToHueAndSaturationCallback(cmd);
        break;
      }
      case ZCL_MOVE_TO_SATURATION_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToSaturationCallback(cmd);
        break;
      }
  #endif // SUPPORT_HUE_SATURATION
  #ifdef SUPPORT_CIE_1931
      case ZCL_MOVE_TO_COLOR_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToColorCallback(cmd);
        break;
      }
      case ZCL_MOVE_COLOR_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveColorCallback(cmd);
        break;
      }
      case ZCL_STEP_COLOR_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterStepColorCallback(cmd);
        break;
      }
  #endif // SUPPORT_CIE_1931
  #ifdef SUPPORT_COLOR_TEMPERATURE
      case ZCL_MOVE_TO_COLOR_TEMPERATURE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveToColorTemperatureCallback(cmd);
        break;
      }
      case ZCL_MOVE_COLOR_TEMPERATURE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterMoveColorTemperatureCallback(cmd);
        break;
      }
      case ZCL_STEP_COLOR_TEMPERATURE_COMMAND_ID:
      {
        wasHandled = emberAfColorControlClusterStepColorTemperatureCallback(cmd);
        break;
      }
  #endif // SUPPORT_COLOR_TEMPERATURE
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
