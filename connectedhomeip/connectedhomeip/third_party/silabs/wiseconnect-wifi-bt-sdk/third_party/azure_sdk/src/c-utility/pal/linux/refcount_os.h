// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file gets included into refcount.h as a means of extending the behavior of
// atomic increment, decrement, and test.
#ifndef REFCOUNT_OS_H__LINUX
#define REFCOUNT_OS_H__LINUX


// This Linux-specific header offers 3 strategies:
//   REFCOUNT_ATOMIC_DONTCARE     -- no atomicity guarantee
//   REFCOUNT_USE_STD_ATOMIC      -- C11 atomicity
//   REFCOUNT_USE_GNU_C_ATOMIC    -- GNU-specific atomicity

#if defined(__GNUC__)
#define REFCOUNT_USE_GNU_C_ATOMIC 1
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ == 201112)
#define REFCOUNT_USE_STD_ATOMIC 1
#undef REFCOUNT_USE_GNU_C_ATOMIC
#endif

// This FREERTOS_ARCH_ESP8266 behavior is deprecated. Microcontrollers
// using the Azure IoT C SDK should include the c-utility file
// pal/generic/refcount_os.h instead of this one.
#if defined(FREERTOS_ARCH_ESP8266)
#define REFCOUNT_ATOMIC_DONTCARE 1
#undef REFCOUNT_USE_STD_ATOMIC
#undef REFCOUNT_USE_GNU_C_ATOMIC
#endif

#if defined(REFCOUNT_ATOMIC_DONTCARE)
#define COUNT_TYPE uint32_t
#elif defined(REFCOUNT_USE_STD_ATOMIC)
#define COUNT_TYPE _Atomic uint32_t
#else  // REFCOUNT_USE_GNU_C_ATOMIC
#define COUNT_TYPE uint32_t
#endif // defined(REFCOUNT_ATOMIC_DONTCARE)


/*the following macros increment/decrement a ref count in an atomic way, depending on the platform*/
/*The following mechanisms are considered in this order
REFCOUNT_ATOMIC_DONTCARE does not use atomic operations
- will result in ++/-- used for increment/decrement.
C11
- will result in #include <stdatomic.h>
- will use atomic_fetch_add/sub;
- about the return value: "Atomically, the value pointed to by object immediately before the effects"
gcc
- will result in no include (for gcc these are intrinsics build in)
- will use __sync_fetch_and_add/sub
- about the return value: "... returns the value that had previously been in memory." (https://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html#Atomic-Builtins)
*/


/*if macro DEC_REF returns DEC_RETURN_ZERO that means the ref count has reached zero.*/
#if defined(REFCOUNT_ATOMIC_DONTCARE)
#define DEC_RETURN_ZERO (0)
#define INC_REF_VAR(count) ++(count)
#define DEC_REF_VAR(count) --(count)
#define INIT_REF_VAR(count) do { count = 1; } while((void)0,0)

#elif defined(REFCOUNT_USE_STD_ATOMIC)
#include <stdatomic.h>
#define DEC_RETURN_ZERO (1)
#define INC_REF_VAR(count) atomic_fetch_add(&(count), 1)
#define DEC_REF_VAR(count) atomic_fetch_sub(&(count), 1)
#define INIT_REF_VAR(count) atomic_store(&(count), 1)

#elif defined(REFCOUNT_USE_GNU_C_ATOMIC)
#define DEC_RETURN_ZERO (0)
#define INC_REF_VAR(count) __sync_add_and_fetch(&(count), 1)
#define DEC_REF_VAR(count) __sync_sub_and_fetch(&(count), 1)
#define INIT_REF_VAR(count) do { count = 1; __sync_synchronize(); } while((void)0,0)

#endif /*defined(REFCOUNT_USE_GNU_C_ATOMIC)*/

#endif // REFCOUNT_OS_H__LINUX
