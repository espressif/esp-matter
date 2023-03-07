/***************************************************************************//**
 * @file
 * @brief User Interface rendering for DMP demo
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

#include PLATFORM_HEADER
#include <stdio.h>

#include "sl_board_control.h"
#include "sl_simple_button_instances.h"
#include "em_assert.h"
#include "glib.h"
#include "dmd.h"
#include "app/framework/include/af.h"

#include <string.h>
#include <stdio.h>
#include "sl_dmp_bitmaps.h"
#include "sl_dmp_ui.h"
#include "app/util/common/uc-temp-macros.h"
#include "zigbee_device_config.h"

#define helpmenu_line1     "      **HELP**       "
#define helpmenu_line2     "PB0 - Toggle Light   "
#define helpmenu_line3     "PB1 - NWK Control    "
#define helpmenu_line6     " Press>3s: Leave NWK "

#define helpmenu_line4_light     " No NWK : Form NWK   "
#define helpmenu_line5_light     " NWK    : Permit join"

#define helpmenu_line4_lightSed  " No NWK : Join NWK   "
#define helpmenu_line5_lightSed  " NWK    : Identify   "

#define TMP_STR_LEN        9
#define BLE_NAME_LEN       10
#define APP_NAME_LEN       20
#define DEV_NAME_LEN       20

#ifdef UC_BUILD
sl_zigbee_event_t lcdPermitJoinEvent;
#define permitJoinEvent (&lcdPermitJoinEvent)
#else // !UC_BUILD
extern EmberEventControl lcdPermitJoinEventControl;
#define permitJoinEvent (lcdPermitJoinEventControl)
#endif // UC_BUILD

GLIB_Context_t glibContext;          /* Global glib context */

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static bool isBluetoothConnected = false;
static sl_dmp_ui_light_state_t light1State = DMP_UI_LIGHT_OFF;
static sl_dmp_ui_light_state_t light2State = DMP_UI_LIGHT_OFF;
static DmpUiLightDirection_t lightDirection = DMP_UI_DIRECTION_INVALID;
static bool helpMenuDisplayed = false;
static uint16_t dmpUiDirectDisplayStartTime = 0;
static char bleName[BLE_NAME_LEN] = { 0 };
static char deviceName[DEV_NAME_LEN];
static uint8_t eventTimeLeft;
static bool panIdDisplayToggle = false;
static bool blockPanIdDisplay = false;
DmpUiZigBeeNetworkState_t last_nwState = DMP_UI_STATE_UNKNOWN;
void lcdPermitJoinEventHandler(SLXU_UC_EVENT);
/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static void dmpUiUpdateZigbeeStatus(DmpUiZigBeeNetworkState_t nwState,
                                    bool withDisplayUpdate)
{
  int32_t xPosition = 2;
  char tempStr[TMP_STR_LEN] = { 0 };
  char *pTempStr = tempStr;

  if (!helpMenuDisplayed) {
    EmberPanId panId = emberAfGetPanId();

    if (nwState == DMP_UI_STATE_UNKNOWN) {
      if (panId == 0xFFFF) {
        switch (last_nwState) {
          case DMP_UI_JOINING:
          case DMP_UI_FORMING:
            nwState = last_nwState;
            break;
          default:
            nwState = DMP_UI_NO_NETWORK;
            break;
        }
      } else {
        EmberNetworkStatus status = emberAfNetworkState();
        switch (status) {
          case EMBER_JOINED_NETWORK:
            nwState = DMP_UI_NETWORK_UP;
            break;
          case EMBER_JOINED_NETWORK_NO_PARENT:
            nwState = DMP_UI_LOST_NETWORK;
            break;
          default:
            nwState = DMP_UI_NO_NETWORK;
            break;
        }
      }
    }

    switch (nwState) {
      case DMP_UI_NO_NETWORK:
        dmpUiDirectDisplayStartTime = 0;
        pTempStr = "No Nwk";
        break;
      case DMP_UI_LOST_NETWORK:
        dmpUiDirectDisplayStartTime = 0;
        pTempStr = "Lost Nwk";
        break;
      case DMP_UI_SCANNING:
        pTempStr = "Scanning";
        break;
      case DMP_UI_JOINING:
        pTempStr = "Joining";
        break;
      case DMP_UI_FORMING:
        pTempStr = "Forming";
        break;
      case DMP_UI_DISCOVERING:
        pTempStr = "Discvrng";
        break;
      case DMP_UI_NETWORK_UP:
        tempStr[0] = 'P';
        tempStr[1] = 'A';
        tempStr[2] = 'N';
        tempStr[3] = ':';
        tempStr[4] = ascii_lut[(panId & 0xF000) >> 12];
        tempStr[5] = ascii_lut[(panId & 0x0F00) >>  8];
        tempStr[6] = ascii_lut[(panId & 0x00F0) >>  4];
        tempStr[7] = ascii_lut[(panId & 0x000F)];
        tempStr[8] = '\0';
        break;
      default:
        break;
    }

    GLIB_drawString(&glibContext, pTempStr,
                    strlen(tempStr) + 1, xPosition, glibContext.pDisplayGeometry->ySize - 10, 0);

    if (withDisplayUpdate) {
      DMD_updateDisplay();
    }
    last_nwState = nwState;
  }
}

static void dmpUiDisplayLogo(void)
{
  GLIB_drawBitmap(&glibContext,
                  SILICONLABS_X_POSITION,
                  SILICONLABS_Y_POSITION,
                  SILICONLABS_BITMAP_WIDTH,
                  SILICONLABS_BITMAP_HEIGHT,
                  siliconlabsBitmap);
}

static void dmpUiDisplayZigbeeLogo(void)
{
  int32_t xPosition = ZIGBEE_X_POSITION;
  int32_t yPosition = ZIGBEE_Y_POSITION;

  GLIB_drawBitmap(&glibContext,
                  xPosition,
                  yPosition,
                  ZIGBEE_BITMAP_WIDTH,
                  ZIGBEE_BITMAP_HEIGHT,
                  zigbeeBitmap);

  if (!blockPanIdDisplay) {
    dmpUiUpdateZigbeeStatus(DMP_UI_STATE_UNKNOWN, false);
  }
}

static void dmpUiDisplayBluetoothLogo(void)
{
  if (strlen(bleName)) {
    GLIB_drawString(&glibContext, bleName,
                    strlen(bleName) + 1, 79, glibContext.pDisplayGeometry->ySize - 10, 0);
  }

  if (isBluetoothConnected) {
    GLIB_drawBitmap(&glibContext,
                    BLUETOOTH_X_POSITION,
                    BLUETOOTH_Y_POSITION,
                    BLUETOOTH_BITMAP_WIDTH,
                    BLUETOOTH_BITMAP_HEIGHT,
                    bluetoothConnectedBitmap);
  } else {
    GLIB_drawBitmap(&glibContext,
                    BLUETOOTH_X_POSITION,
                    BLUETOOTH_Y_POSITION,
                    BLUETOOTH_BITMAP_WIDTH,
                    BLUETOOTH_BITMAP_HEIGHT,
                    bluetoothBitmap);
  }
}

static void dmpUiDisplayAppName(const char *device)
{
  char appName[APP_NAME_LEN] = "DMP Demo ";
  strncpy(&appName[9], device, APP_NAME_LEN - 9);

  GLIB_drawString(&glibContext, appName,
                  strlen(appName) + 1, 16, SILICONLABS_BITMAP_HEIGHT + 2, 0);
}

static void dmpUiDisplayDirection(DmpUiLightDirection_t direction)
{
  if (direction == DMP_UI_DIRECTION_ZIGBEE) {
    GLIB_drawLine(&glibContext,
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 5),
                  (ZIGBEE_Y_POSITION + ZIGBEE_BITMAP_HEIGHT / 2),
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 15),
                  (ZIGBEE_Y_POSITION + ZIGBEE_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 12),
                  (ZIGBEE_Y_POSITION + (ZIGBEE_BITMAP_HEIGHT / 2) - 3),
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 15),
                  (ZIGBEE_Y_POSITION + ZIGBEE_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 12),
                  (ZIGBEE_Y_POSITION + (ZIGBEE_BITMAP_HEIGHT / 2) + 3),
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 15),
                  (ZIGBEE_Y_POSITION + (ZIGBEE_BITMAP_HEIGHT / 2)));
  } else if (direction == DMP_UI_DIRECTION_BLUETOOTH) {
    GLIB_drawLine(&glibContext,
                  (BLUETOOTH_X_POSITION - 5),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2),
                  (BLUETOOTH_X_POSITION - 15),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  ((BLUETOOTH_X_POSITION - 15) + 3),
                  ((BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2) - 3),
                  (BLUETOOTH_X_POSITION - 15),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  ((BLUETOOTH_X_POSITION - 15) + 3),
                  ((BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2) + 3),
                  (BLUETOOTH_X_POSITION - 15),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2));
  }

  lightDirection = direction;

  DMD_updateDisplay();
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void sl_dmp_ui_init(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(permitJoinEvent, lcdPermitJoinEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA:
    {
      light1State = DMP_UI_LIGHT_OFF;
      light2State = DMP_UI_LIGHT_OFF;

      if (SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE <= SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER) {
        strncpy(deviceName, "Light", DEV_NAME_LEN);
      } else if (SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE == SLI_ZIGBEE_NETWORK_DEVICE_TYPE_SLEEPY_END_DEVICE) {
        strncpy(deviceName, "LightSed", DEV_NAME_LEN);
      } else {
        strncpy(deviceName, "Unknown", DEV_NAME_LEN);
      }
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_DONE:
    {
      uint32_t status;
      /* Enable the memory lcd */
      status = sl_board_enable_display();
      EFM_ASSERT(status == SL_STATUS_OK);

      /* Initialize the DMD support for memory lcd display */
      status = DMD_init(0);
      EFM_ASSERT(status == DMD_OK);

      /* Initialize the glib context */
      status = GLIB_contextInit(&glibContext);
      EFM_ASSERT(status == GLIB_OK);

      glibContext.backgroundColor = White;
      glibContext.foregroundColor = Black;

      /* Fill lcd with background color */
      GLIB_clear(&glibContext);

      status = GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
      sl_dmp_ui_display_help();
      break;
    }
  }
}

void sl_dmp_ui_light_on(void)
{
  helpMenuDisplayed = false;
  GLIB_clear(&glibContext);

  light1State = DMP_UI_LIGHT_ON;
  GLIB_drawBitmap(&glibContext,
                  LIGHT_X_POSITION,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  lightOnBitMap);

  dmpUiDisplayLogo();
  dmpUiDisplayAppName(deviceName);
  dmpUiDisplayZigbeeLogo();

  dmpUiDisplayBluetoothLogo();

  DMD_updateDisplay();
}

void dmpUiLightUpdateLight(sl_dmp_ui_light_state_t updateLight1, sl_dmp_ui_light_state_t updateLight2)
{
  if (updateLight1 != DMP_UI_LIGHT_UNCHANGED) {
    light1State = updateLight1;
  }

  if (updateLight2 != DMP_UI_LIGHT_UNCHANGED) {
    light2State = updateLight2;
  }

  const uint8_t *picLight1 = (light1State == DMP_UI_LIGHT_ON) ? lightOnBitMap : lightOffBitMap;
  const uint8_t *picLight2 = (light2State == DMP_UI_LIGHT_ON) ? lightOnBitMap : lightOffBitMap;

  helpMenuDisplayed = false;
  GLIB_clear(&glibContext);
  GLIB_drawBitmap(&glibContext,
                  0,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  picLight1);

  GLIB_drawBitmap(&glibContext,
                  64,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  picLight2);

  dmpUiDisplayLogo();
  dmpUiDisplayAppName(deviceName);
  dmpUiDisplayZigbeeLogo();
  DMD_updateDisplay();
}

void sl_dmp_ui_light_off(void)
{
  helpMenuDisplayed = false;
  GLIB_clear(&glibContext);

  light1State = DMP_UI_LIGHT_OFF;
  GLIB_drawBitmap(&glibContext,
                  LIGHT_X_POSITION,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  lightOffBitMap);
  dmpUiDisplayLogo();
  dmpUiDisplayAppName(deviceName);
  dmpUiDisplayZigbeeLogo();

  dmpUiDisplayBluetoothLogo();

  DMD_updateDisplay();
}

void sl_dmp_ui_display_help(void)
{
  uint8_t y_position = SILICONLABS_BITMAP_HEIGHT + 20;
  char *line1, *line2, *line3, *line4, *line5, *line6;

  line1 = helpmenu_line1;
  line2 = helpmenu_line2;
  line3 = helpmenu_line3;
  line6 = helpmenu_line6;

  helpMenuDisplayed = true;

  dmpUiDisplayLogo();
  dmpUiDisplayAppName(deviceName);

  //Only lines 4 & 5 in the help menu are different for the 2 device types
  if (SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE <= SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER) {
    line4 = helpmenu_line4_light;
    line5 = helpmenu_line5_light;
  } else if (SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE == SLI_ZIGBEE_NETWORK_DEVICE_TYPE_SLEEPY_END_DEVICE) {
    line4 = helpmenu_line4_lightSed;
    line5 = helpmenu_line5_lightSed;
  } else {
  }

  GLIB_drawString(&glibContext, line1, strlen(line1) + 1, 2, y_position, 0);
  GLIB_drawString(&glibContext, line2, strlen(line2) + 1, 2, y_position + 10, 0);
  GLIB_drawString(&glibContext, line3, strlen(line3) + 1, 2, y_position + 20, 0);
  GLIB_drawString(&glibContext, line4, strlen(line4) + 1, 2, y_position + 30, 0);
  GLIB_drawString(&glibContext, line5, strlen(line5) + 1, 2, y_position + 40, 0);
  GLIB_drawString(&glibContext, line6, strlen(line6) + 1, 2, y_position + 50, 0);

  DMD_updateDisplay();
}

void sl_dmp_ui_zigbee_permit_join(bool enable)
{
  eventTimeLeft = (enable) ? emberGetPermitJoining() : 0;
  panIdDisplayToggle = false;
  blockPanIdDisplay = true;

  if ( enable ) {
    slxu_zigbee_event_set_delay_ms(permitJoinEvent, DMP_UI_PJOIN_EVENT_DURATION);
  }
}

void sl_dmp_ui_update_direction(DmpUiLightDirection_t direction)
{
  dmpUiDisplayDirection(direction);

  dmpUiDirectDisplayStartTime = halCommonGetInt16uMillisecondTick();

  slxu_zigbee_event_set_delay_ms(permitJoinEvent, DMP_UI_PJOIN_EVENT_DURATION);
}

void sl_dmp_ui_bluetooth_connected(bool connectionState)
{
  if ( !helpMenuDisplayed) {
    isBluetoothConnected = connectionState;
    dmpUiDisplayBluetoothLogo();
    DMD_updateDisplay();
  }
}

void lcdPermitJoinEventHandler(SLXU_UC_EVENT)
{
  if (!helpMenuDisplayed) {
    if (SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE <= SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER) {
      eventTimeLeft = emberGetPermitJoining();
    } else {
      //for non-COO devices, app will indicate when to stop
    }

    dmpUiClrLcdDisplayMainScreen();

    if (panIdDisplayToggle) {
      dmpUiUpdateZigbeeStatus(DMP_UI_STATE_UNKNOWN, true);
    }
    panIdDisplayToggle = !panIdDisplayToggle; // toggle panId display flag.

    if (dmpUiDirectDisplayStartTime != 0) {
      if ((halCommonGetInt16uMillisecondTick() - dmpUiDirectDisplayStartTime) < 1000) {
        dmpUiDisplayDirection(lightDirection);
      } else {
        dmpUiDirectDisplayStartTime = 0;
      }
    }

    if (eventTimeLeft || (dmpUiDirectDisplayStartTime != 0)) {
      slxu_zigbee_event_set_delay_ms(permitJoinEvent, DMP_UI_PJOIN_EVENT_DURATION);
    } else {
      blockPanIdDisplay = false;
      panIdDisplayToggle = false;
      dmpUiUpdateZigbeeStatus(DMP_UI_STATE_UNKNOWN, true);
      slxu_zigbee_event_set_inactive(permitJoinEvent);
    }
  } else {
    slxu_zigbee_event_set_delay_ms(permitJoinEvent, DMP_UI_PJOIN_EVENT_DURATION);
  }
}

void sl_dmp_ui_display_zigbee_state(DmpUiZigBeeNetworkState_t nwState)
{
  if (!helpMenuDisplayed) {
    bool restoreBlockPanId = blockPanIdDisplay;
    blockPanIdDisplay = true;

    dmpUiClrLcdDisplayMainScreen();

    dmpUiUpdateZigbeeStatus(nwState, true);

    blockPanIdDisplay = restoreBlockPanId;
  }
}

void dmpUiClrLcdDisplayMainScreen(void)
{
  if (light1State == DMP_UI_LIGHT_OFF) {
    sl_dmp_ui_light_off();
  } else {
    sl_dmp_ui_light_on();
  }
}

void sl_dmp_ui_set_ble_device_name(char *devName)
{
  strncpy(bleName, devName, BLE_NAME_LEN);
}

void sl_dmp_ui_set_light_direction(DmpUiLightDirection_t direction)
{
  lightDirection = direction;
}
DmpUiLightDirection_t sl_dmp_ui_get_light_direction(void)
{
  return lightDirection;
}
