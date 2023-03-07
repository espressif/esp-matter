/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * hal.c
 *   Hardware Abstraction Layer for ARM devices.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_H_
#define _HAL_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "hal_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBsp.h"
#include "gp_kx.h"
#if defined(HAL_IR_RT_CONFIG) || defined(GP_BSP_IR_RT_CONFIG)
#include "hal_ExtendedIr.h"
#endif
#include "hal_WB.h"
#include "hal_Sleep.h"
#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 *                    Function and Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    STACK DEBUG
 *****************************************************************************/

UInt8* hal_GetStackStartAddress(void);
UInt8* hal_GetStackEndAddress(void);
void   hal_dump_backtrace(UInt8 unused);

#define HAL_STACK_LOW_TO_HIGH  0
#define HAL_STACK_START_ADDRESS     hal_GetStackStartAddress()
#define HAL_STACK_END_ADDRESS       hal_GetStackEndAddress()

/*****************************************************************************
 *                    HEAP DEBUG
 *****************************************************************************/

void hal_GetHeapInUse(UInt32* pInUse, UInt32* pReserved, UInt32* pMax);

/*****************************************************************************
 *                    INTERRUPTS
 *****************************************************************************/

#define HAL_LONG_INTERRUPT_TIME_IN_US      (750)

/*****************************************************************************
 *                    CLK frequency
 *****************************************************************************/

UInt8 hal_GetMcuClockSpeed(void);
void hal_SetMcuClockSpeed(UInt8 clockSpeed);

#define HAL_GET_MCU_CLOCK_SPEED()           hal_GetMcuClockSpeed()
#define HAL_SET_MCU_CLOCK_SPEED(clockSpeed) hal_SetMcuClockSpeed(clockSpeed)

/*****************************************************************************
 *                    GPIO
 *****************************************************************************/
typedef struct{
    UInt8 bitBandOffset;
#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
    UInt16 portIndex;
#endif
}hal_gpiodsc_t;
extern const hal_gpiodsc_t gpios[];

#define hal_WakeUpModeNone      0
#define hal_WakeUpModeRising    1
#define hal_WakeUpModeFalling   2
#define hal_WakeUpModeBoth      3
typedef UInt8 hal_WakeUpMode_t;

Bool hal_gpioGetSetClr(hal_gpiodsc_t gpio);
void hal_gpioSet(hal_gpiodsc_t gpio);
void hal_gpioClr(hal_gpiodsc_t gpio);
Bool hal_gpioGet(hal_gpiodsc_t gpio);

void hal_gpioModePP(hal_gpiodsc_t gpio, Bool enable);
Bool hal_gpioGetModePP(hal_gpiodsc_t gpio);
void hal_gpioModePU(UInt8 gpio, Bool enable);
void hal_gpioModePD(UInt8 gpio, Bool enable);

void hal_gpioSetWakeUpMode(UInt8 gpio, hal_WakeUpMode_t mode);

#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
typedef void (* hal_cbGpioExti_t) (void);

void hal_gpioInit(void);
void hal_gpioConfigureInterrupt(UInt8 gpio, Bool expectedVal, hal_cbGpioExti_t cbExti);
void hal_gpioUnconfigureInterrupt(UInt8 gpio);
void hal_gpioSetExpValue(UInt8 gpio, UInt8 val);
Bool hal_gpioGetExpValue(UInt8 gpio);
void hal_gpioDisableInt(UInt8 gpio);
void hal_gpioEnableInt(UInt8 gpio);
#endif

/*****************************************************************************
 *                    Timer
 *****************************************************************************/
#define HAL_TIMER_0             0
#define HAL_TIMER_1             1
#define HAL_TIMER_2             2
#define HAL_TIMER_3             3
#define HAL_TIMER_4             4

#define HAL_TIMER_CLKSELINTCLK  0
#define HAL_TIMER_CLKSELTMR0    1
#define HAL_TIMER_CLKSELTMR1    2
#define HAL_TIMER_CLKSELTMR2    3
#define HAL_TIMER_CLKSELTMR3    4
#define HAL_TIMER_CLKSELTMR4    5

/** @typedef halTimer_timerId_t
 *  @brief Identifying the timer
 */
typedef UInt8 halTimer_timerId_t;

/** @typedef halTimer_clkSel_t
 *  @brief Specify the input clock to timer
 *  A timer can use the wrap of a previous timer as a clock input
 *  or the internal clock
 */
typedef UInt8 halTimer_clkSel_t;

/* Callback type (called on timer wrap interrupt) */
typedef void (*halTimer_cbTimerWrapInterruptHandler_t)(void);

GP_API void halTimer_initTimer(halTimer_timerId_t timerId,
        UInt8 prescalerDiv,
        halTimer_clkSel_t clkSel,
        UInt16 threshold,
        halTimer_cbTimerWrapInterruptHandler_t Inthandler,
        Bool isPeriodic);

GP_API void halTimer_startTimer(halTimer_timerId_t timerId);
GP_API void halTimer_stopTimer(halTimer_timerId_t timerId);
GP_API void halTimer_resetTimer(halTimer_timerId_t timerId);
GP_API void halTimer_setMaskTimerWrapInterrupt(halTimer_timerId_t timerId, UInt8 val);

void hal_Waitus(UInt16 us);
void hal_Waitms(UInt16 ms);

#define HAL_WAIT_US hal_Waitus
#define HAL_WAIT_MS hal_Waitms

/*****************************************************************************
 *                    Cycle count
 *****************************************************************************/


/*****************************************************************************
 *                    DEBUG
 *****************************************************************************/

void hal_EnableDebugMode(void);
void hal_DisableDebugMode(void);

#define HAL_ENABLE_DEBUG_MODE()     hal_EnableDebugMode()
#define HAL_DISABLE_DEBUG_MODE()    hal_DisableDebugMode()

/*****************************************************************************
 *                    LED
 *****************************************************************************/

#ifdef GRN
#endif

#ifdef RED
#endif

#ifdef ORNG
#endif

#ifdef WHITE
#endif

#ifndef GP_DIVERSITY_NO_LED

#define HAL_LED_INIT() HAL_LED_INIT_LEDS()

#define HAL_LED_SET(x)   HAL_LED_SET_##x()
#define HAL_LED_CLR(x)   HAL_LED_CLR_##x()
#define HAL_LED_TST(x)   HAL_LED_TST_##x()
#define HAL_LED_TGL(x)   HAL_LED_TGL_##x()

#define HAL_USERLED_INIT()

#define HAL_USERLED_SET(x)
#define HAL_USERLED_CLR(x)
#define HAL_USERLED_TST(x)
#define HAL_USERLED_TGL(x)

#else

#define HAL_LED_INIT()
#define HAL_LED_CLR(led)
#define HAL_LED_SET(led)
#define HAL_LED_TST(led)            false
#define HAL_LED_TGL(led)

#define HAL_USERLED_INIT()

#define HAL_USERLED_SET(x)
#define HAL_USERLED_CLR(x)
#define HAL_USERLED_TST(x)
#define HAL_USERLED_TGL(x)

#endif //GP_DIVERSITY_NO_LED

/** @brief Enable the LED driver output of LED block @p ledId
 *
 *  @param ledId    Index of the used led peripheral
 */
void hal_ledSet(UInt8 ledId);

/** @brief Disable the LED driver output of LED block @p ledId
 *
 *  @param ledId    Index of the used led peripheral
 */
void hal_ledClr(UInt8 ledId);

/** @brief Return if the LED driver output of LED block @p ledId is enabled
 *
 *  @param ledId    Index of the used led peripheral
 */
Bool hal_ledIsSet(UInt8 ledId);

/** @brief Configure the LED driver output of LED block @p ledId to use a duty cycled output
 *         effectively defining the dimming level of the LED, when it is enabled
 *
 *  @param ledId        Index of the used led peripheral
 *  @param threshold
 */
void hal_ledSetThreshold(UInt8 ledId, UInt8 threshold);


/*****************************************************************************
 *                    Button
 *****************************************************************************/


#define HAL_BTN_INIT()        HAL_BTN_INIT_BTNS()
#define HAL_BTN_PRESSED(btn)  HAL_BUTTON_##btn##_IS_PRESSED()


#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "hal_CodeJumpTableFlash_Defs.h"
#endif  //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Init Function
 *****************************************************************************/

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void hal_Init (void);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#define HAL_INIT()  hal_Init()


/*****************************************************************************
 *                    Atomic
 *****************************************************************************/

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void hal__AtomicOff (void);

void hal__AtomicOn (void);
#if defined(GP_DIVERSITY_JUMPTABLES)
Bool hal_GlobalIntEnabled(void);
void hal_InitializeGlobalInt(void);
void* hal_memset(void *str, int c, size_t n);
void* hal_memcpy(void *str1, const void *str2, size_t n);
int  hal_memcmp(const void *str1, const void *str2, size_t n);
#endif //defined(GP_DIVERSITY_JUMPTABLES)
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */


#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define HAL_INITIALIZE_GLOBAL_INT()     (hal_InitializeGlobalInt())
#define HAL_GLOBAL_INT_ENABLED()        (hal_GlobalIntEnabled())
#else //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define HAL_INITIALIZE_GLOBAL_INT()     { HAL_FORCE_DISABLE_GLOBAL_INT(); l_n_atomic = 1; }
#define HAL_GLOBAL_INT_ENABLED()        (l_n_atomic == 0)
#endif //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

extern volatile UInt8 l_n_atomic;

#define HAL_FORCE_ENABLE_GLOBAL_INT()   { l_n_atomic = 0; __enable_irq(); } /* Reset counter - starting anew */
#define HAL_FORCE_DISABLE_GLOBAL_INT()  __disable_irq()

#define HAL_ENABLE_GLOBAL_INT()        hal__AtomicOff()
#define HAL_DISABLE_GLOBAL_INT()       hal__AtomicOn()


/*****************************************************************************
 *                    RESET UC
 *****************************************************************************/

NORETURN void hal_Reset(void);
#define HAL_RESET_UC( )  hal_Reset()

/*****************************************************************************
 *                    SLEEP
 *****************************************************************************/

extern Bool hal_maySleep;

#ifndef GP_DIVERSITY_JUMPTABLES
static INLINE Bool hal_CanGotoSleep(void)
{
    return hal_maySleep;
}
static INLINE void hal_EnableGotoSleep(void)
{
    hal_maySleep = true;
}
static INLINE void hal_DisableGotoSleep(void)
{
    hal_maySleep = false;
}
#endif //ndef GP_DIVERSITY_JUMPTABLES

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void hal_sleep_uc(UInt32 timeToSleep);
#ifdef GP_DIVERSITY_JUMPTABLES
Bool hal_CanGotoSleep(void);
void hal_EnableGotoSleep(void);
void hal_DisableGotoSleep(void);
#endif //ndef GP_DIVERSITY_JUMPTABLES
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

void hal_IntHandlerPrologue(void); // not part of uchal but used by gpHal, gpKeyScan
void hal_IntHandlerEpilogue(void); // should be called at the end of each interrupt handler

#define HAL_SLEEP_INDEFINITE_SLEEP_TIME   0xFFFFFFFF
#define HAL_SLEEP_MAX_SLEEP_TIME         (0x7FFFFFFFUL)

#define HAL_ENABLE_SLEEP_UC()  hal_EnableGotoSleep()
#define HAL_DISABLE_SLEEP_UC() hal_DisableGotoSleep()
#define HAL_SLEEP_UC_1US(time) hal_sleep_uc(time)

#ifndef GP_DIVERSITY_FREERTOS
#if defined (GP_DIVERSITY_GPHAL_K8E) 
/* Enable sysTick timer to trigger an interrupt every time number of ticks='ticks' elapsed */
void hal_EnableSysTick(UInt32 ticks);
#endif /* defined (GP_DIVERSITY_GPHAL_K8C) || defined (GP_DIVERSITY_GPHAL_K8D)  || defined (GP_DIVERSITY_GPHAL_K8E) */
#endif /* GP_DIVERSITY_FREERTOS */

/*****************************************************************************
 *                    WATCHDOG
 *****************************************************************************/

typedef void  (* hal_WatchdogTimeoutCallback_t) (void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void hal_WatchdogInit(void);
void hal_EnableWatchdog(UInt16 timeout);
void hal_DisableWatchdog(void);
void hal_ResetWatchdog(void);
void hal_TriggerWatchdog(void);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
void hal_EnableWatchdogInterrupt(UInt16 timeout);
void hal_WatchdogRegisterTimeoutCallback(hal_WatchdogTimeoutCallback_t callback);

#define HAL_WDT_ENABLE(timeout) hal_EnableWatchdog(timeout)
#define HAL_WDT_DISABLE()       hal_DisableWatchdog()
#define HAL_WDT_RESET()         hal_ResetWatchdog()
#define HAL_WDT_FORCE_TRIGGER() hal_TriggerWatchdog()

/*****************************************************************************
 *                    Reset Reason
 *****************************************************************************/

#define hal_ResetReason_Unspecified 0x0
#define hal_ResetReason_HWPor       0x1
#define hal_ResetReason_SWPor       0x2
#define hal_ResetReason_BrownOut    0x3
#define hal_ResetReason_Watchdog    0x4
typedef UInt8 hal_ResetReason_t;

#define hal_WakeupReason_Unspecified 0x0
#define hal_WakeupReason_Reset       0x1
#define hal_WakeupReason_Timer       0x2
#define hal_WakeupReason_LpComp      0x3
#define hal_WakeupReason_Gpio        0x4
typedef UInt8 hal_WakeupReason_t;

/** @brief Return a status code indicating the reason for the most recent reset of the chip. */
hal_ResetReason_t hal_GetResetReason(void);

/** @brief Return a status code indicating the reason for the most recent transition from sleep to active mode. */
hal_WakeupReason_t hal_GetWakeupReason(void);

#define HAL_GET_RESET_REASON()      hal_GetResetReason()

#if defined (GP_DIVERSITY_GPHAL_K8E) 
#define HAL_GET_WAKEUP_REASON()     hal_GetWakeupReason()
#else
#define HAL_GET_WAKEUP_REASON()     (hal_WakeupReason_Unspecified)
#endif

/*****************************************************************************
 *                    UART
 *****************************************************************************/

#include "hal_UART.h"

/*****************************************************************************
 *                    USB
 *****************************************************************************/

#include "hal_USB.h"

/*****************************************************************************
 *                    SPI
 *****************************************************************************/

void  hal_InitSPI(UInt32 frequency, UInt8 mode, Bool lsbFirst);
void  hal_DeInitSPI(void);
UInt8 hal_WriteReadSPI(UInt8 byte);
void hal_WriteStreamSPI(UInt8 length, UInt8* pData);

GP_API void hal_EnableFreeRunningSPI( Bool wordMode, UInt8 length, UInt8* pData);
GP_API void hal_DisableSPI(void);

/* SPI Slave */
void hal_InitSSPI(void);
Bool hal_ReadSSPI(UInt8 *pByte);
Bool hal_WriteSSPI(UInt8 byte);


#define HAL_SPI_TX(x)   hal_WriteReadSPI(x)
#define HAL_SPI_RX(x)   (x) = hal_WriteReadSPI(0)

#include "hal_SPI_slave.h"

/*****************************************************************************
 *                    MUTEX
 *****************************************************************************/

#include "hal_Mutex.h"

/*****************************************************************************
 *                    IR
 *****************************************************************************/

GP_API void hal_InitIR(void);
GP_API void hal_SetIRCarrierPeriod(UInt16 period); //in us
GP_API void hal_MarkEndOfIRCode(void);
GP_API void hal_DisableIRCarrier(void);
GP_API void hal_SendIRSetClr(UInt32 time1, UInt32 time2); //in us
GP_API void hal_TriggerIR(void);
GP_API void hal_SetIRTimingParams (UInt8 internalCpuFreq);

GP_API void hal_cbIrStartOfPattern(void);
GP_API void hal_ConfigureRegisterMode(void);
GP_API void hal_ConfigurePatternMode(UInt8 timeUnit);
GP_API void hal_ConfigureTimeMode(UInt8 timeUnit);
GP_API void hal_SetIRRegisterValue(Bool on);
GP_API void hal_PlaySequenceFromRam(UInt16* pPattern, UInt16 numEntries);
GP_API void hal_SetIRDutyCycle(UInt8 dutyCycle);
GP_API void hal_IrConfigureIRIndexMatch(UInt16 index);
GP_API void hal_IrDisableIndexMatch(void);

#define halIr_InterruptIdStart   0x00
#define halIr_InterruptIdRepeat  0x01
#define halIr_InterruptIdDone    0x02
#define halIr_interruptIdMatch   0x03
typedef UInt8 halIr_InterruptId_t;

typedef void (*halIr_IntCallback_t)(halIr_InterruptId_t id);
GP_API void hal_RegisterIrIntCallback(halIr_IntCallback_t callback);

GP_API void hal_GoToBootloader(void);


/*****************************************************************************
 *                    TWI
 *****************************************************************************/

GP_API void hal_InitTWI(void);
GP_API Bool hal_WriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer);
GP_API Bool hal_PolledAckWriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer);
GP_API Bool hal_WasActiveTWI(void);

/*****************************************************************************
 *                    STWI
 *****************************************************************************/

#ifdef HAL_DIVERSITY_TWI_SLAVE
typedef void (* halSTWI_cbRxData_t)(UInt8 data, Bool generalCall);
typedef void (* halSTWI_cbStopIndication_t)(Bool direction);

/* Register TWI Slave Rx callback */
void hal_RegisterSTWI_RxDataCallback(halSTWI_cbRxData_t callbacks);
void hal_RegisterSTWI_StopIndicationCallback(halSTWI_cbStopIndication_t callbacks);

/** @brief Intialize Two Wire Interface Slave function.
 *
 *  @param slaveAddress      The slave address to of the slave device
 *  @param acceptGeneralCall Enable to allow the slave device to response general call address
 */
void hal_InitSTWI(UInt8 slaveAddress, Bool acceptGeneralCall);

/** @brief Enable Two Wire Interface Slave interrupt function .
 *
 *  @param en       Flag to enable/disable the TWI Slave interrupt
 */
void hal_EnableIntSTWI(Bool en);                                /* Enable TWI Slave interrupt */

/** @brief Prepare the transmit buffer for Two Wire Interface Slave to send out function.
 *
 *  @param length    the data length of the Data to send
 *  @param txData    pointer to the data to send
 */
void hal_SendDataSTWI(UInt8 length, UInt8* txData);             /* Prepare the TWI Slave to send data to master */
#endif // HAL_DIVERSITY_TWI_SLAVE

/*****************************************************************************
 *                    ADC
 *****************************************************************************/

#define hal_AdcChannelANIO0           0x0
#define hal_AdcChannelANIO1           0x1
#define hal_AdcChannelANIO2           0x2
#define hal_AdcChannelANIO3           0x3

#if !defined(GP_DIVERSITY_GPHAL_K8E)
#define hal_AdcChannelANIO4           0x4
#define hal_AdcChannelANIO5           0x5
#define hal_AdcChannelANIO6           0x6
#define hal_AdcChannelANIO7           0x7
#endif
#define hal_AdcChannelBattery         0xB
#define hal_AdcChannelTemperature     0xC //Returned in 8.8 fixed point - integer./1000
#define hal_AdcChannelNone            0xFF
typedef UInt8 hal_AdcChannel_t;

#define hal_AdcConfigRange0_1V       0x0
#define hal_AdcConfigRange0_2V       0x1
#define hal_AdcConfigRange0_3V       0x2
typedef UInt8 hal_AdcConfig_t;

#define hal_AdcAboveMaximum         0x00
#define hal_AdcBelowMinimum         0x01
typedef UInt8 hal_AdcInterruptState_t;
typedef void (*halAdc_callback_t)(hal_AdcInterruptState_t state);

/*  Voltage measurements returned in 2.14 format
    2 bits V, 14 bits 1/16384 V (in 0.001 units)
*/
#define HAL_ADC_VOLTAGE_GET_INTEGER_PART(tempAdc)   ((tempAdc & 0xC000) >> 14)
#define HAL_ADC_VOLTAGE_GET_FLOATING_PART(tempAdc)  ((((UInt32)tempAdc & 0x3FFF)*1000) >> 14)

/* Conversion macro to go from natural voltage (e.g. 1,2 V) to Q2_14 format */
#define HAL_ADC_VOLTAGE_TO_Q2_14_FORMAT(integerPart, floatingPart) (((integerPart << 14) & 0xC000) | (((floatingPart << 14) / 1000) & 0x3FFF))

/*  Temperature measurement returned in 8.8 format
    8 bits degrees Celsius, 8 bits 1/256 degrees Celsius (in 0.001 units)
*/
#define HAL_ADC_TEMPERATURE_GET_INTEGER_PART(tempAdc)   (Int8)((tempAdc & 0xFF00) >> 8)
#define HAL_ADC_TEMPERATURE_GET_FLOATING_PART(tempAdc)  ((((UInt32)tempAdc & 0x00FF)*1000) >> 8)

/* Conversion macro to go from natural celsius temperature (e.g. 20,3 degrees celsius) to Q8_8 format */
#define HAL_ADC_TEMPERATURE_TO_Q8_8_FORMAT(integerPart, floatingPart) (((integerPart << 8) & 0xFF00) | (((floatingPart << 8) / 1000) & 0x00FF))

#define HAL_ADC_TEMPERATURE_MAX_DEGREES_CELSIUS            127
#define HAL_ADC_TEMPERATURE_MIN_DEGREES_CELSIUS            -128

GP_API void   hal_InitADC(void);
GP_API Bool   hal_MeasureADC_8(hal_AdcChannel_t channel, hal_AdcConfig_t measurementConfig, UQ2_6 *pValue);
GP_API Bool   hal_MeasureADC_16(hal_AdcChannel_t channel, hal_AdcConfig_t measurementConfig, UQ2_14* pValue);
GP_API Q8_8   halADC_MeasureTemperature(void);

#define HAL_MEASURE_ADC_CONFIG_NO_OPTIMIZE_FLAG   0x80
#define HAL_MEASURE_ADC_CONFIG_VOLTAGE_RANGE_MASK 0x03

#define GP_HAL_ADC_INVALID_TEMPERATURE      Q_PRECISION_INCR8(-128) //-128 Celsius

#define HAL_MEASURE_ADC_CONFIG_OPTIMISATION_USED(config) (!(config & HAL_MEASURE_ADC_CONFIG_NO_OPTIMIZE_FLAG))
#define HAL_MEASURE_ADC_CONFIG_VOLTAGE_RANGE(config)     (  config & HAL_MEASURE_ADC_CONFIG_VOLTAGE_RANGE_MASK)

#define HAL_ENABLE_HOLD_MAX                 true
#define HAL_DISABLE_HOLD_MAX                false
#define HAL_ENABLE_HOLD_MIN                 true
#define HAL_DISABLE_HOLD_MIN                false
#define HAL_ENABLE_3V6                      true
#define HAL_DISABLE_3V6                     false
#define HAL_DONT_CARE_3V6                   false

/* @brief Start continuous background ADC measurement
 */
GP_API Bool   hal_StartContinuousADCMeasurement(hal_AdcChannel_t channel, Bool maxHold, Bool minHold, Bool anioRange3V6);


/* @brief Start continuous background ADC measurement with the possibility of registering a callback for fast "Out-of-Range"
 * interrupt event when measurement goes below/above certain threshold

   @param channel           Channel to activate measurement on
   @param minThreshold      Threshold in natural units (e.g. volts or celsius) below which the interrupt will fire
   @param maxThreshold      Threshold in natural units (e.g. volts or celsius) above which the interrupt will fire
   @param anioRange3V6      Extended ANIO voltage range enable
   @param cb                Callback which will be called if interrupt is triggered (note: callback is called in interrupt context!)
 */
GP_API Bool hal_StartContinuousADCMeasurementWithOutOfRangeInterrupt(hal_AdcChannel_t channel, UQ2_14 minThreshold, UQ2_14 maxThreshold, Bool anioRange3V6, halAdc_callback_t cb );

/*
* @brief Start continuous background ADC measurement with the ability to pass all possible parameters like
* bypassUnityGainBuffer.
*
* Bypassing the unity gain buffer allows full range 0-3.3V conversion results be obtained,
* at a cost of lower impedance input will be used which may affect the gain of the ADC module.
*/
GP_API Bool hal_StartContinuousADCMeasurementWithParameters(hal_AdcChannel_t channel, Bool maxHold, Bool minHold, Bool outOfRange, UQ2_14 minThreshold, UQ2_14 maxThreshold, Bool anioRange3V6, Bool bypassUnityGainBuffer);

/* @brief Stop continuous background ADC measurement with the out of range interrupt possibility

   @param channel           Channel to stop measurement on
 */
GP_API void hal_StopContinuousADCMeasurementWithOutOfRangeInterrupt(hal_AdcChannel_t channel);

/* @brief function to enable or disable the "Out-of-Range" interrupt
 *        Can be used to rate-limit the amount of OOR interrupts generated during a corrective action
 *        to get the analog value back in range
 */
void halAdc_ToggleOutOfRangeInterrupts(Bool enable);

GP_API void   hal_StopContinuousADCMeasurement(hal_AdcChannel_t channel);
GP_API UQ2_14 hal_GetContinuousADCMeasurement(hal_AdcChannel_t channel);
GP_API void   hal_ClearContinuousADCMeasurement(hal_AdcChannel_t channel);
GP_API UInt8  hal_GetBufferUsedBy(hal_AdcChannel_t channel);
GP_API UQ2_14 halADC_ConvertToFixedPointValue(UInt16 raw, UInt8 channel);

/*****************************************************************************
 *                    PWM
 *****************************************************************************/

#ifndef HAL_PWM_MAX_MAIN_COUNTER
#define HAL_PWM_MAX_MAIN_COUNTER            0xFF          /* PWM clock @62.5 Khz with 8bit resolution */
#endif //HAL_PWM_MAX_MAIN_COUNTER

#define HAL_PWM_CARRIER_COUNTER_WRAP_POWER  2

#define HAL_PWM_CHANNEL_0                   0
#define HAL_PWM_CHANNEL_1                   1
#define HAL_PWM_CHANNEL_2                   2
#define HAL_PWM_CHANNEL_3                   3
#define HAL_PWM_CHANNEL_4                   4
#define HAL_PWM_CHANNEL_5                   5

#define HAL_PCM_DC                          ((HAL_PWM_MAX_MAIN_COUNTER)>>1)

// duty cycle percent in 0.01% steps
#define HAL_PWM_MAX_DUTY_CYCLE_PC           (10000UL)

/** @brief Initialize the PWM peripheral.
 *         It will connect a main source clock and configure the PWM peripheral to run on a fixed frequency:
 *         HAL_PWM_FREQUENCY (defaults to 250Hz)
 *
 *         Additional source clocks (timers) will be installed to be used as carrier and timestamp inputs.
 */
GP_API void hal_InitPWM(void);

/** @brief Enable or disable the source clocks (timers) that drive the PWM peripheral.
 *         When disabled, all pins will be disconnected from the PWM peripheral.
 *
 *  @param enable   true to enable, false to disable
 *
 *  @note: This function will also make sure the chip is not going to deep sleep by calling
 *         gpHal_GoToSleepWhenIdle(!enable). This means that this API needs to be always called in-sync,
 *         meaning never call "enable" or "disable" two times in a row
 */
GP_API void hal_EnablePwm(Bool enable);

/** @brief Enable or disable the PWM function of the GPIO pin for the specified PWM channel.
 *
 *  @param channel  PWM channel (HAL_PWM_CHANNEL_x)
 *  @param enable   True to enable the PWM function of the associated GPIO, False to disable it.
 */
GP_API void hal_SetChannelEnabled(UInt8 channel, Bool enabled);

/** @brief Returns if the @p channel is mapped to a GPIO pin
 *
 *  @return true when the channel is mapped
 */
GP_API Bool hal_PwmIsChannelEnabled(UInt8 channel);

/** @brief Returns if a channel of the PWM module is mapped to a GPIO pin
 *
 *  @return true when any channel is mapped
 */
GP_API Bool hal_PwmIsAnyChannelEnabled(void);

/** @brief Enable or disable inverted output for the specified PWM channel.
 *
 *  @param channel  PWM channel (HAL_PWM_CHANNEL_x)
 *  @param enable   True to enable inverted output (output low for duration of duty cycle).
 */
GP_API void hal_InvertOutput(UInt8 channel, Bool invert);

// Set duty cycle. Deprecated in favour of hal_SetDutyCyclePercentage().
// Obsolete function hal_SetDutyCycle.
GP_API void hal_SetDutyCycle(UInt8 channel, UInt16 dutyCycle);

/** @brief Set duty cycle (output active ratio) of a PWM channel.
 *
 *  dutyCyclePercent is set in 0.01 percent increments
 *
 *  @param channel           PWM channel (HAL_PWM_CHANNEL_x)
 *  @param dutyCyclePercent  Active output time ratio in range 0 .. HAL_PWM_MAX_DUTY_CYCLE_PC
 */
GP_API void hal_SetDutyCyclePercentage(UInt8 channel, UInt16 dutyCyclePercent);

/** @brief Enable or disable symmetric PWM output on the specified channel.
 *
 *  In symmetric mode, the counter counts up and down to create PWM pulses
 *  that are symmetric around the center of the PWM base interval.
 *
 *  @param channel  PWM channel (HAL_PWM_CHANNEL_x)
 *  @param enable   True to enable symmbetric mode, False to disable.
 */
GP_API void hal_SetSymmetricMode(UInt8 channel, Bool enable);

// Adjust PWM base frequency and PWM range. Deprecated in favour of hal_SetFrequencyPwm().
GP_API void hal_SetFrequencyCorrectionFactor(UInt8 factor);

/** @brief Configure PWM function for PCM playback via DMA.
 *
 *  @param counterResolution     Maximum PCM sample value (e.g. 255 for 8-bit samples)
 *  @param carrier               Nr of bits in carrier counter.
 *  @param wordmode              True for 16-bit samples, false for 8-bit samples.
 *  @param shiftThresholdUpdate  Output on multiple PWM channels.
 *  @param channel               PWM channel for PCM output (HAL_PWM_CHANNEL_x).
 *
 *  Effective sample rate is 16 MHz / (counterResolution + 1) / 2**carrier.
 *  For example, counterResolution=255, carrier=2 --> 15625 samples/s.
 *
 *  16-bit mode is currently not supported.
 *  Multi-channel output is currently not supported.
 */
GP_API void hal_ConfigPWM(
            UInt8 counterResolution,
            UInt8 carrier,
            Bool wordmode,
            Bool shiftThresholdUpdate,
            UInt8 channel);

// Adjust PWM base frequency. Deprecated in favour of hal_SetFrequencyPwm().
GP_API void hal_SetPrescalerCounterWrapPower(UInt8 val);

/** @brief Set base frequency of the PWM system.
 *
 *  @param frequency  Desired frequency in Hz. Must be > 0.
 *
 *  Duty cycles of all PWM channels will be invalid after changing the base frequency.
 *  Subsequent calls to hal_SetDutyCyclePercentage() are needed to set valid duty cycles.
 */
GP_API void hal_SetFrequencyPwm(UInt32 frequency);

/** @brief Start PCM playback via DMA. */
GP_API void halPWM_DmaStart(void);

/** @brief Stop PCM playback via DMA. */
GP_API void halPWM_DmaStop(void);

/** @brief Callback invoked to fetch more samples during PCM playback via DMA.
 *
 *  This function should be implemented by applications using PCM playback via DMA.
 *
 *  @param sizeToFill  Size in bytes of new sample data needed.
 *  @param dma_Buffer  Pointer to sample buffer.
 *  @param writeIndex  Offset in bytes in the buffer where new sample data must be written.
 *  @return True when succesful, False if no more data is available and playback should be halted.
 */
Bool halPWM_cbWriteDmaBufferIndication(
                  UInt16 sizeToFill,
                  UInt8* dma_Buffer,
                  UInt16 writeIndex);

typedef enum {
    PLAYBACK_START = 0,
    PLAYBACK_BUSY,
    PLAYBACK_STOP,
} playback_status_t;

/*****************************************************************************
 *                    DMA
 *****************************************************************************/

GP_API void hal_InitDMA( void );
GP_API Bool hal_LockDMA( void );
GP_API void hal_ReleaseDMA( void );
GP_API void hal_handleAlmostFullDMA( UInt8 *dest_rd_ptr, UInt8 *dest_rd_ptr_wrap, UInt16 dmaBufLength );

/*****************************************************************************
 *                    RADIO INTERRUPT
 *****************************************************************************/
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
GP_API Bool hal_HandleRadioInterrupt(Bool execute);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#define HAL_RADIO_INT_EXEC_IF_OCCURED()    hal_HandleRadioInterrupt(true)
#define HAL_RADIO_INT_CHECK_IF_OCCURED()   hal_HandleRadioInterrupt(false)

/*****************************************************************************
 *                    ASP
 *****************************************************************************/


#ifndef HAL_ASP_MAX_CHANNELS
#define HAL_ASP_MAX_CHANNELS 2
#endif //HAL_ASP_MAX_CHANNELS

#define hal_AspEdgeFalling  0
#define hal_AspEdgeRising   1
#define hal_AspEdgeBoth     2
typedef UInt8 hal_AspEdge_t;

//2MHz based decimation factors
#define HAL_ASP_DECIMATION_2MHZ_80KHZ      25  //2Mhz     -> 80 kHz     (/5 left for SW to get to 16kHz)
#define HAL_ASP_DECIMATION_2MHZ_62_5KHZ    32  //2Mhz     -> 62.5 kHz   (+- /4 left for SW to get to 16kHz)
#define HAL_ASP_DECIMATION_2MHZ_40KHZ      50  //2Mhz     -> 40 kHz     (/5 left for SW to get to 8kHz)
#define HAL_ASP_DECIMATION_2MHZ_31_125KHZ  64  //2Mhz     -> 31.125 kHz (+- /2 left for SW to get to 16kHz)
#define HAL_ASP_DECIMATION_2MHZ_16KHZ      125 //2Mhz     -> 16 kHz

#define HAL_ASP_DECIMATION_2048KHZ_32KHZ      64

#define HAL_ASP_DECIMATION_2048KHZ_64KHZ      32
typedef UInt8 hal_AspDecimation_t;

#define HAL_ASP_MODE_VOICE            0
#define HAL_ASP_MODE_ULTRASOUND       1
typedef UInt8 hal_AspMode_t;

/* FIR decimation */
#ifndef GP_AUDIO_DIVERSITY_BYPASS_FIR_DECIMATION
#define HAL_ASP_FIR_DECI_80KHZ_16KHZ                       (5)
#define HAL_ASP_FIR_DECI_64KHZ_16KHZ                       (4)
#define HAL_ASP_FIR_DECI_40KHZ_8KHZ                        (5)
#define HAL_ASP_FIR_DECI_32KHZ_8KHZ                        (4)

#else
/* Make sure the length of the DMA buffer gets compensated */
#define HAL_ASP_FIR_DECI_80KHZ_16KHZ                       (1)
#define HAL_ASP_FIR_DECI_64KHZ_16KHZ                       (1)
#define HAL_ASP_FIR_DECI_40KHZ_8KHZ                        (1)
#define HAL_ASP_FIR_DECI_32KHZ_8KHZ                        (1)
#endif /* GP_AUDIO_DIVERSITY_BYPASS_FIR_DECIMATION */

#define HAL_ASP_FIR_DECI_VOICE_16KHZ           (HAL_ASP_FIR_DECI_64KHZ_16KHZ)     /* 4 */
#define HAL_CIC_DECIMATION_VOICE_16KHZ         (HAL_ASP_DECIMATION_2048KHZ_64KHZ) /* 32 */
#define HAL_ASP_FIR_DECI_VOICE_8KHZ            (HAL_ASP_FIR_DECI_32KHZ_8KHZ)      /* 4 */
#define HAL_CIC_DECIMATION_VOICE_8KHZ          (HAL_ASP_DECIMATION_2048KHZ_32KHZ) /* 64 */

#if !defined(GP_DIVERSITY_GPHAL_K8E) 

#define HAL_ASP_FIR_DECI_ULTRASOUND       (2)
#define HAL_CIC_DECIMATION_ULTRASOUND     (22)

#else

/* Default frequency for ultrasound equals 3,072Mhz /32 /2 = 48kHz */
#define HAL_ASP_FIR_DECI_ULTRASOUND       (2)
#define HAL_CIC_DECIMATION_ULTRASOUND     (32)

#endif /* !GP_DIVERSITY_GPHAL_K8C && !GP_DIVERSITY_GPHAL_K8D && !defined(GP_DIVERSITY_GPHAL_K8E) */


void hal_InitAsp();
void hal_DeInitAsp(void);

//Only to be called after hal_InitAsp()
void hal_StartAsp(UInt16 numBytes,
                  Int16* pData_ch0,
                  Int16* pData_ch1,
                  hal_AspEdge_t edge,
#if defined (GP_DIVERSITY_GPHAL_K8E) 
                  hal_AspMode_t mode,
#endif
                  hal_AspDecimation_t decimation);

void hal_StopAsp(void);

/*****************************************************************************
 *                    I2S
 *****************************************************************************/

#define GP_HAL_I2SM_8KHZ                      (0)
#define GP_HAL_I2SM_16KHZ                     (1)
#define GP_HAL_I2SM_45_5KHZ                   (2)
#define GP_HAL_I2SM_46_5KHZ                   (3)
#define GP_HAL_I2SM_48KHZ                     (4)

// Master interface
void hal_initI2S_m();
void hal_configI2S_m(UInt8 output_samplerate,
                        UInt8 bytes_left, UInt8 bytes_right);
void hal_startI2S_m(UInt16 app_dma_buffer_size,
                    UInt8 *samples_in_dma);
void hal_stopI2S_m(void);

#if defined (GP_DIVERSITY_GPHAL_K8E) 
// Slave interface
void hal_initI2S_s();
void hal_configI2S_s(UInt8 output_samplerate,
                        UInt8 bytes_left, UInt8 bytes_right);
void hal_startI2S_s(UInt16 app_dma_buffer_size,
                    UInt8 *samples_in_dma);
void hal_stopI2S_s(void);
#endif

/*****************************************************************************
 *                    MPU
 *****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_HAL_H_
