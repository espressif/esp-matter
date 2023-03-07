/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_PROFILER_H_
#define _RUN_INTEGRATION_PROFILER_H_

#include "stdio.h"
#include "string.h"

/* cc lib */
#include "cc_pal_types.h"
#include "cc_pal_perf.h"

/* pal */
#include "test_pal_mem.h"
#include "run_integration_pal_log.h"

#define PARAM_LEN                   30

typedef uint64_t runItPerfData_t;

typedef enum runItPerfType_t
{
    PERF_TYPE_TEST_NOT_SET,

    TEST_MAX = 256,

    RESERVE32 = 0x7FFFFFFF
} runItPerfType_t;

/**
 * @brief   initialise performance test mechanism
 *
 * @param[in]
 * *
 * @return None
 */
void runIt_perfInit(void);

/**
 * register api to DB
 *
 * @param func          address of function
 * @param name          name of function
 * @param param         param ti distinguish between flows
 */
void runIt_perfEntryInit(const char* name, const char* param, uint8_t isHw);

/**
 * @brief   opens new entry in perf buffer to record new entry
 *
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return A non-zero value in case of failure.
 */
runItPerfData_t runIt_perfOpenNewEntry(runItPerfType_t entryType);


/**
 * @brief   closes entry in perf buffer previously opened by runIt_perfOpenNewEntry
 *
 * @param[in] idx -  index of the entry to be closed, the return value of runIt_perfOpenNewEntry
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return A non-zero value in case of failure.
 */
void runIt_perfCloseEntry(runItPerfData_t idx, runItPerfType_t entryType);


/**
 * @brief   dumps the performance buffer
 *
 * @param[in] None
 *
 * @return None
 */
void runIt_perfDump(void);


/**
 * @brief   terminates resources used for performance tests
 *
 * @param[in]
 * *
 * @return None
 */
void runIt_perfFin(void);

/**
 *
 * @param typeStr
 * @return
 */
runItPerfType_t runIt_perfTypeFromStr(const char* typeStr, const char* paramStr);

/**
 *
 * @param func
 * @param paramStr
 * @return
 */
runItPerfType_t runIt_perfTypeFromFunc(void* func, const char* paramStr);

/**
 *
 * @param type
 * @param pStr
 * @param buffLen
 * @return
 */
char* runIt_perfTypeStr(runItPerfType_t type, char* pStr, uint32_t buffLen);

#endif //_RUN_INTEGRATION_PROFILER_H_
