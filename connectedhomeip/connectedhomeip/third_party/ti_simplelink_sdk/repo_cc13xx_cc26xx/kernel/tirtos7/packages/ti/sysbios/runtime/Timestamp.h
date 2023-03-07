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
 * @file ti/sysbios/runtime/Timestamp.h
 * @brief Timestamp services
 *
 * This module provides a portable API for getting a 32 or 64-bit timestamp.
 *
 * This module calls a family-specific TimestampProvider function that 
 * does the real work.
 */

/*
 * ======== Timestamp.h ========
 */

#ifndef ti_sysbios_runtime_Timestamp__include
#define ti_sysbios_runtime_Timestamp__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Types.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_Timestamp_long_names
#include "Timestamp_defs.h"
/* @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief get 32-bit timestamp
 *
 * @returns 32-bit timestamp
 */
extern uint32_t Timestamp_get32(void);

/*!
 * @brief get 64-bit timestamp
 *
 * @param timestamp64 pointer to a 64-bit timestamp structure
 */
extern void Timestamp_get64(Types_Timestamp64 *timestamp64);

/*!
 * @brief get timestamp frequency
 *
 * Get the frequency of the timestamp counter in Hz. This
 * function fills in a Types_FreqHz  structure with the high
 * and low 64 bit frequency.
 *
 * @param freq pointer to a 64-bit frequency structure
 */
extern void Timestamp_getFreq(Types_FreqHz *freq);

/*! @cond NODOC */
extern void Timestamp_init(void);
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_runtime_Timestamp__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_Timestamp_long_names
#include "Timestamp_defs.h"
/*! @endcond */
