// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdbool.h>
#include <setjmp.h>
#include <wchar.h>

#include "ctest.h"

#if defined _MSC_VER && !defined(WINCE)
#include "windows.h"
#endif

#ifdef VLD_OPT_REPORT_TO_STDOUT
#include "vld.h"
#endif

const TEST_FUNCTION_DATA* g_CurrentTestFunction;
jmp_buf g_ExceptionJump;
size_t RunTests(const TEST_FUNCTION_DATA* testListHead, const char* testSuiteName)
{
#ifdef VLD_OPT_REPORT_TO_STDOUT
    VLD_UINT initial_leak_count = VLDGetLeaksCount();
#endif
    size_t totalTestCount = 0;
    size_t failedTestCount = 0;
    const TEST_FUNCTION_DATA* currentTestFunction = (const TEST_FUNCTION_DATA*)testListHead->NextTestFunctionData;
    const TEST_FUNCTION_DATA* testSuiteInitialize = NULL;
    const TEST_FUNCTION_DATA* testSuiteCleanup = NULL;
    const TEST_FUNCTION_DATA* testFunctionInitialize = NULL;
    const TEST_FUNCTION_DATA* testFunctionCleanup = NULL;
    int testSuiteInitializeFailed = 0;

#if defined _MSC_VER && !defined(WINCE)
    _set_abort_behavior(_CALL_REPORTFAULT, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

    // Set output mode to handle virtual terminal sequences
    HANDLE std_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    bool SetConsoleMode_succeeded = false;
    DWORD console_mode_initial = 0;
    if (std_out_handle == INVALID_HANDLE_VALUE)
    {
        (void)printf("Error getting console handle, no coloring available. GetLastError()=%" PRIx32 "\n", GetLastError());
    }
    else
    {
        if (!GetConsoleMode(std_out_handle, &console_mode_initial))
        {
            (void)printf("Error getting console mode, no coloring available. GetLastError()=%" PRIx32 "\n", GetLastError());
        }
        else
        {
            if (!SetConsoleMode(std_out_handle, console_mode_initial | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            {
                (void)printf("Error setting console mode, no coloring available. GetLastError()=%" PRIx32 "\n", GetLastError());
            }
            else
            {
                SetConsoleMode_succeeded = true;
            }
        }
    }

#endif

    g_CurrentTestFunction = NULL;

    (void)printf(" === Executing test suite %s ===\n", testSuiteName);

    while (currentTestFunction->TestFunction != NULL)
    {
        if (currentTestFunction->FunctionType == CTEST_TEST_FUNCTION_INITIALIZE)
        {
            testFunctionInitialize = currentTestFunction;
        }

        if (currentTestFunction->FunctionType == CTEST_TEST_FUNCTION_CLEANUP)
        {
            testFunctionCleanup = currentTestFunction;
        }

        if (currentTestFunction->FunctionType == CTEST_TEST_SUITE_INITIALIZE)
        {
            testSuiteInitialize = currentTestFunction;
        }

        if (currentTestFunction->FunctionType == CTEST_TEST_SUITE_CLEANUP)
        {
            testSuiteCleanup = currentTestFunction;
        }

        currentTestFunction = (TEST_FUNCTION_DATA*)currentTestFunction->NextTestFunctionData;
    }

    if (testSuiteInitialize != NULL)
    {
        if (setjmp(g_ExceptionJump) == 0)
        {
            testSuiteInitialize->TestFunction();
        }
        else
        {
            testSuiteInitializeFailed = 1;
            (void)printf("TEST_SUITE_INITIALIZE failed - suite ending\n");
        }
    }

    if (testSuiteInitializeFailed == 1)
    {
        /* print results */
        (void)printf(CTEST_ANSI_COLOR_RED "0 tests ran, ALL failed, NONE succeeded.\n" CTEST_ANSI_COLOR_RESET);
        failedTestCount = 1;
    }
    else
    {
        unsigned int is_test_runner_ok = 1;

        currentTestFunction = (const TEST_FUNCTION_DATA*)testListHead->NextTestFunctionData;
        while (currentTestFunction->TestFunction != NULL)
        {
            if (currentTestFunction->FunctionType == CTEST_TEST_FUNCTION)
            {
                if (is_test_runner_ok == 1)
                {
                    int testFunctionInitializeFailed = 0;

                    if (testFunctionInitialize != NULL)
                    {
                        if (setjmp(g_ExceptionJump) == 0)
                        {
                            testFunctionInitialize->TestFunction();
                        }
                        else
                        {
                            testFunctionInitializeFailed = 1;
                            (void)printf(CTEST_ANSI_COLOR_RED "TEST_FUNCTION_INITIALIZE failed - next TEST_FUNCTION will fail\n" CTEST_ANSI_COLOR_RESET);
                        }
                    }


                    if (testFunctionInitializeFailed)
                    {
                        *currentTestFunction->TestResult = TEST_FAILED;
                        (void)printf(CTEST_ANSI_COLOR_YELLOW "Not executing test %s ...\n" CTEST_ANSI_COLOR_RESET, currentTestFunction->TestFunctionName);
                    }
                    else
                    {
                        (void)printf("Executing test %s ...\n", currentTestFunction->TestFunctionName);

                        g_CurrentTestFunction = currentTestFunction;

                        if (setjmp(g_ExceptionJump) == 0)
                        {
                            currentTestFunction->TestFunction();
                        }
                        else
                        {
                            /*can only get here if there was a longjmp called while executing currentTestFunction->TestFunction();*/
                            /*we don't do anything*/
                        }
                        g_CurrentTestFunction = NULL;/*g_CurrentTestFunction is limited to actually executing a TEST_FUNCTION, otherwise it should be NULL*/

                        /*in the case when the cleanup can assert... have to prepare the long jump*/
                        if (setjmp(g_ExceptionJump) == 0)
                        {
                            if (testFunctionCleanup != NULL)
                            {
                                testFunctionCleanup->TestFunction();
                            }
                        }
                        else
                        {
                            /* this is a fatal error, if we got a fail in cleanup we can't do much */
                            *currentTestFunction->TestResult = TEST_FAILED;
                            is_test_runner_ok = 0;
                        }
                    }
                }
                else
                {
                    *currentTestFunction->TestResult = TEST_NOT_EXECUTED;
                }

                if (*currentTestFunction->TestResult == TEST_FAILED)
                {
                    failedTestCount++;
                    (void)printf(CTEST_ANSI_COLOR_RED "Test %s result = !!! FAILED !!!\n" CTEST_ANSI_COLOR_RESET, currentTestFunction->TestFunctionName);
                }
                else if (*currentTestFunction->TestResult == TEST_NOT_EXECUTED)
                {
                    failedTestCount++;
                    (void)printf(CTEST_ANSI_COLOR_YELLOW "Test %s ... SKIPPED due to a failure in test function cleanup. \n" CTEST_ANSI_COLOR_RESET, currentTestFunction->TestFunctionName);
                }
                else
                {
                    (void)printf(CTEST_ANSI_COLOR_GREEN "Test %s result = Succeeded.\n" CTEST_ANSI_COLOR_RESET, currentTestFunction->TestFunctionName);
                }
                totalTestCount++;
            }

            currentTestFunction = (TEST_FUNCTION_DATA*)currentTestFunction->NextTestFunctionData;
        }

        if (setjmp(g_ExceptionJump) == 0)
        {
            if (testSuiteCleanup != NULL)
            {
                testSuiteCleanup->TestFunction();
            }
        }
        else
        {
            /*only get here when testSuiteCleanup did asserted*/
            /*should fail the tests*/
            (void)printf(CTEST_ANSI_COLOR_RED "TEST_SUITE_CLEANUP failed - all tests are marked as failed\n" CTEST_ANSI_COLOR_RESET);
            failedTestCount = (totalTestCount > 0) ? totalTestCount : SIZE_MAX;
        }

        /* print results */
        (void)printf("%s%d tests ran, %d failed, %d succeeded.\n" CTEST_ANSI_COLOR_RESET, (failedTestCount > 0) ? (CTEST_ANSI_COLOR_RED) : (CTEST_ANSI_COLOR_GREEN), (int)totalTestCount, (int)failedTestCount, (int)(totalTestCount - failedTestCount));
    }

#if defined _MSC_VER && !defined(WINCE)
    if (std_out_handle != INVALID_HANDLE_VALUE)
    {
        if (SetConsoleMode_succeeded)
        {
            /*revert console to initial state*/
            if (!SetConsoleMode(std_out_handle, console_mode_initial))
            {
                (void)printf("Error resetting console mode to initial value of %" PRIx32 ". GetLastError()=%" PRIx32 "\n", console_mode_initial, GetLastError());
            }
        }
    }
#endif

#ifdef VLD_OPT_REPORT_TO_STDOUT
    failedTestCount = (failedTestCount > 0) ? failedTestCount : (size_t)(-(int)(VLDGetLeaksCount() - initial_leak_count));
#endif

    return failedTestCount;
}

void int_ToString(char* string, size_t bufferSize, int val)
{
    (void)snprintf(string, bufferSize, "%d", val);
}

void char_ToString(char* string, size_t bufferSize, char val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void short_ToString(char* string, size_t bufferSize, short val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void long_ToString(char* string, size_t bufferSize, long val)
{
    (void)snprintf(string, bufferSize, "%ld", (long)val);
}

void size_t_ToString(char* string, size_t bufferSize, size_t val)
{
    (void)snprintf(string, bufferSize, "%d", (int)val);
}

void float_ToString(char* string, size_t bufferSize, float val)
{
    (void)snprintf(string, bufferSize, "%.02f", val);
}

void double_ToString(char* string, size_t bufferSize, double val)
{
    (void)snprintf(string, bufferSize, "%.02f", val);
}

void long_double_ToString(char* string, size_t bufferSize, long_double val)
{
    (void)snprintf(string, bufferSize, "%.02Lf", val);
}

void char_ptr_ToString(char* string, size_t bufferSize, const char* val)
{
    (void)snprintf(string, bufferSize, "%s", val);
}

void wchar_ptr_ToString(char* string, size_t bufferSize, const wchar_t* val)
{
    (void)snprintf(string, bufferSize, "%ls", val);
}

void void_ptr_ToString(char* string, size_t bufferSize, const void* val)
{
    (void)snprintf(string, bufferSize, "%p", val);
}

void unsigned_long_ToString(char* string, size_t bufferSize, unsigned long val)
{
    (void)snprintf(string, bufferSize, "%lu", val);
}

int int_Compare(int left, int right)
{
    return left != right;
}

int char_Compare(char left, char right)
{
    return left != right;
}

int short_Compare(short left, short right)
{
    return left != right;
}

int long_Compare(long left, long right)
{
    return left != right;
}

int unsigned_long_Compare(unsigned long left, unsigned long right)
{
    return left != right;
}

int size_t_Compare(size_t left, size_t right)
{
    return left != right;
}

int float_Compare(float left, float right)
{
    return left != right;
}

int double_Compare(double left, double right)
{
    return left != right;
}

int long_double_Compare(long double left, long double right)
{
    return left != right;
}

int char_ptr_Compare(const char* left, const char* right)
{
    if ((left == NULL) &&
        (right == NULL))
    {
        return 0;
    }
    else if (left == NULL)
    {
        return -1;
    }
    else if (right == NULL)
    {
        return 1;
    }
    else
    {
        return strcmp(left, right);
    }
}

int wchar_ptr_Compare(const wchar_t* left, const wchar_t* right)
{
    if ((left == NULL) &&
        (right == NULL))
    {
        return 0;
    }
    else if (left == NULL)
    {
        return -1;
    }
    else if (right == NULL)
    {
        return 1;
    }
    else
    {
        return wcscmp(left, right);
    }
}

int void_ptr_Compare(const void* left, const void* right)
{
    return left != right;
}

#if defined CTEST_USE_STDINT
void uint8_t_ToString(char* string, size_t bufferSize, uint8_t val)
{
    (void)snprintf(string, bufferSize, "%"PRIu8, val);
}

void int8_t_ToString(char* string, size_t bufferSize, int8_t val)
{
    (void)snprintf(string, bufferSize, "%"PRId8, val);
}

void uint16_t_ToString(char* string, size_t bufferSize, uint16_t val)
{
    (void)snprintf(string, bufferSize, "%"PRIu16, val);
}

void int16_t_ToString(char* string, size_t bufferSize, int16_t val)
{
    (void)snprintf(string, bufferSize, "%"PRId16, val);
}

void uint32_t_ToString(char* string, size_t bufferSize, uint32_t val)
{
    (void)snprintf(string, bufferSize, "%"PRIu32, val);
}

void int32_t_ToString(char* string, size_t bufferSize, int32_t val)
{
    (void)snprintf(string, bufferSize, "%"PRId32, val);
}

void uint64_t_ToString(char* string, size_t bufferSize, uint64_t val)
{
    (void)snprintf(string, bufferSize, "%"PRIu64, val);
}

void int64_t_ToString(char* string, size_t bufferSize, int64_t val)
{
    (void)snprintf(string, bufferSize, "%"PRId64, val);
}

int uint8_t_Compare(uint8_t left, uint8_t right)
{
    return left != right;
}

int int8_t_Compare(int8_t left, int8_t right)
{
    return left != right;
}

int uint16_t_Compare(uint16_t left, uint16_t right)
{
    return left != right;
}

int int16_t_Compare(int16_t left, int16_t right)
{
    return left != right;
}

int uint32_t_Compare(uint32_t left, uint32_t right)
{
    return left != right;
}

int int32_t_Compare(int32_t left, int32_t right)
{
    return left != right;
}

int uint64_t_Compare(uint64_t left, uint64_t right)
{
    return left != right;
}

int int64_t_Compare(int64_t left, int64_t right)
{
    return left != right;
}

#endif

static char* ctest_vsprintf_char(const char* format, va_list va)
{
    char* result;
    int neededSize = vsnprintf(NULL, 0, format, va);
    if (neededSize < 0)
    {
        (void)printf("failure in vsnprintf\n");
        result = NULL;
    }
    else
    {
        result = malloc(neededSize + 1);
        if (result == NULL)
        {
            (void)printf("failure in malloc\n");
            /*return as is*/
        }
        else
        {
            if (vsnprintf(result, neededSize + 1, format, va) != neededSize)
            {
                (void)printf("inconsistent vsnprintf behavior\n");
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

/*returns a char* that is as if printed by printf*/
/*needs to be free'd after usage*/
char* ctest_sprintf_char(const char* format, ...)
{
    char* result;
    va_list va;
    va_start(va, format);
    result = ctest_vsprintf_char(format, va);
    va_end(va);
    return result;
}

void ctest_sprintf_free(char* string)
{
    free(string);
}

#if defined _MSC_VER
#elif defined __cpluplus
#elif defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
#else

int snprintf(char * s, size_t n, const char * format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = vsprintf(s, format, args);
    va_end(args);
    return result;
}

#endif
#endif

void do_jump(jmp_buf *exceptionJump, const volatile void* expected, const volatile void* actual)
{
    /*setting a breakpoint here allows catching the jump before it happens*/
    (void)expected;
    (void)actual;
#if CTEST_ABORT_ON_FAIL
    (void)exceptionJump;
    abort();
#else
    longjmp(*exceptionJump, 0xca1e4);
#endif
}
