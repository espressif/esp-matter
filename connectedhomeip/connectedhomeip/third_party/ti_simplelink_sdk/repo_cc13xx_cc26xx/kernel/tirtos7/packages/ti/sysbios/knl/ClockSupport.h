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
 * ======== ClockSupport.h ========
 */

#ifndef ti_sysbios_knl_ClockSupport__include
#define ti_sysbios_knl_ClockSupport__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ti_sysbios_knl_ClockSupport_long_names
#include <ti/sysbios/knl/ClockSupport_defs.h>

extern uint32_t ClockSupport_getCurrentTick(bool save);
extern void ClockSupport_getFreq(Types_FreqHz *freq);
extern uint32_t ClockSupport_getMaxTicks(void);
extern uint32_t ClockSupport_getPeriod(void);
extern void ClockSupport_init(void);
extern void ClockSupport_setNextTick(uint32_t ticks);
extern bool ClockSupport_setPeriodMicroSecs(uint32_t tickPeriod);
extern void ClockSupport_start(void);
extern void ClockSupport_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_knl_ClockSupport__include */

#undef ti_sysbios_knl_ClockSupport_long_names
#include <ti/sysbios/knl/ClockSupport_defs.h>
