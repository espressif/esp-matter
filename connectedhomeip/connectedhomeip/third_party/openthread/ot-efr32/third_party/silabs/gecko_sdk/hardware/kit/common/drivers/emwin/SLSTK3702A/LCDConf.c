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
   File        : LCDConf.c
   Purpose     : Display controller configuration (single layer)
   ---------------------------END-OF-HEADER------------------------------
 */

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "LCDConf.h"
#include "WM.h"
#include "bsp_tdk.h"
#include "captouchconfig.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "i2ccaptouch.h"

#define DISPLAY_BACKLIGHT_PORT (gpioPortI)
#define DISPLAY_BACKLIGHT_PIN  (6)
#define DISPLAY_ENABLE_PORT    (gpioPortI)
#define DISPLAY_ENABLE_PIN     (1)

/*********************************************************************
 *
 *       Layer configuration (to be modified)
 *
 **********************************************************************
 */

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_M565 // Pixel format: RRRRRGGGGGGBBBBB

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_LIN_16

/*********************************************************************
 *
 *       Configuration checking
 *
 **********************************************************************
 */
#ifndef   VRAM_ADDR
  #define VRAM_ADDR ((void *) EBI_MEM_BASE)
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif

/*********************************************************************
 *
 *       Static code
 *
 **********************************************************************
 */
/*********************************************************************
 *
 *       _InitController
 *
 * Purpose:
 *   Should initialize the display controller
 */
static void _InitController(void)
{
  // Enable display backlight
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(DISPLAY_BACKLIGHT_PORT, DISPLAY_BACKLIGHT_PIN,
                  gpioModePushPull, 1);
  GPIO_PinModeSet(DISPLAY_ENABLE_PORT, DISPLAY_ENABLE_PIN,
                  gpioModePushPull, 1);
}

/*********************************************************************
 *
 *       _SetVRAMAddr
 *
 * Purpose:
 *   Should set the frame buffer base address
 */
static void _SetVRAMAddr(void * pVRAM)
{
  /* TBD by customer */
  (void) pVRAM;
}

/*********************************************************************
 *
 *       _SetOrg
 *
 * Purpose:
 *   Should set the origin of the display typically by modifying the
 *   frame buffer base address register
 */
static void _SetOrg(int xPos, int yPos)
{
  /* TBD by customer */
  (void) xPos;
  (void) yPos;
}

/*********************************************************************
 *
 *       _SetLUTEntry
 *
 * Purpose:
 *   Should set the desired LUT entry
 */
static void _SetLUTEntry(LCD_COLOR Color, U8 Pos)
{
  /* TBD by customer */
  (void) Color;
  (void) Pos;
}

/*********************************************************************
 *
 *       Public code
 *
 **********************************************************************
 */

/**************************************************************************//**
 * @brief Gets touch data from CTP controller and updates GUI_TOUCH
 *****************************************************************************/
void readTouch(void)
{
  CAPT_Touch touch;
  uint8_t reads;
  int x, y;
  uint32_t status;

  status = CAPT_getTouches(&touch, 1, &reads);
  if (status != CAPT_OK) {
    return;
  }

  if (reads > 0) {
    x = (int) (touch.x * XSIZE_PHYS);
    y = (int) (touch.y * YSIZE_PHYS);
    GUI_TOUCH_StoreState(x, y);
  } else {
    GUI_TOUCH_StoreState(-1, -1);
  }
}

/*********************************************************************
 *
 *       LCD_X_Config
 *
 * Purpose:
 *   Called during the initialization process in order to set up the
 *   display driver configuration.
 *
 */
void LCD_X_Config(void)
{
  // Crank up the clocks
  CMU_HFRCOBandSet(cmuHFRCOFreq_72M0Hz);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  // Enable framebuffer SRAM and Direct Drive
  BSP_initEbiSram();
  BSP_initEbiFlash();
  BSP_initEbiTftDirectDrive();

  // Set display driver and color conversion for 1st layer
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);

  // Display driver configuration
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);

  // Set user palette data (only required if no fixed palette is used)
  #if defined(PALETTE)
  LCD_SetLUTEx(0, PALETTE);
  #endif

  // Enable driver for touch controller
  CAPT_init();
  CAPT_enable(true, true);
  WM_SetpfPollPID(readTouch);
}

/*********************************************************************
 *
 *       LCD_X_DisplayDriver
 *
 * Purpose:
 *   This function is called by the display driver for several purposes.
 *   To support the according task the routine needs to be adapted to
 *   the display controller. Please note that the commands marked with
 *   'optional' are not cogently required and should only be adapted if
 *   the display controller supports these features.
 *
 * Parameter:
 *   LayerIndex - Index of layer to be configured
 *   Cmd        - Please refer to the details in the switch statement below
 *   pData      - Pointer to a LCD_X_DATA structure
 *
 * Return Value:
 *   < -1 - Error
 *     -1 - Command not handled
 *      0 - Ok
 */
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
  int r;

  (void) LayerIndex;

  switch (Cmd) {
    //
    // Required
    //
    case LCD_X_INITCONTROLLER: {
      //
      // Called during the initialization process in order to set up the
      // display controller and put it into operation. If the display
      // controller is not initialized by any external routine this needs
      // to be adapted by the customer...
      //
      _InitController();
      return 0;
    }
    case LCD_X_SETVRAMADDR: {
      //
      // Required for setting the address of the video RAM for drivers
      // with memory mapped video RAM which is passed in the 'pVRAM' element of p
      //
      LCD_X_SETVRAMADDR_INFO * p;
      p = (LCD_X_SETVRAMADDR_INFO *)pData;
      _SetVRAMAddr(p->pVRAM);
      return 0;
    }
    case LCD_X_SETORG: {
      //
      // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
      //
      LCD_X_SETORG_INFO * p;
      p = (LCD_X_SETORG_INFO *)pData;
      _SetOrg(p->xPos, p->yPos);
      return 0;
    }
    case LCD_X_SETLUTENTRY: {
      //
      // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
      //
      LCD_X_SETLUTENTRY_INFO * p;
      p = (LCD_X_SETLUTENTRY_INFO *)pData;
      _SetLUTEntry(p->Color, p->Pos);
      return 0;
    }
    case LCD_X_ON: {
      //
      // Required if the display controller should support switching on and off
      //
      return 0;
    }
    case LCD_X_OFF: {
      //
      // Required if the display controller should support switching on and off
      //
      // ...
      return 0;
    }
    default:
      r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
