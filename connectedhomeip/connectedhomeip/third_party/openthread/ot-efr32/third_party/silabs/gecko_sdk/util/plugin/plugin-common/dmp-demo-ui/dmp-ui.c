/*****************************************************************************/
/**
 * @brief User Interface rendering for DMP demo
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_types.h"
#include "glib.h"
#include "dmd/dmd.h"
#include "display.h"
#include <string.h>
#include <stdio.h>
#include "dmp-ui.h"
#include "dmp-bitmaps.h"

#ifndef DEVICE_TYPE
#define DEVICE_TYPE "Light"
#endif

//#define helpmenu_line1_light  "1. PB0               "
//#define helpmenu_line2_light  "Network Present      "
//#define helpmenu_line3_light  "   Short Press:Pjoin "
//#define helpmenu_line4_light  "   Long Press :Leave "
//#define helpmenu_line5_light  "No Network"
//#define helpmenu_line6_light  "   Form Network "
//#define helpmenu_line7_light  "2. PB1:Light Ctrl    "
//#define helpmenu_line1_switch "No Network           "
//#define helpmenu_line2_switch "1. PB0 : Join        "
//#define helpmenu_line3_switch "Network Present      "
//#define helpmenu_line4_switch "1. PB0 :             "
//#define helpmenu_line5_switch "Shrt Press:Light Ctrl"
//#define helpmenu_line6_switch "Long Press:Leave Nwk "
//#define helpmenu_line7_switch "2. PB1:Light Ctrl    "

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

static GLIB_Context_t glibContext;          /* Global glib context */

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static bool helpMenuDisplayed = false;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/ \
  static void dmpUiDisplayLogo(void)
{
  GLIB_drawBitmap(&glibContext,
                  SILICONLABS_X_POSITION,
                  SILICONLABS_Y_POSITION,
                  SILICONLABS_BITMAP_WIDTH,
                  SILICONLABS_BITMAP_HEIGHT,
                  siliconlabsBitmap);
}

static void dmpUiDisplayAppName(uint8_t* device)
{
  char appName[20];
  sprintf(appName, "DMP Demo %s", (const char*)device);

  GLIB_drawString(&glibContext, appName,
                  strlen(appName) + 1, 18, SILICONLABS_BITMAP_HEIGHT + 2, 0);
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void dmpUiInit(void)
{
  EMSTATUS status;

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    while (1)
      ;
  }

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

void dmpUiDisplayHeader(uint8_t* name)
{
  dmpUiDisplayLogo();
  dmpUiDisplayAppName(name);
  DMD_updateDisplay();
}

void dmpUiDisplayHelp(bool networkForming)
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

void dmpUiDisplayLight(bool on)
{
  GLIB_drawBitmap(&glibContext,
                  LIGHT_X_POSITION,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  (on ? lightOnBitMap : lightOffBitMap));
  DMD_updateDisplay();
}

void dmpUiDisplayProtocol(DmpUiProtocol protocol, bool isConnected)
{
  GLIB_drawBitmap(&glibContext,
                  (protocol == DMP_UI_PROTOCOL1 ? PROT1_X_POSITION : PROT2_X_POSITION),
                  (protocol == DMP_UI_PROTOCOL1 ? PROT1_Y_POSITION : PROT2_Y_POSITION),
                  (protocol == DMP_UI_PROTOCOL1 ? PROT1_BITMAP_WIDTH : PROT2_BITMAP_WIDTH),
                  (protocol == DMP_UI_PROTOCOL1 ? PROT1_BITMAP_HEIGHT : PROT2_BITMAP_HEIGHT),
                  (protocol == DMP_UI_PROTOCOL1 ? (isConnected ? PROT1_BITMAP_CONN : PROT1_BITMAP)
                   : (isConnected ? PROT2_BITMAP_CONN : PROT2_BITMAP)));
  DMD_updateDisplay();
}

void dmpUiDisplayDirection(DmpUiLightDirection_t direction)
{
  if (direction == DMP_UI_DIRECTION_PROT1) {
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
  } else if (direction == DMP_UI_DIRECTION_PROT2) {
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

void dmpUiClearDirection(DmpUiLightDirection_t direction)
{
  GLIB_Rectangle_t rect;
  if (direction == DMP_UI_DIRECTION_PROT1) {
    rect.xMin = (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 5);
    rect.yMin = (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2) - 3);
    rect.xMax = (PROT1_X_POSITION + PROT1_BITMAP_WIDTH + 15);
    rect.yMax = (PROT1_Y_POSITION + (PROT1_BITMAP_HEIGHT / 2) + 3);
  } else if (direction == DMP_UI_DIRECTION_PROT2) {
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

void dmpUiDisplayId(DmpUiProtocol protocol, uint8_t* id)
{
  char tmpId[10] = { 0 };
  strncpy(tmpId, (char*)id, 9);
  if (strlen(tmpId)) {
    GLIB_drawString(&glibContext,
                    tmpId,
                    strlen(tmpId) + 1,
                    (protocol == DMP_UI_PROTOCOL1
                     ? PROT1_ID_X_POSITION : 79),
                    glibContext.pDisplayGeometry->ySize - 10,
                    0);
  }
  DMD_updateDisplay();
}

void dmpUiDisplayChan(uint8_t channel)
{
  uint8_t msg[9];
  sprintf(msg, "CHAN:%d", channel);
  dmpUiDisplayId(DMP_UI_PROTOCOL1, msg);
}

void dmpUiClearMainScreen(uint8_t* name, bool showPROT1, bool showPROT2)
{
  GLIB_clear(&glibContext);
  dmpUiDisplayHeader(name);
  dmpUiDisplayLight(false);
  if (showPROT1) {
    dmpUiDisplayProtocol(DMP_UI_PROTOCOL1, false);
    dmpUiClearDirection(DMP_UI_DIRECTION_PROT1);
  }

  if (showPROT2) {
    dmpUiDisplayProtocol(DMP_UI_PROTOCOL2, false);
    dmpUiClearDirection(DMP_UI_DIRECTION_PROT2);
  }
}
