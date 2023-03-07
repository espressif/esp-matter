/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       WatchdogCC26XX.h
 *
 *  @brief      Watchdog driver implementation for CC13XX/CC26XX
 *
 *  # Driver include #
 *  The Watchdog header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/Watchdog.h>
 *  #include <ti/drivers/watchdog/WatchdogCC26XX.h>
 *  @endcode
 *
 *  Refer to @ref Watchdog.h for a complete description of APIs.
 *
 *  # Overview #
 *
 *  The general Watchdog API should be used in application code, i.e.
 *  #Watchdog_open() should be used instead of WatchdogCC26XX_open(). The board
 *  file will define the device specific config, and casting in the general API
 *  will ensure that the correct device specific functions are called.
 *
 *  # General Behavior #
 *  This Watchdog driver implementation is designed to operate on a CC13XX/CC26XX
 *  device. Before using the Watchdog in CC13XX/CC26XX, the Watchdog driver is
 *  initialized by calling #Watchdog_init(). The Watchdog HW is configured by
 *  calling #Watchdog_open(). Once opened, the Watchdog will count down from
 *  the reload value specified in #WatchdogCC26XX_HWAttrs. If it times out, a
 *  non-maskable interrupt will be triggered, the Watchdog interrupt flag will
 *  be set, and a user-provided callback function will be called. If reset is
 *  enabled in the #Watchdog_Params and the Watchdog timer is allowed to time
 *  out again while the interrupt flag is still pending, a reset signal will be
 *  generated. To prevent a reset, #Watchdog_clear() must be called to clear the
 *  interrupt flag and to reload the timer.
 *
 *  The Watchdog counts down at a rate of the device clock SCLK_HF (48 MHz)
 *  divided by a fixed-division ratio of 32, which equals to 1.5 MHz. The
 *  Watchdog rate will change if SCLK_HF deviates from 48 MHz.
 *
 *  @note The Watchdog interrupt is configured as a non-maskable interrupt
 *  (NMI) and the user-defined callback function is called in the context of
 *  NMI. Because the Watchdog interrupt is non-maskable, it is not safe to call
 *  any BIOS APIs from the Watchdog callback function. Calls to DPL and TIDRIVERS
 *  APIs should also be avoided given that they may indirectly make calls to BIOS
 *  APIs. Furthermore, the watchdog callback should only be reached if the
 *  watchdog times out. This should indicate an error event and allow logging
 *  or other housekeeping before resetting the device. Clearing of the watchdog
 *  and checking alive signals should be done in another execution context.
 *
 *  The reload value from which the Watchdog timer counts down may be changed
 *  during runtime using #Watchdog_setReload(). This value should be specified
 *  in Watchdog clock ticks and should not exceed the maximum value of 32 bits,
 *  which corresponds to a timeout period of 2863.3 seconds at the Watchdog rate
 *  of 1.5 MHz. If the reload value is set to zero, the Watchdog interrupt is
 *  immediately generated.
 *
 *  Since the device is not reset on the first Watchdog timeout, the maximum
 *  time lapse between the time when the device gets locked up and the time when
 *  it is reset can be up to <b>two</b> Watchdog timeout periods.
 *
 *  Watchdog_close() is <b>not</b> supported by this driver implementation. Once
 *  started, the Watchdog timer can only be stopped by a hardware reset.
 *
 *  <b>No</b> CC13XX/CC26XX specific command has been implemented. Any call to
 *  Watchdog_control() will receive the return code Watchdog_STATUS_UNDEFINEDCMD.
 *
 *  By default the Watchdog driver has reset enabled. However, it may be
 *  disabled in the #Watchdog_Params which allows the Watchdog Timer to be used
 *  like another timer interrupt. This functionality is <b>not</b> supported by
 *  all platforms, refer to device specific documentation for details.
 *
 *  # Power Management #
 *  Once started, the Watchdog will keep running in Active or Idle mode. When
 *  the device enters Standby mode, the Watchdog timer will stop counting down
 *  but the counter value will be preserved. When the device wakes up from
 *  Standby, the Watchdog timer will continue to count down from the previous
 *  counter value.
 *
 *  This means that if a system goes into Standby 50% of the time and the
 *  Watchdog reload value is set to 1 second, the Watchdog timer will actually
 *  time out in 2 seconds. A system which is only in Active/Idle mode for 1% of
 *  the time, the Watchdog timer will time out in 100 seconds. However, if a bug
 *  locks up the application in Active mode, the Watchdog timer will time out in
 *  the configured time.
 *
 *
 * # Supported Functions #
 *  | Generic API Function          | API Function                      | Description                                       |
 *  |------------------------------ |---------------------------------- |---------------------------------------------------|
 *  | #Watchdog_init()             | WatchdogCC26XX_init()             | Initialize Watchdog driver                        |
 *  | #Watchdog_open()             | WatchdogCC26XX_open()             | Initialize Watchdog HW and set system dependencies|
 *  | #Watchdog_clear()            | WatchdogCC26XX_clear()            | Clear Watchdog interrupt flag and reload counter  |
 *  | #Watchdog_setReload()        | WatchdogCC26XX_setReload()        | Set Watchdog timer reload value in clock ticnks   |
 *  | #Watchdog_convertMsToTicks() | WatchdogCC26XX_convertMsToTicks() | Converts milliseconds to clock ticks              |
 *
 *  @note All calls should go through the generic API. Please refer to @ref Watchdog.h for a
 *  complete description of the generic APIs.
 *
 * # Use Cases #
 * ## Basic Watchdog #
 *  In this basic watchdog example, the application is expected to define a
 *  Watchdog callback function and start the Watchdog timer by calling #Watchdog_open().
 *  If needed, #Watchdog_setReload() may be called to change the timeout period.
 *  If all monitored tasks are doing alright, #Watchdog_clear() should be called
 *  regularly to reload the counter so as to restart the timeout period and to
 *  avoid the Watchdog interrupt being triggered. If the #Watchdog_clear() is
 *  missed and the Watchdog timer is allowed to timeout, the user-defined
 *  callback function is called. In this function, the user may do whatever is
 *  appropriate for the application.
 *  Here are some suggestions:
 *  - do nothing so that the timer will timeout again and trigger the reset
 *  - immediately reset the device
 *  - do self-test to check the integrity of the application
 *
 *  @note The Watchdog interrupt is configured as a non-maskable interrupt
 *  (NMI) and the user-defined callback function is called in NMI context.
 *  Therefore it is not safe to call any OS APIs from the Watchdog callback
 *  function. This includes any driver calls that rely on OS APIs.
 *
 *  The following code example shows how to define the callback function and to
 *  start the Watchdog timer.
 *  @code
 *  void watchdogCallback(uintptr_t handle);
 *
 *  ...
 *
 *  Watchdog_Handle handle;
 *  Watchdog_Params params;
 *  uint32_t tickValue;
 *
 *  Watchdog_Params_init(&params);
 *  params.callbackFxn = watchdogCallback;
 *  handle = Watchdog_open(Watchdog_configIndex, &params);
 *  // set timeout period to 100 ms
 *  tickValue = Watchdog_convertMsToTicks(handle, 100);
 *  Watchdog_setReload(handle, tickValue);
 *
 *  ...
 *
 *  void watchdogCallback(uintptr_t handle)
 *  {
 *      // User-defined code here
 *      ...
 *  }
 *
 *  @endcode
 */

#ifndef ti_drivers_watchdog_WatchdogCC26XX__include
#define ti_drivers_watchdog_WatchdogCC26XX__include

#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/dpl/HwiP.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup Watchdog_STATUS
 *  WatchdogCC26XX_STATUS_* macros are command codes only defined in the
 *  WatchdogCC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/watchdog/WatchdogCC26XX.h>
 *  @endcode
 *  @{
 */

/* Add WatchdogCC26XX_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup Watchdog_CMD
 *  WatchdogCC26XX_CMD_* macros are command codes only defined in the
 *  WatchdogCC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/watchdog/WatchdogCC26XX.h>
 *  @endcode
 *  @{
 */

/* Add WatchdogCC26XX_CMD_* macros here */

/** @}*/

/*! @brief  Watchdog function table for CC26XX */
extern const Watchdog_FxnTable WatchdogCC26XX_fxnTable;

/*!
 *  @brief  Watchdog hardware attributes for CC26XX
 */
typedef struct {
    unsigned int baseAddr;       /*!< Base adddress for Watchdog */
    unsigned long reloadValue;   /*!< Reload value in milliseconds for Watchdog */
} WatchdogCC26XX_HWAttrs;

/*!
 *  @brief      Watchdog Object for CC26XX
 *
 *  Not to be accessed by the user.
 */
typedef struct {
    bool                isOpen;             /* Flag for open/close status */
    Watchdog_Callback   callbackFxn;        /* Pointer to callback. Not supported
                                               on all targets. */
    Watchdog_ResetMode  resetMode;          /* Mode to enable resets.
                                               Not supported on all targets. */
    Watchdog_DebugMode  debugStallMode;     /* Mode to stall Watchdog at breakpoints.
                                               Not supported on all targets. */
    /* Watchdog SYS/BIOS objects */
    HwiP_Struct hwi; /* Hwi object */
} WatchdogCC26XX_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_watchdog_WatchdogCC26XX__include */
