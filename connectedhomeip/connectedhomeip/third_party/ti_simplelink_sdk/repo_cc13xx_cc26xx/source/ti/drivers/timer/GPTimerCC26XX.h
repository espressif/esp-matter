/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file       GPTimerCC26XX.h
 *  @brief      GPTimer driver implementation for CC26XX/CC13XX
 *
 *     # Overview #
 *  This TI RTOS driver can be used to configure GPTimer modules to the modes
 *  supported by the GPTimer. The board file or application must define the device
 *  specific configuration before using the driver.
 *     # Configuration #
 *  The GPTimer can be used in two different configurations. In 32-bit mode the
 *  timer will act as a full-width timer and is controlled using the Timer A unit.
 *  In split (16-bit) mode the timer is split into 2x 16-bit timers. In 16-bit mode
 *  a prescaler is available for each timer unit, effectively increasing the
 *  resolution in this mode to 24-bit. All supported modes by driver in split
 *  configuration uses prescaler as timer extension.
 *
 *  # Modes #
 *  The GPTimer driver supports the following timer modes:
 *    - Oneshot mode counting upwards. When timer reaches load value, the timer
 *      is stopped automatically. Supported in both 16 and 32-bit configuration.
 *    - Periodic mode counting upwards. When timer reaches load value it wraps and
 *      starts counting from 0 again. Supported in both 16 and 32-bit configuration.
 *    - Input edge-count. Timer counts the number of events on its input capture port
 *      upwards from 0. Events can be rising-edge, falling-edge, or both.
 *      Supported only in 16-bit mode.
 *    - Input edge-time. Timer counts upwards from 0 and captures the time of an
 *      event on its input capture port. This can be used to count the time
 *      between events. Events can be rising-edge, falling-edge or both.
 *      Supported only in 16-bit mode.
 *    - PWM mode. Timer counts downwards from load value. CCP is set to 1 when
 *      reaching timeout (0) and toggles when reaching match value.
 *
 * # Power Management #
 * The TI-RTOS power management framework will try to put the device into the most
 * power efficient mode whenever possible. Please see the technical reference
 * manual for further details on each power mode.
 *
 *  The GPTimerCC26XX driver will set constraints on disallowed power modes when
 *  needed, removing the need for the application to handle this.
 *  The following statements are valid:
 *    - After GPTimerCC26XX_open():
 *        The device is still allowed to enter Standby. When the device is
 *        active the corresponding GPTimer peripheral will be enabled and clocked.
 *    - After GPTimerCC26XX_start():
 *        The device will only go to Idle power mode since the high-frequency
 *        clock is needed for timer operation.
 *    - After GPTimerCC26XX_stop():
 *        Conditions are equal as for after GPTimerCC26XX_open
 *    - After GPTimerCC26XX_close():
 *        The underlying GPTimer is turned off and the device is allowed to go
 *        to standby.
 *
 *  # Accuracy #
 *  The GPTimer clock is dependent on the MCU system clock.
 *  If very high-accuracy outputs are needed, the application should request
 *  using the external HF crystal:
 *  @code
 *  #include <ti/sysbios/family/arm/cc26xx/Power.h>
 *  #include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
 *  Power_setDependency(XOSC_HF);
 *  @endcode
 *
 *  # Limitations #
 *  - DMA usage is not supported
 *  - Timer synchronization is not supported
 *  - Down counting modes (except for PWM) are not supported by driver
 *
 *  # GPTimerCC26XX usage #
 *
 *  ## Periodic timer ##
 *  The example below will generate an interrupt using the GPTimer every 1 ms.
 *
 *  @code
 *  GPTimerCC26XX_Handle hTimer;
 *  void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask) {
 *      // interrupt callback code goes here. Minimize processing in interrupt.
 *  }
 *
 *  void taskFxn(uintptr_t a0, uintptr_t a1) {
 *    GPTimerCC26XX_Params params;
 *    GPTimerCC26XX_Params_init(&params);
 *    params.width          = GPT_CONFIG_16BIT;
 *    params.mode           = GPT_MODE_PERIODIC;
 *    params.direction      = GPTimerCC26XX_DIRECTION_UP;
 *    params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
 *    hTimer = GPTimerCC26XX_open(CC2650_GPTIMER0A, &params);
 *    if(hTimer == NULL) {
 *      Log_error0("Failed to open GPTimer");
 *      Task_exit();
 *    }
 *
 *    Types_FreqHz  freq;
 *    BIOS_getCpuFreq(&freq);
 *    GPTimerCC26XX_Value loadVal = freq.lo / 1000 - 1; //47999
 *    GPTimerCC26XX_setLoadValue(hTimer, loadVal);
 *    GPTimerCC26XX_registerInterrupt(hTimer, timerCallback, GPT_INT_TIMEOUT);
 *
 *    GPTimerCC26XX_start(hTimer);
 *
 *    while(1) {
 *      Task_sleep(BIOS_WAIT_FOREVER);
 *    }
 *  }
 *  @endcode
 *
 *
 *  ## PWM output ##
 *  See the PWM2TimerCC26XX driver
 *******************************************************************************
 */

#ifndef ti_drivers_timer_GPTIMERCC26XX__include
#define ti_drivers_timer_GPTIMERCC26XX__include

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <ti/drivers/dpl/HwiP.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_gpt.h)
#include DeviceFamily_constructPath(driverlib/event.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/timer.h)

#ifdef __cplusplus
extern "C" {
#endif

/* Backwards compatibility - old timer modes. New behaviour is count-up by default but configurable. */
#define GPT_MODE_ONESHOT_UP GPT_MODE_ONESHOT
#define GPT_MODE_PERIODIC_UP GPT_MODE_PERIODIC
#define GPT_MODE_EDGE_COUNT_UP GPT_MODE_EDGE_COUNT
#define GPT_MODE_EDGE_TIME_UP GPT_MODE_EDGE_TIME

/*!
 *  @brief
 *  Definitions for specifying the GPTimer configuration (width)
 */
typedef enum GPTimerCC26XX_Width
{
    GPT_CONFIG_32BIT = GPT_CFG_CFG_32BIT_TIMER,
    GPT_CONFIG_16BIT = GPT_CFG_CFG_16BIT_TIMER,
} GPTimerCC26XX_Width;

/*!
 *  @brief
 *  Definitions for supported GPTimer modes. Driver code assumes only modes
 *  using prescaler as timer extension in 16-bit configuration are used.
 *  Therefore new modes must not be added to the below description without
 *  also updating driver.
 *
 */
typedef enum GPTimerCC26XX_Mode
{
    /* One shot mode counting upwards */
    GPT_MODE_ONESHOT    = GPT_TAMR_TAMR_ONE_SHOT | GPT_TAMR_TAMIE,
    /* Periodic mode counting upwards */
    GPT_MODE_PERIODIC   = GPT_TAMR_TAMR_PERIODIC | GPT_TAMR_TAMIE,
    /* Edge count mode counting upwards */
    GPT_MODE_EDGE_COUNT = GPT_TAMR_TAMR_CAPTURE  | GPT_TAMR_TACM_EDGCNT,
    /* Edge count mode counting upwards */
    GPT_MODE_EDGE_TIME  = GPT_TAMR_TAMR_CAPTURE  | GPT_TAMR_TACM_EDGTIME,
    /* PWM mode counting downwards. This specific configuration is used by the
       PWM2TimerCC26XX driver */
    GPT_MODE_PWM           = GPT_TAMR_TAMR_PERIODIC      | GPT_TAMR_TAPWMIE_EN  | \
                             GPT_TAMR_TAAMS_PWM          | GPT_TAMR_TACM_EDGCNT | \
                             GPT_TAMR_TAPLO_CCP_ON_TO,
} GPTimerCC26XX_Mode;

/*!
 *  @brief
 *  Definitions for supported GPTimer interrupts. GPTimerCC26XX_IntMask
 *  arguments should be a bit vector containing these definitions.
 *  See description in Technical Reference
 */
typedef enum GPTimerCC26XX_Interrupt
{
    GPT_INT_TIMEOUT           = 1 << 0,
        GPT_INT_CAPTURE_MATCH = 1 << 1,
        GPT_INT_CAPTURE       = 1 << 2,
        GPT_INT_MATCH         = 1 << 3,
} GPTimerCC26XX_Interrupt;

/* Number of entries in GPTimerCC26XX_Interrupt */
#define GPT_NUM_INTS    4

/*!
 *  @brief
 *  Definitions for GPTimer parts (Timer A / Timer B).
 *  Used in GPTimer configuration structure GPTimerCC26XX_config to
 *  configure the corresponding timer unit.
 */
typedef enum GPTimerCC26XX_Part
{
    GPT_A = 0,
    GPT_B,
} GPTimerCC26XX_Part;

#define GPT_PARTS_COUNT 2

/*!
 *  @brief
 *  Definitions for input / output ports in IO controller to connect GPTimer
 *  to a pin. Used in gptimerCC26xxHWAttrs for static timer configuration
 *  PIN driver is used to mux a pin to the timer.
 *  @sa  PINCC26XX_setMux
 *  @sa  GPTimerCC26XX_getPinMux
 */
typedef enum GPTimerCC26XX_PinMux
{
    GPT_PIN_0A = IOC_PORT_MCU_PORT_EVENT0,
    GPT_PIN_0B = IOC_PORT_MCU_PORT_EVENT1,
    GPT_PIN_1A = IOC_PORT_MCU_PORT_EVENT2,
    GPT_PIN_1B = IOC_PORT_MCU_PORT_EVENT3,
    GPT_PIN_2A = IOC_PORT_MCU_PORT_EVENT4,
    GPT_PIN_2B = IOC_PORT_MCU_PORT_EVENT5,
    GPT_PIN_3A = IOC_PORT_MCU_PORT_EVENT6,
    GPT_PIN_3B = IOC_PORT_MCU_PORT_EVENT7,
} GPTimerCC26XX_PinMux;

/*!
 *  @brief
 *  Definitions for controlling timer debug stall mode
 */
typedef enum GPTimerCC26XX_DebugMode
{
    GPTimerCC26XX_DEBUG_STALL_OFF = 0,
    GPTimerCC26XX_DEBUG_STALL_ON,
} GPTimerCC26XX_DebugMode;

/*!
 *  @brief
 *  Definitions for controlling timer counting direction.
 *  Setting the Direction for PWM operation has no effect (always counts down).
 */
typedef enum GPTimerCC26XX_Direction
{
    GPTimerCC26XX_DIRECTION_DOWN = 0,
    GPTimerCC26XX_DIRECTION_UP,
} GPTimerCC26XX_Direction;

/*!
 *  @brief
 *  Definitions for new value loading behaviour.
 *
 *  If set to NEXT_CLOCK, then the new match value is updated immediately.
 *  If set to ON_TIMEOUT the new match will only be applied to the next timer cycle.
 *
 *  Only match setting is affected by this option. Load setting is always applied immediately.
 */
typedef enum GPTimerCC26XX_SetMatchTiming
{
    GPTimerCC26XX_SET_MATCH_NEXT_CLOCK = 0,
    GPTimerCC26XX_SET_MATCH_ON_TIMEOUT,
} GPTimerCC26XX_SetMatchTiming;

/*!
 *  @brief
 *  Definitions for controlling edges used for timer capture.
 *  Used in GPTimer edge-time and edge-count modes.
 */
typedef enum GPTimerCC26XX_Edge
{
    GPTimerCC26XX_POS_EDGE   = GPT_CTL_TAEVENT_POS,
    GPTimerCC26XX_NEG_EDGE   = GPT_CTL_TAEVENT_NEG,
    GPTimerCC26XX_BOTH_EDGES = GPT_CTL_TAEVENT_BOTH,
} GPTimerCC26XX_Edge;


/* Forward declaration of GPTimer configuration */
typedef struct GPTimerCC26XX_Config   GPTimerCC26XX_Config;

/* GPTimer handle is pointer to configuration structure */
typedef GPTimerCC26XX_Config *              GPTimerCC26XX_Handle;

/* Interrupt bit vector. See GPTimerCC26XX_Interrupt for available interrupts */
typedef uint16_t                            GPTimerCC26XX_IntMask;

/* Timer value */
typedef  uint32_t                           GPTimerCC26XX_Value;

/* Function prototype for interrupt callbacks */
typedef void (*GPTimerCC26XX_HwiFxn) (GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask);

/*!
 *  @brief  GPTimer26XX Hardware attributes
 *
 *  These fields are used by the driver to set up underlying GPTimer
 *  driver statically. A sample structure is shown below:
 *
 *  @code
 *  // GPTimer hardware attributes, one per timer unit (Timer 0A, 0B, 1A, 1B..)
 *  const GPTimerCC26XX_HWAttrs gptimerCC26xxHWAttrs[CC2650_GPTIMERPARTSCOUNT] = {
 *    {.baseAddr = GPT0_BASE, .intNum = INT_TIMER0A, .powerMngrId = PERIPH_GPT0, .pinMux = GPT_PIN_0A, },
 *    {.baseAddr = GPT0_BASE, .intNum = INT_TIMER0B, .powerMngrId = PERIPH_GPT0, .pinMux = GPT_PIN_0B, },
 *    {.baseAddr = GPT1_BASE, .intNum = INT_TIMER1A, .powerMngrId = PERIPH_GPT1, .pinMux = GPT_PIN_1A, },
 *    {.baseAddr = GPT1_BASE, .intNum = INT_TIMER1B, .powerMngrId = PERIPH_GPT1, .pinMux = GPT_PIN_1B, },
 *    {.baseAddr = GPT2_BASE, .intNum = INT_TIMER2A, .powerMngrId = PERIPH_GPT2, .pinMux = GPT_PIN_2A, },
 *    {.baseAddr = GPT2_BASE, .intNum = INT_TIMER2B, .powerMngrId = PERIPH_GPT2, .pinMux = GPT_PIN_2B, },
 *    {.baseAddr = GPT3_BASE, .intNum = INT_TIMER3A, .powerMngrId = PERIPH_GPT3, .pinMux = GPT_PIN_3A, },
 *    {.baseAddr = GPT3_BASE, .intNum = INT_TIMER3B, .powerMngrId = PERIPH_GPT3, .pinMux = GPT_PIN_3B, },
 *  };
 *  @endcode
 */
typedef struct GPTimerCC26XX_HWAttrs
{
    /*! GPTimer peripheral base address */
    uint32_t baseAddr;
    /*! GPTimer peripheral interrupt vector */
    uint8_t  intNum;
    /*! GPTimer peripheral's interrupt priority.
        The CC26xx uses three of the priority bits,
        meaning ~0 has the same effect as (7 << 5).
        (7 << 5) will apply the lowest priority.
        (1 << 5) will apply the highest priority.
        Setting the priority to 0 is not supported by this driver.
        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency
        interrupts, thus invalidating the critical sections in this driver.
     */
    uint8_t              intPriority;
    /*! GPTimer peripheral's power manager ID */
    uint8_t              powerMngrId;
    /*! GPTimer half timer unit */
    GPTimerCC26XX_Part   timer;
    /*! PIN driver MUX */
    GPTimerCC26XX_PinMux pinMux;
} GPTimerCC26XX_HWAttrs;

/*!
 *  @brief  GPTimer26XX Object
 *
 * These fields are used by the driver to store and modify GPTimer configuration
 * during run-time.
 * The application must not edit any member variables of this structure.
 * Appplications should also not access member variables of this structure
 * as backwards compatibility is not guaranteed. An example structure is shown
 * below:
 * @code
 * // GPTimer objects, one per full-width timer (A+B) (Timer 0, Timer 1..)
 * GPTimerCC26XX_Object gptimerCC26XXObjects[CC2650_GPTIMERCOUNT];
 * @endcode
 */
typedef struct GPTimerCC26XX_Object
{
    GPTimerCC26XX_Width  width;                            /*!< Timer width configuration (16/32bit)*/
    bool                 isOpen[GPT_PARTS_COUNT];          /*!< Object is opened flag  */
    HwiP_Struct           hwi[GPT_PARTS_COUNT];             /*!< Hardware interrupt struct */
    GPTimerCC26XX_HwiFxn hwiCallbackFxn[GPT_PARTS_COUNT];  /*!< Hardware interrupt callback function */
    volatile bool        powerConstraint[GPT_PARTS_COUNT]; /*!< Standby power constraint flag */
    uint32_t             arg;                              /*!< Arbritrary Argument */
} GPTimerCC26XX_Object;


/*!
 *  @brief  GPTimer Global configuration
 *
 *  The GPTimerCC26XX_Config structure contains a set of pointers
 *  used to characterize the GPTimer driver implementation.
 *  An example structure is shown below:
 *  @code
 *  // GPTimer configuration (used as GPTimer_Handle by driver and application)
 *  const GPTimerCC26XX_Config GPTimerCC26XX_config[CC2650_GPTIMERPARTSCOUNT] = {
 *    { &gptimerCC26XXObjects[0], &gptimerCC26xxHWAttrs[0], GPT_A},
 *    { &gptimerCC26XXObjects[0], &gptimerCC26xxHWAttrs[1], GPT_B},
 *    { &gptimerCC26XXObjects[1], &gptimerCC26xxHWAttrs[2], GPT_A},
 *    { &gptimerCC26XXObjects[1], &gptimerCC26xxHWAttrs[3], GPT_B},
 *    { &gptimerCC26XXObjects[2], &gptimerCC26xxHWAttrs[4], GPT_A},
 *    { &gptimerCC26XXObjects[2], &gptimerCC26xxHWAttrs[5], GPT_B},
 *    { &gptimerCC26XXObjects[3], &gptimerCC26xxHWAttrs[6], GPT_A},
 *    { &gptimerCC26XXObjects[3], &gptimerCC26xxHWAttrs[7], GPT_B},
 *  };
 *  @endcode
 */
struct GPTimerCC26XX_Config
{
    GPTimerCC26XX_Object        *object;
    const GPTimerCC26XX_HWAttrs *hwAttrs;
    GPTimerCC26XX_Part          timerPart;
};

/*!
 *  @brief GPTimerCC26XX Parameters
 *
 *  GPTimer parameters are used to with the GPTimerCC26XX_open() call.
 *  Default values for these parameters are set using GPTimerCC26XX_Params_init().
 *
 *  @sa     GPTimerCC26XX_Params_init()
 */
typedef struct GPTimerCC26XX_Params
{
    GPTimerCC26XX_Width             width;          /*!< Timer configuration (32/16-bit)  */
    GPTimerCC26XX_Mode              mode;           /*!< Timer mode */
    GPTimerCC26XX_SetMatchTiming    matchTiming;    /*!< Set new match values on next timeout or next cycle */
    GPTimerCC26XX_Direction         direction;      /*!< Count up or down */
    GPTimerCC26XX_DebugMode         debugStallMode; /*!< Timer debug stall mode */
} GPTimerCC26XX_Params;


/*!
 *  @brief  Function to initialize the GPTimerCC26XX_Params struct to
 *          its default values
 *
 *  @param  params      An pointer to GPTimerCC26XX_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      - 32-bit configuration
 *      - Periodic mode counting upwards
 *      - Debug stall mode disabled
 */
extern void GPTimerCC26XX_Params_init(GPTimerCC26XX_Params *params);

/*!
 *  @brief  This function opens a given GPTimer peripheral. Will set dependency
 *          on timer and configure it into specified mode.
 *
 *  @param  index         Logical peripheral number for the GPTimer indexed into
 *                        the GPTimerCC26XX_config table
 *
 *  @param  params        Pointer to a parameter block. If NULL, it will use
 *                        default values.
 *
 *  @return A GPTimerCC26XX_Handle on success or a NULL on an error or if it has been
 *          opened already. If NULL is returned, further GPTimerCC26XX API calls will
 *          result in undefined behaviour.
 *
 *  @sa     GPTimerCC26XX_close()
 */
extern GPTimerCC26XX_Handle GPTimerCC26XX_open(unsigned int index, const GPTimerCC26XX_Params *params);

/*!
 *  @brief  Function to close a GPTimer peripheral specified by the GPTimer handle.
 *          Closing timer will releae dependency on timer and clear configuration
 *
 *  @pre    GPTimerCC26XX_open() has to be called first.
 *  @pre    GPTimerCC26XX_stop() should to be called first if GPTimer is started
 *
 *  @param  handle A GPTimerCC26XX handle returned from GPTimerCC26XX_open()
 *
 *  @sa     GPTimerCC26XX_open()
 *  @sa     GPTimerCC26XX_start()
 *  @sa     GPTimerCC26XX_stop()
 */
extern void GPTimerCC26XX_close(GPTimerCC26XX_Handle handle);

/*!
 *  @brief  Function to start the specified GPTimer with current settings
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle      A GPTimerCC26XX handle returned from GPTimerCC26XX_open()
 *
 *  @sa     GPTimerCC26XX_open()
 *  @sa     GPTimerCC26XX_stop()
 */
extern void GPTimerCC26XX_start(GPTimerCC26XX_Handle handle);

/*!
 *  @brief  Function to stop the specified GPTimer.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle      A GPTimerCC26XX handle returned from GPTimerCC26XX_open()
 *
 *  @sa     GPTimerCC26XX_open()
 *  @sa     GPTimerCC26XX_start()
 */
extern void GPTimerCC26XX_stop(GPTimerCC26XX_Handle handle);

/*!
 *  @brief  Function to set load value of the specified GPTimer. Function can
 *          be called while GPTimer is running.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *   @param  handle      A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  loadValue   Load value to set the GPTimer to.
 *
 *  @sa     GPTimerCC26XX_open()
 */
extern void GPTimerCC26XX_setLoadValue(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Value loadValue);

/*!
 *  @brief  Function to set match value of the specified GPTimer. Function can
 *          be called while GPTimer is running.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle      A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  matchValue  Match value to set the GPTimer to.
 *
 *  @sa     GPTimerCC26XX_open()
 */
extern void GPTimerCC26XX_setMatchValue(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Value matchValue);


/*!
 *  @brief  Function to set which input edge the GPTimer capture should
 *          use. Applies to edge-count and edge-time modes
 *          Function can be called while GPTimer is running.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle      A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  edge        The edge that should trigger a capture
 *
 *  @sa     GPTimerCC26XX_open()
 */
extern void GPTimerCC26XX_setCaptureEdge(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Edge edge);

/*!
 *  @brief  Function to retrieve the current free-running value of timer
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *
 *  @return Current free-running timer value for all modes
 *
 *  @sa     GPTimerCC26XX_open()
 */
extern GPTimerCC26XX_Value GPTimerCC26XX_getFreeRunValue(GPTimerCC26XX_Handle handle);

/*!
 *  @brief  Function to retrieve the current value of timer
 *          This returns the value of the timer in all modes except for
 *          input edge count and input edge time mode.
 *          In edge count mode, this register contains the number of edges that
 *          have occurred. In input edge time, this register contains the
 *          timer value at which the last edge event took place.
 *
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *
 *  @return Current free-running timer value for all modes
 *
 *  @sa     GPTimerCC26XX_open()
 */
extern GPTimerCC26XX_Value GPTimerCC26XX_getValue(GPTimerCC26XX_Handle handle);


/*!
 *  @brief  Function to register a CPU interrupt for a given timer handle and
 *          enable a set of timer interrupt sources. The interrupt to the CPU
 *          will be a bitwise OR of the enabled interrupt sources.
 *          When an interrupt occurs, the driver will clear the
 *          interrupt source and call the application provided callback.
 *          The callback is executed in HW interrupt context and processing in
 *          callback should be minimized.
 *
 *          Interrupt sources can also be individually disabled and enabled  by
 *          using GPTimerCC26XX_enableInterrupt / GPTimerCC26XX_disableInterrupt.
 *
 *          This function should only be called once for a handle after opening
 *          the timer.
 *          If closing a GPTimer, interrupts for the timer should be
 *          unregistered first using GPTimerCC26XX_unregisterInterrupt.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle   A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  callback An application provided callback function
 *  @param  intMask  A bit vector mask containing values from GPTimerCC26XX_Interrupt
 *
 *  @sa     GPTimerCC26XX_open
 *  @sa     GPTimerCC26XX_enableInterrupt
 *  @sa     GPTimerCC26XX_disableInterrupt
 */
extern void GPTimerCC26XX_registerInterrupt(GPTimerCC26XX_Handle handle, GPTimerCC26XX_HwiFxn callback, GPTimerCC26XX_IntMask intMask);

/*!
 *  @brief  Function to disable a CPU interrupt for a given timer handle and
 *          disable all interrupt sources for corresponding GPTimer unit.
 *
 *          This function should only be called once for a handle after opening
 *          the timer and registering the interrupt.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *  @pre    GPTimerCC26XX_registerInterrupt() has to be called first
 *
 *  @param  handle   A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *
 *  @sa     GPTimerCC26XX_open
 *  @sa     GPTimerCC26XX_registerInterrupt
 */
extern void GPTimerCC26XX_unregisterInterrupt(GPTimerCC26XX_Handle handle);

/*!
 *  @brief  Function to enable a set of GPTimer interrupt sources.
 *          The interrupt to the CPU must be enabled using
 *          GPTimerCC26XX_registerInterrupt.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle         A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  interruptMask  A bit vector mask containing values from GPTimerCC26XX_Interrupt
 *
 *  @sa     GPTimerCC26XX_open
 *  @sa     GPTimerCC26XX_disableInterrupt
 */
extern void GPTimerCC26XX_enableInterrupt(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask);
/*!
 *  @brief  Function to disable a set of GPTimer interrupt sources.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle         A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  interruptMask  A bit vector mask containing values from GPTimerCC26XX_Interrupt
 *
 *  @sa     GPTimerCC26XX_open
 *  @sa     GPTimerCC26XX_enableInterrupt
 */
extern void GPTimerCC26XX_disableInterrupt(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask);

/*!
 *  @brief  Function to control timer debug stall mode.
 *          When enabled, the timer will stop when the debugger halts the CPU.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle   A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  mode     Configuration for debug stall mode (enable/disable)
 *
 *  @sa     GPTimerCC26XX_open
 */
extern void GPTimerCC26XX_configureDebugStall(GPTimerCC26XX_Handle handle, GPTimerCC26XX_DebugMode mode);

/*!
 *  @brief  Function to get a custom argument.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *  @pre    GPTimerCC26XX_setArg() has to be called first
 *
 *  @param  handle   A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *
 *  @sa     GPTimerCC26XX_setArg
 *  @sa     GPTimerCC26XX_open
 */
extern uint32_t GPTimerCC26XX_getArg(GPTimerCC26XX_Handle handle);

/*!
 *  @brief  Function to set a custom argument.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle   A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *  @param  arg      Pointer to a custom argument
 *
 *  @sa     GPTimerCC26XX_getArg
 *  @sa     GPTimerCC26XX_open
 */
extern void GPTimerCC26XX_setArg(GPTimerCC26XX_Handle handle, void *arg);

/*!
 *  @brief  Function to return the PIN mux used by the GPTimer identified by
 *          handle. This is used to connect a GPTimer capture/compare port to
 *          a device DIO using PINCC26XX_setMux.
 *          This is typically used in PWM mode and Timer Edge-Count / Edge-Time
 *          modes.
 *          Function assumes correct pinMux is set up in device specific
 *          GPTimerCC26XX_HWAttrs.
 *
 *  @pre    GPTimerCC26XX_open() has to be called first successfully
 *
 *  @param  handle   A GPTimerCC26XX handle returned from GPTimerCC26XX_open
 *
 *  @sa     GPTimerCC26XX_open
 */
static inline GPTimerCC26XX_PinMux GPTimerCC26XX_getPinMux(GPTimerCC26XX_Handle handle)
{
    return handle->hwAttrs->pinMux;
}


#ifdef __cplusplus
}
#endif
#endif /* ti_drivers_timer_GPTIMERCC26XX__include */
