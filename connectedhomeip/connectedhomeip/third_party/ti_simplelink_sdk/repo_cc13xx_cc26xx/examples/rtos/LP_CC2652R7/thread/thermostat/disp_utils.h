/******************************************************************************

 @file disp_utils.h

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
#ifndef DISP_UTILS_H
#define DISP_UTILS_H
/******************************************************************************
 Includes
 *****************************************************************************/
/* TIRTOS specific driver header files */
#include <ti/display/Display.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Global variables
 *****************************************************************************/
#if BOARD_DISPLAY_USE_UART && !TIOP_CUI
/* handle to the serial display */
extern Display_Handle serialHandle;
#endif /* BOARD_DISPLAY_USE_UART && !TIOP_CUI */

#if BOARD_DISPLAY_USE_LCD
/* handle to the LCD display */
extern Display_Handle lcdHandle;
#endif /* BOARD_DISPLAY_USE_LCD */

/******************************************************************************
 Constants and Macro Definitions
 *****************************************************************************/

/**
 * Printf functions for the serial interface.
 */
#if BOARD_DISPLAY_USE_UART && !TIOP_CUI
#define DISPUTILS_SERIALPRINTF(...) do { if(serialHandle) \
                                           Display_printf(serialHandle, __VA_ARGS__ ); } while (0)
#else
#define DISPUTILS_SERIALPRINTF(...)
#endif /* BOARD_DISPLAY_USE_UART && !TIOP_CUI */
/**
 * Printf function for the lcd interface.
 */
#if BOARD_DISPLAY_USE_LCD
#define DISPUTILS_LCDPRINTF(...) do { if(lcdHandle) \
                                           Display_printf(lcdHandle, __VA_ARGS__ ); } while (0)

#else

#define DISPUTILS_LCDPRINTF(...)

#endif /* BOARD_DISPLAY_USE_LCD */

/******************************************************************************
 External Functions
 *****************************************************************************/

/**
 * @brief Initialize and opens the display interface to the
 *        serial and the lcd if available.
 *
 * @return None
 */
extern void DispUtils_open(void);


#if BOARD_DISPLAY_USE_LCD
/**
 * @brief Display's the image on the LCD screen.
 *
 * @param pImage pointer to the Graphics Image data structure
 *              which has the information of the image data.
 * @return None
 */
extern void DispUtils_lcdDraw(const Graphics_Image* pImage);
#endif /* BOARD_DISPLAY_USE_LCD */

#ifdef __cplusplus
}
#endif

#endif /* DISP_UTILS_H */
