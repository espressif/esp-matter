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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "siliconlabs_logo.h"

#include "app_rtcc.h"
#include "app_screens.h"

void drawInitScreen(GLIB_Context_t *pContext)
{
  char str[APP_STR_LEN];
  GLIB_Rectangle_t rectBackground = { 0, 0, APP_MAX_X, APP_MAX_Y };

  pContext->backgroundColor = White;
  pContext->foregroundColor = White;
  GLIB_drawRectFilled(pContext, &rectBackground);

  pContext->foregroundColor = Black;
  GLIB_drawBitmap(pContext,
                  ((APP_MAX_X + 1 - APP_SILICONLABS_BITMAP_WIDTH) / 2),
                  5,
                  APP_SILICONLABS_BITMAP_WIDTH,
                  APP_SILICONLABS_BITMAP_HEIGHT,
                  siliconlabsBitmap);

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);

  strncpy(str, "EFM32GG11", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  APP_CENTER_X - ((APP_GLIB_FONT_WIDTH * strlen(str)) / 2),
                  80,
                  0);
  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  strncpy(str, "Swipe to begin", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  APP_CENTER_X - ((APP_GLIB_FONT_WIDTH * strlen(str)) / 2),
                  100,
                  0);
}

/**************************************************************************//**
 * @brief Function for drawing the RGB LED app screen
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param swipeLock
 *        If true, swiping the touch slider will change the current settings
 *        parameter instead of changing app screens
 * @param forceTriggered
 *        True if force touch trigger treshold has been exceeded
 * @param ledSettings
 *        Structure holding the RGB LED app settings
 *****************************************************************************/
void drawLedScreen(GLIB_Context_t *pContext,
                   int8_t xOffset,
                   bool swipeLock,
                   bool forceTriggered,
                   RGBLED_Settings_t ledSettings)
{
  char str[APP_STR_LEN];
  uint16_t led0ColorPos, led1ColorPos;
  GLIB_Rectangle_t rectBrightBound = { 44 + xOffset, 41, APP_MAX_X + xOffset - 6, 46 };
  GLIB_Rectangle_t rectColorBox = { 44 + xOffset, 31, 56 + xOffset, 36 };
  CSEN_Event_t curTouchEvent;

  strncpy(str, "RGB LEDs", APP_STR_LEN);
  drawAppScreenHeading(pContext, xOffset, str);

  pContext->backgroundColor = White;
  pContext->foregroundColor = Black;

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  strncpy(str, "LED0", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  5 + xOffset,
                  20,
                  0);

  strncpy(str, "LED1", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  5 + xOffset,
                  56,
                  0);

  strncpy(str, " Color ", APP_STR_LEN);
  drawLedSettingLabel(pContext, (swipeLock && ledSettings.curSetting == 0) ? true : false, str, 2 + xOffset, 30);
  drawLedSettingLabel(pContext, (swipeLock && ledSettings.curSetting == 2) ? true : false, str, 2 + xOffset, 66);

  strncpy(str, " Power ", APP_STR_LEN);
  drawLedSettingLabel(pContext, (swipeLock && ledSettings.curSetting == 1) ? true : false, str, 2 + xOffset, 40);
  drawLedSettingLabel(pContext, (swipeLock && ledSettings.curSetting == 3) ? true : false, str, 2 + xOffset, 76);

  pContext->backgroundColor = White;
  pContext->foregroundColor = Black;

  GLIB_drawRect(pContext, &rectBrightBound);
  rectBrightBound.yMin = 77;
  rectBrightBound.yMax = 82;
  GLIB_drawRect(pContext, &rectBrightBound);

  if (ledSettings.led1Power > 0) {
    rectBrightBound.xMax = 44 + xOffset + ((ledSettings.led1Power * 13) / 2);
    GLIB_drawRectFilled(pContext, &rectBrightBound);
  }

  if (ledSettings.led0Power > 0) {
    rectBrightBound.yMin = 41;
    rectBrightBound.yMax = 46;
    rectBrightBound.xMax = 44 + xOffset + ((ledSettings.led0Power * 13) / 2);
    GLIB_drawRectFilled(pContext, &rectBrightBound);
  }

  led0ColorPos = (ledSettings.led0Color * 78) / 360;
  drawTriPointer(pContext, 44 + led0ColorPos + xOffset, 30);

  led1ColorPos = (ledSettings.led1Color * 78) / 360;
  drawTriPointer(pContext, 44 + led1ColorPos + xOffset, 66);

  pContext->foregroundColor = Yellow;
  GLIB_drawRectFilled(pContext, &rectColorBox);
  rectColorBox.yMin = 67;
  rectColorBox.yMax = 72;
  GLIB_drawRectFilled(pContext, &rectColorBox);

  pContext->foregroundColor = Red;
  rectColorBox.xMin = 57 + xOffset;
  rectColorBox.xMax = 69 + xOffset;
  GLIB_drawRectFilled(pContext, &rectColorBox);
  rectColorBox.yMin = 31;
  rectColorBox.yMax = 36;
  GLIB_drawRectFilled(pContext, &rectColorBox);

  pContext->foregroundColor = Magenta;
  rectColorBox.xMin = 70 + xOffset;
  rectColorBox.xMax = 82 + xOffset;
  GLIB_drawRectFilled(pContext, &rectColorBox);
  rectColorBox.yMin = 67;
  rectColorBox.yMax = 72;
  GLIB_drawRectFilled(pContext, &rectColorBox);

  pContext->foregroundColor = Blue;
  rectColorBox.xMin = 83 + xOffset;
  rectColorBox.xMax = 95 + xOffset;
  GLIB_drawRectFilled(pContext, &rectColorBox);
  rectColorBox.yMin = 31;
  rectColorBox.yMax = 36;
  GLIB_drawRectFilled(pContext, &rectColorBox);

  pContext->foregroundColor = Cyan;
  rectColorBox.xMin = 96 + xOffset;
  rectColorBox.xMax = 108 + xOffset;
  GLIB_drawRectFilled(pContext, &rectColorBox);
  rectColorBox.yMin = 67;
  rectColorBox.yMax = 72;
  GLIB_drawRectFilled(pContext, &rectColorBox);

  pContext->foregroundColor = Green;
  rectColorBox.xMin = 109 + xOffset;
  rectColorBox.xMax = 121 + xOffset;
  GLIB_drawRectFilled(pContext, &rectColorBox);
  rectColorBox.yMin = 31;
  rectColorBox.yMax = 36;
  GLIB_drawRectFilled(pContext, &rectColorBox);

  if (swipeLock) {
    curTouchEvent = csenGetEvent();
    drawButton(pContext, xOffset, 1, (forceTriggered && curTouchEvent.sliderPos > APP_RIGHT_BUTTON_LIMIT) ? 1 : 0, "Next >>");
    drawButton(pContext, xOffset, 0, 0, "Exit");
  } else {
    drawButton(pContext, xOffset, 1, 0, "Settings");
  }
}

/**************************************************************************//**
 * @brief Function for drawing the hall-effect sensor app screen
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param fieldStrength
 *        Field trength in milli-teslas (mT)
 *****************************************************************************/
void drawHallSensorScreen(GLIB_Context_t *pContext, int8_t xOffset, int16_t fieldStrength)
{
  char str[APP_STR_LEN];
  uint8_t indicatorPos;
  GLIB_Rectangle_t rectMagBound = { 6 + xOffset, 61, APP_MAX_X + xOffset - 6, 66 };

  strncpy(str, "Si7210", APP_STR_LEN);
  drawAppScreenHeading(pContext, xOffset, str);

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  strncpy(str, "Hall Effect Sensor", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  5 + xOffset,
                  15,
                  0);

  snprintf(str, APP_STR_LEN, "Field\nstrength: %4i.%.2i mT", fieldStrength / 80, abs((int8_t)(((fieldStrength * 25 / 2) % 1000) / 10)));
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  5 + xOffset,
                  75,
                  0);

  // Draw rod magnet (field strength indicator bar)
  GLIB_drawRect(pContext, &rectMagBound);
  pContext->foregroundColor = Red;
  rectMagBound.xMin = (APP_MAX_X / 2) + xOffset;
  rectMagBound.xMax--;
  rectMagBound.yMin++;
  rectMagBound.yMax--;
  GLIB_drawRectFilled(pContext, &rectMagBound);

  // Draw indicator magnet
  indicatorPos = (APP_MAX_X / 2) + (fieldStrength / 330);
  if (indicatorPos > 112) {
    indicatorPos = 112;
  }
  if (indicatorPos < 15) {
    indicatorPos = 15;
  }
  indicatorPos += xOffset;
  GLIB_drawCircleFilled(pContext,
                        indicatorPos,
                        44,
                        9);
  rectMagBound.xMin = indicatorPos - 9;
  rectMagBound.xMax = indicatorPos + 9;
  rectMagBound.yMin = 45;
  rectMagBound.yMax = 58;
  GLIB_drawRectFilled(pContext, &rectMagBound);
  pContext->foregroundColor = White;
  GLIB_drawCircleFilled(pContext,
                        indicatorPos,
                        44,
                        3);
  rectMagBound.xMin += 6;
  rectMagBound.xMax -= 6;
  GLIB_drawRectFilled(pContext, &rectMagBound);
}

/**************************************************************************//**
 * @brief Function for drawing the relative humidity and temperature sensor app screen
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param useCelsius
 *        true: temperature is shown in centigrades
 *        false: temperature is shown in fahrenheit
 * @param tempData
 *        Temperature in millidegrees celsius
 * @param rhData
 *        Relative humidity in millipercent
 * @param forceTriggered
 *        True if force touch trigger treshold has been exceeded
 *****************************************************************************/
void drawHumTempScreen(GLIB_Context_t *pContext,
                       int8_t xOffset,
                       bool useCelsius,
                       int32_t tempData,
                       uint32_t rhData,
                       bool forceTriggered)
{
  char str[APP_STR_LEN];
  GLIB_Rectangle_t mercuryLevel;
  CSEN_Event_t curTouchEvent;

  strncpy(str, "Si7021", APP_STR_LEN);
  drawAppScreenHeading(pContext, xOffset, str);

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  strncpy(str, "RHT Sensor", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  5 + xOffset,
                  15,
                  0);

  drawThermometer(pContext, 34 + xOffset, 24);
  drawThermometer(pContext, 92 + xOffset, 24);

  pContext->foregroundColor = Red;
  GLIB_drawCircleFilled(pContext, 34 + xOffset, 93, 9);
  mercuryLevel.xMin = 32 + xOffset;
  mercuryLevel.xMax = 36 + xOffset;
  mercuryLevel.yMax = 90;

  if (useCelsius) {
    mercuryLevel.yMin = 79 - (tempData * 51 / 35000);
    GLIB_drawRectFilled(pContext, &mercuryLevel);
    pContext->foregroundColor = Black;
    strncpy(str, "35  100", APP_STR_LEN);
    GLIB_drawString(pContext,
                    str,
                    strlen(str),
                    APP_CENTER_X - ((APP_GLIB_FONT_WIDTH * strlen(str)) / 2) + xOffset,
                    25,
                    0);
    pContext->foregroundColor = White;
    GLIB_drawString(pContext, "C", 1, 32 + xOffset, 89, 0);
    snprintf(str, APP_STR_LEN, "%2li.%.1li", tempData / 1000, (tempData % 1000) / 100);
  } else {
    mercuryLevel.yMin = 79 - ((tempData * 9 / 5) * 51 / 95000);
    GLIB_drawRectFilled(pContext, &mercuryLevel);
    pContext->foregroundColor = Black;
    strncpy(str, "95  100", APP_STR_LEN);
    GLIB_drawString(pContext,
                    str,
                    strlen(str),
                    APP_CENTER_X - ((APP_GLIB_FONT_WIDTH * strlen(str)) / 2) + xOffset,
                    25,
                    0);
    pContext->foregroundColor = White;
    GLIB_drawString(pContext, "F", 1, 32 + xOffset, 89, 0);
    snprintf(str, APP_STR_LEN, "%2li.%.1li", (tempData * 9 / 5000) + 32, ((tempData * 9 / 5) % 1000) / 100);
  }

  /// Draw min/max lines and numbers
  pContext->foregroundColor = Black;
  GLIB_drawLineH(pContext, 28 + xOffset, 28, 40 + xOffset);
  GLIB_drawLineH(pContext, 28 + xOffset, 79, 40 + xOffset);

  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  3 + xOffset,
                  mercuryLevel.yMin - 3,
                  0);
  GLIB_drawLineH(pContext, 28 + xOffset, mercuryLevel.yMin, 36 + xOffset);
  strncpy(str, "0     0", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  APP_CENTER_X - ((APP_GLIB_FONT_WIDTH * strlen(str)) / 2) + xOffset,
                  76,
                  0);

  pContext->foregroundColor = Blue;
  GLIB_drawCircleFilled(pContext, 92 + xOffset, 93, 9);
  mercuryLevel.xMin = 90 + xOffset;
  mercuryLevel.xMax = 94 + xOffset;
  mercuryLevel.yMin = 79 - (rhData * 51 / 100000);
  GLIB_drawRectFilled(pContext, &mercuryLevel);

  pContext->foregroundColor = Black;
  GLIB_drawLineH(pContext, 86 + xOffset, 28, 98 + xOffset);
  GLIB_drawLineH(pContext, 86 + xOffset, 79, 98 + xOffset);
  snprintf(str, APP_STR_LEN, "%ld%%", rhData / 1000);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  100 + xOffset,
                  mercuryLevel.yMin - 3,
                  0);
  GLIB_drawLineH(pContext, 90 + xOffset, mercuryLevel.yMin, 98 + xOffset);

  pContext->foregroundColor = White;
  GLIB_drawString(pContext, "%", 1, 90 + xOffset, 89, 0);

  curTouchEvent = csenGetEvent();
  drawButton(pContext, xOffset, 1, (forceTriggered && curTouchEvent.sliderPos > APP_RIGHT_BUTTON_LIMIT) ? 1 : 0, "C/F");
}

/**************************************************************************//**
 * @brief Function for drawing the force touch app screen
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param swipeLock
 *        If true, swiping the touch slider will not change the app screen
 * @param csenData
 *        Struct containing data about the last touch event
 *****************************************************************************/
void drawForceTouchScreen(GLIB_Context_t *pContext,
                          int8_t xOffset,
                          bool swipeLock,
                          CSEN_Event_t csenData)
{
  char str[APP_STR_LEN];
  static int16_t lastPos = 0;
  GLIB_Rectangle_t rectPos = { 4 + xOffset, 65, 5 + xOffset, 80 };
  GLIB_Rectangle_t rectForce = { 103 + xOffset, 61, 112 + xOffset, 80 };

  uint16_t posIndicatorX = 4 + xOffset, posIndicatorY = 64;
  uint8_t i;

  strncpy(str, "Touch", APP_STR_LEN);
  drawAppScreenHeading(pContext, xOffset, str);

  pContext->foregroundColor = Black;

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);

  snprintf(str, APP_STR_LEN, "Touch event\nduration: %ld.%02lds", csenData.eventDuration / 1000, (csenData.eventDuration % 1000) / 10);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  20,
                  0);

  strncpy(str, "Position", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  24 + xOffset,
                  85,
                  0);

  strncpy(str, "Force", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  94 + xOffset,
                  85,
                  0);

  for (i = 0; i < 22; i++) {
    GLIB_drawRectFilled(pContext, &rectPos);
    rectPos.xMin += 4;
    rectPos.xMax += 4;
    if (i % 2 == 1) {
      rectPos.yMin--;
    }
  }

  if (csenData.eventActive) {
    lastPos = csenData.sliderPos;
  }

  posIndicatorX += (lastPos * 84) / 192;
  posIndicatorY -= (lastPos * 10) / 192;

  drawTriPointer(pContext, posIndicatorX, posIndicatorY);

  snprintf(str, APP_STR_LEN, "%i", lastPos);

  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  posIndicatorX - 3,
                  posIndicatorY - 14,
                  0);

  if (swipeLock) {
    drawButton(pContext, xOffset, 1, 1, "Swipe lock");
    drawButton(pContext, xOffset, 0, 0, "Unlock");
  } else {
    drawButton(pContext, xOffset, 1, 0, "Swipe lock");
  }

  pContext->foregroundColor = Green;
  GLIB_drawRectFilled(pContext, &rectForce);
  pContext->foregroundColor = Yellow;
  rectForce.yMin -= 20;
  rectForce.yMax -= 20;
  GLIB_drawRectFilled(pContext, &rectForce);
  pContext->foregroundColor = Red;
  rectForce.yMin -= 20;
  rectForce.yMax -= 20;
  GLIB_drawRectFilled(pContext, &rectForce);
  pContext->foregroundColor = White;
  rectForce.yMax += 39;
  if (csenData.eventActive) {
    rectForce.yMax -= csenData.touchForce / 1000;
  }
  GLIB_drawRectFilled(pContext, &rectForce);
}

/**************************************************************************//**
 * @brief Function for drawing the LC sensor app screen
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param forceTriggered
 *        True if force touch trigger treshold has been exceeded
 * @param trigCnt
 *        The number of times the LC sensor has been triggered
 * @param rngResult[4]
 *        Array containing the 128-bit result from the TRNG
 *****************************************************************************/
void drawLCSenseScreen(GLIB_Context_t *pContext,
                       int8_t xOffset,
                       bool forceTriggered,
                       uint16_t trigCnt,
                       uint32_t * rngResult)
{
  char str[APP_STR_LEN];
  GLIB_Rectangle_t rectBackground = { 107 + xOffset, 75, 109 + xOffset, 93 };
  CSEN_Event_t curTouchEvent;

  strncpy(str, "LC Sense", APP_STR_LEN);
  drawAppScreenHeading(pContext, xOffset, str);

  strncpy(str, "Trigger LC sensor\nwith metal object to", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  14,
                  0);
  strncpy(str, "randomize LED colors", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  34,
                  0);

  pContext->backgroundColor = Yellow;
  snprintf(str, APP_STR_LEN, " Trigger count: %3d ", trigCnt);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  44,
                  1);

  pContext->backgroundColor = White;

  strncpy(str, "Last TRNG result:\n0x", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  54,
                  0);

  snprintf(str, APP_STR_LEN, "%08lx\n%08lx\n%08lx\n%08lx", rngResult[0], rngResult[1], rngResult[2], rngResult[3]);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  16 + xOffset,
                  64,
                  0);

  // Draw LC symbol
  pContext->foregroundColor = Blue;
  GLIB_drawCircle(pContext, 110 + xOffset, 78, 3);
  GLIB_drawCircle(pContext, 110 + xOffset, 84, 3);
  GLIB_drawCircle(pContext, 110 + xOffset, 90, 3);

  pContext->foregroundColor = White;
  GLIB_drawRectFilled(pContext, &rectBackground);

  pContext->foregroundColor = Blue;
  GLIB_drawLineV(pContext, 109 + xOffset, 71, 75);
  GLIB_drawLineV(pContext, 109 + xOffset, 93, 97);

  GLIB_drawLineH(pContext, 95 + xOffset, 71, 108 + xOffset);
  GLIB_drawLineH(pContext, 95 + xOffset, 97, 108 + xOffset);

  GLIB_drawLineH(pContext, 89 + xOffset, 82, 99 + xOffset);
  GLIB_drawLineH(pContext, 89 + xOffset, 86, 99 + xOffset);

  GLIB_drawLineV(pContext, 94 + xOffset, 71, 81);
  GLIB_drawLineV(pContext, 94 + xOffset, 87, 97);

  curTouchEvent = csenGetEvent();
  drawButton(pContext, xOffset, 1, (forceTriggered && curTouchEvent.sliderPos > APP_RIGHT_BUTTON_LIMIT) ? 1 : 0, "Clr Count");
}

/**************************************************************************//**
 * @brief Function for drawing the Backup Power mode app screen
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param chargeEn
 *        True if the backup battery is charging
 * @param disCharge
 *        True if the backup batteru is discharging. chargeEn and disCharge
 *        cannot both be true
 * @param buVddMeasBuffer[90]
 *        Array containing BUVDD voltage measurements over time, in 1/100 volts
 * @param buBufferIndex
 *        Index of the last BUVDD voltage measurement in buVddMeasBuffer
 * @param avddVoltage
 *        Voltage of the main power supply (AVDD), in 1/100 volts
 * @param bodCnt
 *        Number of backup power events encountered
 * @param bodTime
 *        Total time spent in backup mode in seconds
 * @param buTime
 *        Duration of the last backup power event in seconds
 *****************************************************************************/
void drawBuModeScreen(GLIB_Context_t *pContext,
                      int8_t xOffset,
                      bool chargeEn,
                      bool disCharge,
                      uint16_t * buVddMeasBuffer,
                      uint8_t buBufferIndex,
                      uint16_t avddVoltage,
                      uint32_t bodCnt,
                      uint32_t bodTime,
                      uint32_t buTime)
{
  char str[APP_STR_LEN];
  uint8_t i, top;

  strncpy(str, "BU Power", APP_STR_LEN);
  drawAppScreenHeading(pContext, xOffset, str);

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GLIB_drawLineH(pContext, 31 + xOffset, 25, 123 + xOffset);
  GLIB_drawLineH(pContext, APP_MAX_X + xOffset - 96, 42, APP_MAX_X + xOffset - 4);
  GLIB_drawLineH(pContext, APP_MAX_X + xOffset - 96, 58, APP_MAX_X + xOffset - 4);
  GLIB_drawLineV(pContext, APP_MAX_X + xOffset - 93, 25, 61);
  GLIB_drawLineV(pContext, APP_MAX_X + xOffset - 63, 25, 61);
  GLIB_drawLineV(pContext, APP_MAX_X + xOffset - 33, 25, 61);
  GLIB_drawLineV(pContext, APP_MAX_X + xOffset - 4, 25, 61);
  snprintf(str, APP_STR_LEN, "BUVDD          %d.%02dV\n100%%", buVddMeasBuffer[buBufferIndex] / 100, buVddMeasBuffer[buBufferIndex] % 100);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  14,
                  0);

  strncpy(str, " 50%", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  39,
                  0);

  strncpy(str, "  0%\nTime", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  2 + xOffset,
                  54,
                  0);

  strncpy(str, "-90 -60  -30 Now", APP_STR_LEN);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  29 + xOffset,
                  64,
                  0);

  snprintf(str, APP_STR_LEN, "AVDD: %d.%02dV", avddVoltage / 100, avddVoltage % 100);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  2 + xOffset,
                  74,
                  0);

  if (bodCnt == 0) {
    strncpy(str, "No BOD events\nencountered", APP_STR_LEN);
  } else {
    switch ((millis() % 12000) / 4000) {
      case 2:
        snprintf(str, APP_STR_LEN, "Total BU\nduration: %02ld:%02ld:%02ld", bodTime / (3600), (bodTime % (3600)) / (60), bodTime % (60));
        break;

      case 1:
        snprintf(str, APP_STR_LEN, "Last BU event\nduration: %02ld:%02ld:%02ld", buTime / (3600), (buTime % (3600)) / (60), buTime % (60));
        break;

      default:
        snprintf(str, APP_STR_LEN, "Number of BU\nevents: %ld", bodCnt);
        break;
    }
  }
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  2 + xOffset,
                  84,
                  0);

  pContext->foregroundColor = Blue;

  for (i = 0; i < 90; i++) {
    top = buVddMeasBuffer[(buBufferIndex + i + 1) % 90] / 10;
    if (top > 31) {
      top = 31;
    }
    GLIB_drawLineV(pContext, 34 + xOffset + i, 57 - top, 57);
  }

  pContext->foregroundColor = White;
  pContext->backgroundColor = Black;

  if (chargeEn) {
    strncpy(str, " Charging ", APP_STR_LEN);
    GLIB_drawString(pContext,
                    str,
                    strlen(str),
                    79 + xOffset - ((strlen(str) * APP_GLIB_FONT_WIDTH) / 2),
                    30,
                    1);
  } else if (disCharge) {
    strncpy(str, " Discharging ", APP_STR_LEN);
    GLIB_drawString(pContext,
                    str,
                    strlen(str),
                    79 + xOffset - ((strlen(str) * APP_GLIB_FONT_WIDTH) / 2),
                    30,
                    1);
  }

  if ((buVddMeasBuffer[buBufferIndex] < 200 && !chargeEn && !disCharge) || (chargeEn && buVddMeasBuffer[buBufferIndex] < 230)) {
    strncpy(str, " Low bat ", APP_STR_LEN);
    pContext->backgroundColor = Red;
    GLIB_drawString(pContext,
                    str,
                    strlen(str),
                    79 + xOffset - ((strlen(str) * APP_GLIB_FONT_WIDTH) / 2),
                    47,
                    1);
  }
  if ((chargeEn && buVddMeasBuffer[buBufferIndex] >= 230) || (!chargeEn && buVddMeasBuffer[buBufferIndex] >= 200)) {
    strncpy(str, " BU ready ", APP_STR_LEN);
    pContext->backgroundColor = Green;
    GLIB_drawString(pContext,
                    str,
                    strlen(str),
                    79 + xOffset - ((strlen(str) * APP_GLIB_FONT_WIDTH) / 2),
                    47,
                    1);
  }

  drawButton(pContext, xOffset, true, chargeEn, "Charge On");
  drawButton(pContext, xOffset, false, disCharge, "Discharge");
}

/**************************************************************************//**
 * @brief Helper function for drawing a button at the lower edge of the screen.
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param rightSide
 *        If set to true, the button is drawn on the right half of the screen
 * @param darkBkg
 *        If set to true, the button is drawn with a black background and white text
 * @param str
 *        Pointer to the text string to be shown on the button face
 *****************************************************************************/
void drawButton(GLIB_Context_t *pContext, int8_t xOffset, bool rightSide, bool darkBkg, char * str)
{
  uint8_t xPos = 33 - ((APP_GLIB_FONT_WIDTH * strlen(str)) / 2) + xOffset;
  GLIB_Rectangle_t rectBackground = { xOffset, 110, APP_CENTER_X + xOffset - 1, APP_MAX_Y };

  pContext->foregroundColor = Black;

  GLIB_drawLineV(pContext, APP_CENTER_X + xOffset, 109, APP_MAX_Y);

  if (rightSide) {
    GLIB_drawLineH(pContext, APP_CENTER_X + xOffset, 109, APP_MAX_X + xOffset);
    xPos += 64;
    rectBackground.xMin = APP_CENTER_X + 1 + xOffset;
    rectBackground.xMax = APP_MAX_X + xOffset;
  } else {
    GLIB_drawLineH(pContext, 0 + xOffset, 109, APP_CENTER_X + xOffset);
  }

  if (darkBkg) {
    GLIB_drawRectFilled(pContext, &rectBackground);
    pContext->foregroundColor = White;
  }

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  xPos,
                  115,
                  0);
}

/**************************************************************************//**
 * @brief Helper function for drawing a thermometer.
 * @param pContext
 *        Pointer to the GLIB context
 * @param xoffset
 *        Top left pixel X offset
 * @param yoffset
 *        Top left pixel Y offset
 *****************************************************************************/
void drawThermometer(GLIB_Context_t *pContext, int8_t xoffset, int8_t yoffset)
{
  GLIB_Rectangle_t thermoScale;

  // Draw outer frame
  pContext->foregroundColor = Black;
  thermoScale.xMin = xoffset - 4;
  thermoScale.xMax = xoffset + 4;
  thermoScale.yMin = yoffset;
  thermoScale.yMax = yoffset + 76;
  GLIB_drawCircleFilled(pContext, xoffset, yoffset + 69, 11);
  GLIB_drawRectFilled(pContext, &thermoScale);

  thermoScale.xMin += 2;
  thermoScale.xMax -= 2;
  thermoScale.yMin += 2;
  thermoScale.yMax -= 2;
  pContext->foregroundColor = White;
  GLIB_drawRectFilled(pContext, &thermoScale);
}

/**************************************************************************//**
 * @brief Helper function for drawing a white background and the heading of an app screen.
 * @param pContext
 *        Pointer to the GLIB context
 * @param xOffset
 *        Horizontal offset of the app screen
 * @param str
 *        Pointer to the text string containing the screen heading
 *****************************************************************************/
void drawAppScreenHeading(GLIB_Context_t *pContext, int8_t xOffset, char * str)
{
  GLIB_Rectangle_t rectBackground = { 0 + xOffset, 0, APP_MAX_X + xOffset, APP_MAX_Y };
  uint32_t curTime = millis() / 1000;

  // Clear only the part of the screen we will be using
  pContext->backgroundColor = White;
  pContext->foregroundColor = White;
  GLIB_drawRectFilled(pContext, &rectBackground);

  // Draw the screen heading
  pContext->foregroundColor = Black;
  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  4 + xOffset,
                  4,
                  0);

  snprintf(str, APP_STR_LEN, "%02ld:%02ld:%02ld", curTime / 3600, (curTime % 3600) / 60, curTime % 60);
  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  76 + xOffset,
                  4,
                  0);
}

/**************************************************************************//**
 * @brief Helper function for drawing an indicator pointer (down-facing triangle).
 * @param pContext
 *        Pointer to the GLIB context
 * @param xPos
 *        Horizontal position on the screen
 * @param yPos
 *        Vertical position on the screen
 *****************************************************************************/
void drawTriPointer(GLIB_Context_t *pContext, int16_t xPos, int16_t yPos)
{
  GLIB_drawLine(pContext, xPos, yPos, xPos + 3, yPos - 6);
  GLIB_drawLine(pContext, xPos, yPos, xPos - 3, yPos - 6);
  GLIB_drawLine(pContext, xPos - 3, yPos - 6, xPos + 3, yPos - 6);
}

void drawLedSettingLabel(GLIB_Context_t *pContext, bool highLight, char * str, uint8_t xPos, uint8_t yPos)
{
  if (highLight) {
    pContext->backgroundColor = Blue;
    pContext->foregroundColor = White;
  } else {
    pContext->backgroundColor = White;
    pContext->foregroundColor = Black;
  }

  GLIB_drawString(pContext,
                  str,
                  strlen(str),
                  xPos,
                  yPos,
                  1);
}
