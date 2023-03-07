/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 */

/*
 *  ======== Timestamp_tirtos.c ========
 */

#include <stdint.h>

#include <ti/sysbios/BIOS.h>
#if defined(ti_sysbios_BIOS_version) && (ti_sysbios_BIOS_version <= 0x6FFFF)

#include <ti/utils/osal/Timestamp.h>
#include <xdc/runtime/Types.h>
#define xdc_runtime_Timestamp__nolocalnames
#include <xdc/runtime/Timestamp.h>

/*
 *  ======== Timestamp_get64 ========
 */
void Timestamp_get64(Timestamp_Val64 *ts)
{
    xdc_runtime_Timestamp_get64((Types_Timestamp64 *)ts);
}

/*
 *  ======== Timestamp_init ========
 */
void Timestamp_init(void)
{
    /* nop */
}

#else

#define ti_utils_osal_Timestamp__nolocalnames
#include <ti/utils/osal/Timestamp.h>
#include <ti/sysbios/runtime/Types.h>
#include <ti/sysbios/runtime/Timestamp.h>

/*
 *  ======== Timestamp_get64 ========
 */
void ti_utils_osal_Timestamp_get64(ti_utils_osal_Timestamp_Val64 *ts)
{
    Timestamp_get64((Types_Timestamp64 *)ts);
}

/*
 *  ======== ti_utils_osal_Timestamp_init ========
 */
void ti_utils_osal_Timestamp_init(void)
{
    /* nop */
}
#endif
