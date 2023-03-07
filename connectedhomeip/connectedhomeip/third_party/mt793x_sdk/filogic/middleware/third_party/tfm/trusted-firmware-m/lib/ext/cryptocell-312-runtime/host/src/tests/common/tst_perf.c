/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>  //memset()
#include <limits.h>
#include <unistd.h>
#include "tst_perf.h"
#include "cc_pal_perf.h"

const char * tstPerfOp2Str[TST_PERF_OP_NUM] = {
    /* TST_PERF_OP_NULL,          */   "null",
    /* TST_PERF_OP_RSA_ENC_PRIM,  */   "RSA_ENC_PRIM",
    /* TST_PERF_OP_RSA_ENC_1_5 ,  */   "RSA_ENC_1_5 ",
    /* TST_PERF_OP_RSA_ENC_2_1 ,  */   "RSA_ENC_2_1 ",
    /* TST_PERF_OP_RSA_DEC_PRIM,  */   "RSA_DEC_PRIM",
    /* TST_PERF_OP_RSA_DEC_1_5 ,  */   "RSA_DEC_1_5 ",
    /* TST_PERF_OP_RSA_DEC_2_1 ,  */   "RSA_DEC_2_1 ",
    /* TST_PERF_OP_RSA_SIGN_1_5,  */   "RSA_SIGN_1_5",
    /* TST_PERF_OP_RSA_SIGN_2_1,  */   "RSA_SIGN_2_1",
    /* TST_PERF_OP_RSA_VER_1_5 ,  */   "RSA_VER_1_5 ",
    /* TST_PERF_OP_RSA_VER_2_1 ,  */   "RSA_VER_2_1 ",
    /* TST_PERF_OP_ECC_SIGN ,  */      "ECC_SIGN ",
    /* TST_PERF_OP_ECC_VER ,  */       "ECC_VER ",
};

/* Performance statistics for init/process/finalize/integrated */
PerfStats_t perfStats[TST_PERF_OP_NUM];


/*!
 * Accumulate given cycles or latency (in nSec) to the statistics
 *
 * \param val cycles/time in nsec
 * \param opType Operation type
 */
void StatsValAccum(CCPalPerfData_t val,  TST_PerfOpType_t opType)
{
    perfStats[opType].opCount++;
    perfStats[opType].totalVal += val;
    if ((val < perfStats[opType].minVal) ||
        (1 == perfStats[opType].opCount)) {
        perfStats[opType].minVal = val;
    }
    if (val > perfStats[opType].maxVal) {
        perfStats[opType].maxVal = val;
    }

}


/*!
 * Initialize statistics infrastructure
 */
void TST_PerfStatsCycleInit(void)
{
    memset(&perfStats, 0, sizeof(perfStats));
}

/*!
 * Initialize statistics infrastructure
 */
void TST_PerfStatsCycleFin(void)
{
    memset(&perfStats, 0, sizeof(perfStats));
}

/*!
 * Start code segment cycle measurement
 *
 * \param cycleCount Container for current time sample
 */
CCPalPerfData_t TST_PerfStatsCycleStart(void)
{
    CCPalPerfData_t tmp = 0;
    CC_PAL_PERF_OPEN_NEW_ENTRY(tmp, 0); // type has no meaning
    return tmp;
}

/*!
 * End code segment cycle measurement and append statistics to given operation
 *
 * \param startCycles Data initialize with TST_PerfStatsCycleStart
 * \param opType test operation type
 */
void TST_PerfStatsCycleEnd(CCPalPerfData_t startCycles, TST_PerfOpType_t opType)
{
    CCPalPerfData_t curCycles = 0;
    CCPalPerfData_t totalVal = 0;

    if ((opType >= TST_PERF_OP_NUM) ||
        (0 == startCycles)) {
        printf("%s Invalid opType=%d or startCycles %lld\n", __FUNCTION__, opType, startCycles);
        return;
    }
    CC_PAL_PERF_OPEN_NEW_ENTRY(curCycles, opType); // type has no meaning
    if (0 == curCycles) {
        printf("Test operations stats:\n");
        return;
    }
    if (curCycles < startCycles) {
        totalVal = (UINT64_MAX-startCycles)+curCycles;
    } else {
        totalVal = curCycles - startCycles;
    }
    StatsValAccum(totalVal,  opType);
}

/*!
 * Output to stdout the current statistics
 */
void TST_PerfStatsCycleReport(void)
{
    int i;
    CCPalPerfData_t avgCycles;

    CC_PAL_PERF_DUMP();
    printf("Test operations stats:\n");
    for (i = 1; i < TST_PERF_OP_NUM; i++) {
        if (perfStats[i].opCount != 0) {
            avgCycles = perfStats[i].totalVal / perfStats[i].opCount;
        } else {
            continue;
        }
        printf("%s: count=%d  cycles=[min,avg,max,sum] %lld %lld %lld %lld \n",
               tstPerfOp2Str[i], perfStats[i].opCount, perfStats[i].minVal, avgCycles, perfStats[i].maxVal, perfStats[i].totalVal );
    }
}

