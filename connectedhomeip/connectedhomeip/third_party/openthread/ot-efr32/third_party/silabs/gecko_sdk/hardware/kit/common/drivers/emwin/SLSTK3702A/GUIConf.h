/*********************************************************************
 *                SEGGER Microcontroller GmbH & Co. KG                *
 *        Solutions for real time microcontroller applications        *
 **********************************************************************
 *                                                                    *
 *        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
 *                                                                    *
 *        Internet: www.segger.com    Support:  support@segger.com    *
 *                                                                    *
 **********************************************************************

 ** emWin V5.34 - Graphical user interface for embedded applications **
   All  Intellectual Property rights  in the Software belongs to  SEGGER.
   emWin is protected by  international copyright laws.  Knowledge of the
   source code may not be used to write a similar product.  This file may
   only be used in accordance with the following terms:

   The  software has  been licensed  to Silicon Labs Norway, a subsidiary
   of Silicon Labs Inc. whose registered office is 400 West Cesar Chavez,
   Austin,  TX 78701, USA solely for  the purposes of creating  libraries
   for its  ARM Cortex-M3, M4F  processor-based devices,  sublicensed and
   distributed  under the  terms and conditions  of the  End User License
   Agreement supplied by Silicon Labs.
   Full source code is available at: www.segger.com

   We appreciate your understanding and fairness.
   ----------------------------------------------------------------------
   Licensing information

   Licensor:                 SEGGER Software GmbH
   Licensed to:              Silicon Laboratories Norway
   Licensed SEGGER software: emWin
   License number:           GUI-00140
   License model:            See Agreement, dated 20th April 2012
   Licensed product:         -
   Licensed platform:        Cortex M3, Cortex M4F
   Licensed number of seats: -
   ----------------------------------------------------------------------
   File        : GUIConf.h
   Purpose     : Configures emWins abilities, fonts etc.
   ----------------------------------------------------------------------
 */

#ifndef GUICONF_H
#define GUICONF_H

/*********************************************************************
 *
 *       Multi layer/display support
 */
#define GUI_NUM_LAYERS            1    // Maximum number of available layers

/*********************************************************************
 *
 *       Multi tasking support
 */
#define GUI_OS                    (0)  // Compile with multitasking support

/*********************************************************************
 *
 *       Configuration of touch support
 */
#define GUI_SUPPORT_TOUCH         (1)  // Support a touch screen (req. WM)

/*********************************************************************
 *
 *       Default font
 */
#define GUI_DEFAULT_FONT          &GUI_Font6x8

/*********************************************************************
 *
 *         Configuration of available packages
 */
#define GUI_SUPPORT_MOUSE    1    // Mouse support
#define GUI_WINSUPPORT       1    // Use Window Manager
#define GUI_SUPPORT_MEMDEV   1    // Use Memory Devices

#endif  // Avoid multiple inclusion

/*************************** End of file ****************************/
