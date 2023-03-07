// Copyright (C) Microsoft Corporation. All rights reserved.

#include "windows.h"

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct TIMER_HANDLE_DATA_TAG* TIMER_HANDLE;

#include "umock_c/umock_c_prod.h"


MOCKABLE_FUNCTION(, TIMER_HANDLE, timer_create);
MOCKABLE_FUNCTION(, void, timer_start, TIMER_HANDLE, handle);
MOCKABLE_FUNCTION(, double, timer_get_elapsed, TIMER_HANDLE, timer);
MOCKABLE_FUNCTION(, double, timer_get_elapsed_ms, TIMER_HANDLE, timer);
MOCKABLE_FUNCTION(, void, timer_destroy, TIMER_HANDLE, timer);

#ifdef __cplusplus
}
#endif


#endif
