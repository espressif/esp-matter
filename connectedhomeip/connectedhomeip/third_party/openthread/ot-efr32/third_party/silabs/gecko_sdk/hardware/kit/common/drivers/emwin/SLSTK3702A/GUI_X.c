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
   File        : GUI_X.C
   Purpose     : Config / System dependent externals for GUI
   ---------------------------END-OF-HEADER------------------------------
 */

#include "em_cmu.h"
#include "em_rtc.h"
#include "GUI.h"

#define RTC_FREQ        32768 // 32.768 kHz LFRCO
#define RTC_MS_SHIFT    5 // system timer unit
#define RTC_HIGHEST_BIT (_RTC_COMP_COMP_MASK ^ (_RTC_COMP_COMP_MASK >> 1))
#ifndef RTC_IF_COMP0
  #define RTC_IF_COMP0  (1 << _RTC_IF_COMP_SHIFT)
#endif

/*********************************************************************
 *
 *       Global data
 */
volatile GUI_TIMER_TIME OS_TimeMS = 0;
volatile bool rtcCounting = false;

/***************************************************************************//**
*  @brief
*    RTC Interrupt Handler, invoke callback if defined.
*******************************************************************************/
void RTC_IRQHandler(void)
{
  uint32_t flags;

  flags = RTC_IntGet();

  if (flags & RTC_IF_COMP0) {
    RTC_IntClear(RTC_IF_COMP0);
    RTC_IntDisable(RTC_IF_COMP0);
    rtcCounting = false;
  }
}

/***************************************************************************//**
 *  @brief
 *    returns system time in milisecond unit.
 *  @details
 *    This function returns system time. The unit is 1/1024 of second due to
 *    fact that RTC is used for counting time.
 ******************************************************************************/
GUI_TIMER_TIME GUI_X_GetTime(void)
{
  int timeNow;

  timeNow = RTC_CounterGet() >> RTC_MS_SHIFT;

  if ((timeNow ^ OS_TimeMS) & (RTC_HIGHEST_BIT >> RTC_MS_SHIFT)) {
    // RTC counter overload, increase virtual counter bits
    OS_TimeMS |= _RTC_COMP_COMP_MASK >> RTC_MS_SHIFT;
    OS_TimeMS++;
    OS_TimeMS |= timeNow;
  } else {
    // Counter didn't overload since last call
    OS_TimeMS &= ~(_RTC_COMP_COMP_MASK >> RTC_MS_SHIFT);
    OS_TimeMS |= timeNow;
  }

  return OS_TimeMS;
}

/***************************************************************************//**
 *  @brief
 *    is used to stop code execution for specified time
 *  @param[in] ms
 *    contains number of miliseconds to suspend program. Maximum allowed
 *    value is 10000 (10 seconds).
 *  @details
 *    This routine could enter into EM1 or EM2 mode to reduce power
 *    consumption. If touch panel is not pressed EM2 is executed, otherwise
 *    due to fact that ADC requires HF clock, only EM1 is enabled. This
 *    function is also used to handle joystick state and move cursor
 *    according to it. In addition it could also reinitialize LCD if
 *    previously Advanced Energy Monitor screen was active.
 ******************************************************************************/
void GUI_X_Delay(int ms)
{
  uint32_t currentCount, targetCount;

  // Clear old counter
  RTC_IntDisable(RTC_IF_COMP0);
  RTC_IntClear(RTC_IF_COMP0);

  // Setup new counting
  currentCount = RTC_CounterGet();
  targetCount = currentCount + ((ms * RTC_FREQ) / 1000);
  RTC_CompareSet(0, targetCount & _RTC_COMP_COMP_MASK);
  rtcCounting = true;
  RTC_IntEnable(RTC_IF_COMP0);

  // Start counting
  RTC_Enable(true);
  while (rtcCounting) {
  }
}

/*********************************************************************
 *
 *       GUI_X_Init()
 *
 * Note:
 *     GUI_X_Init() is called from GUI_Init is a possibility to init
 *     some hardware which needs to be up and running before the GUI.
 *     If not required, leave this routine blank.
 */
void GUI_X_Init(void)
{
  RTC_Init_TypeDef ri;

  // Setup clocks
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO); // LFRCO -> LFACLK -> RTC
  CMU_ClockEnable(cmuClock_RTC, true);
  CMU_ClockEnable(cmuClock_HFLE, true); // Enable LE peripherals

  // Start RTC as disabled
  ri.enable = false;
  ri.debugRun = false;
  ri.comp0Top = false;
  RTC_Init(&ri);

  // Setup interrupts
  RTC_IntDisable(_RTC_IF_MASK); // Disable interrupts from compare channels
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn); // Enable RTC interrupt line
}

/*********************************************************************
 *
 *       GUI_X_ExecIdle
 *
 * Note:
 *  Called if WM is in idle state
 */
void GUI_X_ExecIdle(void)
{
  GUI_X_Delay(100);
}

/*********************************************************************
 *
 *      Logging: OS dependent

   Note:
   Logging is used in higher debug levels only. The typical target
   build does not use logging and does therefor not require any of
   the logging routines below. For a release build without logging
   the routines below may be eliminated to save some space.
   (If the linker is not function aware and eliminates unreferenced
   functions automatically)

 */
void GUI_X_Log(const char *s)
{
  GUI_USE_PARA(s);
}
void GUI_X_Warn(const char *s)
{
  GUI_USE_PARA(s);
}
void GUI_X_ErrorOut(const char *s)
{
  GUI_USE_PARA(s);
}

/*************************** End of file ****************************/
