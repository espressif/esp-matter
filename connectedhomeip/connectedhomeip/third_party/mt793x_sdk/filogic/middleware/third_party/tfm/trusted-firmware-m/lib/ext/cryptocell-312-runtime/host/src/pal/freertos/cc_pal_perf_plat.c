/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include "stdio.h"
#include "stdint.h"
#include "math.h"

#include "cc_pal_perf.h"
#include "cc_pal_perf_plat.h"
#include "dx_reg_base_host.h"
#include "dx_env.h"
#include "cc_hal.h"
#include "cc_pal_mem.h"

#include "FreeRTOS.h"

#define MICROSECONDS         1000000

/*!
 * Read CryptoCell memory-mapped-IO Env register.
 *
 * \param regOffset The offset of the Env register to read
 * \return uint32_t Return the value of the given register
 */
#define CC_PAL_PerfReadEnvRegister(regOffset)               \
        (*((volatile uint32_t *)(DX_BASE_ENV_REGS + (regOffset))))

/*!
 * Write CryptoCell memory-mapped-IO Env register.
 *
 * \param regOffset The offset of the Env register to write
 * \param val The value to write
 */
#define CC_PAL_PerfWriteEnvRegister(regOffset, val)         \
        (*((volatile uint32_t *)(DX_BASE_ENV_REGS + (regOffset))) = (val))

#define MAX(a,b)            (a) > (b) ? (a) : (b)
#define MIN(a,b)            (a) < (b) ? (a) : (b)

typedef struct PalPerfCounter_t {
    uint32_t numOfEntries;

    uint64_t totalCycles;
    uint64_t totalStdDev;

    CCPalPerfData_t max;
    CCPalPerfData_t min;

    CCPalPerfData_t prevCycles;
    uint32_t totalCounts;
}PalPerfCounter_t;

/**
 * An array of results per function. Each function updated the data.
 */
static PalPerfCounter_t resultsArr[PERF_TEST_TYPE_MAX];

static CCPalPerfData_t CC_PalGetCycles(void);
static CCPalPerfData_t CC_PalGetCycles(void)
{
    return CC_PAL_PerfReadEnvRegister(DX_ENV_COUNTER_RD_REG_OFFSET);
}


/**
 *
 * @param cycles
 * @return          microSeconds
 */
static uint32_t CC_PalGetMicroSec(CCPalPerfData_t cycles);
static uint32_t CC_PalGetMicroSec(CCPalPerfData_t cycles)
{
    return (uint32_t)(cycles * (uint64_t)MICROSECONDS / configCPU_CLOCK_HZ);
}


/**
 * @brief   initialize performance test mechanism
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalPerfInit(void)
{
    CCPalPerfType_t funcIndex;
    for (funcIndex = (CCPalPerfType_t)0; funcIndex < PERF_TEST_TYPE_MAX; ++funcIndex)
    {
        PalPerfCounter_t* pCyclesData = &resultsArr[funcIndex];

        pCyclesData->prevCycles = 0;
        pCyclesData->totalCounts = 0;
        pCyclesData->totalStdDev = 0;
        pCyclesData->totalCycles = 0;
        pCyclesData->max = 0;
        pCyclesData->min = (CCPalPerfData_t)-1;
    }
}


/**
 * @brief   terminates resources used for performance tests
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalPerfFin(void)
{
    // nothing to be done
}
/**
 * @brief   opens new entry in perf buffer to record new entry
 *
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return Returns a non-zero value in case of failure
 */
CCPalPerfData_t CC_PalPerfOpenNewEntry(CCPalPerfType_t entryType)
{
    PalPerfCounter_t* pCyclesData = &resultsArr[entryType];

    CCPalPerfData_t cycles = CC_PalGetCycles();

    pCyclesData->prevCycles = cycles;


    return (CCPalPerfData_t)entryType;
}


/**
 * @brief   closes entry in perf buffer previously opened by CC_PalPerfOpenNewEntry
 *
 * @param[in] idx -  index of the entry to be closed, the return value of CC_PalPerfOpenNewEntry
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return Returns a non-zero value in case of failure
 */
void CC_PalPerfCloseEntry(CCPalPerfData_t idx, CCPalPerfType_t entryType)
{
    PalPerfCounter_t* pCyclesData = &resultsArr[entryType];

    CCPalPerfData_t cycles = CC_PalGetCycles();

    cycles = cycles - pCyclesData->prevCycles;

    pCyclesData->totalCycles += cycles;
    pCyclesData->totalStdDev += cycles * cycles;
    pCyclesData->totalCounts += 1;

    pCyclesData->max = MAX(pCyclesData->max, cycles);
    pCyclesData->min = MIN(pCyclesData->min, cycles);
}


/**
 * @brief   dumps the performance buffer
 *
 * @param[in] None
 *
 * @return None
 */
void CC_PalPerfDump(void)
{
#define CC_PALPERFDUMP_LENGTH 256
#define CC_PALPERFDUMP_SEPERATOR "----------------------------------------------------------------" \
                "----------------------------------------------------------------" \
                "----------------------------------------------------------------" \
                "----------------------------------------------------------------"

    CCPalPerfType_t i;
    char buff[CC_PALPERFDUMP_LENGTH];

    uint32_t length = snprintf(buff, CC_PALPERFDUMP_LENGTH, "| %-30.30s | %-10.10s | %-15.15s | %-15.15s | %-15.15s | %-15.15s | %-15.15s | %-15.15s |",
           "Function", "countTests", "TimePerCall,uS", "CallsPerSec", "ClocksPerCall", "ClocksStDev", "ClocksMin", "ClocksMax");

    // Print header
    printf("%*.*s\n%s\n%*.*s\n", length, length, CC_PALPERFDUMP_SEPERATOR, buff, length, length, CC_PALPERFDUMP_SEPERATOR);

    for (i = (CCPalPerfType_t)0; i < PERF_TEST_TYPE_MAX; i++)
    {
        CCPalPerfData_t totalCycles = 0;
        uint32_t totalCounts = 0;
        CCPalPerfData_t avgCycles;
        CCPalPerfData_t min;
        CCPalPerfData_t max;
        uint64_t totalStdDev;
        uint64_t stdDev;


        totalCycles = resultsArr[i].totalCycles;
        totalCounts = resultsArr[i].totalCounts;
        totalStdDev = resultsArr[i].totalStdDev;
        min = resultsArr[i].min;
        max = resultsArr[i].max;

        if (totalCounts == 0)
        {
            continue;
        }

        avgCycles = (0 != totalCounts) ? (CCPalPerfData_t)(totalCycles / totalCounts) : 0;
        stdDev = sqrt(totalStdDev - totalCounts * avgCycles * avgCycles) / (totalCounts - 1);


        printf("| %-30.30s | %10u | %15lu | %15lu | %15lu | %15lu | %15lu | %15lu |\n",
               CC_PalPerfTypeStr(i, buff, CC_PALPERFDUMP_LENGTH), (uint32_t)totalCounts, (uint32_t)CC_PalGetMicroSec(avgCycles),
               (uint32_t)(MICROSECONDS / CC_PalGetMicroSec(avgCycles)),
               (uint32_t)avgCycles, (uint32_t)stdDev, (uint32_t)min, (uint32_t)max);
    }

    printf("%*.*s\n", length, length, CC_PALPERFDUMP_SEPERATOR);

}

