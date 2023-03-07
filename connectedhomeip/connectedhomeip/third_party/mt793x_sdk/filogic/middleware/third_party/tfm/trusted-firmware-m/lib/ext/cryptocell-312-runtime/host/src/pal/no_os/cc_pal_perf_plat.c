/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_perf.h"
#include "cc_pal_perf_plat.h"
#include "dx_reg_base_host.h"
#include "dx_env.h"
#include "cc_hal.h"
#include "stdint.h"
#include "cc_pal_mem.h"

#define PERF_TRACE_CMPLT_BIT 0x80000000

#define MAX_PERF_COUNT_ENTRIES 256

#define PERF_LOG_ADDRESS  0x18001000


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


typedef struct {
    unsigned int          countType;
    unsigned int          countVal;
}PalPerfCounter_t;


uint8_t  *perfBuffAddr = (uint8_t *)PERF_LOG_ADDRESS;
static uint8_t  perfBuffNextIdx = 0;

#ifdef ARM_DSM
/**
 * @brief   initialize performance test mechanism
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalPerfInit(void)
{
    CC_PAL_PerfWriteEnvRegister(DX_ENV_COUNTER_CLR_REG_OFFSET/*0x118*/, 0);
    perfBuffNextIdx = 0;
    CC_PalMemSetZero(perfBuffAddr, MAX_PERF_COUNT_ENTRIES * sizeof(PalPerfCounter_t));
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
    PalPerfCounter_t *counterEntry;
    if (0 == perfBuffAddr){
        return -2;
    }

    if(perfBuffNextIdx >= MAX_PERF_COUNT_ENTRIES) {
        counterEntry = &(((PalPerfCounter_t *)perfBuffAddr)[MAX_PERF_COUNT_ENTRIES]);
        counterEntry->countVal++;
        return (-1);
    }
    counterEntry = &(((PalPerfCounter_t *)perfBuffAddr)[perfBuffNextIdx]);
    counterEntry->countType = (entryType & ~(PERF_TRACE_CMPLT_BIT));
    counterEntry->countVal = CC_PAL_PerfReadEnvRegister(DX_ENV_COUNTER_RD_REG_OFFSET/*0x11C*/);
    return perfBuffNextIdx++;
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
    PalPerfCounter_t *counterEntry;
    uint32_t regVal;

    if ((0 == perfBuffAddr) ||
        (idx >= MAX_PERF_COUNT_ENTRIES)) {
        return;
    }
    counterEntry = &(((PalPerfCounter_t *)perfBuffAddr)[idx]);
    if(counterEntry->countType != (entryType & ~(PERF_TRACE_CMPLT_BIT))) {
        counterEntry = &(((PalPerfCounter_t *)perfBuffAddr)[MAX_PERF_COUNT_ENTRIES]);
        counterEntry->countType++;
        return;
    }

    regVal = CC_PAL_PerfReadEnvRegister(DX_ENV_COUNTER_RD_REG_OFFSET/*0x11C*/);
    counterEntry->countType |= PERF_TRACE_CMPLT_BIT;
    counterEntry->countVal = (regVal-counterEntry->countVal);
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
    uint32_t i;
    PalPerfCounter_t *counterEntry;

    if (0 == perfBuffAddr){
        return;
    }

    counterEntry = &(((PalPerfCounter_t *)perfBuffAddr)[0]);
    for (i=0; (i< MAX_PERF_COUNT_ENTRIES) && (counterEntry->countType != 0); i++) {
        CC_PAL_PerfWriteEnvRegister(DX_ENV_DUMMY_ADDR_REG_OFFSET/*0x108*/, counterEntry->countType);
        CC_PAL_PerfWriteEnvRegister(DX_ENV_DUMMY_ADDR_REG_OFFSET/*0x108*/, counterEntry->countVal);
        counterEntry++;
    }
}


/**
 * @brief   DSM environment bug - sometimes very long write operation.
 *     to overcome this bug we added while to make sure write opeartion is completed
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalDsmWorkarround()
{
    CCPalPerfData_t perfIdx = 0;
    uint32_t tmpCount = 2000;

    perfIdx = CC_PalPerfOpenNewEntry(PERF_TEST_TYPE_CMPLT_SLEEP);
    while(tmpCount--);
    CC_PalPerfCloseEntry(perfIdx, PERF_TEST_TYPE_CMPLT_SLEEP);
}

#else ARM_DSM
/**
 * @brief   initialize performance test mechanism
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalPerfInit(void)
{
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
}
#endif // ARM_DSM
