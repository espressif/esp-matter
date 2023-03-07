/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /*
  CRYS tester performance statistics functions
  */

#ifndef __TST_PERF_H__
#define __TST_PERF_H__

#include <stdint.h>

typedef enum {
    TST_PERF_OP_NULL,
    TST_PERF_OP_RSA_ENC_PRIM,
    TST_PERF_OP_RSA_ENC_1_5,
    TST_PERF_OP_RSA_ENC_2_1,
    TST_PERF_OP_RSA_DEC_PRIM,
    TST_PERF_OP_RSA_DEC_1_5,
    TST_PERF_OP_RSA_DEC_2_1,
    TST_PERF_OP_RSA_SIGN_1_5,
    TST_PERF_OP_RSA_SIGN_2_1,
    TST_PERF_OP_RSA_VER_1_5,
    TST_PERF_OP_RSA_VER_2_1,
    TST_PERF_OP_ECC_SIGN,
    TST_PERF_OP_ECC_VER,
    TST_PERF_OP_NUM
} TST_PerfOpType_t;


typedef struct {
    uint32_t opCount; /* Number of operations measured */
    uint64_t totalVal; /* Accumulated cycles/uSec */
    uint64_t maxVal; /* Maximum cycles/usec */
    uint64_t minVal; /* Minimum cycles/usec */
} PerfStats_t;

#ifdef TST_PERF
/* Performance data */
#define TST_PERF_INIT  TST_PerfStatsCycleInit
#define TST_PERF_START(perfData)  perfData = TST_PerfStatsCycleStart()
#define TST_PERF_END(perfData, op) TST_PerfStatsCycleEnd(perfData, op)
#define TST_PERF_REPORT TST_PerfStatsCycleReport
#define TST_PERF_FIN  TST_PerfStatsCycleFin
void TST_PerfStatsCycleInit(void);
uint64_t TST_PerfStatsCycleStart(void);
void TST_PerfStatsCycleEnd(uint64_t startCycles, TST_PerfOpType_t opType);
void TST_PerfStatsCycleReport(void);
void TST_PerfStatsCycleFin(void);
#else
#define TST_PERF_INIT()
#define TST_PERF_START(perfData) (perfData = 0)
#define TST_PERF_END(perfData, op)
#define TST_PERF_REPORT()
#define TST_PERF_FIN()
#endif



#endif /*__TST_PERF_H__*/
