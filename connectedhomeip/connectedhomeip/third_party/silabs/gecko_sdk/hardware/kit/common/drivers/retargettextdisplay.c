/***************************************************************************//**
 * @file
 * @brief Provide stdio retargeting to text display interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>

#include "displayconfigall.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/*******************************************************************************
 ********************************  STATICS  ************************************
 ******************************************************************************/

/* Handle which references the selected text display to print text on. */
TEXTDISPLAY_Handle_t  textDisplayHandle = 0;

/** @endcond */

/*******************************************************************************
 **************************     GLOBAL FUNCTIONS      **************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief Initialize/retarget a TEXTDISPLAY device to receivie stdout(put).
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS RETARGET_TextDisplayInit(void)
{
  EMSTATUS              status;
  DISPLAY_Device_t      displayDevice;
  TEXTDISPLAY_Config_t  textDisplayConfig;

  /* Query that the specified DISPLAY device is available.  */
  status = DISPLAY_DeviceGet(RETARGETTEXTDISPLAY_DISPLAY_NO, &displayDevice);

  if (DISPLAY_EMSTATUS_OK == status) {
    textDisplayConfig.displayDeviceNo  = RETARGETTEXTDISPLAY_DISPLAY_NO;
    textDisplayConfig.scrollEnable     = RETARGETTEXTDISPLAY_SCROLL_MODE;
    textDisplayConfig.lfToCrLf         = RETARGETTEXTDISPLAY_LINE_FEED_MODE;

    status = TEXTDISPLAY_New(&textDisplayConfig, &textDisplayHandle);

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
    if (TEXTDISPLAY_EMSTATUS_OK == status) {
      /* Set unbuffered mode for stdout (newlib) */
      setvbuf(stdout, NULL, _IONBF, 0);
    }
#endif
  }

  return status;
}

/**************************************************************************//**
 * @brief Receive a byte
 *    No input method from the text display is possible, thus we always
 *    return -1
 *
 * @return -1 on failure
 *****************************************************************************/
int RETARGET_ReadChar(void)
{
  return -1;
}

/**************************************************************************//**
 * @brief Write a single byte to the text display
 *
 * @param c Character to write
 *
 * @return Printed character if text display is initialized.
 *         -1 if text display is not initialized.
 *****************************************************************************/
int RETARGET_WriteChar(char c)
{
  if (textDisplayHandle) {
    TEXTDISPLAY_WriteChar(textDisplayHandle, c);
    return c;
  } else {
    return -1;
  }
}

/**************************************************************************//**
 * @brief Write a string of characters to the RETARGET text display device.
 *
 * @param[in] str     String to write.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS RETARGET_WriteString(char*   str)
{
  if (textDisplayHandle) {
    return TEXTDISPLAY_WriteString(textDisplayHandle, str);
  } else {
    return TEXTDISPLAY_EMSTATUS_NOT_INITIALIZED;
  }
}

/***************  THE REST OF THE FILE IS DOCUMENTATION ONLY !  ***************/

/*******************************************************************************
 **************************       DOCUMENTATION       **************************
 ******************************************************************************/

/**************************************************************************//**
   @addtogroup RetargetIo

   @{

   @n @section retargettextdisplay_doc Retarget TextDisplay Module

   The source code of the RETARGETTEXTDISPLAY module is implemented in
   kits/common/drivers/retargettextdisplay.c and retargettextdisplay.h.

   @li @ref retargettextdisplay_intro
   @li @ref retargettextdisplay_config

   @n @section retargettextdisplay_intro Introduction

   The RETARGETTEXTDISPLAY Library implements a stdout interface to a textdisplay
   device (@ref textdisplay_doc) in order for the user to print text by calling
   standard C language functions that prints text to stdout.

   @n @section retargettextdisplay_config Retarget TextDisplay Configuration.

   This section contains a description of the configuration parameters of
   the RETARGETTEXTDISPLAY Library.

   @verbatim
 #define RETARGETTEXTDISPLAY_SCROLL_MODE
        Set to 'true' to enable scroll mode on the text display device where
        stdout is retargeted. Set to 'false' to disable scroll mode.
 #define RETARGETTEXTDISPLAY_LINE_FEED_MODE
        Set to 'true' to enable adding Carriage Return (CR) to Line Feed (LF)
        characters on the text display device where stdout is retargeted.
        Set to 'false' to disable line feed mode.
 #define RETARGETTEXTDISPLAY_DISPLAY_NO
        Select which TEXTDISPLAY device number to retarget stdout to. Normally
        there is only one display device present in the system therefore this
        parameter should be zero. However if there are more than one display
        device the user may want to select a different display device.
   @endverbatim

   @}

   (end group RetargetIo) */
