/***************************************************************************//**
 * @file
 * @brief Text display interface
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

#ifndef _TEXTDISPLAY_H_
#define _TEXTDISPLAY_H_

#include <stdbool.h>
#include "emstatus.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Textdisplay
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ********************************  DEFINES  ************************************
 ******************************************************************************/

/** EMSTATUS codes of the textdisplay module. */
#define TEXTDISPLAY_EMSTATUS_OK                                              (0) /**< Operation successful. */
#define TEXTDISPLAY_EMSTATUS_INVALID_PARAM       (TEXTDISPLAY_EMSTATUS_BASE | 1) /**< Invalid parameter. */
#define TEXTDISPLAY_EMSTATUS_OUT_OF_RANGE        (TEXTDISPLAY_EMSTATUS_BASE | 2) /**< Parameters out of range. */
#define TEXTDISPLAY_EMSTATUS_ALREADY_INITIALIZED (TEXTDISPLAY_EMSTATUS_BASE | 3) /**< Device is already initialized. */
#define TEXTDISPLAY_EMSTATUS_NOT_SUPPORTED       (TEXTDISPLAY_EMSTATUS_BASE | 4) /**< Feature/option not supported. */
#define TEXTDISPLAY_EMSTATUS_NOT_ENOUGH_MEMORY   (TEXTDISPLAY_EMSTATUS_BASE | 5) /**< Not enough memory. */
#define TEXTDISPLAY_EMSTATUS_NOT_INITIALIZED     (TEXTDISPLAY_EMSTATUS_BASE | 6) /**< Device is not initialized. */

/** VT52/VT100 Escape Sequence codes supported by TEXTDISPLAY: */
#ifdef INCLUDE_VIDEO_TERMINAL_ESCAPE_SEQUENCE_SUPPORT
#define TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100       "\033[H" /**< Move cursor to upper left corner. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT52         "\033H" /**< Move cursor to upper left corner. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE       "\033A" /**< Move cursor up one line. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_DOWN_ONE_LINE     "\033B" /**< Move cursor down one line. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR    "\033C" /**< Move cursor right one column. */
#define TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR     "\033D" /**< Move cursor left one column. */
#endif

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** Handle for text display device. */
typedef void*  TEXTDISPLAY_Handle_t;

/** Initialization data structure for text display devices. */
typedef struct TEXTDISPLAY_Config_t{
  int   displayDeviceNo;    /**< Display device number to initialize test
                                 display on. */
  bool  scrollEnable;       /**< Enable/disable scrolling mode on the text
                                 display. Scrolling mode will consume more
                                 memory because the lines will have to be
                                 stored in memory. */

  bool  lfToCrLf;           /**< Enable or disable LF to CR+LF conversion. */
} TEXTDISPLAY_Config_t;

/*******************************************************************************
 **************************    FUNCTION PROTOTYPES    **************************
 ******************************************************************************/

EMSTATUS TEXTDISPLAY_New         (TEXTDISPLAY_Config_t  *config,
                                  TEXTDISPLAY_Handle_t  *handle);
EMSTATUS TEXTDISPLAY_Delete      (TEXTDISPLAY_Handle_t  handle);
EMSTATUS TEXTDISPLAY_WriteChar   (TEXTDISPLAY_Handle_t  handle,
                                  char                  c);
EMSTATUS TEXTDISPLAY_WriteString (TEXTDISPLAY_Handle_t  handle,
                                  const char*           str);
EMSTATUS TEXTDISPLAY_LfToCrLf    (TEXTDISPLAY_Handle_t  handle,
                                  bool                  on);

#ifdef __cplusplus
}
#endif

/** @} (end group TEXTDISPLAY) */
/** @} (end group kitdrv) */

#endif /* _TEXTDISPLAY_H_ */
