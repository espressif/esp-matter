/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_PAL_LOG_H_
#define _RUN_INTEGRATION_PAL_LOG_H_

#include <inttypes.h>

#include "test_pal_thread.h"
#include "dx_reg_base_host.h"
#include "dx_env.h"
#include "test_proj.h"

#define RUNIT_DEBUG_STACK 0

#if defined(DX_PLAT_ZYNQ7000)
#define CLOCK_HZ 666000000
#define RUNIT_DEBUG_EXEC_TIME 0
#elif defined(DX_PLAT_MPS2_PLUS)
#define CLOCK_HZ 25000000
#define RUNIT_DEBUG_EXEC_TIME 1
#endif

/************************************************************
 *
 * macros
 *
 ************************************************************/
extern uint8_t gRunItPrintEnable;

/** print a line */
#define RUNIT_PRINT(format, ...)  \
                printf(format, ##__VA_ARGS__)

#if RUNIT_DEBUG_EXEC_TIME
#define RUNIT_START_EXEC_TIME() \
    do { \
    } while(0)
#define RUNIT_PRINT_EXEC_TIME() \
    do { \
        printf(" %8"PRIu32"ms", runIt_getMicroSecPublic(runIt_getCyclesPublic() - startTime) / 1000) ;\
    } while(0)
#else
#define RUNIT_START_EXEC_TIME() do {} while(0)
#define RUNIT_PRINT_EXEC_TIME() do {} while(0)
#endif /* RUNIT_DEBUG_STACK */

#if RUNIT_DEBUG_STACK
#define RUNIT_PRINT_STACK()  \
                if (gRunItPrintEnable == 1) {\
                    char stack[10]; \
                    snprintf(stack, 9, "%d", gRunItStackSize - Test_PalThreadStackHighestWatermark(NULL)); \
                    RUNIT_PRINT(" %5.5s", stack); \
                } while(0)
#else
#define RUNIT_PRINT_STACK() do {} while(0)
#endif /* RUNIT_DEBUG_STACK */

/** Print the function name, line and debug level */
#define RUNIT_PRINT_HEADER() \
                if (gRunItPrintEnable == 1) { \
                    char dash[] = "------------------------------------------------------------------------------------------------------------------------------------"; \
                    RUNIT_PRINT("       %-44.44s %-75.75s %-10.10s", "test_name", "description", "status"); \
                    if (RUNIT_DEBUG_STACK) printf(" %-5.5s", "stack"); \
                    if (RUNIT_DEBUG_EXEC_TIME) printf(" %-10.10s", "exec time"); \
                    RUNIT_PRINT("\n      %-44.44s %-75.75s %-10.10s", dash, dash, dash); \
                    if (RUNIT_DEBUG_STACK) printf(" %-5.5s", dash); \
                    if (RUNIT_DEBUG_EXEC_TIME) printf(" %-10.10s", dash); \
                    RUNIT_PRINT("\n"); \
                }

#define RUNIT_PRINT_FUNC_AND_LEVEL(_lvl) \
                if (gRunItPrintEnable == 1) { \
                    char _buff[50]; \
                    snprintf(_buff, 49, "%-.25s:%d", __func__, __LINE__); \
                    RUNIT_PRINT("%-35.35s: %-5.5s: ", _buff, _lvl); \
                }

/** print the test result */
#define RUNIT_TEST_RESULT(_testName) \
                if (gRunItPrintEnable == 1) {\
                    char label[121] = {0}; \
                    snprintf(label, 120, "%s.........................................................................................................", _testName); \
                    RUNIT_PRINT("Test: %-120.120s %-10.10s", label, rc == RUNIT_ERROR__OK ? "Passed" : "Failed"); \
                    RUNIT_PRINT_STACK(); \
                    RUNIT_PRINT_EXEC_TIME(); \
                    RUNIT_PRINT("\n"); \
                }


#define RUNIT_SUB_TEST_RESULT_W_PARAMS(_testName, _format, ...) \
                if (gRunItPrintEnable == 1) {\
                    char buff[100]; \
                    snprintf(buff, 99, _format, ##__VA_ARGS__); \
                    RUNIT_PRINT("Test:     %-40.40s %-75.75s %-10.10s", _testName, buff, rc == RUNIT_ERROR__OK ? "Passed" : "Failed"); \
                    RUNIT_PRINT_STACK(); \
                    RUNIT_PRINT_EXEC_TIME(); \
                    RUNIT_PRINT("\n"); \
                }

#define RUNIT_SUB_TEST_RESULT(_testName) \
    RUNIT_SUB_TEST_RESULT_W_PARAMS(_testName, " ")

/** print the test start line */
#define RUNIT_TEST_START(_testName) \
                uint32_t startTime = runIt_getCyclesPublic(); \
                (void)startTime; \
                if (gRunItPrintEnable == 1) {\
                    RUNIT_PRINT("Test: %-120.120s %-10.10s", _testName, "                  "); \
                    RUNIT_PRINT_STACK(); \
                    RUNIT_START_EXEC_TIME(); \
                    RUNIT_PRINT("\n"); \
                }

/** print the sub test start line */
#define RUNIT_SUB_TEST_START(_testName) \
                uint32_t startTime = runIt_getCyclesPublic(); \
                (void)startTime; \
                do { \
                    RUNIT_START_EXEC_TIME(); \
                } while(0)


/** print an error message */
#define RUNIT_PRINT_ERROR(format, ...)  \
                do {\
                    RUNIT_PRINT_FUNC_AND_LEVEL("error"); \
                    RUNIT_PRINT(format, ##__VA_ARGS__); \
                } while(0)



/** print an debug message */
#if defined(TEST_DEBUG)
#define RUNIT_PRINT_DBG(format, ...)    \
                do {\
                    RUNIT_PRINT_FUNC_AND_LEVEL("debug"); \
                    RUNIT_PRINT(format, ##__VA_ARGS__); \
                } while(0)

/** print an error message */
#define RUNIT_PRINT_BUF(_buff, _size, _label)  \
                do {\
                    uint32_t i = 0, j = 0;\
                    for (i = 0; i * 16 + j < _size; i++, j = 0)\
                    { \
                        char tmpBuff[256] = {0}; \
                        for (j = 0; i * 16 + j < _size && j < 16; j++) \
                        { \
                            sprintf(tmpBuff + strlen(tmpBuff), "%02x", ((uint8_t*)_buff)[i * 16 + j]); \
                        } \
                        RUNIT_PRINT_FUNC_AND_LEVEL("debug"); \
                        RUNIT_PRINT("%-10.10s %04"PRIx32": %s\n", _label, i * 16, tmpBuff); \
                    } \
                } while(0)
#else
#define RUNIT_PRINT_DBG(format, ...) do {} while(0)
#define RUNIT_PRINT_BUF(_buff, _size, _label) do {} while(0)
#endif

static __inline uint32_t runIt_getCyclesPublic(void);
static __inline uint32_t runIt_getCyclesPublic(void)
{
    return *(volatile uint32_t *)(processMap.processTeeHwEnvBaseAddr + DX_ENV_COUNTER_RD_REG_OFFSET);
}


/**
*
* @param cycles
* @return          microSeconds
*/
static __inline uint32_t runIt_getMicroSecPublic(uint32_t cycles);
static __inline uint32_t runIt_getMicroSecPublic(uint32_t cycles)
{
    static const uint32_t MICROSECONDS = 1000000;

    return (uint32_t)((uint64_t)cycles * (uint64_t)MICROSECONDS / CLOCK_HZ);
}


#endif //_RUN_INTEGRATION_PAL_LOG_H_
