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
/*!****************************************************************************
 *  @file       ITMCC26XX.h
 *
 *  @brief      CC26XX specific ITM implementation
 *
 *  This file should only be included in the board file to fill out the object
 *  and hwAttrs structures
 *
 *  # CC26XX specific members #
 *  The CC26XX is capable of treating the pins used by the SWO interface as
 *  GPIOs. This means that the pins must be managed by the ITM driver when not
 *  in use by the TPIU.
 *
 */

#ifndef ti_drivers_itm_ITMCC26XX__include
#define ti_drivers_itm_ITMCC26XX__include

#include <stdint.h>

#include <ti/drivers/ITM.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ITM_BASE_HWATTRS
    uint8_t          swoPin;    /*!< IOID number of SWO pin */
} ITMCC26XX_HWAttrs;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_itm_ITMCC26XX__include */
