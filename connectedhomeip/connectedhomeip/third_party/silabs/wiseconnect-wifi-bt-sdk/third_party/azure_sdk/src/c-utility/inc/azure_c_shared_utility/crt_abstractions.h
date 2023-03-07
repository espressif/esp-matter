// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CRT_ABSTRACTIONS_H
#define CRT_ABSTRACTIONS_H

#ifdef __cplusplus
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cmath>
#else // __cplusplus
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#endif // __cplusplus

#include "umock_c/umock_c_prod.h"

#ifdef _MSC_VER

#ifdef QUARKGALILEO
#define HAS_STDBOOL
#ifdef __cplusplus
typedef bool _Bool;
#else
/*galileo apparently has _Bool and bool as built in types*/
#endif
#endif // QUARKGALILEO

#define HAS_STDBOOL
#ifdef __cplusplus
/*because C++ doesn't do anything about _Bool... */
#define _Bool bool
#else // __cplusplus
#include <stdbool.h>
#endif // __cplusplus

#else //  _MSC_VER

#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
/*C99, C11 (including GNU 4.6) or C18 compiler */
#define HAS_STDBOOL
#include <stdbool.h>
#endif //  ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
#endif // __STDC_VERSION__
#endif //  _MSC_VER

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef HAS_STDBOOL
#ifdef __cplusplus
#define _Bool bool
#else // __cplusplus
typedef unsigned char _Bool;
typedef unsigned char bool;
#define false 0
#define true 1
#endif // __cplusplus
#endif // HAS_STDBOOL


/* Codes_SRS_CRT_ABSTRACTIONS_99_001:[The module shall not redefine the secure functions implemented by Microsoft CRT.] */
/* Codes_SRS_CRT_ABSTRACTIONS_99_040 : [The module shall still compile when building on a Microsoft platform.] */
/* Codes_SRS_CRT_ABSTRACTIONS_99_002: [CRTAbstractions module shall expose the following API]*/
#if defined (_MSC_VER) || defined (MINGW_HAS_SECURE_API)
#else // _MSC_VER || MINGW_HAS_SECURE_API

/* Adding definitions from errno.h & crtdefs.h */
#if !defined (_TRUNCATE)
#define _TRUNCATE ((size_t)-1)
#endif  /* !defined (_TRUNCATE) */

#if !defined STRUNCATE
#define STRUNCATE       80
#endif  /* !defined (STRUNCATE) */

int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src);
int strcat_s(char* dst, size_t dstSizeInBytes, const char* src);
int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t maxCount);
int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ...);
#endif // _MSC_VER || MINGW_HAS_SECURE_API

unsigned long long strtoull_s(const char* nptr, char** endPtr, int base);
float strtof_s(const char* nptr, char** endPtr);
long double strtold_s(const char* nptr, char** endPtr);

#ifdef _MSC_VER
#define stricmp _stricmp
#endif // _MSC_VER

MOCKABLE_FUNCTION(, int, mallocAndStrcpy_s, char**, destination, const char*, source);
MOCKABLE_FUNCTION(, int, unsignedIntToString, char*, destination, size_t, destinationSize, unsigned int, value);
MOCKABLE_FUNCTION(, int, size_tToString, char*, destination, size_t, destinationSize, size_t, value);
MOCKABLE_FUNCTION(, int, uint64_tToString, char*, destination, size_t, destinationSize, uint64_t, value);

/*following logic shall define the TOUPPER and ISDIGIT, we do that because the SDK is not happy with some Arduino implementation of it.*/
#define TOUPPER(c)      ((((c)>='a') && ((c)<='z'))?(c)-'a'+'A':c)
#define ISDIGIT(c)      ((((c)>='0') && ((c)<='9'))?1:0)

/*following logic shall define the ISNAN macro*/
/*if runing on Microsoft Visual C compiler, than ISNAN shall be _isnan*/
/*else if running on C99 or C11, ISNAN shall be isnan*/
/*else if running on C89 ... #error and inform user*/

#ifdef _MSC_VER
#define ISNAN _isnan
#else // _MSC_VER
#if defined __STDC_VERSION__
#if ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
/*C99, C11 (including GNU 4.6) or C18 compiler */
#define ISNAN isnan
#else //  ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
#error update this file to contain the latest C standard.
#endif // ((__STDC_VERSION__  == 199901L) || (__STDC_VERSION__ == 201000L) || (__STDC_VERSION__ == 201112L) || (__STDC_VERSION__ == 201710L))
#else // __STDC_VERSION__
#ifdef __cplusplus
/*C++ defines isnan... in C11*/
extern "C++" {
#define ISNAN std::isnan
}
#else // __cplusplus
#error unknown (or C89) compiler, provide ISNAN with the same meaning as isnan in C99 standard
#endif // __cplusplus

#endif // __STDC_VERSION__
#endif // _MSC_VER

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* CRT_ABSTRACTIONS_H */
