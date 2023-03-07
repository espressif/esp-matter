// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TESTMUTEX_H
#define TESTMUTEX_H

#ifdef _MSC_VER
#ifdef __cplusplus
extern "C" {
#endif

    typedef void* TEST_MUTEX_HANDLE;

    extern TEST_MUTEX_HANDLE testmutex_create(void);
    extern int testmutex_acquire(TEST_MUTEX_HANDLE mutex);
    extern int testmutex_release(TEST_MUTEX_HANDLE mutex);
    extern void testmutex_destroy(TEST_MUTEX_HANDLE mutex);

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER */

#endif /*MICROMOCKTESTMUTEX_H*/

