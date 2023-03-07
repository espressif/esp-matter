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
/*!
 * @file ti/sysbios/family/arm/cc26xx/TimestampProvider.h
 * @brief CC26xx RTC Timestamp Provider
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center"></colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th><th>  Main**  </th><th>  Startup***  </th></tr>
 *    <!--                                                                                                                 -->
 *    <tr><td> get32           </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> get64           </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getFreq         </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== TimestampProvider.h ========
 */

#ifndef ti_sysbios_family_arm_cc26xx_TimestampProvider__include
#define ti_sysbios_family_arm_cc26xx_TimestampProvider__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Types.h>

#include <ti/sysbios/family/arm/cc26xx/Timer.h>

#include "TimestampProvider_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! @cond NODOC */
typedef struct TimestampProvider_Module_State TimestampProvider_Module_State;

struct TimestampProvider_Module_State {
    Timer_Handle timer;
};
/*! @endcond */

/*!
 * @brief Return a 32-bit timestamp
 *
 * Returns a 32-bit timestamp value. Use TimestampProvider_getFreq
 * to convert this value into units of real time.
 *
 * @retval 32-bit timestamp
 */
extern uint32_t TimestampProvider_get32(void);

/*!
 * @brief Return a 64-bit timestamp
 *
 * This parameter is a pointer to a structure representing a 64-bit wide
 * timestamp value where the current timestamp is written.
 *
 * If the underlying hardware does not support 64-bit resolution, the `hi` field
 * of `result` is always set to 0.
 * So, it is possible for the `lo` field to wrap around without any
 * change to the `hi` field. Use TimestampProvider_getFreq to
 * convert this value into units of real time.
 *
 * @param timestamp64 pointer to 64-bit result
 */
extern void TimestampProvider_get64(Types_Timestamp64 *timestamp64);

/*!
 * @brief Get the timestamp timer's frequency (in Hz)
 *
 * This parameter is a pointer to a structure representing a 64-bit wide
 * frequency value where the timer's frequency (in Hz) is written.
 * This function provides a way of converting
 * timestamp values into units of real time.
 *
 * @param freq pointer to a 64-bit result
 */
extern void TimestampProvider_getFreq(Types_FreqHz *freq);

/*! @cond NODOC */
extern void TimestampProvider_init(void);

extern TimestampProvider_Module_State TimestampProvider_Module_state;

#define TimestampProvider_module ((TimestampProvider_Module_State *) &(TimestampProvider_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif
