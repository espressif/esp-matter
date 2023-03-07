/******************************************************************************

 @file  _hal_sleep.c

 @brief This module contains the HAL power management procedures for the
        CC2540. This is throw away code for 10/1 delivery.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2006-2022, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_sleep.h"
#include "hal_rtc_wrapper.h"
#include "hal_gpio_wrapper.h"
#include "osal_clock.h"
#include "rf_hal.h"
#include "osal.h"
//
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ioc.h>
#include <inc/hw_ints.h>
#include <driverlib/prcm.h>
#include <driverlib/aon_rtc.h>
#include <driverlib/aon_ioc.h>
#include <driverlib/pwr_ctrl.h>
#include <driverlib/vims.h>
#include <driverlib/aon_wuc.h>
#include <driverlib/aux_wuc.h>
#include <driverlib/osc.h>
#include "hapi.h"

// TEMP?
#include "ll_rat.h"
#include "hal_trng_wrapper.h"
#include "hal_drivers.h"

// SW Tracer
#ifdef DEBUG_SW_TRACE
#define DBG_ENABLE
#include "dbgid_sys_slv.h"
#include "dbgid_sys_mst.h"
#endif // DEBUG_SW_TRACE

#if defined( PM_DISABLE_PWRDOWN )
#include "onboard.h"
#endif // PM_DISABLE_PWRDOWN

/*******************************************************************************
 * MACROS
 */

// convert msec to 32kHz units without rounding up, so may lose 30.517us.
// Note: The ratio of 32 kHz ticks to msec ticks is 32768/1000 or 4096/125.
#define HAL_SLEEP_MS_TO_32KHZ( ms )         ((((uint32) (ms)) * 4096) / 125)

// convert 32kHz units to msec without rounding up, so may lose 1ms
// Note : The ratio of msec ticks to 32 kHz ticks is 1000/32768 or 125/4096.
#define HAL_SLEEP_32KHZ_TO_MS( ms )         ((((uint32) (ms)) * 125) / 4096)

/*******************************************************************************
 * CONSTANTS
 */

// Minimum Sleep Overhead
#define MIN_TIME_TO_SLEEP                    1  // in ms

// RTC Interrupt Reasons
#define RTC_INT_NONE                         0
#define RTC_INT_OSAL_TIMER_EVENT             1
#define RTC_INT_RADIO_EVENT                  2

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// flag used to override sleep due to an interrupt
// There is a race condition with OSAL PM where an interrupt that sets an OSAL
// event can happen after the OSAL loop checks for events, and Sleep could
// result in a hang. This flag is used to override such hangs.
volatile uint8 halSleepOverride = FALSE;

// RTC Timestamps (taken before/after Sleep)
uint32 halTimestamp1;
uint32 halTimestamp2;

// variable to save/restore GPIOs during PM (if used)
volatile uint32 savedGPIO;

// variable to indicate cold/warm boot
volatile uint32 halBootState = HAL_BOOT_COLD;

/*******************************************************************************
 * PROTOTYPES
 */

// RTC Based Callbacks
void halRtcIntForRfCback( void );
void halRtcIntForOsalCback( void );

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn          halSleepInit
 *
 * @brief       This function is called from main.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halSleepInit( void )
{
  // register RTC interrupt callback for channel 0 - OSAL Timer Event
  halRtcRegIntCBack( AON_RTC_CH0, halRtcIntForOsalCback );

  // register RTC interrupt callback for channel 1 - RF Radio Event
  halRtcRegIntCBack( AON_RTC_CH1, halRtcIntForRfCback );

  return;
}


/*******************************************************************************
 * @fn          halSleep
 *
 * @brief       This function is called from the OSAL task loop using and
 *              existing OSAL interface. It determines if an OSAL timer is
 *              pending, in which case it sets up the RTC to wake the device
 *              for that event, and puts the device to sleep. The OSAL timers
 *              are adjusted upon wake in case the device was awoken sooner due
 *              to some other interrupt. If no OSAL timer event is pending,
 *              then the device is put to sleep.
 *
 *              Note: Presently, only CM3 is powered down.
 *
 * input parameters
 *
 * @param       osal_timeout - Next OSAL timer timeout, in msec.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halSleep( uint32 osal_timeout )
{
  // check if an interrupt occurred
  if ( halSleepOverride == TRUE )
  {
    // yes, so don't bother with sleep
    return;
  }
  else
  {
    //HAL_ASSERT( HAL_INTERRUPTS_ARE_ENABLED() );
    HAL_DISABLE_INTERRUPTS();

    // check if there's an OSAL timer event
    if ( osal_timeout == 0 )
    {
      // stop sysTick interrupts so we don't wake every 1ms
      SysTickIntDisable();

      //ENTER_SLEEP( timestamp1, timestamp2 );
      halEnterSleep();

      // update OSAL timer by elapsed sleep time
      halAdjustOsalTimer( halTimestamp1, halTimestamp2 );

      // allow sysTick interrupts again
      SysTickIntEnable();
    }
    // check if the time to the next OSAL timeout greater than the minimum
    else if ( osal_timeout > MIN_TIME_TO_SLEEP )
    {
      uint32 rtcTime;

      // build RTC comparator, adding any fractional rollover to seconds
      rtcTime = halBuildRtcCompare( osal_timeout );

      // stop sysTick interrupts so we don't wake every 1ms
      SysTickIntDisable();

      // set RTC comparator
      halSetRtcTimerEvent( AON_RTC_CH0, rtcTime );

      //ENTER_SLEEP( timestamp1, timestamp2 );
      halEnterSleep();

      // check if we woke from the RTC interrupt
      if ( halGetRtcIntFlag(AON_RTC_CH0) )
      {
        // yes, so adjust timers by OSAL timeout
        osalAdjustTimer( osal_timeout );
      }
      else // woke up from some other asynchronous cause
      {
        // so disable RTC interrupt
        halClearRtcTimerEvent( AON_RTC_CH0 );

        // update OSAL timer by elapsed sleep time
        halAdjustOsalTimer( halTimestamp1, halTimestamp2 );
      }

      // allow sysTick interrupts again
      SysTickIntEnable();
    }

    HAL_ENABLE_INTERRUPTS();
  }

  return;
}


/*******************************************************************************
 * @fn          halEnterSleep
 *
 * @brief       This function is called to enter sleep, and should be called
 *              from OSAL with interrupts disabled. If an interrupt occurs
 *              after interrupts are disabled, then the MCU will not enter
 *              sleep.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halEnterSleep( void )
{
  HAL_GPIO_SET( HAL_GPIO_3 );

  halTimestamp1 = halReadRtcTimer();

  // check if an interrupt occurred before interrupts were disabled
  if ( halSleepOverride == FALSE )
  {
    // power off any peripherals and domains in CM3 core
    halPowerOffReq();

    // stop the CM3
    // Note: If an interrupt occurred any time after Disable Interrupts and this
    //       call, then CM3 will not Sleep.
    if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
    {
      // indicate boot as warm when we wake from Deep Sleep
      halBootState = HAL_BOOT_WARM;

      // when Power Down mode is used, there is no return from here
      PRCMDeepSleep();

      // we didn't deep sleep if we get here, so reset flag back to cold boot
      halBootState = HAL_BOOT_COLD;

//#if defined( PM_DISABLE_PWRDOWN )
      // simulate warm boot
      // TEMP: BANGS R0/R1, BUT APPEARS TO WORK OKAY AS THEY ARE JUNK REGS.
      //SystemResetSoft();
//#endif // PM_DISABLE_PWRDOWN
    }
    else // RF Core operation imminent, so don't power down all the domains
    {
      PRCMSleep();
    }

    // power on any peripherals and domains in CM3 core
    // TODO: MOVE TO AFTER PRCMSleep WHEN CM3 SHUTDOWN IS USED AFTER DEEP SLEEP?
    halPowerOnReq();

    // open window to allow time to process the interrupt that woke CM3
    // Note: When the CM3 is sleeping, any interrupt will wake it.
    HAL_ENABLE_INTERRUPTS();
    HAL_DISABLE_INTERRUPTS();
  }

  halTimestamp2 = halReadRtcTimer();

  HAL_GPIO_CLR( HAL_GPIO_3 );

  return;
}


/*******************************************************************************
 * @fn          halPowerOnReq
 *
 * @brief       This function is called to power on the CM3 core related
 *              peripherals and domains.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halPowerOnReq( void )
{
  uint32 domainState;

  // enable clock interface
  // Note: This forces power and clock on the AUX domain.
  OSCInterfaceEnable();

  // check if the RF Core is powered down; i.e. we are coming out of deep sleep
  if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
  {
    // request HF/MF to source from XOSC HF and set LF source to XOSC LF
    OSCClockSourceSet(OSC_SRC_CLK_HF | OSC_SRC_CLK_MF, OSC_XOSC_HF);
    //OSCClockSourceSet(OSC_SRC_CLK_LF, OSC_XOSC_LF);
  }

  // set power domains to enable
  domainState = PRCM_DOMAIN_PERIPH |
                PRCM_DOMAIN_SYSBUS |
                PRCM_DOMAIN_VIMS   |
#if defined(HAL_UART) && (HAL_UART==TRUE)
                PRCM_DOMAIN_SERIAL |
#endif // HAL_UART
                PRCM_DOMAIN_CPU;

  // power on peripheral domain
  // Note: Any other domains that were powered off will be powered on by
  //       wake from PRCMDeepSleep.
  PRCMPowerDomainOn( domainState );
  while( PRCMPowerDomainStatus( domainState ) != PRCM_DOMAIN_POWER_ON );

  // turn off TRNG and CRYPTO when the CM3 is not running
  // TODO: IF DISABLED IN RUN MODE, IS IT THEN DISABLED IN SLEEP OR DEEP SLEEP
  //       MODES TOO?
  PRCMPeripheralRunEnable( PRCM_PERIPH_TRNG );
  PRCMPeripheralRunEnable( PRCM_PERIPH_CRYPTO );
#ifdef DEBUG_GPIO
  PRCMPeripheralRunEnable( PRCM_PERIPH_GPIO );
#endif // DEBUG_GPIO
  PRCMLoadSet();

  // wait for the clock gating to be enabled
  while( !PRCMLoadGet() );

  // reinitialize the TRNG
  HalTRNG_InitTRNG();

  // check if the RF Core is powered down; i.e. we are coming out of deep sleep
  if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
  {
    // carry out clock switching requests
    // Note: There must be a request pending or the system will be stalled.
    // Note: This executes out of ROM to avoid messing up the flash.
    HapiHFSourceSafeSwitch();
  }

  // enable the RTC interrupt
  // Note: We'll Wake on the RTC interrupt event, but we won't take the
  //       the interrupt unless we re-enable this interrupt.
  IntEnable( INT_AON_RTC );

  // switch power source
  PowerCtrlSourceSet( PWRCTRL_PWRSRC_DCDC );

  // restore GPIO values
  // Note: Use the GPIOS that had a value of one when saved to specify the pin
  //       to be used to set to a value of one.
  HAL_RESTORE_GPIOS( savedGPIO );

  return;
}


/*******************************************************************************
 * @fn          halPowerOffReq
 *
 * @brief       This function is called to power off the CM3 core related
 *              peripherals and domains.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halPowerOffReq( void )
{
  uint32 domainState;

  // latch the current value of the GPIOs and save current values
  HAL_SAVE_GPIOS( savedGPIO );

  // check if the RF Core is powered down; i.e. we are coming out of deep sleep
  if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
  {
    // request HF/MF to source from XOSC HF and set LF source to XOSC LF
    // Note: OSC interface is already enabled!
    OSCClockSourceSet(OSC_SRC_CLK_HF | OSC_SRC_CLK_MF, OSC_RCOSC_HF);
    //OSCClockSourceSet(OSC_SRC_CLK_LF, OSC_RCOSC_LF);

    // carry out clock switching requests
    // Note: There must be a request pending or the system will be stalled.
    // Note: This executes out of ROM to avoid messing up the flash.
    HapiHFSourceSafeSwitch();

    // disable clock interface
    // Note: This forces off power and clock in the AUX domain.
    OSCInterfaceDisable();
  }

  // if disabled in Run mode then disabled in Sleep and Deep Sleep modes too?
  // Note: PRCMSleep or PRCMDeepSleep will gate the clocks anyway.
  PRCMPeripheralRunDisable( PRCM_PERIPH_TRNG );
  PRCMPeripheralRunDisable( PRCM_PERIPH_CRYPTO );
#ifdef DEBUG_GPIO
  PRCMPeripheralRunDisable( PRCM_PERIPH_GPIO );
#endif // DEBUG_GPIO
  PRCMLoadSet();

  // wait for the clock gating to be enabled
  while( !PRCMLoadGet() );

  // set the minimum domains that can be powered off
  domainState = PRCM_DOMAIN_PERIPH;
#if defined(HAL_UART) && (HAL_UART==TRUE)
  domainState |= PRCM_DOMAIN_SERIAL;
#endif // HAL_UART

  // check if the RF Core is powered down
  if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
  {
    // it is, so it is safe to power down additional domains
    // Note: PRCMDeepSleep is required!
    // TODO: SEEMS TO BE SOME ISSUE WHEN PRCMDeepSleep IS USED.
    domainState |= PRCM_DOMAIN_SYSBUS | PRCM_DOMAIN_VIMS | PRCM_DOMAIN_CPU;
  }

  // power off domains
  PRCMPowerDomainOff( domainState );
  while( PRCMPowerDomainStatus( domainState ) != PRCM_DOMAIN_POWER_OFF );

//#if !defined( PM_DISABLE_PWRDOWN )
  // check if the RF Core is powered down; i.e. we are coming out of deep sleep
  if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
  {
    // Power Down all but RAM retention (i.e. AON voltage domain is on)

    // request power off of entire MCU voltage domain
    PRCMMcuPowerOff();

    // control which one of the clock sources that is fed into the MCU domain
    AONWUCMcuPowerDownConfig( AONWUC_NO_CLOCK );

    // power down the AUX (sensor domain)
    AUXWUCPowerCtrl( AUX_WUC_POWER_OFF );

    // power down mode on AUX and MCU domain
    AONWUCDomainPowerDownEnable();

    // poll until AUX power is off
    while( AONWUCPowerStatus() & AONWUC_AUX_POWER_ON );
  }
//#endif // PM_DISABLE_PWRDOWN

  // check if the RF Core is powered down
  if ( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING )
  {
    // DCDC and GLDO (high power source) are mutually exclusive.
    PowerCtrlSourceSet( PWRCTRL_PWRSRC_GLDO );
  }

  // Set cache mode
  // Disable cache
  VIMSModeSet( VIMS_BASE, VIMS_MODE_DISABLED );

  // Wait for disabling to be complete
  while ( VIMSModeGet( VIMS_BASE ) != VIMS_MODE_DISABLED );

  return;
}


/*******************************************************************************
 * @fn          halWarmBoot
 *
 * @brief       This function is called to warm boot the system after waking
 *              from Power Down after PRCMDeepSleep.
 *
 *              Note: This is one solution. But notice that the code below
 *                    really is the same code as that which follows DeepSleep!
 *                    So a better solution would be to have the Reset ISR
 *                    detect Warm Boot as it does now, only have it restore
 *                    the PC back to the instruction following the DeepSleep
 *                    call! That would render this routine moot.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halWarmBoot( void )
{
  // clear boot cause for safety
  halBootState = HAL_BOOT_COLD;

  // VIMS enabled (so iCache is enabled)

  // power on any peripherals and domains in CM3 core
  halPowerOnReq();

  // open window to allow time to process the interrupt that woke CM3
  // Note: When the CM3 is sleeping, any interrupt will wake it.
  HAL_ENABLE_INTERRUPTS();
  HAL_DISABLE_INTERRUPTS();

  // get second timestamp
  halTimestamp2 = halReadRtcTimer();

  // indicate we're CM3 awake
  HAL_GPIO_CLR( HAL_GPIO_3 );

  // check if we woke from the RTC interrupt
  if ( !halGetRtcIntFlag(AON_RTC_CH0) )
  {
    // so disable RTC interrupt
    halClearRtcTimerEvent( AON_RTC_CH0 );
  }

  // update OSAL timer by elapsed sleep time
  halAdjustOsalTimer( halTimestamp1, halTimestamp2 );

  // allow sysTick interrupts again
  SysTickIntEnable();

  // restore SP
  asm("MOV LR, #0");
  asm("LDR SP, [LR, #0]");
  asm("SUB SP, #0x10");

  HAL_ENABLE_INTERRUPTS();

  // start OSAL
  // Note: No return from here.
  osal_start_system();

  return;
}


/*******************************************************************************
 * @fn          halRestoreSleepLevel
 *
 * @brief       Restore the deepest timer sleep level.
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halRestoreSleepLevel( void )
{
  // Stub
#ifdef PM_TEST
  osal_start_timerEx (Hal_TaskID, HAL_SLEEP_TIMER_EVENT, 1000);
#endif // PM_TEST

  return;
}


/*******************************************************************************
 * @fn          TimerElapsed
 *
 * @brief       Determine the number of OSAL timer ticks elapsed during sleep.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Number of timer ticks elapsed during sleep.
 */
uint32 TimerElapsed( void )
{
  return( 0 );
}


/*******************************************************************************
 * @fn          halAdjustOsalTimer
 *
 * @brief       Update OSAL timer by elapsed sleep time.
 *
 * input parameters
 *
 * @param       timestamp1 - Timestamp from before sleep began.
 * @param       timestamp2 - Timestamp from after sleep ended.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halAdjustOsalTimer( uint32 timestamp1, uint32 timestamp2 )
{
  uint32 rtcTime;

  // get sleep duration
  rtcTime = halRtcTimerDelta( timestamp1, timestamp2 );

  // convert to ms
  rtcTime = ((rtcTime >> 16) * 1000) + HAL_SLEEP_32KHZ_TO_MS( (rtcTime & 0xFFFF) );

  // update OSAL timers with elapsed sleep time, in ms
  osalAdjustTimer( rtcTime );

  return;
}


/*******************************************************************************
 * @fn          halBuildRtcCompare
 *
 * @brief       Creates the proper RTC compare based on a time that is relative
 *              to current RTC time.
 *
 * input parameters
 *
 * @param       timeout - Compare time that is relative to current RTC time.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      RTC compare value corresponding to the relative timeout from
 *              current RTC time.
 */
uint32 halBuildRtcCompare( uint32 timeout )
{
  uint32 timeStamp;
  uint32 rtcTime;

  // take a snapshot of the current RTC time
  timeStamp = halReadRtcTimer();

  // convert timeout in ms to 32kHz ticks and add to RTC fractional part
  // Note: RTC counter bumps ms count by twohalCalcRtcCompare each tick.
  // Note: Rollover in seconds contained in variable time.
  // Note: The ms to 32kHz conversion could be early by one 32kHz timer tick
  //       due to rounding.
  rtcTime = (timeStamp & 0xFFFF) + (HAL_SLEEP_MS_TO_32KHZ(timeout)<<1);

  // TODO: check if the time to next wake event is greater than max sleep
  //       time; this would be 18.2 hours, so no need until RTC is used as
  //       part of some calender service.

  // build RTC comparator, adding any fractional rollover to seconds
  rtcTime = (timeStamp & 0xFFFF0000) + rtcTime;

#if !defined(POWER_SAVING)
#ifdef DEBUG_SW_TRACE
  DBG_PRINT0(DBGSYS, "");
  DBG_PRINTL1(DBGSYS, "Input (ms)  = 0x%08X", timeout);
  DBG_PRINTL1(DBGSYS, "Current RTC = 0x%08X", timeStamp);
  DBG_PRINTL1(DBGSYS, "RTC Compare = 0x%08X", rtcTime);
  DBG_PRINT0(DBGSYS, "");
#endif // DEBUG_SW_TRACE
#endif // !POWER_SAVING

  return( rtcTime );
}


/*******************************************************************************
 * @fn          halRtcIntForOsalCback
 *
 * @brief       This function is called from the RTC ISR when a RTC channel 0
 *              event occurs. RTC Channel 0 is used for a RTC Compare based
 *              on an OSAL Timer event.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halRtcIntForOsalCback( void )
{
#if !defined(POWER_SAVING)
#ifdef DEBUG_SW_TRACE
  DBG_PRINT0(DBGSYS, "RTC Chan 0 Int Callback Done");
#endif // DEBUG_SW_TRACE
#endif // !POWER_SAVING

  return;
}


/*******************************************************************************
 * @fn          halRtcIntForRfCback
 *
 * @brief       This function is called from the RTC ISR when a RTC channel 1
 *              event occurs. RTC Channel 1 is used for a RTC Compare based
 *              on an RF Radio event.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void halRtcIntForRfCback( void )
{
  // check if the RFCore is sleeping
  HAL_ASSERT( rfCoreState == RFHAL_RFCORE_STATE_SLEEPING );

  // power up the RF Core
  LL_PowerOnReq();

#ifdef DEBUG
  // error check if we got command done interrupt
  if ( MB_RFCPEIFG_REG & (MB_COMMAND_DONE_INT | MB_LAST_COMMAND_DONE_INT) )
  {
#ifdef DEBUG_SW_TRACE
    DBG_PRINT0(DBGSYS, "Uh Oh, pending interrupts left over!");
#endif // DEBUG_SW_TRACE
    HAL_ASSERT( FALSE );

#ifdef DEBUG_SW_TRACE
  DBG_PRINT0(DBGSYS, "RTC Chan 1 Int Callback Done");
#endif // DEBUG_SW_TRACE
  }
#endif // DEBUG

  return;
}


/*******************************************************************************
 */















