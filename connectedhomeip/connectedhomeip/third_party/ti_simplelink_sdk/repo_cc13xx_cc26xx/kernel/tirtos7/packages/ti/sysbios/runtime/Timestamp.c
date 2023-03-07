/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * ======== Timestamp.c ========
 */

#include <ti/sysbios/runtime/Types.h>
#include <ti/sysbios/runtime/Timestamp.h>

extern uint32_t TimestampProvider_get32_D(void);
extern void TimestampProvider_get64_D(Types_Timestamp64 *timestamp64);
extern void TimestampProvider_getFreq_D(Types_FreqHz *freq);
extern void TimestampProvider_init_D(void);

/*
 *  ======== Timestamp_get32 ========
 */
uint32_t Timestamp_get32(void)
{
    return (TimestampProvider_get32_D());
}

/*
 *  ======== Timestamp_get64 ========
 */
void Timestamp_get64(Types_Timestamp64 *timestamp64)
{
    TimestampProvider_get64_D(timestamp64);
}

/*
 *  ======== Timestamp_getFreq ========
 */
void Timestamp_getFreq(Types_FreqHz *freq)
{
    TimestampProvider_getFreq_D(freq);
}

void Timestamp_init(void)
{
    TimestampProvider_init_D();
}
