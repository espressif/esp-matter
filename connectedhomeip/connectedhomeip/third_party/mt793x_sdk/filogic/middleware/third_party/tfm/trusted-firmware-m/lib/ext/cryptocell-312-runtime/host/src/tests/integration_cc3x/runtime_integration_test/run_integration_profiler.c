/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <limits.h>

/************* Include Files ****************/

/* local */
#include "run_integration_test.h"
#include "run_integration_profiler.h"

char* runIt_perfTypeStr(runItPerfType_t type, char* pStr, uint32_t buffLen)
{
    (void) type;
    (void) pStr;
    (void) buffLen;
    return NULL;
}

runItPerfType_t runIt_perfTypeFromStr(const char* typeStr, const char* paramStr)
{
    (void) typeStr;
    (void) paramStr;
    return PERF_TYPE_TEST_NOT_SET;
}
/* END - DO NOT CHANGE */

/**
 * @brief   initialise performance test mechanism
 *
 * @param[in]
 * *
 * @return None
 */
void runIt_perfInit(void)
{
}

/**
 * @brief   terminates resources used for performance tests
 *
 * @param[in]
 * *
 * @return None
 */
void runIt_perfFin(void)
{
    // nothing to be done
}

/**
 * register api to DB
 *
 * @param func          address of function
 * @param name          name of function
 * @param param         param ti distinguish between flows
 */
void runIt_perfEntryInit(const char* name, const char* param, uint8_t isHw)
{
    (void) name;
    (void) param;
    (void) isHw;
}

/**
 * @brief   opens new entry in perf buffer to record new entry
 *
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return Returns a non-zero value in case of failure
 */
runItPerfData_t runIt_perfOpenNewEntry(runItPerfType_t entryType)
{
    (void) entryType;

    return PERF_TYPE_TEST_NOT_SET;
}

/**
 * @brief   closes entry in perf buffer previously opened by runIt_perfOpenNewEntry
 *
 * @param[in] idx -  index of the entry to be closed, the return value of runIt_perfOpenNewEntry
 * @param[in] entryType -  entry type (defined in cc_pal_perf.h) to be recorded in buffer
 *
 * @return Returns a non-zero value in case of failure
 */
void runIt_perfCloseEntry(runItPerfData_t idx, runItPerfType_t entryType)
{
    (void) idx;
    (void) entryType;
}

/**
 * @brief   dumps the performance buffer
 *
 * @param[in] None
 *
 * @return None
 */
void runIt_perfDump(void)
{
}

