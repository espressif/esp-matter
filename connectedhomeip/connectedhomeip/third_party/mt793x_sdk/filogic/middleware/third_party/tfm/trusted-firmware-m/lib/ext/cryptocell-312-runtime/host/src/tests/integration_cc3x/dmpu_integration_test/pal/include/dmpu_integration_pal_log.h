/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _DMPU_INTEGRATION_PAL_LOG_H_
#define _DMPU_INTEGRATION_PAL_LOG_H_

/************************************************************
 *
 * macros
 *
 ************************************************************/
/** Print the function name, line and debug level */
#define DMPUIT_PRINT_FUNC_AND_LEVEL(_lvl) \
                do { \
                    char _buff[50]; \
                    snprintf(_buff, 49, "%-.25s:%d", __func__, __LINE__); \
                    printf("%-35.35s: %-5.5s: ", _buff, _lvl); \
                }while(0)

/** print a line */
#define DMPUIT_PRINT(format, ...)  \
                printf(format, ##__VA_ARGS__)

/** print the test result */
#define DMPUIT_TEST_RESULT(_testName) \
                do {\
                    printf("Test: %-50.50s %-10.10s\n", _testName, rc == DMPUIT_ERROR__OK ? "Passed" : "Failed"); \
                } while(0)

/** print the test start line */
#define DMPUIT_TEST_START(_testName) \
                do {\
                    /* printf("Test: %-50.50s %-10.10s\n", _testName, "Started...."); */ \
                } while(0)

/** print an error message */
#define DMPUIT_PRINT_ERROR(format, ...)  \
                do {\
                    DMPUIT_PRINT_FUNC_AND_LEVEL("error"); \
                    DMPUIT_PRINT(format, ##__VA_ARGS__); \
                } while(0)

/** print an debug message */
#if defined(TEST_DEBUG)
#define DMPUIT_PRINT_DBG(format, ...)   \
                do {\
                    DMPUIT_PRINT_FUNC_AND_LEVEL("debug"); \
                    DMPUIT_PRINT(format, ##__VA_ARGS__); \
                } while(0)
#else
#define DMPUIT_PRINT_DBG(format, ...) \
                do {\
                } while(0)
#endif

#endif //_DMPU_INTEGRATION_PAL_LOG_H_
