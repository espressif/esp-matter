/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_ES.c
 *
 *  This file defines all functions for the event scheduler and sleep modes.  These functions can be used to schedule certain actions : an interrupt, TX of a packet, etc.
 *  The diffent sleep and wakeup modes can also be initialized and used with these functions.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal.h"
#include "gpHal_ES.h"
#include "gpHal_DEFS.h"

#include "gpHal_HW.h"
#include "gpHal_reg.h"
#include "gpHal_Calibration.h"
#include "gpAssert.h"
#include "gpSched.h"
#include "gpStat.h"
#include "gpHal_OscillatorBenchmark.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

// Benchmark factor:
// ES_CALIBRATION_FACTOR = GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR * (nr of 32 MHz ticks / nr of sleep counter ticks)
#define GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR       0x100000

// Nominal ES_CALIBRATION_FACTOR for 16 MHz sleep mode (31.25 kHz sleep counter).
#define GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR_16MHZ 0x40000000UL

// How many measurements to use for calculating the average benchmark value
#define GP_HAL_ES_MAX_NR_OF_BENCHMARK_MEASUREMENTS_HISTORY      32

#ifndef GP_HAL_ES_BENCHMARK_POWER_XTAL
#define GP_HAL_ES_BENCHMARK_POWER_XTAL                      8 // 8 PPM
#endif
/* (1000000.0us/16000000Hz)*( 1000000.0us / ((1000000.0us/32000Hz)*pow(2,GP_HAL_ES_BENCHMARK_POWER_XTAL)) ) <= 8PPM */

#define GP_HAL_ES_DEFAULT_STARTUP_SYMBOL_TIME_OTHER_US      (UInt16)1200
#define GP_HAL_ES_DEFAULT_LONG_STARTUP_SYMBOL_TIME_OTHER_US (GP_HAL_ES_DEFAULT_STARTUP_SYMBOL_TIME_OTHER_US + GP_HAL_ES_BACKUP_DURATION_OTHER_US)
#define GP_HAL_ES_MIN_STARTUP_SYMBOL_TIME_OTHER_US          (UInt16)400
#define GP_HAL_ES_MAX_STARTUP_SYMBOL_TIME_OTHER_US          (UInt16)4000

#define GP_HAL_ES_DEFAULT_STARTUP_SYMBOL_TIME_16MHZ_US              100
#define GP_HAL_ES_DEFAULT_LONG_STARTUP_SYMBOL_TIME_16MHZ_US         300

#define GP_HAL_ES_STARTUP_SYMBOL_TIME_INCREMENT_WHEN_TOO_LATE_US     100
#define GP_HAL_ES_STARTUP_SYMBOL_TIME_DECREMENT_DEFAULT_US           5

#ifndef GP_DIVERSITY_GPHAL_32KHZ_INIT_CALIBRATION_MIN_BENCHMARKS
#define GP_DIVERSITY_GPHAL_32KHZ_INIT_CALIBRATION_MIN_BENCHMARKS    10
#endif // GP_DIVERSITY_GPHAL_32KHZ_INIT_CALIBRATION_MIN_BENCHMARKS
#ifndef GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_MIN_BENCHMARKS
#define GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_MIN_BENCHMARKS     4
#endif //GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_MIN_BENCHMARKS

// Maximal duration of a benchmark measurement (before we consider it as failed)
#define GPHAL_OSCILLATOR_BENCHMARK_TO_MS        5000

#define GPHAL_ES_FREQUENCY_32MHZ                32000000
#define GPHAL_ES_RC_SLEEP_CLOCK_FREQUENCY       32000

// Interval between triggering benchmark of the RC oscillator
#ifndef GP_HAL_RC_BENCHMARK_CALIBRATION_PERIOD_US
#define GP_HAL_RC_BENCHMARK_CALIBRATION_PERIOD_US              10000UL /* 10 ms */
#endif

// Interval between triggering benchmark of the xt32k oscillator
#ifndef GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_PERIOD_MS
#define GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_PERIOD_MS    900000UL /* 15 min */
#endif

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#define reEvaluateEvents() \
    { \
        /*ReevaluateEvents + Wait untill done*/ \
        GP_WB_ES_REEVALUATE_EVENTS(); \
        __DSB(); \
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_NEXT_EVENT_SEARCH_BUSY(), GP_HAL_DEFAULT_TIMEOUT); \
    };

#define setEventValid(eventNbr,enable)  \
    do {                                                                \
        if(enable)  \
            GP_WB_WRITE_U8(GP_WB_ES_SET_VALID_EVENTS_0_ADDRESS + eventNbr/8, (1 << (eventNbr % 8)));  \
        else    \
            GP_WB_WRITE_U8(GP_WB_ES_CLR_VALID_EVENTS_0_ADDRESS + eventNbr/8, (1 << (eventNbr % 8)));  \
    } while(false)

/** @brief Writes a complete eventDescriptor_t structure to the corresponding Absolute Event entry.
 *
 *  Writes a complete eventDescriptor_t structure to the corresponding Absolute Event entry.
 *
 * @param pAbsoluteEventDescriptor Pointer to the AbsoluteEventDescriptor_t structure containing the Event options.
 * @param eventNbr                 The index of the Absolute Event (1..16).
*/
#define GP_ES_WRITE_EVENT_DESCRIPTOR(pAbsoluteEventDescriptor, eventNbr )       \
    do {                                                                        \
        /* Convert endianness (if needed) */                                    \
        HOST_TO_RF_UINT32(&(pAbsoluteEventDescriptor)->exTime);                 \
        HOST_TO_RF_UINT32(&(pAbsoluteEventDescriptor)->recPeriod);              \
        HOST_TO_RF_UINT16(&(pAbsoluteEventDescriptor)->recAmount);              \
        HOST_TO_RF_UINT16(&(pAbsoluteEventDescriptor)->customData);             \
        HOST_TO_RF_ENUM(&(pAbsoluteEventDescriptor)->control);                  \
        HOST_TO_RF_ENUM(&(pAbsoluteEventDescriptor)->type);                     \
                                                                                \
        GP_HAL_WRITE_BYTE_STREAM                                                       \
            (                                                                   \
             (gpHal_Address_t)(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET),    \
             (pAbsoluteEventDescriptor),                                        \
             GP_MM_RAM_EVENT_OFFSET                                           \
             );                                                                 \
                                                                                \
        /* Convert endianness (if needed) */                                    \
        RF_TO_HOST_UINT32(&(pAbsoluteEventDescriptor)->exTime);                 \
        RF_TO_HOST_UINT32(&(pAbsoluteEventDescriptor)->recPeriod);              \
        RF_TO_HOST_UINT16(&(pAbsoluteEventDescriptor)->recAmount);              \
        RF_TO_HOST_UINT16(&(pAbsoluteEventDescriptor)->customData);             \
        RF_TO_HOST_ENUM(&(pAbsoluteEventDescriptor)->control);                  \
        RF_TO_HOST_ENUM(&(pAbsoluteEventDescriptor)->type);                     \
    } while (false)

#define DEFAULT_RC_SLEEP_CLOCK_NOF_TICKS                                32  //Corresponding to 1ms benchmark time @32Khz
#define DEFAULT_RC_SLEEP_CLOCK_BENCHMARK_QUEUE_DEPTH                    8   //max GP_HAL_ES_MAX_NR_OF_BENCHMARK_MEASUREMENTS_HISTORY
#define DEFAULT_RC_SLEEP_CLOCK_BENCHMARK_ENABLE_MULTIPLE_MEASUREMENTS   1   //enable multiple measurements in each period chip is awake, at GP_HAL_RC_BENCHMARK_CALIBRATION_PERIOD_US interval
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
static UInt16 gpHal_ES_AbsoluteEventsInUse;
#endif //GP_COMP_GPHAL_ES_ABS_EVENT

// State to use for averaging benchmark calibration measurements
static UInt8 gpHal_EsBenchmarkCurrentIndex;
static UInt32 gpHal_EsBenchmarkMeasurements[GP_HAL_ES_MAX_NR_OF_BENCHMARK_MEASUREMENTS_HISTORY];

// Last timestamp when a benchmark calibration was performed
static UInt32 gpHalES_SleepModeRC_TsLastBenchmark;

static UInt32 gpHal_SleepMode32kHz_stable_benchmark = GPHAL_ES_BENCHMARK_COUNTER_INVALID;
static UInt32 gpHal_SleepModeRC_stable_benchmark = GPHAL_ES_BENCHMARK_COUNTER_INVALID;

static gpHal_SleepMode_t gpHal_background_benchmark_mode;
static Bool gpHalEs_32kHzMeasurementAborted;

static Bool gpHalEs_IsQueueFull;

static gpHal_SleepClockMeasurementStatus_t gpHalEs_32kHz_benchmark_status;
UInt8 calTaskHandleRC    = GP_HAL_CALIBRATION_INVALID_TASK_HANDLE;
UInt8 calTaskHandleXt32K = GP_HAL_CALIBRATION_INVALID_TASK_HANDLE;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static UInt32 gpHalES_GetBenchmarkAverage(UInt8 measurementHistory);

static void gpHalES_StartBackground_Benchmark(gpHal_SleepMode_t sleepMode, UInt32 nowTs);
static UInt32 gpHalES_ReadOscillatorBenchmark(void);
static UInt32 gpHalES_FilterRCBenchmarkMeasurement(UInt32 benchmark);
static void gpHal_OscillatorBenchmark_3Phase_Complete(gpHal_OscillatorBenchmark_Status_t status);

static void gpHalEs_TriggerOscillatorBenchmarkRcMeasurement(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
typedef struct {
    Bool is_initialized;
    Bool EnableMultipleMeasurements;
    UInt8 QueueDepth;
    UInt16 NoOfLPTicks;
} gpHalEs_RCCalibProperties;

static gpHalEs_RCCalibProperties gpHalEs_RCCalibProp;

void gpHalES_EnableOscillatorBenchmark(Bool enable, Bool keepAwake)
{
    if(enable)
    {
        // Before starting, make sure we do not have an interrupt pending
        GP_WB_ES_CLR_OSCILLATOR_BENCHMARK_DONE_INTERRUPT();
    }

    // Enable circuit itself
    GP_WB_WRITE_ES_ENABLE_OSCILLATOR_BENCHMARK(enable);
    // Enable clock to circuit
    GP_WB_WRITE_ES_ENABLE_CLK_TIME_REFERENCE_OSCILLATOR_BENCHMARK_BY_UC(enable);
    GP_WB_WRITE_INT_CTRL_MASK_ES_OSCILLATOR_BENCHMARK_DONE_INTERRUPT(enable);
    // Make sure to keep chip awake during a measurement
    GP_WB_WRITE_ES_KEEP_AWAKE_DURING_OSCILLATOR_BENCHMARK_MEASUREMENT(keepAwake);
}

UInt32 gpHalES_GetBenchmarkAverage(UInt8 measurementHistory)
{
    UInt32 total = 0;
    UInt8 nrOfMeasurements = 0;
    UInt32 avg = 0;

    while(nrOfMeasurements < measurementHistory && (gpHal_EsBenchmarkMeasurements[nrOfMeasurements] != 0))
    {
        total += gpHal_EsBenchmarkMeasurements[nrOfMeasurements];
        nrOfMeasurements++;
    }

    gpHalEs_IsQueueFull = (nrOfMeasurements == measurementHistory);
    avg = (nrOfMeasurements > 0)? total/nrOfMeasurements : GPHAL_ES_BENCHMARK_COUNTER_INVALID;
    return avg;
}

void gpHalES_SetStartupSymbolTimes(UInt32 normalStartupTime, UInt32 longStartupTime)
{
    GP_WB_WRITE_ES_STARTUP_SYMBOL_TIME(normalStartupTime);
    GP_WB_WRITE_ES_LONG_STARTUP_SYMBOL_TIME(longStartupTime);

    // Apply and wait for done
    GP_WB_ES_APPLY_STARTUP_SYMBOL_TIME();
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_RECONVERSION_BUSY(), GP_HAL_DEFAULT_TIMEOUT);
}


void gpHalEs_StartOscillatorBenchmark(void)
{

    //Perform benchmark measurement
    GP_LOG_PRINTF("t_obm %ld", 0, GP_WB_READ_ES_KEEP_AWAKE_DURING_OSCILLATOR_BENCHMARK_MEASUREMENT());
    GP_WB_ES_TRIGGER_OSCILLATOR_BENCHMARK();
    // Probably not needed anymore
    __DSB();
}


void gpHalEs_TriggerOscillatorBenchmarkRcMeasurement(void)
{
    if (GP_BSP_32KHZ_CRYSTAL_AVAILABLE())
    {
        if (GPHAL_ES_BENCHMARK_COUNTER_INVALID == gpHal_SleepModeRC_stable_benchmark)
        {
            UInt32 tnow = GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();
            gpHalES_StartBackground_Benchmark(gpHal_SleepModeRC, tnow);
        }
    }
}

// Called regularly via calibration framework to re-benchmark the oscillator frequency.
void gpHalEs_RCCalibrationHandler(const gpHal_CalibrationTask_t* pTask)
{
    NOT_USED(pTask);
    gpHal_SleepMode_t sleepMode = gpHal_GetSleepMode();
    Bool isFirstCalibAfterWakeup = gpHal_CalibrationGetFirstAfterWakeup();

    /* return if the current selected sleep mode is not RC*/
    if (sleepMode != gpHal_SleepModeRC)
    {
        return;
    }

    GP_LOG_PRINTF("LJRC recal", 0);
    /* Detect if chip went to sleep after triggering calibration (KEEP_AWAKE = 0) while calibration RC oscillator */
    if(isFirstCalibAfterWakeup &&
        GP_ES_IS_OSCILLATOR_BENCHMARK_RUNNING() &&
        (gpHal_background_benchmark_mode == gpHal_SleepModeRC))
    {
        /* Chip can go to sleep after triggering measurement only if keep awake is zero */
        if(GP_WB_READ_ES_KEEP_AWAKE_DURING_OSCILLATOR_BENCHMARK_MEASUREMENT() == 0)
        {
            gpHalES_EnableOscillatorBenchmark(false, false);
        }
    }

    if (isFirstCalibAfterWakeup || gpHalEs_RCCalibProp.EnableMultipleMeasurements)
    {
        // if multiple measurements in wake cycle is not defined then do recalibrations only
        // once after wake up
        gpHalES_TriggerOscillatorBenchmark();
    }
}

#ifdef GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
void gpHalEs_Xt32KCalibrationHandler(const gpHal_CalibrationTask_t* pTask)
{
    NOT_USED(pTask);
    gpHal_SleepMode_t sleepMode = gpHal_GetSleepMode();
    Bool isFirstCalibAfterWakeup = gpHal_CalibrationGetFirstAfterWakeup();

    /* return if the current selected sleep mode is not xt32k*/
    if (sleepMode != gpHal_SleepMode32kHz)
    {
        return;
    }

    GP_LOG_PRINTF("Xt32k recal", 0);
    /* Detect if chip went to sleep after triggering calibration (KEEP_AWAKE = 0) while calibration RC oscillator */
    if(isFirstCalibAfterWakeup &&
        GP_ES_IS_OSCILLATOR_BENCHMARK_RUNNING() &&
        (gpHal_background_benchmark_mode == gpHal_SleepModeRC))
    {
        /* Chip can go to sleep after triggering measurement only if keep awake is zero */
        if(GP_WB_READ_ES_KEEP_AWAKE_DURING_OSCILLATOR_BENCHMARK_MEASUREMENT() == 0)
        {
            gpHalES_EnableOscillatorBenchmark(false, false);
        }
    }

    gpHalES_TriggerOscillatorBenchmark();
}
#endif //#ifdef GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpHal_InitEs(void)
{
    UInt16 rcSleepClockNofTicks;
    UInt8 queueDepth;
    Bool enMultMeasurements;

    UIntLoop i;
    gpHalEs_IsQueueFull = false;

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
    gpHal_ES_AbsoluteEventsInUse = 0;
#endif //GP_COMP_GPHAL_ES_ABS_EVENT
    gpHalEs_32kHzMeasurementAborted = false;
    gpHalES_SleepModeRC_TsLastBenchmark = 0;

    gpHal_EsBenchmarkCurrentIndex = 0;
    for(i = 0; i < GP_HAL_ES_MAX_NR_OF_BENCHMARK_MEASUREMENTS_HISTORY; i++)
    {
        gpHal_EsBenchmarkMeasurements[i] = 0;
    }

    if(GP_WB_READ_NVR_RC_SLEEP_CLOCK_BENCHMARK_NO_OF_LP_TICKS() != 0)
    {
        /*
         * if NVR_RC_SLEEP_CLOCK_BENCHMARK_NO_OF_LP_TICKS is non-zero, all default values for
         * RC sleep calibration will come from the info page
         */
        rcSleepClockNofTicks = GP_WB_READ_NVR_RC_SLEEP_CLOCK_BENCHMARK_NO_OF_LP_TICKS();
        queueDepth = GP_WB_READ_NVR_RC_SLEEP_CLOCK_BENCHMARK_QUEUE_DEPTH();
        enMultMeasurements = GP_WB_READ_NVR_RC_SLEEP_CLOCK_BENCHMARK_ENABLE_MULTIPLE_MEASUREMENTS();
    }
    else
    {
        rcSleepClockNofTicks = DEFAULT_RC_SLEEP_CLOCK_NOF_TICKS;
        queueDepth = DEFAULT_RC_SLEEP_CLOCK_BENCHMARK_QUEUE_DEPTH;
        enMultMeasurements = DEFAULT_RC_SLEEP_CLOCK_BENCHMARK_ENABLE_MULTIPLE_MEASUREMENTS;
    }

    /* Default initialization of RC sleep clock calibration */
    gphalES_RCSleepClockCalibrationInit (rcSleepClockNofTicks,
            queueDepth,
            enMultMeasurements);

    calTaskHandleRC = GP_HAL_CALIBRATION_INVALID_TASK_HANDLE;
    calTaskHandleXt32K = GP_HAL_CALIBRATION_INVALID_TASK_HANDLE;

}

void gphalES_RCSleepClockCalibrationInit (
        UInt16 nofRCSleepClockTicks,
        UInt8 QueueDepth,
        Bool EnableMultipleMeasurments)
{
    gpHalEs_RCCalibProperties *calibprop = &gpHalEs_RCCalibProp;
    GP_ASSERT_SYSTEM((nofRCSleepClockTicks > 0) & (nofRCSleepClockTicks <= 0x0FFF));
    GP_ASSERT_SYSTEM((QueueDepth > 0) && (QueueDepth <= GP_HAL_ES_MAX_NR_OF_BENCHMARK_MEASUREMENTS_HISTORY));

    calibprop->is_initialized = true;
    calibprop->NoOfLPTicks = nofRCSleepClockTicks;
    calibprop->QueueDepth = QueueDepth;
    calibprop->EnableMultipleMeasurements = EnableMultipleMeasurments;

    /* Flush the benchmark measurement queue */
    MEMSET(gpHal_EsBenchmarkMeasurements, 0, sizeof(gpHal_EsBenchmarkMeasurements));
    gpHal_EsBenchmarkCurrentIndex = 0;
    gpHalEs_IsQueueFull = false;

    /* if the sleep clock calibration parameters are changed in between an ongoing RC sleep clock calibration
     * then reset the oscillator benchmark and trigger new calibration*/
    if (gpHal_background_benchmark_mode == gpHal_SleepModeRC)
    {
        gpHal_SleepModeRC_stable_benchmark = GPHAL_ES_BENCHMARK_COUNTER_INVALID;
        if(GP_ES_IS_OSCILLATOR_BENCHMARK_RUNNING())
        {
            gpHalES_EnableOscillatorBenchmark(false, false);
            gpHalES_TriggerOscillatorBenchmark();
        }
    }
}
void gphalES_getRCSleepClockCalibrationParameters (
        UInt16 *nofRCSleepClockTicks,
        UInt8 *QueueDepth,
        Bool *EnableMultipleMeasurments)
{
    gpHalEs_RCCalibProperties *calibprop = &gpHalEs_RCCalibProp;
    if(calibprop->is_initialized)
    {
        *nofRCSleepClockTicks = calibprop->NoOfLPTicks;
        *QueueDepth = calibprop->QueueDepth;
        *EnableMultipleMeasurments = calibprop->EnableMultipleMeasurements;
    } else {
        *nofRCSleepClockTicks = 0;
        *QueueDepth = 0;
        *EnableMultipleMeasurments = 0;
    }
}

void gpHal_StartInitialBenchmarkMeasurements(void)
{
    UInt32 tnow = GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();

    if (GP_BSP_32KHZ_CRYSTAL_AVAILABLE())
    {
        gpHalEs_32kHz_benchmark_status = gpHal_SleepClockMeasurementStatusPending;
        gpHal_OscillatorBenchmark_3Phase_Init(GP_DIVERSITY_GPHAL_32KHZ_INIT_CALIBRATION_MIN_BENCHMARKS);
        gpHalES_StartBackground_Benchmark(gpHal_SleepMode32kHz, tnow);
    }
    else
    {
        gpHalES_StartBackground_Benchmark(gpHal_SleepModeRC, tnow);
    }
}

Bool gpHalES_AdvancedCalibrationAndReconversionBusy(void)
{
    UInt8 timeReferenceState = GP_WB_READ_ES_TIME_REFERENCE_STATE();
    return ((GP_WB_GET_ES_APPLY_EXTERNAL_ADVANCED_CALIBRATION_BUSY_FROM_TIME_REFERENCE_STATE(timeReferenceState) == 1) ||
            (GP_WB_GET_ES_SYMBOL_COUNTER_UPDATED_SINCE_LAST_CALIBRATION_FROM_TIME_REFERENCE_STATE(timeReferenceState) == 0) ||
            (GP_WB_GET_ES_RECONVERSION_BUSY_FROM_TIME_REFERENCE_STATE(timeReferenceState) == 1)
           );
}

static UInt32 gpHalES_GetCalibrationFactor(UInt8 sleepMode)
{
    UInt32 calibrationFactor;

    if (gpHal_SleepMode32kHz == sleepMode)
    {
        GP_ASSERT_DEV_INT(gpHal_SleepMode32kHz_stable_benchmark != GPHAL_ES_BENCHMARK_COUNTER_INVALID);
        calibrationFactor    = gpHal_SleepMode32kHz_stable_benchmark * (GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR >> GP_HAL_ES_BENCHMARK_POWER_XTAL);
    }
    else if (gpHal_SleepModeRC == sleepMode)
    {
        GP_ASSERT_DEV_INT(gpHal_SleepModeRC_stable_benchmark != GPHAL_ES_BENCHMARK_COUNTER_INVALID);
        calibrationFactor    = ((UInt64)gpHal_SleepModeRC_stable_benchmark * GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR) / (gpHalEs_RCCalibProp.NoOfLPTicks);
        /*GP_LOG_SYSTEM_PRINTF("gpHal_RealignTimebase(): stablebenchmark %ld", 0, gpHal_SleepModeRC_stable_benchmark);*/
    }
    else
    {
        // 16 MHz sleep mode - sleep counter is derived from active time reference so always perfectly calibrated.
        GP_ASSERT_DEV_INT(gpHal_SleepMode16MHz == sleepMode);
        calibrationFactor = GP_WB_ES_BENCHMARK_CALIBRATION_FACTOR_16MHZ;
    }

    return calibrationFactor;
}

UInt32 gpHalES_GetOscillatorBenchmark(gpHal_SleepMode_t sleepMode)
{
    UInt32 oscillatorBenchmark = 0;
    if (gpHal_SleepMode32kHz == sleepMode)
    {
        oscillatorBenchmark = gpHal_SleepMode32kHz_stable_benchmark;
    }
    else if (gpHal_SleepModeRC == sleepMode)
    {
        oscillatorBenchmark = gpHal_SleepModeRC_stable_benchmark;
    }
    else
    {
        /* No benchmark avaiable for this sleep mode */
        GP_ASSERT_DEV_INT(0);
    }

    return oscillatorBenchmark;
}

/* Apply sleep counter calibration factor and realign sleep counter to symbol counter.
 *
 * This function is called by the backup handler before entering sleep mode.
 * Must be called before halting the timer reference.
 */
void gpHal_RealignTimebase(void)
{
    UInt32 calibrationFactor;
    UInt8 sleepMode;

    // Determine which calibration factor to use (depends on sleep mode).
    sleepMode = gpHal_GetSleepMode();

    calibrationFactor = gpHalES_GetCalibrationFactor(sleepMode);

    // Write new calibration factor.
    GP_WB_WRITE_ES_CALIBRATION_FACTOR(calibrationFactor);
    GP_WB_WRITE_ES_PHASE_COMPENSATION(0);

    //trigger realignment calibration
    GP_WB_ES_APPLY_EXTERNAL_REALIGNMENT_CALIBRATION();
    //wait until not busy
    while (GP_WB_READ_ES_APPLY_EXTERNAL_REALIGNMENT_CALIBRATION_BUSY());

    //wait until symbol counter updated since last calibration
    while (!GP_WB_READ_ES_SYMBOL_COUNTER_UPDATED_SINCE_LAST_CALIBRATION());

    //there is a 1 es clock cycle gap between assertion of symbol counter updated since last calibration and deassertion of reconversion busy
    HAL_WAIT_US(1);

    //wait until reconversion done
    while (GP_WB_READ_ES_RECONVERSION_BUSY());
}


//-------------------------------------------------------------------------------------------------------
//  CALIBRATION/BENCHMARK FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_ApplyCalibration(Int32 phaseAdjustment,UInt32 frequency)
{
    //int->uint and >>=1 this works because phase_comp is really a 31-bit property, not 32.
    UInt32 phase_comp = phaseAdjustment;

    DISABLE_GP_GLOBAL_INT();

    //Update calibration factor + phase
    GP_WB_WRITE_ES_CALIBRATION_FACTOR(frequency);
    GP_WB_WRITE_ES_PHASE_COMPENSATION(phase_comp);

    //Trigger write of new factor
    gpHalES_ApplySimpleCalibration();

    ENABLE_GP_GLOBAL_INT();
}

void gpHalES_ApplySimpleCalibration(void)
{
    GP_ASSERT_SYSTEM(!GP_WB_READ_ES_DISABLE_CONVERSION());
    GP_WB_ES_APPLY_EXTERNAL_SIMPLE_CALIBRATION();
    __DSB();
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_APPLY_EXTERNAL_SIMPLE_CALIBRATION_BUSY(), GP_HAL_DEFAULT_TIMEOUT);
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_ES_SYMBOL_COUNTER_UPDATED_SINCE_LAST_CALIBRATION(), GP_HAL_DEFAULT_TIMEOUT);
}

void gpHalEs_PrepareOscillatorBenchmark(gpHal_SleepMode_t mode)
{
    UInt8 power;
    UInt16 nofLpTicks;
    Bool keepAwake = false;
    Bool isFirstMeasurement;

    if(gpHal_SleepModeRC == mode)
    {
        /*
         * Keep the chip awake for measurements if
         *  - Queue is not full
         *  - First measurement of a wake cycle
         *  - Benchmark data required for temperature correction
         * Chip can go to sleep if it is subsequent measurements of wake cycle
         */
        isFirstMeasurement = gpHal_CalibrationGetFirstAfterWakeup();
        keepAwake = (isFirstMeasurement) || (!gpHalEs_IsQueueFull);
    } else {
        keepAwake = true;
    }

    gpHalES_EnableOscillatorBenchmark(true, keepAwake);

    COMPILE_TIME_ASSERT(gpHal_SleepModeRC == gpHal_SleepModeEvent); //Warning optimization
    if(gpHal_SleepModeRC == mode)
    {
        nofLpTicks = gpHalEs_RCCalibProp.NoOfLPTicks;
        GP_WB_WRITE_ES_OSCILLATOR_BENCHMARK_CLK_SRC(GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_RC32K_CLOCK);
        GP_WB_WRITE_ES_OSCILLATOR_BENCHMARK_LP_TICKS_THRESHOLD(nofLpTicks - 1);
        /* GP_LOG_SYSTEM_PRINTF("<-- RC bm trigger nof_lp_ticks %d keepAwake %d",0, nofLpTicks, keepAwake); */
        /* Log when the RC calibration is triggered */
        GP_STAT_SAMPLE_TIME();
   }
    else if(gpHal_SleepMode32kHz == mode)
    {
        GP_WB_WRITE_ES_OSCILLATOR_BENCHMARK_CLK_SRC(GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_XT32K_CLOCK);
        power = GP_HAL_ES_BENCHMARK_POWER_XTAL;
        //Only needed for more accurate measurements
        // Value of 0 => 1 edge, i.e. 0 full clock cycles., 1 => 2 edges, i.e. 1 full clock cycle!
        GP_WB_WRITE_ES_OSCILLATOR_BENCHMARK_LP_TICKS_THRESHOLD((0x1<<power)-1);
    }
    else
    {
        // Unsupported benchmark mode
        GP_ASSERT_DEV_INT(false);
        return;
    }

    gpHalEs_StartOscillatorBenchmark();
}

void gpHalES_setupSleepClockRecalibration(gpHal_SleepMode_t mode)
{
    Bool RCTaskInvalid = (calTaskHandleRC == GP_HAL_CALIBRATION_INVALID_TASK_HANDLE);
#ifdef GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
    Bool Xt32kTaskInvalid = (calTaskHandleXt32K == GP_HAL_CALIBRATION_INVALID_TASK_HANDLE);
#endif
    gpHal_CalibrationTask_t calTask;
    MEMSET(&calTask, 0, sizeof(gpHal_CalibrationTask_t));

    /* create calibration task to re-trigger calibrations based on the sleep mode */
    if ((mode == gpHal_SleepModeRC && RCTaskInvalid)
#ifdef GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
        || (mode == gpHal_SleepMode32kHz && Xt32kTaskInvalid)
#endif
        )
    {
        if (mode == gpHal_SleepModeRC)
        {
            calTask.flags = GP_HAL_CALIBRATION_FLAG_PERIODIC | GP_HAL_CALIBRATION_FLAG_CALIBRATE_ON_CHIP_WAKEUP;
            calTask.calibrationPeriod = GP_HAL_RC_BENCHMARK_CALIBRATION_PERIOD_US;
            calTaskHandleRC = gpHal_CalibrationCreateTask(&calTask, gpHalEs_RCCalibrationHandler);
            GP_ASSERT_DEV_EXT(calTaskHandleRC != GP_HAL_CALIBRATION_INVALID_TASK_HANDLE);
        }
#ifdef GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
        else
        {
            calTask.flags = GP_HAL_CALIBRATION_FLAG_PERIODIC;
            calTask.calibrationPeriod = (GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_PERIOD_MS * 1000);
            calTaskHandleXt32K = gpHal_CalibrationCreateTask(&calTask, gpHalEs_Xt32KCalibrationHandler);
            GP_ASSERT_DEV_EXT(calTaskHandleXt32K != GP_HAL_CALIBRATION_INVALID_TASK_HANDLE);
        }
#endif
    }
}

void gpHalES_StartBackground_Benchmark(gpHal_SleepMode_t sleepMode, UInt32 tnow)
{
    GP_LOG_PRINTF("TriggerBackground mode %x", 0, sleepMode);

    GP_ASSERT_DEV_INT(!GP_ES_IS_OSCILLATOR_BENCHMARK_RUNNING());

    gpHal_background_benchmark_mode = sleepMode;
    if(sleepMode == gpHal_SleepModeRC)
    {
        /*
         * TODO - LastBenchmark time stamp is not necessary in the new approach
         * Check if this can be deleted.
         */
        gpHalES_SleepModeRC_TsLastBenchmark = tnow;
        GP_ASSERT_SYSTEM(gpHalEs_RCCalibProp.is_initialized == true);
    }

    gpHalEs_PrepareOscillatorBenchmark(sleepMode);
}

gpHalES_BenchmarkResult_t gpHalES_TriggerOscillatorBenchmark(void)
{
    gpHal_SleepMode_t sleepMode;
    UInt32 tNow;
    if (GP_ES_IS_OSCILLATOR_BENCHMARK_RUNNING())
    {
        /* This calibration was started, no other calibration may interrupt it */
        return gpHalES_BenchmarkResult_Busy;
    }

    // Current time in us
    tNow = GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();

    sleepMode = gpHal_GetSleepMode();


    if(sleepMode == gpHal_SleepModeRC)
    {
        //gpHal_SleepMode32kHz_stable_benchmark stays valid forever, averaging filters just gets fed with new sample
        gpHalES_StartBackground_Benchmark(gpHal_SleepModeRC, tNow);
    }

#ifdef GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
    if (sleepMode == gpHal_SleepMode32kHz)
    {
        GP_ASSERT_DEV_EXT(GP_BSP_32KHZ_CRYSTAL_AVAILABLE());
        {
            gpHal_OscillatorBenchmark_3Phase_Init(GP_DIVERSITY_GPHAL_32KHZ_REDO_CALIBRATION_MIN_BENCHMARKS);
            gpHalES_StartBackground_Benchmark(gpHal_SleepMode32kHz, tNow);
        }
    }
#else // GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
#endif // GP_DIVERSITY_GPHAL_32KHZ_ENABLE_RECALIBRATION
    return gpHalES_BenchmarkResult_Success;
}

UInt32 gpHalES_ReadOscillatorBenchmark(void)
{
    UInt32 benchmark;

    //Read-out benchmark result
    GP_WB_WRITE_ES_LOCK_OSCILLATOR_BENCHMARK(1);
    __DSB();
    benchmark = GP_WB_READ_ES_OSCILLATOR_BENCHMARK_COUNTER();
    GP_WB_WRITE_ES_LOCK_OSCILLATOR_BENCHMARK(0);

    return benchmark;
}

#ifndef GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB
#define GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB
 /* This is the old behaviour where the stack falls back to RC mode automatically */
void gpHal_cb32kHzCalibrationDone(gpHal_SleepClockMeasurementStatus_t status, UInt32 mse)
{
    if (gpHal_SleepClockMeasurementStatusNotStable == status &&
        gpHal_SleepMode32kHz == gpHal_GetSleepMode() /* i.e. don't change if we're in 16Mhz mode! */)
    {
        GP_LOG_SYSTEM_PRINTF("xt32k unstable: switching to RC sleep mode", 0);
        gpHal_SetSleepMode(gpHal_SleepModeRC);
    }
}
#endif // GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB

void gpHal_OscillatorBenchmark_3Phase_Complete(gpHal_OscillatorBenchmark_Status_t status)
{
    gpHalES_EnableOscillatorBenchmark(false, false);
    switch (status)
    {
        case gpHal_OscillatorBenchmark_Result_Stable:
            gpHalEs_32kHz_benchmark_status = gpHal_SleepClockMeasurementStatusStable;
            gpHal_SleepMode32kHz_stable_benchmark = gpHal_OscillatorBenchmark_MSE_GetStableValue();
            break;
        case gpHal_OscillatorBenchmark_Result_Unstable:
            gpHalEs_32kHz_benchmark_status = gpHal_SleepClockMeasurementStatusStable;
            gpHal_SleepMode32kHz_stable_benchmark = gpHal_OscillatorBenchmark_3Phase_GetAvg();
            break;
        case gpHal_OscillatorBenchmark_Result_Broken:
        default:
            GP_LOG_SYSTEM_PRINTF("HW error: 32kHz crystal benchmark timeout!",0);
            gpHalEs_32kHz_benchmark_status = gpHal_SleepClockMeasurementStatusNotStable;
            gpHal_SleepMode32kHz_stable_benchmark = GPHAL_ES_BENCHMARK_COUNTER_INVALID;
    }

#if defined(GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB)
    UInt32 mse = gpHal_OscillatorBenchmark_3Phase_GetMSE();
    gpHal_cb32kHzCalibrationDone(gpHalEs_32kHz_benchmark_status, mse);
#endif // GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB

    if (GP_BSP_32KHZ_CRYSTAL_AVAILABLE())
    {
        // Stop forcing 32 kHz xtal power.
        // Power to xtal will only be enabled when sleep mode is set to gpHal_SleepMode32kHz.
        GP_WB_WRITE_PMUD_CLK_XT32K_PUP_FORCE(0);
    }
    //Triggering initial RC calibration after initial 32kHz XT calibration
    gpHalEs_TriggerOscillatorBenchmarkRcMeasurement();
}


void gpHalES_OscillatorBenchmarkDone_Handler(void)
{
    UInt32 benchmark;
    gpHal_OscillatorBenchmark_Status_t status;


    if(GP_WB_READ_ES_OSCILLATOR_BENCHMARK_RESULT_VALID())
    {
        benchmark = gpHalES_ReadOscillatorBenchmark();
    }
    else
    {
        benchmark = GPHAL_ES_BENCHMARK_COUNTER_INVALID;
    }

    GP_LOG_PRINTF("bm_mode %u bm:%lu",0, gpHal_background_benchmark_mode, benchmark);

    if (gpHal_SleepMode32kHz == gpHal_background_benchmark_mode)
    {
        status = gpHal_OscillatorBenchmark_3Phase_Add(benchmark);

        if (gpHal_OscillatorBenchmark_Result_NeedMoreSamples != status)
        {
            gpHal_OscillatorBenchmark_3Phase_Complete(status);
        }
        else
        {
            gpHalEs_StartOscillatorBenchmark();
        }
    }
    else if (gpHal_SleepModeRC == gpHal_background_benchmark_mode)
    {
        /*GP_LOG_SYSTEM_PRINTF("--> bm interrupt bm_val = %ld bm_mode = %d",0, benchmark, gpHal_background_benchmark_mode);*/
        /* Log when the RC calibration interrupt is handled */
        GP_STAT_SAMPLE_TIME();
        if (benchmark != GPHAL_ES_BENCHMARK_COUNTER_INVALID)
        {
            gpHal_SleepModeRC_stable_benchmark = gpHalES_FilterRCBenchmarkMeasurement(benchmark);

            if(!gpHalEs_IsQueueFull)
            {
                // trigger subsequent benchmarks till the queue is full
                gpHalEs_StartOscillatorBenchmark();

           }
           else
           {
                // disable the benchmark circuit, will be re-enabled for
                // re-calibration (from calibration framework)
                gpHalES_EnableOscillatorBenchmark(false, false);
           }
        }
        else
        {
            // re-trigger calibrations if benchmark result is invalid
            gpHalEs_StartOscillatorBenchmark();
        }
    }
    else
    {
       GP_ASSERT_DEV_INT(false);
    }

}

UInt32 gpHalES_FilterRCBenchmarkMeasurement(UInt32 benchmark)
{
    UInt32 filtered;
    UInt8 measurementHistory = gpHalEs_RCCalibProp.QueueDepth;
    UInt32 benchmarkAvg;

    GP_ASSERT_SYSTEM(measurementHistory != 0);
    gpHal_EsBenchmarkMeasurements[gpHal_EsBenchmarkCurrentIndex] = benchmark;
    // circular increment of index
    gpHal_EsBenchmarkCurrentIndex = (gpHal_EsBenchmarkCurrentIndex + 1) % measurementHistory;

    benchmarkAvg = gpHalES_GetBenchmarkAverage(measurementHistory);
    filtered = benchmarkAvg;

    return filtered;
}

void gpHal_SetDefaultStartupSymbolTimes(gpHal_SleepMode_t mode)
{
    UInt32 startupSymbolTime = 0;
    UInt32 longStartupSymbolTime = 0;

    switch(mode)
    {
        case gpHal_SleepModeRC:
        case gpHal_SleepMode32kHz:
        {
            startupSymbolTime = GP_HAL_ES_DEFAULT_STARTUP_SYMBOL_TIME_OTHER_US;
            longStartupSymbolTime = GP_HAL_ES_DEFAULT_LONG_STARTUP_SYMBOL_TIME_OTHER_US;
            break;
        }
        case gpHal_SleepMode16MHz:
        {
            startupSymbolTime = GP_HAL_ES_DEFAULT_STARTUP_SYMBOL_TIME_16MHZ_US;
            longStartupSymbolTime = GP_HAL_ES_DEFAULT_LONG_STARTUP_SYMBOL_TIME_16MHZ_US;
            break;
        }
        default:
        {
            // Unknown sleep mode
            GP_ASSERT_DEV_INT(false);
            break;
        }
    }

    gpHalES_SetStartupSymbolTimes(startupSymbolTime, longStartupSymbolTime);
}

void gpHal_ResetHandler_SetSymbolStartupTime(void)
{
    UInt32 startupSymbolTime;
    gpHal_SleepMode_t sleepMode;

    sleepMode = gpHal_GetSleepMode();

    if(sleepMode != gpHal_SleepMode16MHz)
    {
        // Decrement symbol startup time (will be incremented again when an event too late was detected)
        startupSymbolTime = GP_WB_READ_ES_STARTUP_SYMBOL_TIME() - GP_HAL_ES_STARTUP_SYMBOL_TIME_DECREMENT_DEFAULT_US;

        if (startupSymbolTime < GP_HAL_ES_MIN_STARTUP_SYMBOL_TIME_OTHER_US)
        {
            startupSymbolTime = GP_HAL_ES_MIN_STARTUP_SYMBOL_TIME_OTHER_US;
        }

        gpHalES_SetStartupSymbolTimes(startupSymbolTime, startupSymbolTime + GP_HAL_ES_BACKUP_DURATION_OTHER_US);
    }
}

//-------------------------------------------------------------------------------------------------------
//  ENABLE / DISABLE CALLBACK FUNCTIONS
//-------------------------------------------------------------------------------------------------------

//ES interrupts
#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
void gpHal_EnableAbsoluteEventCallbackInterrupt(UInt8 eventNbr, Bool enable)
{
    //Enable interrupt mask for the selected absolute interrupt
    DISABLE_GP_GLOBAL_INT();
    GP_WB_MWRITE_U8(GP_WB_INT_CTRL_MASK_ES_EVENT_INTERRUPTS_ADDRESS + eventNbr/8, (1 << (eventNbr % 8)), ((enable ? 1:0) << (eventNbr % 8)));
    ENABLE_GP_GLOBAL_INT();
}
#endif

//-------------------------------------------------------------------------------------------------------
//  EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_GetTime(UInt32* pTime)
{
    GP_ASSERT_SYSTEM(GP_WB_READ_ES_INIT_TIME_REFERENCE_BUSY() == 0);
    DISABLE_GP_GLOBAL_INT();
    *pTime = GP_WB_READ_ES_AUTO_SAMPLED_SYMBOL_COUNTER();
    ENABLE_GP_GLOBAL_INT();
}

//-------------------------------------------------------------------------------------------------------
//  ABSOLUTE EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------

#ifdef GP_COMP_GPHAL_ES_ABS_EVENT
gpHal_AbsoluteEventId_t gpHal_GetAbsoluteEvent(void)
{
    UIntLoop i;

    for (i = 0; i < GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS; i++)
    {
        if (!BIT_TST(gpHal_ES_AbsoluteEventsInUse, i)) {
            BIT_SET(gpHal_ES_AbsoluteEventsInUse, i);
            return i;
        }
    }

    /* Assert instead of returning */
    GP_ASSERT_DEV_EXT(false);

    return GPHAL_ES_ABSOLUTE_EVENT_ID_INVALID;
}

void gpHal_FreeAbsoluteEvent(gpHal_AbsoluteEventId_t eventId)
{
    GP_ASSERT_DEV_EXT(eventId < GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS);
    GP_ASSERT_DEV_EXT(BIT_TST(gpHal_ES_AbsoluteEventsInUse, eventId));
    BIT_CLR(gpHal_ES_AbsoluteEventsInUse, eventId);
}

void gpHal_ScheduleAbsoluteEvent(gpHal_AbsoluteEventDescriptor_t* pAbsoluteEventDescriptor, gpHal_AbsoluteEventId_t eventNbr)
{
    GP_ASSERT_DEV_EXT( GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > eventNbr );

    DISABLE_GP_GLOBAL_INT();

    setEventValid(eventNbr, false);     // Mark event Invalid
    reEvaluateEvents();                 // ReevaluateEvents + Wait untill done

    GP_ES_WRITE_EVENT_DESCRIPTOR( pAbsoluteEventDescriptor , eventNbr );  //Write Absolute Event

    setEventValid(eventNbr, true);  // MarkEventValid
    reEvaluateEvents();             // ReevaluateEvents + Wait until done

    ENABLE_GP_GLOBAL_INT();
}

gpHal_EventState_t gpHal_UnscheduleAbsoluteEvent(gpHal_AbsoluteEventId_t eventNbr)
{
    gpHal_EventState_t state;

    GP_ASSERT_DEV_EXT( GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > eventNbr );

    DISABLE_GP_GLOBAL_INT();
    state = (gpHal_EventState_t) (GP_ES_GET_EVENT_STATE( GP_HAL_READ_REG(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET + offsetof(gpHal_AbsoluteEventDescriptor_t,control)) ));

    if(state != gpHal_EventStateDone)
    {
        setEventValid(eventNbr, false);     // Mark event Invalid
        reEvaluateEvents();                 // ReevaluateEvents + Wait untill done
    }

    ENABLE_GP_GLOBAL_INT();

    return state;
}

void gpHal_RefreshAbsoluteEvent(gpHal_AbsoluteEventId_t eventNbr, UInt32 absTime, UInt8 control)
{
    GP_ASSERT_DEV_EXT( GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > eventNbr );

    DISABLE_GP_GLOBAL_INT();
    setEventValid(eventNbr, false);

    //be sure the event is not being processed (read) by a event reevaluation triggered by another event
    //reading a single event takes 3.25us (KIWI4), so with a delay of 5us we are safe

    HAL_WAIT_US(5);

    //Update Execution Time
    GP_HAL_WRITE_REGS32(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET + offsetof(gpHal_AbsoluteEventDescriptor_t,exTime), &absTime);
    //Update Control field
    GP_HAL_WRITE_REG(GP_MM_RAM_EVENT_START + (eventNbr) * GP_MM_RAM_EVENT_OFFSET + offsetof(gpHal_AbsoluteEventDescriptor_t,control), control);

    setEventValid(eventNbr, true);
    ENABLE_GP_GLOBAL_INT();

    GP_WB_ES_REEVALUATE_EVENTS();    //refresh
}
#endif //GP_COMP_GPHAL_ES_ABS_EVENT

//-------------------------------------------------------------------------------------------------------
//  RELATIVE AND EXTERNAL EVENT SCHEDULER FUNCTIONS
//-------------------------------------------------------------------------------------------------------
#ifdef GP_COMP_GPHAL_ES_REL_EVENT
void gpHal_ScheduleImmediateEvent(gpHal_EventType_t type)
{
    DISABLE_GP_GLOBAL_INT();

    GP_WB_WRITE_ES_RELATIVE_EVENT_TYPE_TO_BE_EXECUTED(type); //Write Relative event
    GP_WB_WRITE_ES_RELATIVE_EVENT_TIME_DELAY(0);             //Schedule immediate
    GP_WB_ES_RELATIVE_EVENT_EXECUTE();                       //Execute Event

    ENABLE_GP_GLOBAL_INT();
}
#endif //GP_COMP_GPHAL_ES_REL_EVENT

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
#ifdef GP_COMP_GPHAL_ES_EXT_EVENT_WKUP
void gpHal_ScheduleExternalEvent(gpHal_ExternalEventDescriptor_t* pExternalEventDescriptor)
{
    DISABLE_GP_GLOBAL_INT();

    // Set Type
    GP_WB_WRITE_ES_EXTERNAL_EVENT_TYPE_TO_BE_EXECUTED(pExternalEventDescriptor->type);
    // Mark Valid
    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(true);

    // No wakeup since chip will always be awake

    ENABLE_GP_GLOBAL_INT();
}
#elif defined(GP_BSP_KX_WRITE_WAKEUP_PIN_MODE) || defined(GP_BSP_KX_WAKEUP_EDGE)
#error "WAKEUP_PIN defined but not GP_COMP_GPHAL_ES_EXT_EVENT_WKUP!"
#endif // GP_COMP_GPHAL_ES_EXT_EVENT_WKUP

#endif //GP_COMP_GPHAL_ES_EXT_EVENT

void gpHal_cbEventTooLate(void)
{
    UInt32 startupSymbolTime;
    gpHal_SleepMode_t sleepMode;

    sleepMode = gpHal_GetSleepMode();

    if(sleepMode != gpHal_SleepMode16MHz)
    {

       startupSymbolTime = GP_WB_READ_ES_STARTUP_SYMBOL_TIME();
       startupSymbolTime += GP_HAL_ES_STARTUP_SYMBOL_TIME_INCREMENT_WHEN_TOO_LATE_US;

       if (startupSymbolTime > GP_HAL_ES_MAX_STARTUP_SYMBOL_TIME_OTHER_US)
       {
          startupSymbolTime = GP_HAL_ES_MAX_STARTUP_SYMBOL_TIME_OTHER_US;
       }

       gpHalES_SetStartupSymbolTimes(startupSymbolTime, startupSymbolTime + GP_HAL_ES_BACKUP_DURATION_OTHER_US);
    }
}

gpHal_SleepClockMeasurementStatus_t gpHal_GetMeasuredSleepClockFrequency(gpHal_SleepMode_t mode, UInt32* frequencymHz)
{
    gpHal_SleepClockMeasurementStatus_t status = gpHal_SleepClockMeasurementStatusNotStarted;
    UInt32 benchmarkValue;
    UInt16 noOfLpTicks;
    GP_ASSERT_DEV_INT(mode <= gpHal_SleepMode16MHz);

    switch(mode)
    {
        case gpHal_SleepModeRC:
        {
            if(gpHal_SleepModeRC_stable_benchmark == GPHAL_ES_BENCHMARK_COUNTER_INVALID)
            {
                if(gpHalES_SleepModeRC_TsLastBenchmark != 0)
                {
                    // Invalid counter, but TS > 0, measurement started, but not finished
                    status = gpHal_SleepClockMeasurementStatusPending;
                }
            }
            else
            {
                // Valid counter, consider stable
                status = gpHal_SleepClockMeasurementStatusStable;
            }

            benchmarkValue = gpHal_SleepModeRC_stable_benchmark;
            noOfLpTicks = gpHalEs_RCCalibProp.NoOfLPTicks;
            break;
        }
        case gpHal_SleepMode32kHz:
        {
            if(GP_BSP_32KHZ_CRYSTAL_AVAILABLE())
            {
                if(gpHal_SleepMode32kHz_stable_benchmark != GPHAL_ES_BENCHMARK_COUNTER_INVALID)
                {
                    // Benchmark counter not invalid, which means the consecutive measurements were stable
                    status = gpHal_SleepClockMeasurementStatusStable;
                }
                else if(gpHalEs_32kHzMeasurementAborted)
                {
                    // Measurement was aborted. Clock not stable or broken
                    status = gpHal_SleepClockMeasurementStatusNotStable;
                }
                else
                {
                    // Measurement is still running
                    status = gpHal_SleepClockMeasurementStatusPending;
                }
            }

            benchmarkValue = gpHal_SleepMode32kHz_stable_benchmark;
            noOfLpTicks = (1 << GP_HAL_ES_BENCHMARK_POWER_XTAL);
            break;
        }
        default:
        {
            // Mode 16 MHz or unknown: no measurements performed
            noOfLpTicks = 1;
            benchmarkValue = 0;
            break;
        }
    }

    if(status == gpHal_SleepClockMeasurementStatusStable)
    {
        // Calculate frequency
        *frequencymHz = (GPHAL_ES_FREQUENCY_32MHZ * noOfLpTicks) / benchmarkValue;
        *frequencymHz *= 1000;
        // Add the milliHz resolution
        *frequencymHz += ((GPHAL_ES_FREQUENCY_32MHZ * noOfLpTicks) % benchmarkValue)*1000/benchmarkValue;
    }

    return status;
}

gpHal_SleepClockMeasurementStatus_t gpHalEs_Get32kHzBenchmarkStatus(void)
{
    return gpHalEs_32kHz_benchmark_status;
}

gpHal_SleepMode_t gpHalES_GetBackgroundBenchmarkMode(void)
{
    return gpHal_background_benchmark_mode;
}


UInt32 gpHal_GetExternalEventPins(void)
{
    return GP_WB_READ_ES_LAST_EXTERNAL_PIN_EVENT_STATE();
}

void gpHal_ResetTime(void)
{
    DISABLE_GP_GLOBAL_INT();

    GP_WB_ES_APPLY_EXTERNAL_ZERO_CALIBRATION();
    //Wait for application of calibration to finish
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_ES_APPLY_EXTERNAL_ZERO_CALIBRATION_BUSY(), GP_HAL_DEFAULT_TIMEOUT);
    //Wait for symbol time to be calculated
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_ES_SYMBOL_COUNTER_UPDATED_SINCE_LAST_CALIBRATION(), GP_HAL_DEFAULT_TIMEOUT);

    ENABLE_GP_GLOBAL_INT();
}

