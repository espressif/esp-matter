/*
 * Copyright (c) 2021, Texas Instruments Incorporated - http://www.ti.com
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
/*!*****************************************************************************
 *  @file       FlashCC26X4_s.h
 *  @brief      Secure Flash Service
 *
 *  @anchor ti_drivers_nvs_FlashCC26X4_s_Overview
 *  # Overview
 *      The Secure Flash Service is used to access the flash driver when using 
 *      SPE configuration. 
 *
 *******************************************************************************
 */

#ifndef ti_drivers_nvs_flash_FlashCC26X4_s__include
#define ti_drivers_nvs_flash_FlashCC26X4_s__include

#include <stddef.h>
#include <stdint.h>
#include <psa_manifest/sid.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * message types >= 16 are reserved for
 * non-standard message types begin here
 */
#define TI_SP_MSG_TYPE_USER                16

#define FLASH_SP_MSG_TYPE_PROTECTION_GET         TI_SP_MSG_TYPE_USER + 0
#define FLASH_SP_MSG_TYPE_SECTOR_ERASE           TI_SP_MSG_TYPE_USER + 1
#define FLASH_SP_MSG_TYPE_PROGRAM                TI_SP_MSG_TYPE_USER + 2
#define FLASH_SP_MSG_TYPE_PROGRAM_4X             TI_SP_MSG_TYPE_USER + 3
#define FLASH_SP_MSG_TYPE_SECTOR_SIZE_GET        TI_SP_MSG_TYPE_USER + 4
#define FLASH_SP_MSG_TYPE_FLASH_SIZE_GET         TI_SP_MSG_TYPE_USER + 5

struct FlashCC26X4_s_program
{
    uint8_t *dataBuffer;
    uint32_t sectorAddress;
    uint32_t count;
};

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_nvs_flash_FlashCC26X4_s__include */