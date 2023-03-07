/***************************************************************************//**
 * @file
 * @brief Graphics code for hall effect wheel demo
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_types.h"
#include "dmd.h"
#include "glib.h"
#include "em_gpio.h"
#include "sl_memlcd.h"
#include "sl_sleeptimer.h"

#include "sl_si72xx.h"
#include "graphics.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BACKGROUND_WIDTH          (gc.pDisplayGeometry->xSize)
#define BACKGROUND_HEIGHT         (gc.pDisplayGeometry->ySize)
#define CENTER_X                  (BACKGROUND_WIDTH / 2)
#define CENTER_Y                  (BACKGROUND_HEIGHT / 2)
#define GLIB_FONT_WIDTH           (gc.font.fontWidth + gc.font.charSpacing)
#define GLIB_FONT_HEIGHT          (gc.font.fontHeight)

static GLIB_Context_t gc;
static sl_sleeptimer_timer_handle_t lcdTimer;

/* Initialize the frame buffer */
static void graphicsInit(void)
{
  GLIB_setFont(&gc, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  gc.backgroundColor = Black;
  gc.foregroundColor = White;
}

static void lcdPinToggle(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void) handle;
  (void) data;
  GPIO_PinOutToggle(SL_MEMLCD_EXTCOMIN_PORT, SL_MEMLCD_EXTCOMIN_PIN);
}

void GRAPHICS_SetupGraphics(void)
{
  EMSTATUS status;
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (true) ;
  }
  status = GLIB_contextInit(&gc);
  if (GLIB_OK != status) {
    while (true) ;
  }
  graphicsInit();

  // Pin toggle frequency of 60 Hz
  GPIO_PinModeSet(SL_MEMLCD_EXTCOMIN_PORT, SL_MEMLCD_EXTCOMIN_PIN, gpioModePushPull, 0);
  uint32_t freq = sl_sleeptimer_get_timer_frequency();
  sl_sleeptimer_start_periodic_timer(&lcdTimer, freq / (60 * 2), lcdPinToggle, NULL, 0, 0);
}

static void drawString(const char * s, int32_t x, int32_t y)
{
  GLIB_drawString(&gc, s, strlen(s), CENTER_X - x, CENTER_Y - y, false);
}

static void glib_ConnectEXP(void)
{
  GLIB_drawString(&gc, "Connect", 15, CENTER_X - 28, CENTER_Y - 30, 0);
  GLIB_drawString(&gc, "Si72xx-EXP", 15, CENTER_X - 40, CENTER_Y - 15, 0);
  GLIB_drawString(&gc, "Press Reset", 15, CENTER_X - 50, CENTER_Y + 5, 0);
}

void GRAPHICS_Demo_Menu(bool i2cError, uint8_t selection)
{
  char string[15];
  int8_t ycoor;

  GLIB_clear(&gc); /* Clear the screen */

  if (i2cError) {
    glib_ConnectEXP();
  } else {
    sprintf(string, "Demo Menu");
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 60);
    sprintf(string, "PB1 to Scroll");
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 45);
    sprintf(string, "PB0 to Select");
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 35);

    ycoor = 30 - (selection * 10);
    drawString("->", 60, ycoor);

    drawString("  EXP Wheel", 56, 20);
    drawString("  EXP Rev Cnt", 56, 10);
    drawString("  PS I2C Data", 56, 0);
    drawString("  PS I2C Temp", 56, -10);
    drawString("  PS Sw/Latch", 56, -20);
    drawString("  PS Analog", 56, -30);
    drawString("  PS PWM", 56, -40);
    drawString("  PS SENT", 56, -50);
  }
  DMD_updateDisplay();
}

static void drawPixelDot(int8_t xcoor, int8_t ycoor)
{
  GLIB_drawPixel(&gc, xcoor, ycoor);
  GLIB_drawPixel(&gc, xcoor + 1, ycoor);
  GLIB_drawPixel(&gc, xcoor - 1, ycoor);
  GLIB_drawPixel(&gc, xcoor, ycoor + 1);
  GLIB_drawPixel(&gc, xcoor, ycoor - 1);
  GLIB_drawPixel(&gc, xcoor + 1, ycoor + 1);
  GLIB_drawPixel(&gc, xcoor - 1, ycoor - 1);
  GLIB_drawPixel(&gc, xcoor - 1, ycoor + 1);
  GLIB_drawPixel(&gc, xcoor + 1, ycoor - 1);
}

static void drawWheelAngleValues(int32_t u1, int32_t u2, int32_t angle)
{
  char string[16];

  sprintf(string, "U2: %3d.%1dmT", abs(u1 / 1000), abs(u1 % 1000) / 100);
  if (u1 < 0) {
    string[4] = '-';
  }
  drawString(string, 42, 10);
  sprintf(string, "U1: %3d.%1dmT", abs(u2 / 1000), abs(u2 % 1000) / 100);
  if (u2 < 0) {
    string[4] = '-';
  }
  drawString(string, 42, 0);
  sprintf(string, "Angle: %3ld", angle);
  drawString(string, 40, -10);
}

/* Coordinates for all angle-dots in the one quadrant */
static const int8_t dotPositions[] = {
  0, 6, 13, 19, 25, 31, 36, 41, 46, 50, 54, 57, 59, 61, 62, 62
};

void drawWheelAngleDots(int second)
{
  int32_t quadrant;
  int32_t quadrantPos;
  int32_t x, y;

  quadrant    = (second) / 15;
  quadrantPos = second % 15;

  if (quadrant == 0) {
    x = CENTER_X + dotPositions[quadrantPos];
    y = CENTER_Y - dotPositions[15 - quadrantPos];
  } else if (quadrant == 1) {
    x = CENTER_X + dotPositions[15 - quadrantPos];
    y = CENTER_Y + dotPositions[quadrantPos];
  } else if (quadrant == 2) {
    x = CENTER_X - dotPositions[quadrantPos];
    y = CENTER_Y + dotPositions[15 - quadrantPos];
  } else {
    x = CENTER_X - dotPositions[15 - quadrantPos];
    y = CENTER_Y - dotPositions[quadrantPos];
  }
  drawPixelDot(x, y);
}

void GRAPHICS_WheelAngle(bool i2cError, int32_t u1, int32_t u2, int32_t angle)
{
  int8_t i;
  /* Clear the screen */
  GLIB_clear(&gc);

  if (i2cError) {
    glib_ConnectEXP();
  } else {
    drawWheelAngleValues(u1, u2, angle);
    for (i = 15; i <= angle / 6 + 15; i++) {
      if (i < 60) {
        drawWheelAngleDots(i);
      }
    }
    if (angle > 270) {
      for (i = 0; i <= (angle - 270) / 6; i++) {
        drawWheelAngleDots(i);
      }
    }
  }
  DMD_updateDisplay();
}

void GRAPHICS_RevCounter(bool i2cError, int8_t revCount, uint8_t quad)
{
  GLIB_clear(&gc);
  char string[15];
  int8_t i, x1, y1, x2, y2;

  if (i2cError) {
    glib_ConnectEXP();
  } else {
    sprintf(string, "Revolution");
    drawString(string, 40, 12);
    sprintf(string, "Count: %d", abs(revCount));
    if (revCount < 0) {
      string[6] = '-';
    }
    drawString(string, 35, 0);
    if (quad) {
      sprintf(string, "Quad:%1d", quad);
      drawString(string, 30, -20);
      for (i = 0; i < 8; i++) {
        if (quad == 1) {
          x1 = x2 = CENTER_X + dotPositions[15 - i];
          y1 = CENTER_Y + dotPositions[i];
          y2 = CENTER_Y - dotPositions[i];
        } else if (quad == 2) {
          x1 = CENTER_X + dotPositions[i];
          x2 = CENTER_X - dotPositions[i];
          y1 = y2 = CENTER_Y + dotPositions[15 - i];
        } else if (quad == 3) {
          x1 = x2 = CENTER_X - dotPositions[15 - i];
          y1 = CENTER_Y + dotPositions[i];
          y2 = CENTER_Y - dotPositions[i];
        } else { // quad == 4
          x1 = CENTER_X + dotPositions[i];
          x2 = CENTER_X - dotPositions[i];
          y1 = y2 = CENTER_Y - dotPositions[15 - i];
        }
        drawPixelDot(x1, y1);
        if (i > 0) {
          drawPixelDot(x2, y2);
        }
      }
    }
  }
  DMD_updateDisplay();
}

void GRAPHICS_PS_I2C(bool i2cError, bool fieldScale, int32_t uTField)
{
  uint16_t fieldInt = 0;
  uint16_t fieldDec = 0;
  char string[15];
  GLIB_clear(&gc);
  drawString("PS Board", 32, 50);
  drawString("Si7210 I2C", 40, 35);
  if (i2cError) {
    drawString("Connect PS", 40, 0);
  } else {
    if (fieldScale == SI7210_200MT) {
      sprintf(string, "200mT Scale");
    } else if (fieldScale == SI7210_20MT) {
      sprintf(string, "20mT Scale");
    }
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 20);
    drawString("Field Strength", 55, 5);

    fieldInt = abs(uTField / 1000);
    fieldDec = abs(uTField % 1000) / 100;
    sprintf(string, " %d.%1dmT", fieldInt, fieldDec);
    if (uTField < 0) {
      string[0] = '-';
    }
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -10);

    sprintf(string, "PB1 to change");
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -40);
    sprintf(string, "mT scale");
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -50);
  }
  DMD_updateDisplay();
}

void GRAPHICS_PS_Temp(bool i2cError, int32_t temp)
{
  char string[18];
  GLIB_clear(&gc);

  drawString("PS Board", 32, 50);
  drawString("Si7210 I2C", 40, 35);
  drawString("Temperature", 44, 20);
  if (i2cError) {
    drawString("Connect PS", 40, 0);
  } else {
    sprintf(string, "Temp: %3ld.%01dC", temp / 1000, abs(temp % 1000) / 100);
    drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 0);
  }
  DMD_updateDisplay();
}

void GRAPHICS_PS_Switch(bool i2cError, uint8_t pin, int16_t tamper)
{
  char string[15];
  GLIB_clear(&gc);

  drawString("PS Board", 32, 50);

  drawString("OUTPUT Status", 52, 0);

  if (i2cError) {
    drawString("Si7201 Switch", 52, 35);
    drawString("Si7202 Latch", 52, 20);
  } else {
    drawString("Si7210 I2C", 40, 35);
  }

  if (pin) {
    sprintf(string, "HIGH");
  } else {
    sprintf(string, "LOW");
  }
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -15);
  if (tamper) {
    drawString("TAMPER", 24, -35);
  }
  DMD_updateDisplay();
}

void GRAPHICS_PS_Analog(uint16_t voltageData, int32_t uTField)
{
  char string[15];
  GLIB_clear(&gc);
  drawString("PS Board", 32, 50);
  drawString("Si7211 Analog", 52, 35);
  drawString("Field Strength", 56, 15);

  sprintf(string, " %d.%01d mT", abs(uTField / 1000), abs(uTField % 1000) / 100);
  if (uTField < 0) {
    string[0] = '-';
  }
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 0);

  sprintf(string, "Vout/Vdd (%c)", '%');
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -20);

  sprintf(string, "%d.%01d", (voltageData / 10), (voltageData % 10));
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -35);

  DMD_updateDisplay();
}

void GRAPHICS_PS_PWM(int32_t uTField, uint32_t dCycle)
{
  char string[18];
  GLIB_clear(&gc);
  drawString("PS Board", 32, 50);
  drawString("Si7212 PWM", 40, 35);

  drawString("Field Strength", 56, 15);
  drawString("Duty-Cycle", 40, -20);

  sprintf(string, " %d.%01d mT", abs(uTField / 1000), abs(uTField / 100) % 10);
  if (uTField < 0) {
    string[0] = '-';
  }
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 0);
  sprintf(string, "PWM(%c)%3lu.%01lu", '%', dCycle / 10, dCycle % 10);
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -35);

  DMD_updateDisplay();
}

void GRAPHICS_PS_SENT(int32_t uTField, uint16_t nibbleData)
{
  char string[15];
  GLIB_clear(&gc);

  drawString("PS Board", 32, 50);
  drawString("Si7213 SENT", 44, 35);

  drawString("Field Strength", 56, 15);

  sprintf(string, " %d.%01d mT", abs(uTField / 1000), abs(uTField / 100) % 10);
  if (uTField < 0) {
    string[0] = '-';
  }
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 0);

  drawString("Nibble Data", 44, -20);
  sprintf(string, "%d", nibbleData);
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -35);

  DMD_updateDisplay();
}

void GRAPHICS_ShowCalString(char *strCal)
{
  char string[20];

  GLIB_clear(&gc);
  sprintf(string, "Wheel Position");
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 55);
  sprintf(string, "Calibration");
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 43);

  sprintf(string, "Turn wheel to");
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), 25);

  drawString(strCal, ((GLIB_FONT_WIDTH * strlen(strCal)) / 2), 0);

  sprintf(string, "Then push PB1");
  drawString(string, ((GLIB_FONT_WIDTH * strlen(string)) / 2), -45);
  DMD_updateDisplay();
}
