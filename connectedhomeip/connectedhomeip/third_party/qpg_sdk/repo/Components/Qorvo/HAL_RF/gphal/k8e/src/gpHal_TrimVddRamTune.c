/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_TrimVddRamTune.c
 *
 * This file contains algorithm that changes vddram_tune based on temperature
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
 * $Header:
 * $Change:
 * $DateTime:
 *
 */

#include "hal.h"
#include "gpHal.h"
#include "global.h"
#include "gpHal_DEFS.h"
#include "gpSched.h"
#include "gpHal_Calibration.h"

#define GP_COMPONENT_ID                                  (GP_COMPONENT_ID_GPHAL)
#define GPHAL_VDD_RAM_TUNE_TEMPERATURE_THESHOLD          (60)
#define GPHAL_VDD_RAM_TUNE_DEFAULT_TUNE_VALUE            (-1)
#define GPHAL_VDD_RAM_TUNE_HIGH_TEMP_TUNE_VALUE          (3)
#define GPHAL_VDD_RAM_TUNE_TEMPERATURE_DELTA             (5)
#define GPHAL_VDD_RAM_TUNE_INVALID_VALUE                 (127)

static Int8 lastMeasuredVddRamTune = GPHAL_VDD_RAM_TUNE_INVALID_VALUE;

static void gpHal_VddRamTuneTrimAlgoRun(const gpHal_CalibrationTask_t* task)
{
    Int8 currentVddRamTune = GP_WB_READ_PMUD_PMU_VDDRAM_TUNE();
    Int8 temperature = Q_PRECISION_DECR8(task->temperature);
    /*
     * if temp > (threshold), vdd_ram_tune -> 3
     * if temp < (theshold),  vdd_ram_tune -> -1
     */
    GP_LOG_PRINTF("TrimVddRamTune: temperature %d vddram_before_corr %d",0, temperature, currentVddRamTune);
    if ((temperature  >= (GPHAL_VDD_RAM_TUNE_TEMPERATURE_THESHOLD))
        && (currentVddRamTune != GPHAL_VDD_RAM_TUNE_HIGH_TEMP_TUNE_VALUE))
    {
        GP_WB_WRITE_PMUD_PMU_VDDRAM_TUNE(GPHAL_VDD_RAM_TUNE_HIGH_TEMP_TUNE_VALUE);
        GP_LOG_SYSTEM_PRINTF("TrimVddRamTune: change vdd ram tune(=%d) to %d", 0, currentVddRamTune, GP_WB_READ_PMUD_PMU_VDDRAM_TUNE());
    }
    else if ((temperature <= (GPHAL_VDD_RAM_TUNE_TEMPERATURE_THESHOLD))
        && (currentVddRamTune != GPHAL_VDD_RAM_TUNE_DEFAULT_TUNE_VALUE))
    {
        GP_WB_WRITE_PMUD_PMU_VDDRAM_TUNE(GPHAL_VDD_RAM_TUNE_DEFAULT_TUNE_VALUE);
        GP_LOG_SYSTEM_PRINTF("TrimVddRamTune: change vdd ram tune(=%d) to %d", 0, currentVddRamTune, GP_WB_READ_PMUD_PMU_VDDRAM_TUNE());
    }

    lastMeasuredVddRamTune = GP_WB_READ_PMUD_PMU_VDDRAM_TUNE();
}

void gpHal_VddRamTuneTrimAlgoInit(void)
{
    lastMeasuredVddRamTune = GPHAL_VDD_RAM_TUNE_INVALID_VALUE;
    // create a calibration task to trigger at every 5C to check if temperature exceeded threshold
    // and correct vddRamTune
    gpHal_CalibrationTask_t calTask;
    UInt8 calTaskHandle;
    MEMSET(&calTask, 0, sizeof(gpHal_CalibrationTask_t));
    calTask.flags = GP_HAL_CALIBRATION_FLAG_TEMPERATURE_SENSITIVE;
    calTask.temperatureThreshold = UQ_PRECISION_INCR8(GPHAL_VDD_RAM_TUNE_TEMPERATURE_DELTA);
    calTaskHandle = gpHal_CalibrationCreateTask(&calTask, gpHal_VddRamTuneTrimAlgoRun);
    GP_ASSERT_DEV_EXT(calTaskHandle != GP_HAL_CALIBRATION_INVALID_TASK_HANDLE);
}

void gpHal_VddRamTuneTrimAlgoRestoreTuneValue(void)
{
    if (lastMeasuredVddRamTune != GPHAL_VDD_RAM_TUNE_INVALID_VALUE)
    {
        GP_LOG_PRINTF("TrimVddRamTune: restore vdd ram (=%d) to %d", 0,  GP_WB_READ_PMUD_PMU_VDDRAM_TUNE(), lastMeasuredVddRamTune);
        GP_WB_WRITE_PMUD_PMU_VDDRAM_TUNE(lastMeasuredVddRamTune);
    }
}
