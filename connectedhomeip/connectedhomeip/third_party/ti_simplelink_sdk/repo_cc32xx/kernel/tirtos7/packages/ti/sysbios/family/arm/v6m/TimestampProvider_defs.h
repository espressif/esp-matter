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
 * ======== TimestampProvider_defs.h ========
 */

/*
 * Remove any previous short name definitions already provided by
 * other TimestampProviders in this compilation unit.
 */
#undef TimestampProvider_get32
#undef TimestampProvider_get64
#undef TimestampProvider_getFreq
#undef TimestampProvider_init
#undef TimestampProvider_module
#undef TimestampProvider_Module_State
#undef TimestampProvider_Module_state
#undef TimestampProvider_rolloverFunc
#undef TimestampProvider_timerStruct

/*
 * Map short names to long names
 */
#ifndef ti_sysbios_family_arm_v6m_TimestampProvider__nolocalnames
#define TimestampProvider_get32 ti_sysbios_family_arm_v6m_TimestampProvider_get32
#define TimestampProvider_get64 ti_sysbios_family_arm_v6m_TimestampProvider_get64
#define TimestampProvider_getFreq ti_sysbios_family_arm_v6m_TimestampProvider_getFreq
#define TimestampProvider_init ti_sysbios_family_arm_v6m_TimestampProvider_init
#define TimestampProvider_Module_State ti_sysbios_family_arm_v6m_TimestampProvider_Module_State
#define TimestampProvider_Module_state ti_sysbios_family_arm_v6m_TimestampProvider_Module_state
#define TimestampProvider_rolloverFunc ti_sysbios_family_arm_v6m_TimestampProvider_rolloverFunc
#define TimestampProvider_timerStruct ti_sysbios_family_arm_v6m_TimestampProvider_timerStruct
#endif

/* @endcond */
