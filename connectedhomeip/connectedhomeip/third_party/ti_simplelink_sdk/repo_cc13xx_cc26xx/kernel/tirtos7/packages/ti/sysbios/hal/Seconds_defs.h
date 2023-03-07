/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

/* @cond LONG_NAMES */

/*
 * ======== Seconds_defs.h ========
 */

#ifdef ti_sysbios_hal_Seconds_long_names
#define Seconds_Time ti_sysbios_hal_Seconds_Time
#define Seconds_Module_State ti_sysbios_hal_Seconds_Module_State
#define Seconds_Module_state ti_sysbios_hal_Seconds_Module_state
#define Seconds_get ti_sysbios_hal_Seconds_get
#define Seconds_getCount ti_sysbios_hal_Seconds_getCount
#define Seconds_getTime ti_sysbios_hal_Seconds_getTime
#define Seconds_set ti_sysbios_hal_Seconds_set
#define Seconds_setTime ti_sysbios_hal_Seconds_setTime
#endif

#if defined(ti_sysbios_hal_Seconds__nolocalnames) && !defined(ti_sysbios_hal_Seconds_long_names)
#undef do_seconds_undef_short_names
#undef Seconds_Time
#undef Seconds_Module_State
#undef Seconds_Module_state
#undef Seconds_get
#undef Seconds_getCount
#undef Seconds_getTime
#undef Seconds_set
#undef Seconds_setTime
#endif

/* @endcond */

