/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

#ifndef _GPHAL_KX_ES_H_
#define _GPHAL_KX_ES_H_

#include "gpHal_ES.h"
#include "gpHal_OscillatorBenchmark.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#if defined(GP_COMP_GPHAL_ES_ABS_EVENT)
#ifndef GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS
#error GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS not defined
#endif
#endif //defined(GP_COMP_GPHAL_ES_ABS_EVENT)

/** @brief Default value for the calibration of the RC oscilator */
#define GPHAL_ES_RC_SLEEP_DEFAULT_CALIB                   (0x3E800000)

/** @brief Event will trigger scheduled Packet queue.*/
#define gpHal_EventTypeTXPacket     GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE0
/** @brief Event will trigger RX on.*/
#define gpHal_EventTypeRXOn         GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON0
/** @brief Event will trigger RX off.*/
#define gpHal_EventTypeRXOff        GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF0
/** @brief Event will trigger a dummy event, it only generates an interrupt.*/
#define gpHal_EventTypeDummy        GP_WB_ENUM_EVENT_TYPE_DUMMY

/** @brief The GP chip will be in event mode during standby and can only be woken up by an external event.
    The event timer is stopped and can only be restarted by a full reset of the GP chip.*/
#define gpHal_SleepModeEvent            GP_WB_ENUM_STANDBY_MODE_RC_MODE

/** @brief The GP chip will run a RC timer during standby.*/
#define gpHal_SleepModeRC               GP_WB_ENUM_STANDBY_MODE_RC_MODE

/** @brief The GP chip will run a 32kHz clock during standby.*/
#define gpHal_SleepMode32kHz            GP_WB_ENUM_STANDBY_MODE_XTAL_32KHZ_MODE

/** @brief The GP chip will run a 16MHz clock during standby.*/
#define gpHal_SleepMode16MHz            GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE

/* typedef UInt8 gpHal_SleepMode_t; */

#define GPHAL_ES_CALIBRATION_FACTOR_USE_PREVIOUS    0xFFFFFFFF

#define GPHAL_ES_BENCHMARK_COUNTER_INVALID          0xFFFFFFFF

#define GP_HAL_ES_BACKUP_DURATION_OTHER_US          (UInt16)500

#define GP_HAL_ES_RC64K_TC_CORRECTION_MIN           (-4)
#define GP_HAL_ES_RC64K_TC_CORRECTION_MAX           (3)
#define GP_HAL_ES_RC64K_TC_CORRECTION_INVALID       (0x7F)

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*pin and edge are provided by BSP
 */
#define GPHAL_ES_ENABLE_EXT_WAKEUP()                    \
     GP_BSP_KX_WRITE_WAKEUP_PIN_MODE(GP_BSP_KX_WAKEUP_EDGE)
#define GPHAL_ES_DISABLE_EXT_WAKEUP()                   \
     GP_BSP_KX_WRITE_WAKEUP_PIN_MODE(GP_WB_ENUM_WAKEUP_PIN_MODE_NO_EDGE)

#define GPHAL_ES_EVENT_NR_TO_START_OFFSET(eventNr)          ((gpHal_Address_t)(GP_MM_RAM_EVENT_START + (eventNr) * GP_MM_RAM_EVENT_OFFSET))

#define GP_ES_IS_OSCILLATOR_BENCHMARK_RUNNING()    GP_WB_READ_INT_CTRL_MASK_ES_OSCILLATOR_BENCHMARK_DONE_INTERRUPT()
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/** @name gpHalES_BenchmarkResult_t*/
//@{
/** @brief The function returned successful. */
#define gpHalES_BenchmarkResult_Success             0x0
/** @brief Already running. */
#define gpHalES_BenchmarkResult_Busy                0x1
/** @typedef gpHalES_BenchmarkResult_t
 *  @brief The gpHalES_BenchmarkResult_t type defines the result of gpHalES_TriggerOscillatorBenchmark.
*/
typedef UInt8 gpHalES_BenchmarkResult_t;
//@}

#ifdef __cplusplus
extern "C" {
#endif

void gpHal_StartInitialBenchmarkMeasurements(void);
void gpHalES_ApplySimpleCalibration(void);
GP_API gpHalES_BenchmarkResult_t gpHalES_TriggerOscillatorBenchmark(void);
void gpHal_SetDefaultStartupSymbolTimes(gpHal_SleepMode_t mode);
void gpHal_RealignTimebase(void);
void gpHal_cbHalSleepUc(void);
void gpHal_ResetHandler_SetSymbolStartupTime(void);

/** @brief Initializes the parameters for RC sleep clock calibration.
 *         This API can be used to tweak the benchmark time, number of measurements that are
 *         averaged to find a stable benchmark value and also the frequency in which measurements
 *         are done.
 *  @param  nofRCSleepClockTicks     The number of ticks of the RC sleep clock over which the
 *                      benchmark measurement has to be done.
 *  @param  QueueDepth  Set the depth of queue in which the benchmark values are stored.
 *                      A stable benchmark value is calculated by averaging values in queue.
 *  @param  EnableMultipleMeasurments Set this to true to do multiple measurements in a wake cycle.
 *                      If set to zero, once the queue is full, only one measurement is done in
 *                      each wake cycle.
 */
void gphalES_RCSleepClockCalibrationInit (
        UInt16 nofRCSleepClockTicks,
        UInt8 QueueDepth,
        Bool EnableMultipleMeasurments);


/** @brief Helper function to get the parameters set for RC sleep clock calibration
 *
 */
void gphalES_getRCSleepClockCalibrationParameters (
        UInt16 *nofRCSleepClockTicks,
        UInt8 *QueueDepth,
        Bool *EnableMultipleMeasurments);


/**
 * @brief Gets the most recent oscillator benchmark value for given sleep mode
 * @param mode The sleepmode enumerated in gpHal_SleepMode.
*/
UInt32 gpHalES_GetOscillatorBenchmark(gpHal_SleepMode_t sleepMode);

/** @brief Return bitmask of the GPIO pins that triggered the most recent external pin event. */
UInt32 gpHal_GetExternalEventPins(void);

/** @brief Set start-up symbol time and long start-up symbol time */
void gpHalES_SetStartupSymbolTimes(UInt32 normalStartupTime, UInt32 longStartupTime);
/** @brief Create calibration tasks to do recalibration of sleep clock */
void gpHalES_setupSleepClockRecalibration(gpHal_SleepMode_t sleepMode);

#if defined(GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB)
/** @brief Always called at the end of the 32kHz calibration.
 *  @param status enumeration that indicates the status of the 32kHz calibration
 *  @param mse The mean square error. Indicates how noisy the 32kHz clock signal is. Smaller is better.
 */
GP_API void gpHal_cb32kHzCalibrationDone(gpHal_SleepClockMeasurementStatus_t status, UInt32 mse);
#endif // GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB

/*  @brief Reset internal state maintained for a sleep mode */
void gpHalES_resetBenchmarkMeasurement(gpHal_SleepMode_t mode);

/* @brief Enable/Disable oscillator benchmark circuit. keepAwake determines if oscillator
benchmark circuit keeps chip awake to complete a measurement */
void gpHalES_EnableOscillatorBenchmark(Bool enable, Bool keepAwake);
/* @brief Trigger a new oscillator benchmark measurement */
void gpHalEs_StartOscillatorBenchmark(void);
/* @brief Get the sleep clock for which background benchmark measurements are going on*/
gpHal_SleepMode_t gpHalES_GetBackgroundBenchmarkMode(void);
#ifdef __cplusplus
}
#endif
#endif  /* _GPHAL_KX_ES_H_ */

