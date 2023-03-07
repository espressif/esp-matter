// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/crt_abstractions.h"

// VS 2008 does not have INFINITY and all the nice goodies...
#if defined (TIZENRT)
#define DEFINE_INFINITY 1
#else

#if defined _MSC_VER
#if _MSC_VER <= 1500
#define DEFINE_INFINITY 1
#endif
#endif
#endif

#if defined DEFINE_INFINITY

#pragma warning(disable:4756 4056) // warning C4756: overflow in constant arithmetic

// These defines are missing in math.h for WEC2013 SDK
#ifndef _HUGE_ENUF
#define _HUGE_ENUF  1e+300  // _HUGE_ENUF*_HUGE_ENUF must overflow
#endif

#define INFINITY   ((float)(_HUGE_ENUF * _HUGE_ENUF))
#define HUGE_VALF  ((float)INFINITY)
#define HUGE_VALL  ((long double)INFINITY)
#define NAN        ((float)(INFINITY * 0.0F))
#endif

#if defined (_MSC_VER) || defined (MINGW_HAS_SECURE_API)
#else

/*Codes_SRS_CRT_ABSTRACTIONS_99_008: [strcat_s shall append the src to dst and terminates the resulting string with a null character.]*/
int strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    int result;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_004: [If dst is NULL or unterminated, the error code returned shall be EINVAL & dst shall not be modified.]*/
    if (dst == NULL)
    {
        result = EINVAL;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_99_005: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]*/
    else if (src == NULL)
    {
        dst[0] = '\0';
        result = EINVAL;
    }
    else
    {
        /*Codes_SRS_CRT_ABSTRACTIONS_99_006: [If the dstSizeInBytes is 0 or smaller than the required size for dst & src, the error code returned shall be ERANGE & dst[0] set to 0.]*/
        if (dstSizeInBytes == 0)
        {
            result = ERANGE;
            dst[0] = '\0';
        }
        else
        {
            size_t dstStrLen = 0;
            size_t src_len = strlen(src);
#ifdef __STDC_LIB_EXT1__
            dstStrLen = strnlen_s(dst, dstSizeInBytes);
#else
            size_t i;
            for(i=0; (i < dstSizeInBytes) && (dst[i]!= '\0'); i++)
            {
            }
            dstStrLen = i;
#endif
            /*Codes_SRS_CRT_ABSTRACTIONS_99_004: [If dst is NULL or unterminated, the error code returned shall be EINVAL & dst shall not be modified.]*/
            if (dstSizeInBytes == dstStrLen) /* this means the dst string is not terminated*/
            {
                result = EINVAL;
            }
            // If we are instructed to write too much data to the buffer
            // return ERANGE
            else if ((src_len + dstStrLen) >= dstSizeInBytes)
            {
                dst[0] = '\0';
                result = ERANGE;
            }
            else
            {
                // memcpy should at most copy the result of strlen(src) or there may be
                // some issues with copying unwanted memory
                size_t bytes_to_cpy = dstSizeInBytes - dstStrLen;
                if (bytes_to_cpy > src_len)
                {
                    bytes_to_cpy = src_len;
                }

                /*Codes_SRS_CRT_ABSTRACTIONS_99_009: [The initial character of src shall overwrite the terminating null character of dst.]*/
                if (memcpy(&dst[dstStrLen], src, bytes_to_cpy) == NULL)
                /*Codes_SRS_CRT_ABSTRACTIONS_99_006: [If the dstSizeInBytes is 0 or smaller than the required size for dst & src, the error code returned shall be ERANGE & dst[0] set to 0.]*/
                {
                    dst[0] = '\0';
                    result = ERANGE;
                }
                else
                {
                    /*Codes_SRS_CRT_ABSTRACTIONS_99_003: [strcat_s shall return Zero upon success.]*/
                    dst[dstStrLen+bytes_to_cpy] = '\0';
                    result = 0;
                }
            }
        }
    }
    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_99_025: [strncpy_s shall copy the first N characters of src to dst, where N is the lesser of MaxCount and the length of src.]*/
int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t maxCount)
{
    int result;
    int truncationFlag = 0;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_020: [If dst is NULL, the error code returned shall be EINVAL and dst shall not be modified.]*/
    if (dst == NULL)
    {
        result = EINVAL;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_99_021: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]*/
    else if (src == NULL)
    {
        dst[0] = '\0';
        result = EINVAL;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_99_022: [If the dstSizeInBytes is 0, the error code returned shall be EINVAL and dst shall not be modified.]*/
    else if (dstSizeInBytes == 0)
    {
        result = EINVAL;
    }
    else
    {
        size_t srcLength = strlen(src);
        if (maxCount != _TRUNCATE)
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_99_041: [If those N characters will fit within dst (whose size is given as dstSizeInBytes) and still leave room for a null terminator, then those characters shall be copied and a terminating null is appended; otherwise, strDest[0] is set to the null character and ERANGE error code returned.]*/
            if (srcLength > maxCount)
            {
                srcLength = maxCount;
            }

            /*Codes_SRS_CRT_ABSTRACTIONS_99_023: [If dst is not NULL & dstSizeInBytes is smaller than the required size for the src string, the error code returned shall be ERANGE and dst[0] shall be set to 0.]*/
            if (srcLength + 1 > dstSizeInBytes)
            {
                dst[0] = '\0';
                result = ERANGE;
            }
            else
            {
                (void)memcpy(dst, src, srcLength);
                dst[srcLength] = '\0';
                /*Codes_SRS_CRT_ABSTRACTIONS_99_018: [strncpy_s shall return Zero upon success]*/
                result = 0;
            }
        }
        /*Codes_SRS_CRT_ABSTRACTIONS_99_026: [If MaxCount is _TRUNCATE (defined as -1), then as much of src as will fit into dst shall be copied while still leaving room for the terminating null to be appended.]*/
        else
        {
            if (srcLength + 1 > dstSizeInBytes )
            {
                srcLength = dstSizeInBytes - 1;
                truncationFlag = 1;
            }
            (void)memcpy(dst, src, srcLength);
            dst[srcLength] = '\0';
            result = 0;
        }
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_99_019: [If truncation occurred as a result of the copy, the error code returned shall be STRUNCATE.]*/
    if (truncationFlag == 1)
    {
        result = STRUNCATE;
    }

    return result;
}

/* Codes_SRS_CRT_ABSTRACTIONS_99_016: [strcpy_s shall copy the contents in the address of src, including the terminating null character, to the location that's specified by dst.]*/
int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    int result;

    /* Codes_SRS_CRT_ABSTRACTIONS_99_012: [If dst is NULL, the error code returned shall be EINVAL & dst shall not be modified.]*/
    if (dst == NULL)
    {
        result = EINVAL;
    }
    /* Codes_SRS_CRT_ABSTRACTIONS_99_013: [If src is NULL, the error code returned shall be EINVAL and dst[0] shall be set to 0.]*/
    else if (src == NULL)
    {
        dst[0] = '\0';
        result = EINVAL;
    }
    /* Codes_SRS_CRT_ABSTRACTIONS_99_014: [If the dstSizeInBytes is 0 or smaller than the required size for the src string, the error code returned shall be ERANGE & dst[0] set to 0.]*/
    else if (dstSizeInBytes == 0)
    {
        dst[0] = '\0';
        result = ERANGE;
    }
    else
    {
        size_t neededBuffer = strlen(src);
        /* Codes_SRS_CRT_ABSTRACTIONS_99_014: [If the dstSizeInBytes is 0 or smaller than the required size for the src string, the error code returned shall be ERANGE & dst[0] set to 0.]*/
        if (neededBuffer + 1 > dstSizeInBytes)
        {
            dst[0] = '\0';
            result = ERANGE;
        }
        else
        {
            (void)memcpy(dst, src, neededBuffer + 1);
            /*Codes_SRS_CRT_ABSTRACTIONS_99_011: [strcpy_s shall return Zero upon success]*/
            result = 0;
        }
    }

    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_99_029: [The sprintf_s function shall format and store series of characters and values in dst.  Each argument (if any) is converted and output according to the corresponding Format Specification in the format variable.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_99_031: [A null character is appended after the last character written.]*/
int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ...)
{
    int result;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_028: [If dst or format is a null pointer, sprintf_s shall return -1 and set errno to EINVAL]*/
    if ((dst == NULL) ||
        (format == NULL))
    {
        errno = EINVAL;
        result = -1;
    }
    else
    {
        /*Codes_SRS_CRT_ABSTRACTIONS_99_033: [sprintf_s shall check the format string for valid formatting characters.  If the check fails, the function returns -1.]*/

#if defined _MSC_VER
#error crt_abstractions is not provided for Microsoft Compilers
#else
        /*not Microsoft compiler... */
#if defined (__STDC_VERSION__) || (__cplusplus)
#if ( \
        ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L)) || \
        (defined __cplusplus) \
    )
        /*C99 compiler*/
        va_list args;
        va_start(args, format);
        /*Codes_SRS_CRT_ABSTRACTIONS_99_027: [sprintf_s shall return the number of characters stored in dst upon success.  This number shall not include the terminating null character.]*/
        result = vsnprintf(dst, dstSizeInBytes, format, args);
        va_end(args);

        /*C99: Thus, the null-terminated output has been completely written if and only if the returned value is nonnegative and less than n*/
        if (result < 0)
        {
            result = -1;
        }
        else if ((size_t)result >= dstSizeInBytes)
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_99_034: [If the dst buffer is too small for the text being printed, then dst is set to an empty string and the function shall return -1.]*/
            dst[0] = '\0';
            result = -1;
        }
        else
        {
            /*do nothing, all is fine*/
        }
#else
#error STDC_VERSION defined, but of unknown value; unable to sprinf_s, or provide own implementation
#endif
#else
#error for STDC_VERSION undefined (assumed C89), provide own implementation of sprintf_s
#endif
#endif
    }
    return result;
}
#endif /* _MSC_VER || MINGW_HAS_SECURE_API */

/*Codes_SRS_CRT_ABSTRACTIONS_21_006: [The strtoull_s must use the letters from a(or A) through z(or Z) to represent the numbers between 10 to 35.]*/
/* returns the integer value that correspond to the character 'c'. If the character is invalid, it returns -1. */
#define DIGIT_VAL(c)         (((c>='0') && (c<='9')) ? (c-'0') : ((c>='a') && (c<='z')) ? (c-'a'+10) : ((c>='A') && (c<='Z')) ? (c-'A'+10) : -1)
#define IN_BASE_RANGE(d, b)  ((d >= 0) && (d < b))

/*Codes_SRS_CRT_ABSTRACTIONS_21_010: [The white-space must be one of the characters ' ', '\f', '\n', '\r', '\t', '\v'.]*/
#define IS_SPACE(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')

/*Codes_SRS_CRT_ABSTRACTIONS_21_001: [The strtoull_s must convert the initial portion of the string pointed to by nptr to unsigned long long int representation.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_002: [The strtoull_s must resembling an integer represented in some radix determined by the value of base.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_003: [The strtoull_s must return the integer that represents the value in the initial part of the string. If any.]*/
unsigned long long strtoull_s(const char* nptr, char** endptr, int base)
{
    unsigned long long result = 0ULL;
    bool validStr = true;
    char* runner = (char*)nptr;
    bool isNegative = false;
    int digitVal;

    /*Codes_SRS_CRT_ABSTRACTIONS_21_005: [The strtoull_s must convert number using base 2 to 36.]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_012: [If the subject sequence is empty or does not have the expected form, the strtoull_s must not perform any conversion; the value of nptr is stored in the object pointed to by endptr, provided that endptr is not a NULL pointer.]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_013: [If no conversion could be performed, the strtoull_s returns the value 0L.]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_035: [If the nptr is NULL, the strtoull_s must **not** perform any conversion and must returns 0L; endptr must receive NULL, provided that endptr is not a NULL pointer.]*/
        if (((base >= 2) || (base == 0)) && (base <= 36) && (runner != NULL))
    {
        /*Codes_SRS_CRT_ABSTRACTIONS_21_011: [The valid sequence starts after the first non-white-space character, followed by an optional positive or negative sign, a number or a letter(depending of the base).]*/
        /*Codes_SRS_CRT_ABSTRACTIONS_21_010: [The white-space must be one of the characters ' ', '\f', '\n', '\r', '\t', '\v'.]*/
        while (IS_SPACE(*runner))
        {
            runner++;
        }
        if ((*runner) == '+')
        {
            runner++;
        }
        else if ((*runner) == '-')
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_21_038: [If the subject sequence starts with a negative sign, the strtoull_s will convert it to the posive representation of the negative value.]*/
            isNegative = true;
            runner++;
        }

        if ((*runner) == '0')
        {
            if ((*(runner+1) == 'x') || (*(runner+1) == 'X'))
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_21_008: [If the base is 0 and '0x' or '0X' precedes the number, strtoull_s must convert to a hexadecimal (base 16).]*/
                /* hexadecimal... */
                if ((base == 0) || (base == 16))
                {
                    base = 16;
                    runner += 2;
                }
            }
            else if((base == 0) || (base == 8))
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_21_009: [If the base is 0 and '0' precedes the number, strtoull_s must convert to an octal (base 8).]*/
                /* octal... */
                base = 8;
                runner++;
            }
        }

        if(base == 0)
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_21_007: [If the base is 0 and no special chars precedes the number, strtoull_s must convert to a decimal (base 10).]*/
            /* decimal... */
            base = 10;
        }

        digitVal = DIGIT_VAL(*runner);
        if (validStr && IN_BASE_RANGE(digitVal, base))
        {
            errno = 0;
            do
            {
                if (((ULLONG_MAX - digitVal) / base) < result)
                {
                    /*Codes_SRS_CRT_ABSTRACTIONS_21_014: [If the correct value is outside the range, the strtoull_s returns the value ULLONG_MAX, and errno will receive the value ERANGE.]*/
                    /* overflow... */
                    result = ULLONG_MAX;
                    errno = ERANGE;
                }
                else
                {
                    result = result * base + digitVal;
                }
                runner++;
                digitVal = DIGIT_VAL(*runner);
            } while (IN_BASE_RANGE(digitVal, base));
        }
        else
        {
            runner = (char*)nptr;
        }
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_21_004: [The strtoull_s must return in endptr a final string of one or more unrecognized characters, including the terminating null character of the input string.]*/
    if (endptr != NULL)
    {
        (*endptr) = (char*)runner;
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_21_038: [If the subject sequence starts with a negative sign, the strtoull_s will convert it to the posive representation of the negative value.]*/
    if (isNegative)
    {
        result = ULLONG_MAX - result + 1;
    }

    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_21_023: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtof_s must return the INFINITY value for float.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_024: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtof_s must return 0.0f and points endptr to the first character after the 'NAN' sequence.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_033: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtold_s must return the INFINITY value for long double.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_034: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtold_s must return 0.0 and points endptr to the first character after the 'NAN' sequence.]*/
static int substricmp(const char* nptr, const char* subsrt)
{
    int result = 0;
    while (((*subsrt) != '\0') && (result == 0))
    {
        result = TOUPPER(*nptr) - TOUPPER(*subsrt);
        nptr++;
        subsrt++;
    }
    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_21_023: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtof_s must return the INFINITY value for float.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_033: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtold_s must return the INFINITY value for long double.]*/
static bool isInfinity(const char** endptr)
{
    bool result = false;
    if (substricmp((*endptr), "INF") == 0)
    {
        (*endptr) += 3;
        result = true;
        if (substricmp((*endptr), "INITY") == 0)
        {
            (*endptr) += 5;
        }
    }
    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_21_024: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtof_s must return 0.0f and points endptr to the first character after the 'NAN' sequence.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_034: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtold_s must return 0.0 and points endptr to the first character after the 'NAN' sequence.]*/
static bool isNaN(const char** endptr)
{
    const char* runner = (*endptr);
    bool result = false;
    if (substricmp(runner, "NAN") == 0)
    {
        runner += 3;
        result = true;
        if ((*runner) == '(')
        {
            do
            {
                runner++;
            } while (((*runner) != '\0') && ((*runner) != ')'));
            if ((*runner) == ')')
                runner++;
            else
                result = false;
        }
    }
    if (result)
        (*endptr) = runner;
    return result;
}

#define FLOAT_STRING_TYPE_VALUES    \
            FST_INFINITY,           \
            FST_NAN,                \
            FST_NUMBER,             \
            FST_OVERFLOW,           \
            FST_ERROR

MU_DEFINE_ENUM(FLOAT_STRING_TYPE, FLOAT_STRING_TYPE_VALUES);

static FLOAT_STRING_TYPE splitFloatString(const char* nptr, char** endptr, int *signal, double *fraction, int *exponential)
{
    FLOAT_STRING_TYPE result = FST_ERROR;

    unsigned long long ullInteger = 0;
    unsigned long long ullFraction = 0;
    int integerSize = 0;
    int fractionSize = 0;
    char* startptr;

    (*endptr) = (char*)nptr;

    /*Codes_SRS_CRT_ABSTRACTIONS_21_018: [The white-space for strtof_s must be one of the characters ' ', '\f', '\n', '\r', '\t', '\v'.]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_028: [The white-space for strtold_s must be one of the characters ' ', '\f', '\n', '\r', '\t', '\v'.]*/
    while (IS_SPACE(**endptr))
    {
        (*endptr)++;
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_21_019: [The valid sequence for strtof_s starts after the first non-white - space character, followed by an optional positive or negative sign, a number, 'INF', or 'NAN' (ignoring case).]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_029: [The valid sequence for strtold_s starts after the first non-white - space character, followed by an optional positive or negative sign, a number, 'INF', or 'NAN' (ignoring case).]*/
    (*signal) = +1;
    if ((**endptr) == '+')
    {
        (*endptr)++;
    }
    else if ((**endptr) == '-')
    {
        (*signal) = -1;
        (*endptr)++;
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_21_023: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtof_s must return the INFINITY value for float.]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_033: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtold_s must return the INFINITY value for long double.]*/
    if (isInfinity((const char**)endptr))
    {
        result = FST_INFINITY;
    }
    /*Codes_SRS_CRT_ABSTRACTIONS_21_034: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtold_s must return 0.0 and points endptr to the first character after the 'NAN' sequence.]*/
    /*Codes_SRS_CRT_ABSTRACTIONS_21_024: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtof_s must return 0.0f and points endptr to the first character after the 'NAN' sequence.]*/
    else if (isNaN((const char**)endptr))
    {
        result = FST_NAN;
    }
    else if (IN_BASE_RANGE(DIGIT_VAL(**endptr), 10))
    {
        result = FST_NUMBER;
        startptr = *endptr;
        /* integers will go to the fraction and exponential. */
        ullInteger = strtoull_s(startptr, endptr, 10);
        integerSize = (int)((*endptr) - startptr);
        if ((ullInteger == ULLONG_MAX) && (errno != 0))
        {
            result = FST_OVERFLOW;
        }

        /* get the real fraction part, if exist. */
        if ((**endptr) == '.')
        {
            startptr = (*endptr) + 1;
            ullFraction = strtoull_s(startptr, endptr, 10);
            fractionSize = (int)((*endptr) - startptr);
            if ((ullFraction == ULLONG_MAX) && (errno != 0))
            {
                result = FST_OVERFLOW;
            }
        }

        if (((**endptr) == 'e') || ((**endptr) == 'E'))
        {
            startptr = (*endptr) + 1;
            (*exponential) = (int)strtol(startptr, endptr, 10);
            if (((*exponential) < (DBL_MAX_10_EXP * (-1))) || ((*exponential) > DBL_MAX_10_EXP))
            {
                result = FST_OVERFLOW;
            }
        }
        else
        {
            (*exponential) = 0;
        }

        if (result == FST_NUMBER)
        {
            /* Add ullInteger to ullFraction. */
            ullFraction += (ullInteger * (unsigned long long)(pow(10, (double)fractionSize)));
            (*fraction) = ((double)ullFraction / (pow(10.0f, ((double)fractionSize + (double)integerSize - 1.00))));

            /* Unify rest of integerSize and fractionSize in the exponential. */
            (*exponential) += integerSize - 1;
        }
    }

    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_21_015: [The strtof_s must convert the initial portion of the string pointed to by nptr to float representation.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_016: [The strtof_s must return the float that represents the value in the initial part of the string. If any.]*/
float strtof_s(const char* nptr, char** endptr)
{
    int signal = 1;
    double fraction;
    int exponential;
    char* runner = (char*)nptr;
    double val;

    /*Codes_SRS_CRT_ABSTRACTIONS_21_021: [If no conversion could be performed, the strtof_s returns the value 0.0.]*/
    float result = 0.0;

    /*Codes_SRS_CRT_ABSTRACTIONS_21_036: [**If the nptr is NULL, the strtof_s must not perform any conversion and must returns 0.0f; endptr must receive NULL, provided that endptr is not a NULL pointer.]*/
    if (nptr != NULL)
    {
        switch (splitFloatString(nptr, &runner, &signal, &fraction, &exponential))
        {
        case FST_INFINITY:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_023: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtof_s must return the INFINITY value for float.]*/
            result = INFINITY * (signal);
            errno = 0;
            break;
        case FST_NAN:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_024: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtof_s must return 0.0f and points endptr to the first character after the 'NAN' sequence.]*/
            result = NAN;
            break;
        case FST_NUMBER:
            val = fraction * pow(10.0, (double)exponential) * (double)signal;
            if ((val >= ((double)FLT_MAX * (-1.0))) && (val <= (double)FLT_MAX))
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_21_016: [The strtof_s must return the float that represents the value in the initial part of the string. If any.]*/
                result = (float)val;
            }
            else
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_21_022: [If the correct value is outside the range, the strtof_s returns the value plus or minus HUGE_VALF, and errno will receive the value ERANGE.]*/
                result = HUGE_VALF * (signal);
                errno = ERANGE;
            }
            break;
        case FST_OVERFLOW:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_022: [If the correct value is outside the range, the strtof_s returns the value plus or minus HUGE_VALF, and errno will receive the value ERANGE.]*/
            result = HUGE_VALF * (signal);
            errno = ERANGE;
            break;
        default:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_020: [If the subject sequence is empty or does not have the expected form, the strtof_s must not perform any conversion and must returns 0.0f; the value of nptr is stored in the object pointed to by endptr, provided that endptr is not a NULL pointer.]*/
            runner = (char*)nptr;
            break;
        }
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_21_017: [The strtof_s must return in endptr a final string of one or more unrecognized characters, including the terminating null character of the input string.]*/
    if (endptr != NULL)
    {
        (*endptr) = runner;
    }

    return result;
}

/*Codes_SRS_CRT_ABSTRACTIONS_21_025: [The strtold_s must convert the initial portion of the string pointed to by nptr to long double representation.]*/
/*Codes_SRS_CRT_ABSTRACTIONS_21_026: [The strtold_s must return the long double that represents the value in the initial part of the string. If any.]*/
long double strtold_s(const char* nptr, char** endptr)
{
    int signal = 1;
    double fraction;
    int exponential;
    char* runner = (char*)nptr;

    /*Codes_SRS_CRT_ABSTRACTIONS_21_031: [If no conversion could be performed, the strtold_s returns the value 0.0.]*/
    long double result = 0.0;

    /*Codes_SRS_CRT_ABSTRACTIONS_21_037: [If the nptr is NULL, the strtold_s must not perform any conversion and must returns 0.0; endptr must receive NULL, provided that endptr is not a NULL pointer.]*/
    if (nptr != NULL)
    {
        switch (splitFloatString(nptr, &runner, &signal, &fraction, &exponential))
        {
        case FST_INFINITY:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_033: [If the string is 'INF' of 'INFINITY' (ignoring case), the strtold_s must return the INFINITY value for long double.]*/
            result = (long double)INFINITY * (long double)(signal);
            errno = 0;
            break;
        case FST_NAN:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_034: [If the string is 'NAN' or 'NAN(...)' (ignoring case), the strtold_s must return 0.0 and points endptr to the first character after the 'NAN' sequence.]*/
            result = (long double)NAN;
            break;
        case FST_NUMBER:
            if ((exponential != DBL_MAX_10_EXP || (fraction <= 1.7976931348623158)) &&
                (exponential != (DBL_MAX_10_EXP * (-1)) || (fraction <= 2.2250738585072014)))
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_21_026: [The strtold_s must return the long double that represents the value in the initial part of the string. If any.]*/
                result = fraction * pow(10.0, (double)exponential) * (double)signal;
            }
            else
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_21_032: [If the correct value is outside the range, the strtold_s returns the value plus or minus HUGE_VALL, and errno will receive the value ERANGE.]*/
                result = (long double)HUGE_VALF * (long double)(signal);
                errno = ERANGE;
            }
            break;
        case FST_OVERFLOW:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_032: [If the correct value is outside the range, the strtold_s returns the value plus or minus HUGE_VALL, and errno will receive the value ERANGE.]*/
            result = (long double)HUGE_VALF * (long double)(signal);
            errno = ERANGE;
            break;
        default:
            /*Codes_SRS_CRT_ABSTRACTIONS_21_030: [If the subject sequence is empty or does not have the expected form, the strtold_s must not perform any conversion and must returns 0.0; the value of nptr is stored in the object pointed to by endptr, provided that endptr is not a NULL pointer.]*/
            runner = (char*)nptr;
            break;
        }
    }

    /*Codes_SRS_CRT_ABSTRACTIONS_21_027: [The strtold_s must return in endptr a final string of one or more unrecognized characters, including the terminating null character of the input string.]*/
    if (endptr != NULL)
    {
        (*endptr) = runner;
    }

    return result;
}


/*Codes_SRS_CRT_ABSTRACTIONS_99_038: [mallocAndstrcpy_s shall allocate memory for destination buffer to fit the string in the source parameter.]*/
int mallocAndStrcpy_s(char** destination, const char* source)
{
    int result;
    int copied_result;
    /*Codes_SRS_CRT_ABSTRACTIONS_99_036: [destination parameter or source parameter is NULL, the error code returned shall be EINVAL and destination shall not be modified.]*/
    if ((destination == NULL) || (source == NULL))
    {
        /*If strDestination or strSource is a NULL pointer[...]these functions return EINVAL */
        result = EINVAL;
    }
    else
    {
        size_t l = strlen(source);
        char* temp = (char*)malloc(l + 1);

        /*Codes_SRS_CRT_ABSTRACTIONS_99_037: [Upon failure to allocate memory for the destination, the function will return ENOMEM.]*/
        if (temp == NULL)
        {
            result = ENOMEM;
        }
        else
        {
            *destination = temp;
            /*Codes_SRS_CRT_ABSTRACTIONS_99_039: [mallocAndstrcpy_s shall copy the contents in the address source, including the terminating null character into location specified by the destination pointer after the memory allocation.]*/
            copied_result = strcpy_s(*destination, l + 1, source);
            if (copied_result < 0) /*strcpy_s error*/
            {
                free(*destination);
                *destination = NULL;
                result = copied_result;
            }
            else
            {
                /*Codes_SRS_CRT_ABSTRACTIONS_99_035: [mallocAndstrcpy_s shall return Zero upon success]*/
                result = 0;
            }
        }
    }
    return result;
}

/*takes "value" and transforms it into a decimal string*/
/*10 => "10"*/
/*return 0 when everything went ok*/
/*Codes_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
int unsignedIntToString(char* destination, size_t destinationSize, unsigned int value)
{
    int result;
    size_t pos;
    /*the below loop gets the number in reverse order*/
    /*Codes_SRS_CRT_ABSTRACTIONS_02_003: [If destination is NULL then unsignedIntToString shall fail.] */
    /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
    if (
        (destination == NULL) ||
        (destinationSize < 2) /*because the smallest number is '0\0' which requires 2 characters*/
        )
    {
        result = MU_FAILURE;
    }
    else
    {
        pos = 0;
        do
        {
            destination[pos++] = '0' + (value % 10);
            value /= 10;
        } while ((value > 0) && (pos < (destinationSize-1)));

        if (value == 0)
        {
            size_t w;
            destination[pos] = '\0';
            /*all converted and they fit*/
            for (w = 0; w <= (pos-1) >> 1; w++)
            {
                char temp;
                temp = destination[w];
                destination[w] = destination[pos - 1 - w];
                destination[pos -1 - w] = temp;
            }
            /*Codes_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then unsignedIntToString shall return 0.] */
            result = 0;
        }
        else
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
            result = MU_FAILURE;
        }
    }
    return result;
}

/*takes "value" and transforms it into a decimal string*/
/*10 => "10"*/
/*return 0 when everything went ok*/
/*Codes_SRS_CRT_ABSTRACTIONS_02_001: [unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.] */
int size_tToString(char* destination, size_t destinationSize, size_t value)
{
    int result;
    size_t pos;
    /*the below loop gets the number in reverse order*/
    /*Codes_SRS_CRT_ABSTRACTIONS_02_003: [If destination is NULL then unsignedIntToString shall fail.] */
    /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
    if (
        (destination == NULL) ||
        (destinationSize < 2) /*because the smallest number is '0\0' which requires 2 characters*/
        )
    {
        result = MU_FAILURE;
    }
    else
    {
        pos = 0;
        do
        {
            destination[pos++] = '0' + (value % 10);
            value /= 10;
        } while ((value > 0) && (pos < (destinationSize - 1)));

        if (value == 0)
        {
            size_t w;
            destination[pos] = '\0';
            /*all converted and they fit*/
            for (w = 0; w <= (pos - 1) >> 1; w++)
            {
                char temp;
                temp = destination[w];
                destination[w] = destination[pos - 1 - w];
                destination[pos - 1 - w] = temp;
            }
            /*Codes_SRS_CRT_ABSTRACTIONS_02_004: [If the conversion has been successfull then unsignedIntToString shall return 0.] */
            result = 0;
        }
        else
        {
            /*Codes_SRS_CRT_ABSTRACTIONS_02_002: [If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.] */
            result = MU_FAILURE;
        }
    }
    return result;
}

/*takes "value" and transforms it into a decimal string*/
/*10 => "10"*/
/*return 0 when everything went ok*/
int uint64_tToString(char* destination, size_t destinationSize, uint64_t value)
{
    int result;
    size_t pos;
    /*the below loop gets the number in reverse order*/
    if (
        (destination == NULL) ||
        (destinationSize < 2) /*because the smallest number is '0\0' which requires 2 characters*/
        )
    {
        result = MU_FAILURE;
    }
    else
    {
        pos = 0;
        do
        {
            destination[pos++] = '0' + (value % 10);
            value /= 10;
        } while ((value > 0) && (pos < (destinationSize - 1)));

        if (value == 0)
        {
            size_t w;
            destination[pos] = '\0';
            /*all converted and they fit*/
            for (w = 0; w <= (pos - 1) >> 1; w++)
            {
                char temp;
                temp = destination[w];
                destination[w] = destination[pos - 1 - w];
                destination[pos - 1 - w] = temp;
            }
            result = 0;
        }
        else
        {
            result = MU_FAILURE;
        }
    }
    return result;
}
