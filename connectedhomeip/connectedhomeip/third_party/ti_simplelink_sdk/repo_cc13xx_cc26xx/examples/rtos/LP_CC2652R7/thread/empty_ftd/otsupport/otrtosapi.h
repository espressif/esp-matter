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
 * This file contains the definitions of the stack protective mutex.
 */

#ifndef OT_RTOS_API_H_
#define OT_RTOS_API_H_

#include <stdint.h>

/**
 * Initialize the RTOS mutex protecting the OpenThread APIs.
 *
 * This function must be called before @ref OtRtosApi_lock and @ref OtRtosApi_unlock.
 */
extern void OtRtosApi_init(void);

/**
 * Lock the OpenThread Stack mutex.
 *
 * This function must be called before interacting with the OpenThread APIs.
 * This function will lock the mutex protecting the OpenThread Stack, and
 * assure single threaded access to the Stack code.
 *
 * The time between @ref OtRtosApi_lock and @ref OtRtosApi_unlock should be kept to
 * a minimum to avoid starving the stack of processing time.
 *
 * @sa OtRtosApi_unlock
 */
extern void OtRtosApi_lock(void);

/**
 * Unlock the openthread stack mutex.
 *
 * This function must be called after interacting with the OpenThread APIs.
 * This function will restore the mutex protecting the OpenThread Stack to the
 * state it was before @ref OtRtosApi_lock was called.
 *
 * The time between @ref OtRtosApi_lock and @ref OtRtosApi_unlock should be kept to
 * a minimum to avoid starving the stack of processing time.
 *
 * @sa OtRtosApi_lock
 */
extern void OtRtosApi_unlock(void);

#endif /* OT_RTOS_API_H_ */

