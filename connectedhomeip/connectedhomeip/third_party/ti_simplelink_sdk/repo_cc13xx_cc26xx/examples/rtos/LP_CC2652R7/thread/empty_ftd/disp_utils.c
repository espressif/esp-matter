/******************************************************************************

 @file disp_utils.c

 @brief Utility functions for the display interface

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
/* Standard Library Header files */
#include <stddef.h>
#include <string.h>
#include <stdio.h> // for varargs

/* Board Header files */
#include "ti_drivers_config.h"

/* grlib header defines `NDEBUG`, undefine here to avoid a compile warning */
#ifdef NDEBUG
#undef NDEBUG
#endif

/* TIRTOS specific driver header files */
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
#include <ti/grlib/grlib.h>

/* OpenThread header files */
#include "openthread-core-config.h"
#include <openthread/config.h>
#include <openthread/platform/logging.h>

#include "disp_utils.h"

/******************************************************************************
 Global variables
 *****************************************************************************/

#if BOARD_DISPLAY_USE_UART && !TIOP_CUI
/* handle to the serial display */
Display_Handle serialHandle;
#endif

#if BOARD_DISPLAY_USE_LCD
/* handle to the LCD display */
Display_Handle lcdHandle;
#endif /* BOARD_DISPLAY_USE_LCD */

/******************************************************************************
 External Functions
 *****************************************************************************/

/**
 * documented in disp_utils.h
 */
void DispUtils_open(void)
{
    Display_Params params;

    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

#if BOARD_DISPLAY_USE_UART && !TIOP_CUI
    serialHandle = Display_open(Display_Type_UART, &params);
#endif /* BOARD_DISPLAY_USE_UART && !TIOP_CUI */

#if BOARD_DISPLAY_USE_LCD
    lcdHandle = Display_open(Display_Type_LCD, &params);
#endif /* BOARD_DISPLAY_USE_LCD */
}

/**
 * Documented in openthread/platform/logging.h.
 *
 * This function is used by the OpenThread stack, be very careful logging
 * application data with it.
 */
#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP)
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion,
               const char *aFormat, ...)
{
#if BOARD_DISPLAY_USE_UART && !TIOP_CUI
    if (NULL != serialHandle)
    {
        va_list args;
        va_start(args, aFormat);

        serialHandle->fxnTablePtr->vprintfFxn(serialHandle, 0, 0, (char *)aFormat, args);

        va_end(args);
    }
#endif /* BOARD_DISPLAY_USE_UART && !TIOP_CUI */

    (void)aLogLevel;
    (void)aLogRegion;
}
#endif


#if BOARD_DISPLAY_USE_LCD
/* refer disp_utils.h */
void DispUtils_lcdDraw(const Graphics_Image* pImage)
{
    /* Check if the selected Display type was found and successfully opened */
    if (lcdHandle)
    {
        /*
         * Use the GrLib extension to get the GraphicsLib context object of the
         * LCD, if it is supported by the display type.
         */
        Graphics_Context *context = DisplayExt_getGraphicsContext(lcdHandle);

        if (context) {
            /* Draw splash */
            Graphics_drawImage(context, pImage, 0, 0);
            Graphics_flushBuffer(context);
        }
    }
}
#else
void DispUtils_lcdDraw(const Graphics_Image* pImage)
{
    (void)pImage;
}
#endif /* BOARD_DISPLAY_USE_LCD */
