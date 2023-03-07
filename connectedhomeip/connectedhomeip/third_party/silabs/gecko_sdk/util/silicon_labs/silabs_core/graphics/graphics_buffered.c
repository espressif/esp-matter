/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "graphics.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <string.h>
#include <stdio.h>

#define X_BORDER 5
#define Y_BORDER 2
#define MAX_DISPLAY_LINES ((DISPLAY0_HEIGHT - Y_BORDER * 2) / (8))
#define MAX_DISPLAY_CHARS ((DISPLAY0_WIDTH - X_BORDER * 2) / (6))

// Create a text buffer that can hold as many characters as our screen can
// assuming a of all the lines we're currently displaying
static char textBuffer[MAX_DISPLAY_LINES][MAX_DISPLAY_CHARS + 1];
static uint8_t textBufferHead = 0;

static uint32_t xOffset, yOffset;

GLIB_Context_t glibContext;          /* Global glib context */

static void GRAPHICS_CreateString(char *string, int32_t value);

/**************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 *****************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    while (1) ;
  }

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) ;
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) ;
  }

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Use Normal font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
}

/**************************************************************************//**
 * @brief This function draws the initial display screen
 *****************************************************************************/
void GRAPHICS_Update(void)
{
  uint32_t count = 0, i = textBufferHead;
  uint32_t xOffset = X_BORDER;
  uint32_t yOffset = Y_BORDER;

  GLIB_clear(&glibContext);

  while (count < MAX_DISPLAY_LINES) {
    GLIB_drawString(&glibContext,
                    textBuffer[i],
                    strlen(textBuffer[i]),
                    xOffset,
                    yOffset,
                    0);
    yOffset += 8; // Increment the yOffset by the font height + 1
    count++;

    // Increment the buffer index while being sure to wrap at the end
    i++;
    if (i >= MAX_DISPLAY_LINES) {
      i = 0;
    }
  }
  DMD_updateDisplay();
}
//      y = y + pContext->font.fontHeight + pContext->font.lineSpacing;

void GRAPHICS_Clear(void)
{
  GLIB_clear(&glibContext);

  // Reset the offset values
  xOffset = X_BORDER;
  yOffset = Y_BORDER;
}

void GRAPHICS_AppendString(char *str)
{
  uint32_t len = strlen(str);

  // Truncate if something too long is passed in
  if (len > MAX_DISPLAY_CHARS) {
    len = MAX_DISPLAY_CHARS;
  }

  // Insert this string at the end of the text buffer
  memcpy(textBuffer[textBufferHead], str, len);
  textBuffer[textBufferHead][len] = '\0';

  // Increment and wrap the tail pointer
  textBufferHead++;
  if (textBufferHead >= MAX_DISPLAY_LINES) {
    textBufferHead = 0;
  }
}

/**************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTC frequency.
 *****************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  return 0;
}
