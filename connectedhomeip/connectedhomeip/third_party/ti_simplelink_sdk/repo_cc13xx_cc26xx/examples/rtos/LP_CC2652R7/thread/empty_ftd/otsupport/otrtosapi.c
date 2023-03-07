/*
 * Copyright (c) 2017, Texas Instruments Incorporated
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *
 * This file contains the implementation of the stack protective mutex.
 */

#include "otrtosapi.h"

#include <assert.h>
#include <stdint.h>

#include <semaphore.h>

static sem_t OtRtosApi_sem;

/**
 * Initialize the RTOS mutex protecting the OpenThread APIs.
 *
 * Documented in ot_rtos_api.h.
 */
extern void OtRtosApi_init(void)
{
    int ret;

    ret = sem_init(&OtRtosApi_sem, 1, 0);
    assert(ret == 0);

    /* if assert is compiled out */
    (void)ret;
}

/**
 * Lock the OpenThread Stack mutex.
 *
 * Documented in ot_rtos_api.h.
 */
extern void OtRtosApi_lock(void)
{
    int ret;

    ret = sem_wait(&OtRtosApi_sem);
    assert(ret == 0);

    /* if assert is compiled out */
    (void)ret;
}

/**
 * Unlock the OpenThread Stack mutex.
 *
 * Documented in ot_rtos_api.h.
 */
extern void OtRtosApi_unlock(void)
{
    int ret;

    ret = sem_post(&OtRtosApi_sem);
    assert(ret == 0);

    /* if assert is compiled out */
    (void)ret;
}

