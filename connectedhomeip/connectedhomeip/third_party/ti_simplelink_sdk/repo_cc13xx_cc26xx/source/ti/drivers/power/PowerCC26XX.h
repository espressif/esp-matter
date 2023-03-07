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
 * EXEMPLARY, OR CONSEQueueNTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       PowerCC26XX.h
 *
 *  @brief      Power manager interface for CC26XX/CC13XX
 *
 *  The Power header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/Power.h>
 *  #include <ti/drivers/power/PowerCC26XX.h>
 *  @endcode
 *
 *  Refer to @ref Power.h for a complete description of APIs.
 *
 *  ## Implementation #
 *  This header file defines the power resources, constraints, events, sleep
 *  states and transition latencies for CC26XX/CC13XX.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_power_PowerCC26XX_
#define ti_drivers_power_PowerCC26XX_

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/Power.h>

#include <ti/devices/DeviceFamily.h>

#ifdef __cplusplus
extern "C" {
#endif

/* \cond */
typedef uint8_t PowerCC26XX_Resource; /* Resource identifier */
/* \endcond */

/*! The latency to reserve for resume from STANDBY (usec). */
#define PowerCC26XX_RESUMETIMESTANDBY  750

/*! The total latency to reserve for entry to and exit from STANDBY (usec). */
#define PowerCC26XX_TOTALTIMESTANDBY   1000

/*! The initial delay when waking from STANDBY (usec). */
#define PowerCC26XX_WAKEDELAYSTANDBY   240

/*! The initial wait time (usec) before checking if RCOSC_LF is stable. */
#define PowerCC26XX_INITIALWAITRCOSC_LF 1000

/*! The retry wait time (usec) when checking to see if RCOSC_LF is stable. */
#define PowerCC26XX_RETRYWAITRCOSC_LF   1000

/*! The initial wait time (usec) before checking if XOSC_HF is stable. */
#define PowerCC26XX_INITIALWAITXOSC_HF  50

/*! The retry wait time (usec) when checking to see if XOSC_HF is stable. */
#define PowerCC26XX_RETRYWAITXOSC_HF    50

/*! The initial wait time (usec) before checking if XOSC_LF is stable. */
#define PowerCC26XX_INITIALWAITXOSC_LF  10000

/*! The retry wait time (usec) when checking to see if XOSC_LF is stable. */
#define PowerCC26XX_RETRYWAITXOSC_LF    5000

/* resource IDs */
#define PowerCC26XX_PERIPH_GPT0    0
/*!< Resource ID: General Purpose Timer 0 */

#define PowerCC26XX_PERIPH_GPT1    1
/*!< Resource ID: General Purpose Timer 1 */

#define PowerCC26XX_PERIPH_GPT2    2
/*!< Resource ID: General Purpose Timer 2 */

#define PowerCC26XX_PERIPH_GPT3    3
/*!< Resource ID: General Purpose Timer 3 */

#define PowerCC26XX_PERIPH_SSI0    4
/*!< Resource ID: Synchronous Serial Interface 0 */

#define PowerCC26XX_PERIPH_SSI1    5
/*!< Resource ID: Synchronous Serial Interface 1 */

#define PowerCC26XX_PERIPH_UART0   6  /*!< Resource ID: UART 0 */

#define PowerCC26XX_PERIPH_I2C0    7  /*!< Resource ID: I2C 0 */

#define PowerCC26XX_PERIPH_TRNG    8
/*!< Resource ID: True Random Number Generator */

#define PowerCC26XX_PERIPH_GPIO    9  /*!< Resource ID: General Purpose I/Os */

#define PowerCC26XX_PERIPH_UDMA    10 /*!< Resource ID: uDMA Controller */

#define PowerCC26XX_PERIPH_CRYPTO  11 /*!< Resource ID: AES Security Module */

#define PowerCC26XX_PERIPH_I2S     12 /*!< Resource ID: I2S */

#define PowerCC26XX_PERIPH_RFCORE  13 /*!< Resource ID: RF Core Module */

#define PowerCC26XX_XOSC_HF        14
/*!< Resource ID: High Frequency Crystal Oscillator */

#define PowerCC26XX_DOMAIN_PERIPH  15
/*!< Resource ID: Peripheral Power Domain */

#define PowerCC26XX_DOMAIN_SERIAL  16
/*!< Resource ID: Serial Power Domain */

#define PowerCC26XX_DOMAIN_RFCORE  17
/*!< Resource ID: RF Core Power Domain */

#define PowerCC26XX_DOMAIN_SYSBUS  18
/*!< Resource ID: System Bus Power Domain */

/* The PKA and UART1 peripherals are not available on CC13X1 and CC26X1 devices */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2 || \
     DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)

/*!< Resource ID: PKA Module */
#define PowerCC26XX_PERIPH_PKA      19

/*!< Resource ID: UART1 */
#define PowerCC26XX_PERIPH_UART1    20

#endif

/* The peripherals below are only available on CC13X4 and CC26X4 devices */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)

/*!< Resource ID: UART2 */
#define PowerCC26XX_PERIPH_UART2    21

/*!< Resource ID: UART3 */
#define PowerCC26XX_PERIPH_UART3    22

/*!< Resource ID: SSI2 */
#define PowerCC26XX_PERIPH_SSI2     23

/*!< Resource ID: SSI3 */
#define PowerCC26XX_PERIPH_SSI3     24

/*!< Resource ID: I2C1 */
#define PowerCC26XX_PERIPH_I2C1     25

#endif

/* \cond */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
#define PowerCC26XX_NUMRESOURCES 19 /* Number of resources in database */
#elif (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1)
#define PowerCC26XX_NUMRESOURCES 19 /* Number of resources in database */
#elif (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
#define PowerCC26XX_NUMRESOURCES 21 /* Number of resources in database */
#elif (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
#define PowerCC26XX_NUMRESOURCES 26 /* Number of resources in database */
#endif
/* \endcond */

/* \cond */
/* resource record bitmasks */
#define PowerCC26XX_PERIPH          0x80 /* resource is a peripheral */
#define PowerCC26XX_SPECIAL         0x40 /* resource requires special handler */
#define PowerCC26XX_DOMAIN          0x00 /* resource is a domain */
#define PowerCC26XX_PARENTMASK      0x3F /* parent resource mask */
#define PowerCC26XX_NOPARENT        0x3F /* if resource has no parent */
/* \endcond */

#define PowerCC26XX_STANDBY         0x1 /*!< The STANDBY sleep state */
/* \cond */
/* internal flags for enabling/disabling resources */
#define PowerCC26XX_ENABLE          1
#define PowerCC26XX_DISABLE         0
/* \endcond */

/* constraints */
#define PowerCC26XX_RETAIN_VIMS_CACHE_IN_STANDBY  0
/*!< Constraint: VIMS RAM must be retained while in STANDBY */

#define PowerCC26XX_DISALLOW_SHUTDOWN  1
/*!< Constraint: Disallow a transition to the SHUTDOWN state */

#define PowerCC26XX_DISALLOW_STANDBY  2
/*!< Constraint: Disallow a transition to the STANDBY sleep state */

#define PowerCC26XX_DISALLOW_IDLE  3
/*!< Constraint: Disallow a transition to the IDLE sleep state */

#define PowerCC26XX_NEED_FLASH_IN_IDLE  4
/*!< Constraint: Flash memory needs to enabled during IDLE */

#define PowerCC26XX_SWITCH_XOSC_HF_MANUALLY 5
/*!< Constraint: Prevent power driver from starting an RTOS clock and
 *   automatically switching to the XOSC_HF when it is ready. The power
 *   driver will turn on the XOSC_HF and return control to the application.
 *   The application must poll the status of the XOSC_HF and make sure that it
 *   is stable before manually switching to it.
 *   If the constraint is released before the application has switched to the
 *   XOSC_HF, the application is still responsible for switching to the
 *   XOSC_HF.
 *   Failing to do so may cause an undefined internal state in the power
 *   driver.
 */

#define PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING 6
/*!< Constraint: Prevent power driver from switching to XOSC_HF when the crystal is
 *   ready. The RTOS clock will be rescheduled to try again in the future.
 *   This is a workaround to prevent the flash from being accessed by a bus master
 *   other than the CPU while switching to XOSC_HF. This would cause a bus stall.
 *   This functionality is only implemented on CC26X0, CC26X0R2, and CC13X0 as the
 *   bug was fixed in hardware on later devices.
 */

/* \cond */
#define PowerCC26XX_NUMCONSTRAINTS  7 /* Number of constraints supported */
/* \endcond */

/* \cond */
/* Deprecated constraint names */
#define PowerCC26XX_SD_DISALLOW  PowerCC26XX_DISALLOW_SHUTDOWN
#define PowerCC26XX_SB_DISALLOW  PowerCC26XX_DISALLOW_STANDBY
#define PowerCC26XX_IDLE_PD_DISALLOW  PowerCC26XX_DISALLOW_IDLE
#define PowerCC26XX_XOSC_HF_SWITCHING_DISALLOW PowerCC26XX_DISALLOW_XOSC_HF_SWITCHING
#define PowerCC26XX_SB_VIMS_CACHE_RETAIN PowerCC26XX_RETAIN_VIMS_CACHE_IN_STANDBY
/* \endcond */

/*
 *  Events
 *
 *  Each event must be a power of two and must be sequential
 *  without any gaps.
 */
#define PowerCC26XX_ENTERING_STANDBY    0x1
/*!< Power event: The device is entering the STANDBY sleep state */

#define PowerCC26XX_ENTERING_SHUTDOWN   0x2
/*!< Power event: The device is entering the SHUTDOWN state */

#define PowerCC26XX_AWAKE_STANDBY       0x4
/*!< Power event: The device is waking up from the STANDBY sleep state */

#define PowerCC26XX_AWAKE_STANDBY_LATE  0x8
/*!< Power event: The device is waking up from STANDBY (this event is sent later during wakeup, after interrupts are re-enabled) */

#define PowerCC26XX_XOSC_HF_SWITCHED    0x10
/*!< Power event: The high frequency (HF) clock source has been switched to XOSC_HF */

#define PowerCC26XX_JTAG_PD_TURNED_ON   0x20
/*!< \warning Note that this power event is only supported by the CC2640R2 device!
 *
 * The JTAG subsystem on the CC26xx devices is automatically enabled after receiving
 * 8 pulses on the TCK pin. This will cause the device to draw more power in all
 * power modes (Active, Idle, Standby, Shutdown).
 * The ::PowerCC26XX_JTAG_PD_TURNED_ON power event will
 * let you know when this has happened outside of a debug session due to noise on the pin.
 * This allows the application to do a reset of the device when it's convenient in order
 * disable the JTAG subsystem and conserve power.
 *
 * In order to turn off the JTAG_PD the application should subscribe to this event.
 * In the callback function the application can call Power_shutdown() and
 * this will force a reset of the device.
 * Alternatively the the callback function can post another event so that the application can
 * reset the device when it's more convenient to do so.
 *
 * When Power_shutdown() is called when the JTAG subsystem is on,
 * the device will reset and branch to the flash image again,
 * only now with the JTAG_PD turned off, thus the excess power is gone.
 * The wakeup source as read through the SysCtrlResetSourceGet() will in this case
 * return RSTSRC_WAKEUP_FROM_SHUTDOWN.
 *
 * The power driver will, each time before entering standby, check to see if the
 * JTAG_PD has been turned on after boot. If so, it will notify all subscribers to the
 * ::PowerCC26XX_JTAG_PD_TURNED_ON event.
 * If the JTAG_PD was turned on during boot, which is the case when
 * using the debugger, the notification will NOT be sent even if the event is registered.
 * This is because when actively developing code with an IDE and emulator, the user typically
 * wants to be able to debug their code through standby without the device resetting.
 *
 * Summary of when the ::PowerCC26XX_JTAG_PD_TURNED_ON notification function will be called.
 *
 * | JTAG_PD state        | Notification function registered | Notification function called |
 * |----------------------|----------------------------------|------------------------
 * | Off                  | Don't care                       | No
 * | Turned on during boot| Don't care                       | No
 * | Turned on after boot | No                               | No
 * | Turned on after boot | Yes                              | Yes
 *
 * \warning If the ::PowerCC26XX_JTAG_PD_TURNED_ON event is registered, and the notification
 * callback function calls Power_shutdown() it will not be possible to attach
 * an emulator to a running target. This is becasue the device will reset as soon as the
 * emulator turns on the JTAG_PD as part of the connect sequence.
 *
 *  Code snippet on how to register the notification and the callback function:
 *  @code
 *  void jtagPdTurnedOnCallbackFxn()
 *  {
 *      // Optionally save any critical application information
 *      // gracefullyShutdownApplication();
 *      // Call shutdown, this will reset device, and the application will reboot with JTAG_PD off.
 *      Power_shutdown(NULL, NULL);
 *      // Power_shutdown(...) should never return, device will reset.
 *  }
 *
 *  void taskFxn(UArg a0, UArg a1)
 *  {
 *      ...
 *      // Register "JTAG power domain turned on" notification function
 *      // Everytime the device is about to enter standby, the power driver will check
 *      // to see if the JTAG_PD has been turned on after boot. If so, the notification
 *      // function will be called before entering standby...
 *      Power_registerNotify(&jtagPdTurnedOnNotifyObj, PowerCC26XX_JTAG_PD_TURNED_ON, (Fxn)jtagPdTurnedOnCallbackFxn, NULL);
 *      ...
 *  }
 *  @endcode
 */


/* \cond */
#define PowerCC26XX_NUMEVENTS           6  /* Number of events supported */
/* \endcond */

/* \cond */
/*
 *  Calibration stages
 */
#define PowerCC26XX_SETUP_CALIBRATE     1
#define PowerCC26XX_INITIATE_CALIBRATE  2
#define PowerCC26XX_DO_CALIBRATE        3
/* \endcond */

/* \cond */
/*! @brief Power resource database record format */
typedef struct {
    uint8_t flags;          /* resource type | first parent */
    uint16_t driverlibID;   /* corresponding driverlib ID for this resource */
} PowerCC26XX_ResourceRecord;
/* \endcond */

/*! @brief Global configuration structure */
typedef struct {
    /*!
     *  @brief The Power Policy's initialization function
     *
     *  If the policy does not have an initialization function, 'NULL'
     *  should be specified.
     */
    Power_PolicyInitFxn policyInitFxn;
    /*!
     *  @brief The Power Policy function
     *
     *  When enabled, this function is invoked in the idle loop, to
     *  opportunistically select and activate sleep states.
     *
     *  Two reference policies are provided:
     *
     *    PowerCC26XX_doWFI() - a simple policy that invokes CPU wait for
     *    interrupt (WFI)
     *
     *    PowerCC26XX_standbyPolicy() - an agressive policy that considers
     *    constraints, time until next scheduled work, and sleep state
     *    latencies, and optionally puts the device into the STANDBY state,
     *    the IDLE state, or as a minimum, WFI.
     *
     *  Custom policies can be written, and specified via this function pointer.
     *
     *  In addition to this static selection, the Power Policy can be
     *  dynamically changed at runtime, via the Power_setPolicy() API.
     */
    Power_PolicyFxn policyFxn;
    /*!
     *  @brief The function to be used for activating RC Oscillator (RCOSC)
     *  calibration
     *
     *  Calibration is normally enabled, via specification of the function
     *  PowerCC26XX_calibrate().  This enables high accuracy operation, and
     *  faster high frequency crystal oscillator (XOSC_HF) startups.
     *
     *  To disable RCOSC calibration, the function PowerCC26XX_noCalibrate()
     *  should be specified.
     */
    bool (*calibrateFxn)(unsigned int);
    /*!
     *  @brief Time in system ticks that specifies the maximum duration the device
     *         may spend in standby.
     *
     *  When the power driver tries to put the device into standby and determines
     *  the next wakeup should usually be further into the future than
     *  maxStandbyDuration system ticks, the power driver will schedule a wakeup
     *  maxStandbyDuration into the future. When the device wakes up after
     *  being in standby for maxStandbyDuration ticks, the power driver will
     *  repeat this process and go back into standby if the state of the system
     *  allows it.
     *
     *  Inserting such periodic wakeups can be used to automatically calibrate
     *  the RCOSC with a maximum period between calibrations or to force the
     *  recalculation of the initial VDDR recharge period. This assumes that
     *  the constraint to prohibit standby is not set and that periods of
     *  inactivity are long enough for the power driver to put the device
     *  into standby.
     *
     *  The value 0 is invalid. When PowerCC26XX_Config.enableMaxStandbyDuration is
     *  set to false, any value (including 0) is ignored and the feature is
     *  disabled.
     *  This feature should not be used to disallow entering standby;
     *  the PowerCC26XX_DISALLOW_STANDBY constraint should be used for
     *  this purpose.
     */
    uint32_t maxStandbyDuration;
     /*!
     *  @brief Margin in SCLK_LF periods subtracted from previous longest
     *  VDDR recharge period.
     *
     *  As the device comes out of standby, it updated its previous initial
     *  VDDR recharge period to be closer to the longest recharge period
     *  experienced during the time spent in standby before waking up.
     *
     *  vddrRechargeMargin is subtracted from the longest VDDR recharge
     *  period in SysCtrlAdjustRechargeAfterPowerDown to ensure there is
     *  some margin between the new initial and converged VDDR recharge
     *  period. The converged recharge period at a certain temperature
     *  is board and device dependent.
     *
     *  The default value of 0 disables this feature.
     */
    uint16_t vddrRechargeMargin;
    /*!
     *  @brief Boolean that enables limiting the duration spent in standby
     *
     *  If false, the power driver will put the device into standby as
     *  appropriate without duration restrictions.
     *
     *  If true, the the power driver will force a wakeup every
     *  PowerCC26XX_Config.maxStandbyDuration system ticks before reevaluating
     *  the state of the system.
     *
     *  This is set to false by default.
     */
    bool enableMaxStandbyDuration;
    /*!
     *  @brief Boolean specifying if the Power Policy function is enabled
     *
     *  If 'true', the policy function will be invoked once for each pass
     *  of the idle loop.
     *
     *  If 'false', the policy will not be invoked.
     *
     *  In addition to this static setting, the power policy can be dynamically
     *  enabled and disabled at runtime, via the Power_enablePolicy() and
     *  Power_disablePolicy() functions, respectively.
     */
    bool enablePolicy;
    /*!
     *  @brief Boolean specifying whether the low frequency RC oscillator
     * (RCOSC_LF) should be calibrated.
     *
     *  If RCOSC calibration is enabled (above, via specification of
     *  an appropriate calibrateFxn), this Boolean specifies whether
     *  RCOSC_LF should be calibrated.
     */
    bool calibrateRCOSC_LF;
    /*!
     *  @brief Boolean specifying whether the high frequency RC oscillator
     * (RCOSC_HF) should be calibrated.
     *
     *  If RCOSC calibration is enabled (above, via specification of
     *  an appropriate calibrateFxn), this Boolean specifies whether
     *  RCOSC_HF should be calibrated.
     */
    bool calibrateRCOSC_HF;
} PowerCC26XX_Config;

/*!
 *  @brief  PowerCC26XX_ModuleState
 *
 *  Power manager state structure. The application must not access any members
 *  of this structure!
 */
typedef struct {
    List_List notifyList;        /*!< Event notification list */
    uint32_t constraintMask;     /*!< Aggregate constraints mask */
    ClockP_Struct clockObj;       /*!< Clock object for scheduling wakeups */
    ClockP_Struct xoscClockObj;   /*!< Clock object for XOSC_HF switching */
    ClockP_Struct lfClockObj;     /*!< Clock object for LF clock checking */
    ClockP_Struct calClockStruct; /*!< Clock object for RCOSC calibration */
    HwiP_Struct hwiStruct;        /*!< Hwi object for RCOSC calibration */
    int32_t nDeltaFreqCurr;      /*!< RCOSC calibration variable */
    int32_t nCtrimCurr;          /*!< RCOSC calibration variable */
    int32_t nCtrimFractCurr;     /*!< RCOSC calibration variable */
    int32_t nCtrimNew;           /*!< RCOSC calibration variable */
    int32_t nCtrimFractNew;      /*!< RCOSC calibration variable */
    int32_t nRtrimNew;           /*!< RCOSC calibration variable */
    int32_t nRtrimCurr;          /*!< RCOSC calibration variable */
    int32_t nDeltaFreqNew;       /*!< RCOSC calibration variable */
    bool bRefine;                /*!< RCOSC calibration variable */
    uint32_t state;              /*!< Current transition state */
    bool xoscPending;            /*!< Is XOSC_HF activation in progress? */
    bool calLF;                  /*!< Calibrate RCOSC_LF? */
    uint8_t hwiState;            /*!< The AUX ISR calibration state */
    bool busyCal;                /*!< Already busy calibrating? */
    uint8_t calStep;             /*!< The current calibration step */
    bool firstLF;                /*!< Is this the first LF calibration? */
    bool enablePolicy;           /*!< Is the Power policy enabled? */
    bool initialized;            /*!< Has Power_init() been called? */
#if defined(DeviceFamily_CC26X0R2)
    bool emulatorAttached;      /*!< Was an emulator detected during boot? */
#endif
    uint8_t constraintCounts[PowerCC26XX_NUMCONSTRAINTS];
    /*!< Array to maintain constraint reference counts */
    uint8_t resourceCounts[PowerCC26XX_NUMRESOURCES];
    /*!< Array to maintain resource dependency reference counts */
    unsigned int (*resourceHandlers[3])(unsigned int);
    /*!< Array of special dependency handler functions */
    Power_PolicyFxn policyFxn;   /*!< The Power policy function */
} PowerCC26XX_ModuleState;

/*!
 *  @brief  The RC Oscillator (RCOSC) calibration function
 *
 *  The function to be used for performing RCOSC calibation.  This is the
 *  default calibration function, and is specified via the calibrateFxn
 *  pointer in the PowerCC26XX_Config structure.
 *
 *  @param  arg      used internally
 *
 *  @return used internally
 */
bool PowerCC26XX_calibrate(unsigned int arg);

/*!
 *  @brief  The Wait for interrupt (WFI) policy
 *
 *  This is a lightweight Power Policy which simply invokes CPU wait for
 *  interrupt.
 *
 *  This policy can be selected statically via the policyFxn pointer in the
 *  PowerCC26XX_Config structure, or dynamically at runtime, via
 *  Power_setPolicy().
 */
void PowerCC26XX_doWFI(void);

/*!
 *  @brief  Get the handle of the Clock object used for scheduling device
 *  wakeups
 *
 *  During initialization, the Power Manager creates a Clock object that a
 *  Power Policy can use to schedule device wakeups.  This function can
 *  be called by a policy function to get the handle of this pre-allocated
 *  Clock object.
 *
 *  @return     The handle of the Clock object
 */
ClockP_Handle PowerCC26XX_getClockHandle(void);

/*!
 *  @brief  Get the estimated HF crystal oscillator (XOSC_HF) startup delay,
 *  for a given delay from now, until startup is initiated
 *
 *  @param timeUntilWakeupInMs  The estimated time until the next wakeup
 *                              event, in units of milliseconds
 *
 *  @return     The estimated HF crystal oscillator startup latency, in
 *              units of microseconds.
 */
uint32_t PowerCC26XX_getXoscStartupTime(uint32_t timeUntilWakeupInMs);

/*!
 *  @brief  Explicitly trigger RC oscillator calibration
 *
 *  When enabled, RCOSC calibration is normally triggered upon each device
 *  wakeup from STANDBY.  To trigger more frequent calibration, an application
 *  can explicitly call this function, to initiate an immediate calibration
 *  cycle.
 *
 *  @return  true if calibration was actually initiated otherwise false
 */
bool PowerCC26XX_injectCalibration(void);

/*!
 *  @brief  Function to specify when RCOSC calibration is to be disabled
 *
 *  This function should be specified as the 'calibrateFxn' in the
 *  PowerCC26XX_Config structure when RCOSC calibration is to be disabled.
 *
 *  Note that the reason a function pointer is used here (versus a simple
 *  Boolean) is so that references to internal calibration subroutines can be
 *  removed, to eliminate pulling the calibration code into the application
 *  image; this enables a significant reduction in memory footprint when
 *  calibration is disabled.
 *
 *  @param  arg      used internally
 *
 *  @return used internally
 */
bool PowerCC26XX_noCalibrate(unsigned int arg);

/*!
 *  @brief  Check if the XOSC_HF is stable and ready to be switched to
 *
 *  @pre    Set PowerCC26XX_SWITCH_XOSC_HF_MANUALLY in the early standby
 *          wakeup notification.
 *
 *  This function should be called when using the
 *  PowerCC26XX_SWITCH_XOSC_HF_MANUALLY power constraint to ensure that
 *  the XOSC_HF is stable before switching to it.
 *
 *  \sa PowerCC26XX_switchXOSC_HF()
 */
bool PowerCC26XX_isStableXOSC_HF(void);

/*!
 *  @brief  Switch the HF clock source to XOSC_HF
 *
 *  @pre    PowerCC26XX_switchXOSC_HF() returns true.
 *
 *  This function should only be called when using the
 *  PowerCC26XX_SWITCH_XOSC_HF_MANUALLY power constraint after ensuring
 *  the XOSC_HF is stable.
 *  If the driver cannot switch to the XOSC_HF despite the crystal being
 *  stable, a clock will be scheduled in the future and the callback will
 *  try to switch again.
 *
 *  \sa PowerCC26XX_isStableXOSC_HF()
 */
void PowerCC26XX_switchXOSC_HF(void);

/*!
 *  @brief  The STANDBY Power Policy
 *
 *  This is an agressive Power Policy, which considers active constraints,
 *  sleep state transition latencies, and time until the next scheduled
 *  work, and automatically transitions the device into the deepest sleep state
 *  possible.
 *
 *  The first goal is to enter STANDBY; if that is not appropriate
 *  given current conditions (e.g., the sleep transition latency is greater
 *  greater than the time until the next scheduled Clock event), then
 *  the secondary goal is the IDLE state; if that is disallowed (e.g., if
 *  the PowerCC26XX_DISALLOW_IDLE constraint is declared), then the policy
 *  will fallback and simply invoke WFI, to clock gate the CPU until the next
 *  interrupt.
 *
 *  In order for this policy to run, it must be selected as the Power
 *  Policy (either by being specified as the 'policyFxn' in the
 *  PowerCC26XX_Config structure, or specified at runtime with
 *  Power_setPolicy()), and the Power Policy must be enabled (either via
 *  'enablePolicy' in the PowerCC26XX_Config structure, or via a call to
 *  Power_enablePolicy() at runtime).
 */
void PowerCC26XX_standbyPolicy(void);

void PowerCC26XX_schedulerDisable(void);
void PowerCC26XX_schedulerRestore(void);

#define Power_getPerformanceLevel(void)   0
#define Power_setPerformanceLevel(level)  Power_EFAIL

#ifdef __cplusplus
}
#endif

#endif /* POWER_CC26XX_ */
