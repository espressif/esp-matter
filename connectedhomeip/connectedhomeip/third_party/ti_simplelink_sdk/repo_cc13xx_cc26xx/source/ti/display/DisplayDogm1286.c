/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* -----------------------------------------------------------------------------
 *  Includes
 * -----------------------------------------------------------------------------
 */
// TI RTOS drivers
#include <ti/drivers/PIN.h>

#include <ti/display/Display.h>
#include <ti/display/DisplayDogm1286.h>
#include <ti/display/lcd/LCDDogm1286.h>

#include <ti/drivers/dpl/SystemP.h>

/* -----------------------------------------------------------------------------
 *  Constants and macros
 * -----------------------------------------------------------------------------
 */
// Timeout of semaphore that controls exclusive to the LCD (500 ms)
#define ACCESS_TIMEOUT    50000

/* -----------------------------------------------------------------------------
 *   Type definitions
 * -----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------
 *                           Local variables
 * -----------------------------------------------------------------------------
 */
/* Display function table for sharp 96x96 implementation */
const Display_FxnTable DisplayDogm1286_fxnTable = {
    DisplayDogm1286_init,
    DisplayDogm1286_open,
    DisplayDogm1286_clear,
    DisplayDogm1286_clearLines,
    DisplayDogm1286_vprintf,
    DisplayDogm1286_close,
    DisplayDogm1286_control,
    DisplayDogm1286_getType,
};

/* -----------------------------------------------------------------------------
 *                                          Functions
 * -----------------------------------------------------------------------------
 */
  /*!
 * @fn          DisplayDogm1286_init
 *
 * @brief       Does nothing.
 *
 * @return      void
 */
void DisplayDogm1286_init(Display_Handle handle)
{
}

/*!
 * @fn          DisplayDogm1286_open
 *
 * @brief       Initialize the LCD
 *
 * @descr       Initializes the pins used by the LCD, creates resource access
 *              protection semaphore, turns on the LCD device, initializes the
 *              frame buffer, initializes to white background/dark foreground,
 *              and finally clears the object->displayColor.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       params - display parameters
 *
 * @return      Pointer to Display_Config struct
 */
Display_Handle DisplayDogm1286_open(Display_Handle hDisplay,
                                    Display_Params *params)
{
    DisplayDogm1286_HWAttrs *hwAttrs = (DisplayDogm1286_HWAttrs *)hDisplay->hwAttrs;
    DisplayDogm1286_Object  *object  = (DisplayDogm1286_Object  *)hDisplay->object;

    PIN_Config pinTable[1 + 1];

    uint32_t   i = 0;
    if (hwAttrs->powerPin != PIN_TERMINATE)
    {
        pinTable[i++] = hwAttrs->powerPin | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX;
    }
    pinTable[i++] = PIN_TERMINATE;

    object->hPins = PIN_open(&object->pinState, pinTable);
    if (object->hPins == NULL)
    {
        return NULL;
    }

    object->lineClearMode = params->lineClearMode;

    object->lcdBuffers[0].pcBuffer = object->lcdBuffer0;
    object->lcdBuffers[0].bufSize  = 1024;
    object->lcdBuffers[0].bufMutex = object->lcdMutex;

    LCD_Params lcdParams;
    LCD_Params_init(&lcdParams);
    object->hLcd = LCD_open(&object->lcdBuffers[0], 1, &lcdParams);

    if (object->hLcd)
    {
        LCD_bufferClear(object->hLcd, 0);
        LCD_update(object->hLcd, 0);
        return hDisplay;
    }
    else
    {
        PIN_close(object->hPins);
        return NULL;
    }
}


/*!
 * @fn          DisplayDogm1286_clear
 *
 * @brief       Clears the display
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayDogm1286_clear(Display_Handle hDisplay)
{
    DisplayDogm1286_Object *object = (DisplayDogm1286_Object  *)hDisplay->object;

    if (object->hLcd)
    {
        LCD_bufferClear(object->hLcd, 0);
        LCD_update(object->hLcd, 0);
    }
}


/*!
 * @fn          DisplayDogm1286_clearLines
 *
 * @brief       Clears lines lineFrom-lineTo of the display, inclusive
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       lineFrom - line index (0 .. )
 * @param       lineTo - line index (0 .. )
 *
 * @return      void
 */
void DisplayDogm1286_clearLines(Display_Handle hDisplay,
                                uint8_t lineFrom, uint8_t lineTo)
{
    DisplayDogm1286_Object *object = (DisplayDogm1286_Object  *)hDisplay->object;

    if (lineTo < lineFrom)
    {
        lineTo = lineFrom;
    }

    if (object->hLcd)
    {
        uint8_t xMin = 0;
        uint8_t xMax = 127;

        LCD_bufferClearPart(object->hLcd, 0, xMin, xMax,
                            (LCD_Page)lineFrom, (LCD_Page)lineTo);

        LCD_update(object->hLcd, 0);
    }
}


/*!
 * @fn          DisplayDogm1286_put5
 *
 * @brief       Write a text string to a specific line/column of the display
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       line - line index (0..)
 * @param       column - column index (0..)
 * @param       fmt - format string
 * @param       aN - optional format arguments
 *
 * @return      void
 */
void DisplayDogm1286_vprintf(Display_Handle hDisplay, uint8_t line,
                             uint8_t column, const char *fmt, va_list va)
{
    DisplayDogm1286_Object *object = (DisplayDogm1286_Object  *)hDisplay->object;

    char    dispStr[22] = { 0 };
    uint8_t xp, clearStartX, clearEndX;

    xp          = column * 6;
    clearStartX = clearEndX = xp;

    switch (object->lineClearMode)
    {
    case DISPLAY_CLEAR_LEFT:
        clearStartX = 0;
        break;
    case DISPLAY_CLEAR_RIGHT:
        clearEndX = 127;
        break;
    case DISPLAY_CLEAR_BOTH:
        clearStartX = 0;
        clearEndX   = 127;
        break;
    case DISPLAY_CLEAR_NONE:
    default:
        break;
    }

    if (clearStartX != clearEndX)
    {
        LCD_bufferClearPart(object->hLcd, 0,
                            clearStartX, clearEndX, (LCD_Page)line, (LCD_Page)(line));
    }


    SystemP_vsnprintf(dispStr, sizeof(dispStr), fmt, va);

    LCD_bufferPrintString(object->hLcd, 0, dispStr, xp, (LCD_Page)line);
    LCD_update(object->hLcd, 0);
}


/*!
 * @fn          DisplayDogm1286_close
 *
 * @brief       Turns of the display and releases the LCD control pins
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayDogm1286_close(Display_Handle hDisplay)
{
    DisplayDogm1286_Object *object = (DisplayDogm1286_Object  *)hDisplay->object;

    if (object->hPins == NULL)
    {
        return;
    }

    // Turn off the display
    PIN_close(object->hPins);
    object->hPins = NULL;

    LCD_close(object->hLcd);
    object->hLcd = NULL;
}

/*!
 * @fn          DisplayDogm1286_control
 *
 * @brief       Function for setting control parameters of the Display driver
 *              after it has been opened.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       cmd - command to execute
 * @param       arg - argument to the command
 *
 * @return      ::DISPLAY_STATUS_UNDEFINEDCMD because no commands are supported
 */
int DisplayDogm1286_control(Display_Handle handle, unsigned int cmd, void *arg)
{
    return DISPLAY_STATUS_UNDEFINEDCMD;
}

/*!
 * @fn          DisplayDogm1286_getType
 *
 * @brief       Returns type of transport
 *
 * @return      Display type define LCD
 */
unsigned int DisplayDogm1286_getType(void)
{
    return Display_Type_LCD;
}
