/***************************************************************************//**
 * @file
 * @brief Functions for drawing the different app screens
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

#ifndef APP_SCREENS_H
#define APP_SCREENS_H

#include "sl_memlcd.h"
#include "dmd.h"
#include "glib.h"

#include "app_rgbled.h"
#include "app_csen.h"

#define APP_RIGHT_BUTTON_LIMIT 135
#define APP_LEFT_BUTTON_LIMIT 70

#define APP_GLIB_FONT_WIDTH           (pContext->font.fontWidth \
                                       + pContext->font.charSpacing)
#define APP_GLIB_FONT_HEIGHT          (pContext->font.fontHeight)

/* Center of display */
#define APP_CENTER_X                  (pContext->pDisplayGeometry->xSize / 2)
#define APP_CENTER_Y                  (pContext->pDisplayGeometry->ySize / 2)

#define APP_MAX_X                     (pContext->pDisplayGeometry->xSize - 1)
#define APP_MAX_Y                     (pContext->pDisplayGeometry->ySize - 1)

#define APP_MIN_X                     0
#define APP_MIN_Y                     0

#define APP_STR_LEN               48

#define APP_BU_BUFFER_SIZE 90

#define APP_SILICONLABS_BITMAP_WIDTH  126
#define APP_SILICONLABS_BITMAP_HEIGHT 60

// App screen function prototypes
void drawInitScreen(GLIB_Context_t *pContext);
void drawLedScreen(GLIB_Context_t *pContext,
                   int8_t xOffset,
                   bool swipeLock,
                   bool forceTriggered,
                   RGBLED_Settings_t ledSettings);
void drawHumTempScreen(GLIB_Context_t *pContext,
                       int8_t xOffset,
                       bool useCelsius,
                       int32_t tempData,
                       uint32_t rhData,
                       bool forceTriggered);
void drawForceTouchScreen(GLIB_Context_t *pContext,
                          int8_t xOffset,
                          bool swipeLock,
                          CSEN_Event_t csenData);
void drawHallSensorScreen(GLIB_Context_t *pContext,
                          int8_t xOffset,
                          int16_t fieldStrength);
void drawLCSenseScreen(GLIB_Context_t *pContext,
                       int8_t xOffset,
                       bool forceTriggered,
                       uint16_t trigCnt,
                       uint32_t * rngResult);
void drawBuModeScreen(GLIB_Context_t *pContext,
                      int8_t xOffset,
                      bool chargeEn,
                      bool disCharge,
                      uint16_t * buVddMeasBuffer,
                      uint8_t buBufferIndex,
                      uint16_t avddVoltage,
                      uint32_t bodCnt,
                      uint32_t bodTime,
                      uint32_t buTime);

// Helper function prototypes
void drawButton(GLIB_Context_t *pContext, int8_t xOffset, bool rightSide, bool darkBkg, char * str);
void drawThermometer(GLIB_Context_t *pContext, int8_t xoffset, int8_t yoffset);
void drawAppScreenHeading(GLIB_Context_t *pContext, int8_t xOffset, char * str);
void drawTriPointer(GLIB_Context_t *pContext, int16_t xPos, int16_t yPos);
void drawLedSettingLabel(GLIB_Context_t *pContext, bool highLight, char * str, uint8_t xPos, uint8_t yPos);

#endif /* APP_SCREENS_H */
