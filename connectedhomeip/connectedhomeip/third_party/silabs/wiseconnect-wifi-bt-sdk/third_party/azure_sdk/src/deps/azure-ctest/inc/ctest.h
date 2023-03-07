// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CTEST_H
#define CTEST_H

#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <csetjmp>
#include <setjmp.h> /* Some compilers do not want to play by the standard, specifically ARM CC */
#include <stdio.h> /* Some compilers do not want to play by the standard, specifically ARM CC */
#define C_LINKAGE "C"
#define C_LINKAGE_PREFIX extern "C"
#else
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <wchar.h>
#define C_LINKAGE
#define C_LINKAGE_PREFIX
#endif

#if defined _MSC_VER
#include "ctest_windows.h"
#define CTEST_USE_STDINT
#if _MSC_VER < 1900 
/*https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/snprintf-snprintf-snprintf-l-snwprintf-snwprintf-l?view=vs-2019 says snprintf is C99 compliant since VS 2015*/
/*https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019 says VS 2015 is 1900*/
/*so for all the "old" VSes use _snprintf*/
#define snprintf _snprintf
#endif
#elif defined __cplusplus
#define CTEST_USE_STDINT
#elif defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L))
#define CTEST_USE_STDINT
#else
C_LINKAGE_PREFIX int snprintf(char * s, size_t n, const char * format, ...);
#endif
#endif

#if defined _MSC_VER && _MSC_VER <= 1700
#pragma warning(disable: 4054 4127 4510 4512 4610) /* MSC 1500 (VS2008) incorrectly fires this */
#endif

#if defined CTEST_USE_STDINT
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_COLORING
#define CTEST_ANSI_COLOR_RED     "\x1b[31m"
#define CTEST_ANSI_COLOR_GREEN   "\x1b[32m"
#define CTEST_ANSI_COLOR_YELLOW  "\x1b[33m"
#define CTEST_ANSI_COLOR_RESET   "\x1b[0m"
#else
#define CTEST_ANSI_COLOR_RED     ""
#define CTEST_ANSI_COLOR_GREEN   ""
#define CTEST_ANSI_COLOR_YELLOW  ""
#define CTEST_ANSI_COLOR_RESET   ""
#endif

typedef void(*TEST_FUNC)(void);

#define CTEST_FUNCTION_TYPE_VALUES \
    CTEST_BEGIN_SUITE, \
    CTEST_END_SUITE, \
    CTEST_TEST_FUNCTION, \
    CTEST_TEST_SUITE_INITIALIZE, \
    CTEST_TEST_SUITE_CLEANUP, \
    CTEST_TEST_FUNCTION_INITIALIZE, \
    CTEST_TEST_FUNCTION_CLEANUP

MU_DEFINE_ENUM(CTEST_FUNCTION_TYPE, CTEST_FUNCTION_TYPE_VALUES)

#define TEST_RESULT_VALUES \
    TEST_SUCCESS, \
    TEST_FAILED, \
    TEST_NOT_EXECUTED \

MU_DEFINE_ENUM(TEST_RESULT, TEST_RESULT_VALUES)

typedef struct TEST_FUNCTION_DATA_TAG
{
    const TEST_FUNC TestFunction;
    const char* TestFunctionName;
    const void* const NextTestFunctionData;
    TEST_RESULT* const TestResult;
    const CTEST_FUNCTION_TYPE FunctionType;
} TEST_FUNCTION_DATA;

#define EXPAND_1(A) A

extern const TEST_FUNCTION_DATA* g_CurrentTestFunction;
extern jmp_buf g_ExceptionJump;

#define CTEST_BEGIN_TEST_SUITE(testSuiteName) \
    C_LINKAGE_PREFIX const int TestListHead_Begin_##testSuiteName = 0; \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(__COUNTER__)) = { NULL, NULL, NULL, NULL, CTEST_BEGIN_SUITE }; \

#define CTEST_FUNCTION(funcName)    \
    static void funcName(void); \
    static TEST_RESULT funcName##_TestResult; \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(MU_INC(__COUNTER__))) = \
{ funcName, #funcName, &MU_C2(TestFunctionData, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), &funcName##_TestResult, CTEST_TEST_FUNCTION }; \
    static void funcName(void)

#define CTEST_SUITE_INITIALIZE()    \
    static void TestSuiteInitialize(void); \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(MU_INC(__COUNTER__))) = \
{ TestSuiteInitialize, "TestSuiteInitialize", &MU_C2(TestFunctionData, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), NULL, CTEST_TEST_SUITE_INITIALIZE }; \
    static void TestSuiteInitialize(void)

#define CTEST_SUITE_CLEANUP()    \
    static void TestSuiteCleanup(void); \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(MU_INC(__COUNTER__))) = \
{ &TestSuiteCleanup, "TestSuiteCleanup", &MU_C2(TestFunctionData, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), NULL, CTEST_TEST_SUITE_CLEANUP }; \
    static void TestSuiteCleanup(void)

#define CTEST_FUNCTION_INITIALIZE()    \
    static void TestFunctionInitialize(void); \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(MU_INC(__COUNTER__))) = \
{ TestFunctionInitialize, "TestFunctionInitialize", &MU_C2(TestFunctionData, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), NULL, CTEST_TEST_FUNCTION_INITIALIZE }; \
    static void TestFunctionInitialize(void)

#define CTEST_FUNCTION_CLEANUP()    \
    static void TestFunctionCleanup(void); \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(MU_INC(__COUNTER__))) = \
{ &TestFunctionCleanup, "TestFunctionCleanup", &MU_C2(TestFunctionData, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), NULL, CTEST_TEST_FUNCTION_CLEANUP }; \
    static void TestFunctionCleanup(void)

#define CTEST_END_TEST_SUITE(testSuiteName) \
    static const TEST_FUNCTION_DATA MU_C2(TestFunctionData, MU_C1(MU_INC(__COUNTER__))) = { NULL, NULL, NULL, NULL, CTEST_TEST_FUNCTION }; \
    C_LINKAGE_PREFIX const TEST_FUNCTION_DATA TestListHead_##testSuiteName = { NULL, NULL, &MU_C2(TestFunctionData, MU_C1(MU_DEC(MU_DEC(__COUNTER__)))), NULL, CTEST_END_SUITE }; \
    static const int* TestListHead_End_##testSuiteName = &TestListHead_Begin_##testSuiteName;

#define PRINT_MY_ARG_2(A)

#define PRINT_MY_ARG_1(A) \
    A +=

#ifdef _MSC_VER
#define PRINT_SECOND_ARG(argCount, B) \
    MU_C2(PRINT_MY_ARG_,argCount) MU_LPAREN B )
#else
#define PRINT_SECOND_ARG(argCount, B) \
    MU_C2(PRINT_MY_ARG_,argCount) (B)
#endif

#define FIRST_ARG(A, ...) A

#define CTEST_RUN_TEST_SUITE(...) \
do \
{ \
    extern C_LINKAGE const TEST_FUNCTION_DATA MU_C2(TestListHead_,FIRST_ARG(__VA_ARGS__)); \
    MU_IF(MU_DIV2(MU_COUNT_ARG(__VA_ARGS__)),MU_FOR_EACH_1_COUNTED(PRINT_SECOND_ARG, __VA_ARGS__),) RunTests(&MU_C2(TestListHead_, FIRST_ARG(__VA_ARGS__)), MU_TOSTRING(FIRST_ARG(__VA_ARGS__))); \
} while ((void)0,0)

typedef const char* char_ptr;
typedef const wchar_t* wchar_ptr;
typedef void* void_ptr;
typedef long double long_double;
typedef unsigned long unsigned_long;

extern C_LINKAGE void int_ToString(char* string, size_t bufferSize, int val);
extern C_LINKAGE void char_ToString(char* string, size_t bufferSize, char val);
extern C_LINKAGE void short_ToString(char* string, size_t bufferSize, short val);
extern C_LINKAGE void long_ToString(char* string, size_t bufferSize, long val);
extern C_LINKAGE void size_t_ToString(char* string, size_t bufferSize, size_t val);
extern C_LINKAGE void float_ToString(char* string, size_t bufferSize, float val);
extern C_LINKAGE void double_ToString(char* string, size_t bufferSize, double val);
extern C_LINKAGE void long_double_ToString(char* string, size_t bufferSize, long double val);
extern C_LINKAGE void char_ptr_ToString(char* string, size_t bufferSize, const char* val);
extern C_LINKAGE void wchar_ptr_ToString(char* string, size_t bufferSize, const wchar_t* val);
extern C_LINKAGE void void_ptr_ToString(char* string, size_t bufferSize, const void* val);
extern C_LINKAGE void unsigned_long_ToString(char* string, size_t bufferSize, unsigned long val);
extern C_LINKAGE int int_Compare(int left, int right);
extern C_LINKAGE int char_Compare(char left, char right);
extern C_LINKAGE int short_Compare(short left, short right);
extern C_LINKAGE int long_Compare(long left, long right);
extern C_LINKAGE int size_t_Compare(size_t left, size_t right);
extern C_LINKAGE int float_Compare(float left, float right);
extern C_LINKAGE int double_Compare(double left, double right);
extern C_LINKAGE int long_double_Compare(long double left, long double right);
extern C_LINKAGE int char_ptr_Compare(const char* left, const char* right);
extern C_LINKAGE int wchar_ptr_Compare(const wchar_t* left, const wchar_t* right);
extern C_LINKAGE int void_ptr_Compare(const void * left, const void* right);
extern C_LINKAGE int unsigned_long_Compare(unsigned long left, unsigned long right);

#if defined CTEST_USE_STDINT
extern C_LINKAGE void uint8_t_ToString(char* string, size_t bufferSize, uint8_t val);
extern C_LINKAGE void int8_t_ToString(char* string, size_t bufferSize, int8_t val);
extern C_LINKAGE void uint16_t_ToString(char* string, size_t bufferSize, uint16_t val);
extern C_LINKAGE void int16_t_ToString(char* string, size_t bufferSize, int16_t val);
extern C_LINKAGE void uint32_t_ToString(char* string, size_t bufferSize, uint32_t val);
extern C_LINKAGE void int32_t_ToString(char* string, size_t bufferSize, int32_t val);
extern C_LINKAGE void uint64_t_ToString(char* string, size_t bufferSize, uint64_t val);
extern C_LINKAGE void int64_t_ToString(char* string, size_t bufferSize, int64_t val);
extern C_LINKAGE int uint8_t_Compare(uint8_t left, uint8_t right);
extern C_LINKAGE int int8_t_Compare(int8_t left, int8_t right);
extern C_LINKAGE int uint16_t_Compare(uint16_t left, uint16_t right);
extern C_LINKAGE int int16_t_Compare(int16_t left, int16_t right);
extern C_LINKAGE int uint32_t_Compare(uint32_t left, uint32_t right);
extern C_LINKAGE int int32_t_Compare(int32_t left, int32_t right);
extern C_LINKAGE int uint64_t_Compare(uint64_t left, uint64_t right);
extern C_LINKAGE int int64_t_Compare(int64_t left, int64_t right);
#endif

extern C_LINKAGE char* ctest_sprintf_char(const char* format, ...);
extern C_LINKAGE void ctest_sprintf_free(char* string);

#define CTEST_COMPARE(toStringType, cType) \
    typedef cType toStringType; \
    static int MU_C2(toStringType,_Compare)(toStringType left, toStringType right)

#define CTEST_TO_STRING(toStringType, cType, string, bufferSize, value) \
static void MU_C2(toStringType,_ToString)(char* string, size_t bufferSize, cType value)

// these are generic macros for formatting the optional message
// they can be used in all the ASSERT macros without repeating the code over and over again
#define GET_MESSAGE_FORMATTED(format, ...) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__), ctest_sprintf_char(format, __VA_ARGS__);(void)(0 && printf(format, __VA_ARGS__)), ctest_sprintf_char(format));

#define GET_MESSAGE_FORMATTED_EMPTY(...) \
    NULL

#define GET_MESSAGE(...) \
    MU_IF(MU_COUNT_ARG(__VA_ARGS__), GET_MESSAGE_FORMATTED, GET_MESSAGE_FORMATTED_EMPTY)(__VA_ARGS__)

void do_jump(jmp_buf *exceptionJump, const volatile void* expected, const volatile void* actual);

#define CTEST_ASSERT_ARE_EQUAL(type, A, B, ...) \
do { \
    const type A_value = (const type)(A); \
    const type B_value = (const type)(B); \
    char expectedString[1024]; \
    char actualString[1024]; \
    MU_C2(type,_ToString)(expectedString, sizeof(expectedString), A_value); /*one evaluation per argument*/ \
    MU_C2(type,_ToString)(actualString, sizeof(actualString), B_value);/*one evaluation per argument*/ \
    if (MU_C2(type,_Compare)(A_value, B_value)) \
    { \
        char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
        (void)printf("  Assert failed in line %d %s Expected: %s, Actual: %s\n", __LINE__, (ctest_message == NULL) ? "" : ctest_message, expectedString, actualString); \
        ctest_sprintf_free(ctest_message); \
        if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
        do_jump(&g_ExceptionJump, expectedString, actualString); \
    } \
} while (0)

#define CTEST_ASSERT_ARE_NOT_EQUAL(type, A, B, ...) \
do { \
    const type A_value = (const type)(A); \
    const type B_value = (const type)(B); \
    char expectedString[1024]; \
    char actualString[1024]; \
    MU_C2(type,_ToString)(expectedString, sizeof(expectedString), A_value); /*one evaluation per argument*/ \
    MU_C2(type,_ToString)(actualString, sizeof(actualString), B_value);/*one evaluation per argument*/ \
    if (!MU_C2(type,_Compare)(A_value, B_value)) \
    { \
        char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
        (void)printf("  Assert failed in line %d: %s Expected: %s, Actual: %s\n", __LINE__, (ctest_message == NULL) ? "" : ctest_message, expectedString, actualString); \
        ctest_sprintf_free(ctest_message); \
        if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
        do_jump(&g_ExceptionJump, "some expected string", actualString); \
    } \
} while (0)

#define CTEST_ASSERT_IS_NULL(value, ...) \
do \
{ \
    const void* copy_of_value = (void*)(value); \
    if (copy_of_value != NULL) \
    { \
        char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
        (void)printf("  Assert failed in line %d: NULL expected, actual: 0x%p. %s\n", __LINE__, copy_of_value, (ctest_message == NULL) ? "" : ctest_message); \
        ctest_sprintf_free(ctest_message); \
        if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
        do_jump(&g_ExceptionJump, "expected it to be NULL (actual is the value)", copy_of_value); \
    } \
} \
while(0)

#define CTEST_ASSERT_IS_NOT_NULL(value, ...) \
do \
{ \
    const void* copy_of_value = (void*)(value); \
    if (copy_of_value == NULL) \
    { \
        char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
        (void)printf("  Assert failed in line %d: non-NULL expected. %s\n", __LINE__, (ctest_message == NULL) ? "" : ctest_message); \
        ctest_sprintf_free(ctest_message); \
        if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
        do_jump(&g_ExceptionJump, "expected it not to be NULL (actual is value)", copy_of_value); \
    } \
}while(0)

#define CTEST_ASSERT_IS_TRUE(expression, ...) \
do { \
    int expression_is_false = ((expression)==0);/*one evaluation per argument*/ \
    if (expression_is_false) \
    { \
        char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
        (void)printf("  Assert failed in line %d: Expression should be true: %s. %s\n", __LINE__, #expression, (ctest_message == NULL) ? "" : ctest_message); \
        ctest_sprintf_free(ctest_message); \
        if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
        do_jump(&g_ExceptionJump, "expected it to be true", "but it wasn't"); \
    } \
}while(0)

#define CTEST_ASSERT_IS_FALSE(expression, ...) \
do { \
    int expression_is_true = ((expression)!=0);/*one evaluation per argument*/ \
    if (expression_is_true) \
    { \
        char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
        (void)printf("  Assert failed in line %d: Expression should be false: %s. %s\n", __LINE__, #expression, (ctest_message == NULL) ? "" : ctest_message); \
        ctest_sprintf_free(ctest_message); \
        if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
        do_jump(&g_ExceptionJump, "expected it to be false", "but it was true"); \
    } \
}while(0)

#define CTEST_ASSERT_FAIL(...) \
do \
{ \
    char* ctest_message = GET_MESSAGE(__VA_ARGS__); \
    (void)printf("  Assert failed in line %d: %s \n" , __LINE__, (ctest_message == NULL) ? "" : ctest_message); \
    ctest_sprintf_free(ctest_message); \
    if (g_CurrentTestFunction != NULL) *g_CurrentTestFunction->TestResult = TEST_FAILED; \
    do_jump(&g_ExceptionJump, (void*)"nothing expected, 100% fail", (void*)"nothing actual, 100% fail"); \
    ctest_sprintf_free(ctest_message); \
} \
while(0)

// this macro expands to the needed _ToString and _Compare functions for an enum,
// while using the macro utils ENUM_TO_STRING
#define CTEST_DEFINE_ENUM_TYPE(enum_name, ...) \
static void MU_C2(enum_name, _ToString)(char* dest, size_t bufferSize, enum_name enumValue) \
{ \
    (void)snprintf(dest, bufferSize, "%s", MU_ENUM_TO_STRING(enum_name, enumValue)); \
} \
static int MU_C2(enum_name, _Compare)(enum_name left, enum_name right) \
{ \
    return left != right; \
}

// this macro expands to the needed _ToString and _Compare functions for an enum defined with MU_DEFINE_ENUM_2,
// while using the macro utils ENUM_TO_STRING_2
#define CTEST_DEFINE_ENUM_2_TYPE(enum_name, ...) \
static void MU_C2(enum_name, _ToString)(char* dest, size_t bufferSize, enum_name enumValue) \
{ \
    (void)snprintf(dest, bufferSize, "%s", MU_ENUM_TO_STRING_2(enum_name, enumValue)); \
} \
static int MU_C2(enum_name, _Compare)(enum_name left, enum_name right) \
{ \
    return left != right; \
}

extern C_LINKAGE size_t RunTests(const TEST_FUNCTION_DATA* testListHead, const char* testSuiteName);

#ifdef __cplusplus
}
#endif

#endif
