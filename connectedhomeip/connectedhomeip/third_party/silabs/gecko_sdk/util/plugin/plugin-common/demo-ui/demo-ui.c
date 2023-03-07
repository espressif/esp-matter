/***************************************************************************//**
 * @file
 * @brief User Interface core logic for demo.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_types.h"
#include "glib.h"
#include "dmd/dmd.h"
#include <string.h>
#include <stdio.h>
#include "demo-ui.h"
#include "demo-ui-bitmaps.h"

#define PROT_ZIGBEE     (1)
#define PROT_RAIL       (2)
#define PROT_CONNECT    (3)
#define PROT_BLUETOOTH  (4)
#define PROT_THREAD     (5)

#ifdef EMBER_STACK_ZIGBEE
#define PROT1           (PROT_ZIGBEE)
#elif EMBER_STACK_CONNECT
#define PROT1           (PROT_CONNECT)
#elif EMBER_STACK_IP
#define PROT1           (PROT_THREAD)
#else
#define PROT1           (PROT_RAIL)
#endif

#define PROT2           (PROT_BLUETOOTH)

#ifndef DEVICE_TYPE
#define DEVICE_TYPE "Light"
#endif

#define helpmenu_line1_light  "      **HELP**       "
#define helpmenu_line2_light  " PB0 - Toggle Light  "
#define helpmenu_line3_light  " PB1 - NWK Control   "
#define helpmenu_line4_light  " No NWK : Form NWK   "
#define helpmenu_line5_light  " NWK    : Permit join"
#define helpmenu_line6_light  " Press>5s: Leave NWK "

#define helpmenu_line1_switch "      **HELP**       "
#define helpmenu_line2_switch " PB0 - Toggle Light  "
#define helpmenu_line3_switch " PB1 - NWK Control   "
#define helpmenu_line4_switch " No NWK:Join NWK     "
#define helpmenu_line5_switch " Press>5s: Leave NWK "
#define helpmenu_line6_switch "                     "

#define helpmenu_line1_no_nwk "      **HELP**       "
#define helpmenu_line2_no_nwk " PB0 - Toggle Light  "
#define helpmenu_line3_no_nwk " PB1 - Toggle Light  "
#define helpmenu_line4_no_nwk "                     "
#define helpmenu_line5_no_nwk "                     "
#define helpmenu_line6_no_nwk "                     "

#define SILICONLABS_X_POSITION          ((glibContext.pDisplayGeometry->xSize - SILICONLABS_BITMAP_WIDTH) / 2)
#define SILICONLABS_Y_POSITION          0
#define LIGHT_BITMAP_WIDTH              64
#define LIGHT_BITMAP_HEIGHT             64
#define LIGHT_X_POSITION                ((glibContext.pDisplayGeometry->xSize - LIGHT_BITMAP_WIDTH) / 2)
#define LIGHT_Y_POSITION                (glibContext.pDisplayGeometry->ySize - LIGHT_BITMAP_HEIGHT - 5)
#define PROT1_ID_X_POSITION             1
#define PROT2_ID_X_POSITION             79

#ifdef PROT_ZIGBEE
#if (PROT1 == PROT_ZIGBEE)
#define PROT1_BITMAP_WIDTH          ZIGBEE_BITMAP_WIDTH
#define PROT1_BITMAP_HEIGHT         ZIGBEE_BITMAP_HEIGHT
#define PROT1_X_POSITION            8
#define PROT1_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2)) + 2
#define PROT1_BITMAP                (zigbeeBitmap)
#define PROT1_BITMAP_CONN           (zigbeeConnectBitmap)
#elif (PROT2 == PROT_ZIGBEE)
#define PROT2_BITMAP_WIDTH          ZIGBEE_BITMAP_WIDTH
#define PROT2_BITMAP_HEIGHT         ZIGBEE_BITMAP_HEIGHT
#define PROT2_X_POSITION            104
#define PROT2_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2)) + 2
#define PROT2_BITMAP                (zigbeeBitmap)
#define PROT2_BITMAP_CONN           (zigbeeConnectedBitmap)
#endif // PROT1 | PROT2
#endif // PROT_ZIGBEE

#ifdef PROT_RAIL
#if (PROT1 == PROT_RAIL)
#define PROT1_BITMAP_WIDTH          RAIL_BITMAP_WIDTH
#define PROT1_BITMAP_HEIGHT         RAIL_BITMAP_HEIGHT
#define PROT1_X_POSITION            8
#define PROT1_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2)) + 2
#define PROT1_BITMAP                (railBitmap)
#define PROT1_BITMAP_CONN           (railConnectedBitmap)
#elif (PROT2 == PROT_RAIL)
#define PROT2_BITMAP_WIDTH          RAIL_BITMAP_WIDTH
#define PROT2_BITMAP_HEIGHT         RAIL_BITMAP_HEIGHT
#define PROT2_X_POSITION            104
#define PROT2_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2)) + 2
#define PROT2_BITMAP                (railBitmap)
#define PROT2_BITMAP_CONN           (railConnectedBitmap)
#endif // PROT1 | PROT2
#endif // PROT_RAIL

#ifdef PROT_CONNECT
#if (PROT1 == PROT_CONNECT)
#define PROT1_BITMAP_WIDTH          CONNECT_BITMAP_WIDTH
#define PROT1_BITMAP_HEIGHT         CONNECT_BITMAP_HEIGHT
#define PROT1_X_POSITION            8
#define PROT1_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2)) + 2
#define PROT1_BITMAP                (connectBitmap)
#define PROT1_BITMAP_CONN           (connectConnectedBitmap)
#elif (PROT2 == PROT_CONNECT)
#define PROT2_BITMAP_WIDTH          CONNECT_BITMAP_WIDTH
#define PROT2_BITMAP_HEIGHT         CONNECT_BITMAP_HEIGHT
#define PROT2_X_POSITION            104
#define PROT2_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2)) + 2
#define PROT2_BITMAP                (connectBitmap)
#define PROT2_BITMAP_CONN           (connectConnectedBitmap)
#endif // PROT1 | PROT2
#endif // PROT_CONNECTS

#ifdef PROT_BLUETOOTH
#if (PROT1 == PROT_BLUETOOTH)
#define PROT1_BITMAP_WIDTH          BLUETOOTH_BITMAP_WIDTH
#define PROT1_BITMAP_HEIGHT         BLUETOOTH_BITMAP_HEIGHT
#define PROT1_X_POSITION            8
#define PROT1_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2))
#define PROT1_BITMAP                (bluetoothBitmap)
#define PROT1_BITMAP_CONN           (bluetoothConnectedBitmap)
#elif (PROT2 == PROT_BLUETOOTH)
#define PROT2_BITMAP_WIDTH          BLUETOOTH_BITMAP_WIDTH
#define PROT2_BITMAP_HEIGHT         BLUETOOTH_BITMAP_HEIGHT
#define PROT2_X_POSITION            104
#define PROT2_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2))
#define PROT2_BITMAP                (bluetoothBitmap)
#define PROT2_BITMAP_CONN           (bluetoothConnectedBitmap)
#endif // PROT1 | PROT2
#endif // PROT_BLUETOOTH

#ifdef PROT_THREAD
#if (PROT1 == PROT_THREAD)
#define PROT1_BITMAP_WIDTH          THREAD_BITMAP_WIDTH
#define PROT1_BITMAP_HEIGHT         THREAD_BITMAP_HEIGHT
#define PROT1_X_POSITION            8
#define PROT1_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2))
#define PROT1_BITMAP                (threadBitmap)
#define PROT1_BITMAP_CONN           (threadBitmap)
#elif (PROT2 == PROT_THREAD)
#define PROT2_BITMAP_WIDTH          THREAD_BITMAP_WIDTH
#define PROT2_BITMAP_HEIGHT         THREAD_BITMAP_HEIGHT
#define PROT2_X_POSITION            104
#define PROT2_Y_POSITION            (LIGHT_Y_POSITION + (LIGHT_Y_POSITION / 2))
#define PROT2_BITMAP                (threadBitmap)
#define PROT2_BITMAP_CONN           (threadBitmap)
#endif // PROT1 | PROT2
#endif // PROT_THREAD

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static GLIB_Context_t glibContext;          /* Global glib context */

static const uint8_t siliconlabsBitmap[] = { SILABS_BITMAP };
static const uint8_t lightOnBitMap[] = { LIGHT_ON_BITMAP };
static const uint8_t lightOffBitMap[] = { LIGHT_OFF_BITMAP };

#if ( (PROT1 == PROT_ZIGBEE) || (PROT2 == PROT_ZIGBEE) )
static const uint8_t zigbeeBitmap[] = { ZIGBEE_BITMAP };
static const uint8_t zigbeeConnectedBitmap[] = { ZIGBEE_CONNECTED_BITMAP };
#endif

#if ( (PROT1 == PROT_RAIL) || (PROT2 == PROT_RAIL) )
static const uint8_t railBitmap[] = { RAIL_BITMAP };
static const uint8_t railConnectedBitmap[] = { RAIL_CONNECTED_BITMAP };
#endif

#if ( (PROT1 == PROT_CONNECT) || (PROT2 == PROT_CONNECT) )
static const uint8_t connectBitmap[] = { CONNECT_BITMAP };
static const uint8_t connectConnectedBitmap[] = { CONNECT_CONNECTED_BITMAP };
#endif

#if ( (PROT1 == PROT_BLUETOOTH) || (PROT2 == PROT_BLUETOOTH) )
static const uint8_t bluetoothBitmap[] = { BLUETOOTH_BITMAP };
static const uint8_t bluetoothConnectedBitmap[] = { BLUETOOTH_CONNECTED_BITMAP };
#endif

#if ( (PROT1 == PROT_THREAD) || (PROT2 == PROT_THREAD) )
static const uint8_t threadBitmap[] = { THREAD_BITMAP };
#endif
/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/ \
  static void demoUIDisplayLogo(void)
{
  GLIB_drawBitmap(&glibContext,
                  SILICONLABS_X_POSITION,
                  SILICONLABS_Y_POSITION,
                  SILICONLABS_BITMAP_WIDTH,
                  SILICONLABS_BITMAP_HEIGHT,
                  siliconlabsBitmap);
}

static void demoUIDisplayAppName(uint8_t* device)
{
  char appName[20];
  sprintf(appName, "Demo %s", (const char*)device);

  GLIB_drawStringOnLine(&glibContext, appName, 5, GLIB_ALIGN_CENTER, 0, 0, true);
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void demoUIInit(void)
{
  EMSTATUS status;

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1)
      ;
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1)
      ;
  }

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GLIB_clear(&glibContext);
  DMD_updateDisplay();
}

void demoUIDisplayHeader(uint8_t* name)
{
  demoUIDisplayLogo();
  demoUIDisplayAppName(name);
  DMD_updateDisplay();
}

void demoUIDisplayHelp(bool networkForming)
{
  GLIB_clear(&glibContext);
  uint8_t y_position = SILICONLABS_BITMAP_HEIGHT + 20;
  if (!networkForming) {
    GLIB_drawString(&glibContext, helpmenu_line1_no_nwk,
                    strlen(helpmenu_line1_no_nwk) + 1, 2, y_position, 0);
    GLIB_drawString(&glibContext, helpmenu_line2_no_nwk,
                    strlen(helpmenu_line2_no_nwk) + 1, 2, y_position + 10, 0);
    GLIB_drawString(&glibContext, helpmenu_line3_no_nwk,
                    strlen(helpmenu_line3_no_nwk) + 1, 2, y_position + 20, 0);
    GLIB_drawString(&glibContext, helpmenu_line4_no_nwk,
                    strlen(helpmenu_line4_no_nwk) + 1, 2, y_position + 30, 0);
    GLIB_drawString(&glibContext, helpmenu_line5_no_nwk,
                    strlen(helpmenu_line5_no_nwk) + 1, 2, y_position + 40, 0);
    GLIB_drawString(&glibContext, helpmenu_line6_no_nwk,
                    strlen(helpmenu_line6_no_nwk) + 1, 2, y_position + 50, 0);
  } else if (!strcmp(DEVICE_TYPE, "Light")) {
    GLIB_drawString(&glibContext, helpmenu_line1_light,
                    strlen(helpmenu_line1_light) + 1, 2, y_position, 0);
    GLIB_drawString(&glibContext, helpmenu_line2_light,
                    strlen(helpmenu_line2_light) + 1, 2, y_position + 10, 0);
    GLIB_drawString(&glibContext, helpmenu_line3_light,
                    strlen(helpmenu_line3_light) + 1, 2, y_position + 20, 0);
    GLIB_drawString(&glibContext, helpmenu_line4_light,
                    strlen(helpmenu_line4_light) + 1, 2, y_position + 30, 0);
    GLIB_drawString(&glibContext, helpmenu_line5_light,
                    strlen(helpmenu_line5_light) + 1, 2, y_position + 40, 0);
    GLIB_drawString(&glibContext, helpmenu_line6_light,
                    strlen(helpmenu_line6_light) + 1, 2, y_position + 50, 0);
  } else {
    GLIB_drawString(&glibContext, helpmenu_line1_switch,
                    strlen(helpmenu_line1_switch) + 1, 2, y_position, 0);
    GLIB_drawString(&glibContext, helpmenu_line2_switch,
                    strlen(helpmenu_line2_switch) + 1, 2, y_position + 10, 0);
    GLIB_drawString(&glibContext, helpmenu_line3_switch,
                    strlen(helpmenu_line3_switch) + 1, 2, y_position + 20, 0);
    GLIB_drawString(&glibContext, helpmenu_line4_switch,
                    strlen(helpmenu_line4_switch) + 1, 2, y_position + 30, 0);
    GLIB_drawString(&glibContext, helpmenu_line5_switch,
                    strlen(helpmenu_line5_switch) + 1, 2, y_position + 40, 0);
    GLIB_drawString(&glibContext, helpmenu_line6_switch,
                    strlen(helpmenu_line6_switch) + 1, 2, y_position + 50, 0);
  }
  DMD_updateDisplay();
}

void demoUIDisplayLight(bool on)
{
  GLIB_drawBitmap(&glibContext,
                  LIGHT_X_POSITION,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  (on ? lightOnBitMap : lightOffBitMap));
  DMD_updateDisplay();
}

void demoUIDisplayProtocol(demoUIProtocol protocol, bool isConnected)
{
  GLIB_drawBitmap(&glibContext,
                  (protocol == DEMO_UI_PROTOCOL1 ? PROT1_X_POSITION : PROT2_X_POSITION),
                  (protocol == DEMO_UI_PROTOCOL1 ? PROT1_Y_POSITION : PROT2_Y_POSITION),
                  (protocol == DEMO_UI_PROTOCOL1 ? PROT1_BITMAP_WIDTH : PROT2_BITMAP_WIDTH),
                  (protocol == DEMO_UI_PROTOCOL1 ? PROT1_BITMAP_HEIGHT : PROT2_BITMAP_HEIGHT),
                  (protocol == DEMO_UI_PROTOCOL1 ? (isConnected ? PROT1_BITMAP_CONN : PROT1_BITMAP)
                   : (isConnected ? PROT2_BITMAP_CONN : PROT2_BITMAP)));
  DMD_updateDisplay();
}

void demoUIDisplayDirection(demoUILightDirection_t direction)
{
  if (direction == DEMO_UI_DIRECTION_PROT1) {
    GLIB_drawLine(&glibContext,
                  (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 5),
                  (PROT1_Y_POSITION + PROT1_BITMAP_HEIGHT / 2),
                  (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 15),
                  (PROT1_Y_POSITION + PROT1_BITMAP_HEIGHT / 2));
    GLIB_drawLine(&glibContext,
                  (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 12),
                  (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2) - 3),
                  (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 15),
                  (PROT1_Y_POSITION + PROT1_BITMAP_HEIGHT / 2));
    GLIB_drawLine(&glibContext,
                  (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 12),
                  (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2) + 3),
                  (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 15),
                  (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2)));
  } else if (direction == DEMO_UI_DIRECTION_PROT2) {
    GLIB_drawLine(&glibContext,
                  (PROT2_X_POSITION - 5),
                  (PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2),
                  (PROT2_X_POSITION - 15),
                  (PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2));
    GLIB_drawLine(&glibContext,
                  ((PROT2_X_POSITION - 15) + 3),
                  ((PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2) - 3),
                  (PROT2_X_POSITION - 15),
                  (PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2));
    GLIB_drawLine(&glibContext,
                  ((PROT2_X_POSITION - 15) + 3),
                  ((PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2) + 3),
                  (PROT2_X_POSITION - 15),
                  (PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2));
  }
  DMD_updateDisplay();
}

void demoUIClearDirection(demoUILightDirection_t direction)
{
  GLIB_Rectangle_t rect;
  if (direction == DEMO_UI_DIRECTION_PROT1) {
    rect.xMin = (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 5);
    rect.yMin = (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2) - 3);
    rect.xMax = (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 15);
    rect.yMax = (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2) + 3);
  } else if (direction == DEMO_UI_DIRECTION_PROT2) {
    rect.xMin = (PROT2_X_POSITION - 15);
    rect.yMin = ((PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2) - 3);
    rect.xMax = (PROT2_X_POSITION - 5);
    rect.yMax = ((PROT2_Y_POSITION + PROT2_BITMAP_HEIGHT / 2) + 3);
  }
  GLIB_setClippingRegion(&glibContext, (const GLIB_Rectangle_t*)&rect);
  GLIB_clearRegion(&glibContext);
  GLIB_resetClippingRegion(&glibContext);
  GLIB_applyClippingRegion(&glibContext);
  DMD_updateDisplay();
}

void demoUIDisplayId(demoUIProtocol protocol, uint8_t* id)
{
  char tmpId[10] = { 0 };
  strncpy(tmpId, (char*)id, 9);
  if (strlen(tmpId)) {
    GLIB_drawString(&glibContext,
                    tmpId,
                    strlen(tmpId) + 1,
                    (protocol == DEMO_UI_PROTOCOL1
                     ? PROT1_ID_X_POSITION : PROT2_ID_X_POSITION),
                    glibContext.pDisplayGeometry->ySize - 10,
                    0);
  }
  DMD_updateDisplay();
}

void demoUIDisplayChan(uint8_t channel)
{
  uint8_t msg[9];
  sprintf((char *)msg, "CHAN:%d", channel);
  demoUIDisplayId(DEMO_UI_PROTOCOL1, msg);
}

void demoUIClearMainScreen(uint8_t* name, bool showPROT1, bool showPROT2)
{
  GLIB_clear(&glibContext);
  demoUIDisplayHeader(name);
  demoUIDisplayLight(false);
  if (showPROT1) {
    demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, false);
    demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
  }

  if (showPROT2) {
    demoUIDisplayProtocol(DEMO_UI_PROTOCOL2, false);
    demoUIClearDirection(DEMO_UI_DIRECTION_PROT2);
  }
}
