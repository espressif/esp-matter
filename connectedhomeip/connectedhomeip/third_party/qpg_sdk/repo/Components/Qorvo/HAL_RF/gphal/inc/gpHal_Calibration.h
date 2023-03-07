/*
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpHal_Calibration.h
 * Support periodic recalibration of hardware.
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

#ifndef _GPHAL_KX_CALIBRATION_H_
#define _GPHAL_KX_CALIBRATION_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief Maximum number of periodic calibration tasks. */
#define GP_HAL_CALIBRATION_MAX_TASKS                            6 /* xtal32M_trimcap, RC_bmrk, xtal32k_bmrk, vddRamTune, RC_tempCorr, FLL */

/** @brief Check if calibrations are needed every 10ms. */
#define GP_HAL_CALIBRATION_CHECK_INTERVAL_US                    10000

/** @brief Check every 1s if temperature based calibrations are needed */
#define GP_HAL_CALIBRATION_TEMPERATURE_BASED_CHECK_INTERVAL_US  1000000

/** @brief Invalid handle for calibration task */
#define GP_HAL_CALIBRATION_INVALID_TASK_HANDLE                  0xFF

/** @brief Peripherial timer used by calibration task */
#define HAL_CALIBRATION_TIMER HAL_TIMER_2

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
/** @name gpHal_CalibrationFlags_t */
//@{
/** @brief Enable periodic calibration. */
#define GP_HAL_CALIBRATION_FLAG_PERIODIC                        0x01
/** @brief Enable calibration on temperature change.  */
#define GP_HAL_CALIBRATION_FLAG_TEMPERATURE_SENSITIVE           0x02
/** @brief Enable calibration after waking up from sleep. */
#define GP_HAL_CALIBRATION_FLAG_CALIBRATE_ON_CHIP_WAKEUP        0x04

/** @brief Bitmask of flags for the calibration task. */
typedef UInt8 gpHal_CalibrationFlags_t;
//@}

/** @brief Calibration task descriptor*/
typedef struct {
    /** @brief Bitmask of GP_HAL_CALIBRATION_FLAG_xxx. */
    gpHal_CalibrationFlags_t flags;
    /** @brief Maximum temperature deviation in Q8_8 format (with GP_HAL_CALIBRATION_FLAG_TEMPERATURE_SENSITIVE). */
    UInt16 temperatureThreshold;
    /** @brief Maximum time between calibrations in microseconds (with GP_HAL_CALIBRATION_FLAG_PERIODIC). */
    UInt32 calibrationPeriod;
    /** @brief Temperature at which calibration is triggered */
    Q8_8 temperature;
    /** @brief May be used to pass additional data to the calibration task. */
    void* pUserData;
} gpHal_CalibrationTask_t;

/** @brief Callback invoked by calibration manager to perform a specific calibration task. */
typedef void (*gpHal_cbCalibrationHandler_t)(const gpHal_CalibrationTask_t* pTask);

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Initialize calibration manager.
 */
void gpHal_InitCalibration(void);

/** @brief Check calibration tasks and invoke pending calibration handlers.
 *
 *  This function is invoked regularly while the chip is in acitve mode,
 *  as well as any time the chip wakes up from sleep.
 */
void gpHal_CalibrationHandleTasks(void);

/** @brief Set flag which indicates first calibration after chip wakes up
 * usually set by the reset handler and cleared after first run of calibration tasks
 * after wakeup.
 */
void gpHal_CalibrationSetFirstAfterWakeup(Bool enable);

/** @brief Get flag which indicates first calibration after chip wakes up
 */
Bool gpHal_CalibrationGetFirstAfterWakeup(void);

/*
 * Following APIs are useful for the user of this framework
 */

/** @brief Create a new periodic calibration task.
 *
 *  @param  pTask      Pointer to calibration task descriptor.
 *                     The descriptor will be copied to internal storage in the calibration manager;
 *                     the pointer will not be used again after this function returns.
 *  @param  cbHandler  Callback to be invoked to perform the calibration.
 *
 *  @return            calibration task id which can be used to set/clear pending on wakeup
 */
UInt8 gpHal_CalibrationCreateTask(const gpHal_CalibrationTask_t* pTask, gpHal_cbCalibrationHandler_t cbHandler);

/** @brief Indicate to the calibration routine that calibration task
 * needs to be run next time chip wakes up from sleep
 */
void gpHal_SetCalibrationPendingOnWakeup(UInt8 calTaskId);

/** @brief Clear calibration pending flag for a calibration task  The process that
called the gpHal_SetCalibrationPendingOnWakeup() is expected to do this as well*/
void gpHal_ClearCalibrationPendingOnWakeup(UInt8 calTaskId);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPHAL_KX_CALIBRATION_H_
