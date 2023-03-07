// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "testmutex.h"

#ifdef _MSC_VER

#include "windows.h"

TEST_MUTEX_HANDLE testmutex_create(void)
{
    return (TEST_MUTEX_HANDLE)CreateMutexW(NULL, FALSE, NULL);
}

int testmutex_acquire(TEST_MUTEX_HANDLE mutex)
{
    return (WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0) ? 0 : 1;
}

void testmutex_destroy(TEST_MUTEX_HANDLE mutex)
{
    (void)CloseHandle(mutex);
}

int testmutex_release(TEST_MUTEX_HANDLE mutex)
{   
    return ReleaseMutex(mutex);
}
#else

// XCode complains about a library with no exported functions
extern void dummy_function_to_silence_compiler_is_never_called()
{
}

#endif /* _MSC_VER */
