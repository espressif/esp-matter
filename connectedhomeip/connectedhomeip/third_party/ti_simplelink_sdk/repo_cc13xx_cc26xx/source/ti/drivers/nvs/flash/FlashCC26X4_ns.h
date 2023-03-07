/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
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
/*!*****************************************************************************
 *  @file       FlashCC26X4_ns.h
 *  @brief      Non-Secure Flash Client Interface for CC26X4 devices.
 *
 *  The defined APIs are identical to the function calls in flash.c. This client
 *  interface allows the access to the Secure Flash Service. 
 * 
 *  # Initialization #
 *  Users will need to call FlashOpen() once prior to any other function calls.
 *  FlashOpen() will only need to be called again if FlashClose() is called.
 *
 *  # Error Codes #
 *  Typically the Flash Driver (flash.c) returns either a FAPI_STATUS code
 *  or a valid value greater than the FAPI_STATUS code range. FAPI_STATUS codes
 *  are positive and are defined below in FlashCC26X4_ns.h. The Non-Secure Flash
 *  Client Interface is responsible for interfacing with the Secure Partition
 *  Manager (SPM). Requests to the SPM may result in Platform Security
 *  Architecture (PSA) error codes. These error codes are referred to as
 *  PSA_ERROR codes. PSA_ERROR codes are negative values and are defined in
 *  "error.h". Due to the FAPI_STATUS and PSA_ERROR codes not overlapping,
 *  API's of the Non-Secure Flash Client Interface can dynamically return either
 *  value type. PSA_ERROR codes are considered higher priority and will take
 *  precedence. Therefore, if a PSA_ERROR occurs, a PSA_ERROR is returned.
 *  Otherwise, a valid Flash driver value will be returned. Each API will follow
 *  this guideline but please reference each API individually for specific
 *  return characteristics.
 * 
 *  # PSA_ERROR_PROGRAMMER_ERROR #
 *  A PSA_ERROR_PROGRAMMER_ERROR is a strict error code returned by the SPM.
 *  Once this error code is returned, the SPM will terminate the connection
 *  to the service. All subsequent requests to the service from the same client
 *  will not be processed and will continue to return a 
 *  PSA_ERROR_PROGRAMMER_ERROR. The client must close the connection to the SPM
 *  and then re-establish the connection to continue. This can be accomplished 
 *  in the Non-Secure Flash Client Interface by calling FlashClose() followed by
 *  FlashOpen(). In the Non-Secure Flash Client Interface,
 *  PSA_ERROR_PROGRAMMER_ERROR codes will primarily be returned due to requests
 *  trying to read from or write to secure memory regions. Please reference the
 *  PSA Firmware Framework specification for more details regarding the
 *  PSA_ERROR_PROGRAMMER_ERROR code.
 * 
 *  ============================================================================
 */

#ifndef ti_drivers_nvs_flash_FlashCC26X4_ns_H__
#define ti_drivers_nvs_flash_FlashCC26X4_ns_H__

#include <stddef.h>
#include <stdint.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_nvmnw.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(cmsis/core/cmsis_compiler.h)

#ifdef __cplusplus
extern "C" {
#endif

/* *************************************************************************** */
/* Values that can be returned from the API functions                          */
/* *************************************************************************** */
#define FAPI_STATUS_SUCCESS                     0x00000000 // Function completed successfully
#define FAPI_STATUS_FSM_BUSY                    0x00000001 // FSM is Busy
#define FAPI_STATUS_FSM_READY                   0x00000002 // FSM is Ready
#define FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH 0x00000003 // Incorrect parameter value
#define FAPI_STATUS_FSM_ERROR                   0x00000004 // Operation failed
#define FAPI_STATUS_ADDRESS_ERROR               0x00000005 // Operation failed
#define FAPI_STATUS_ADDRESS_ALIGN_ERROR         0x00000006 // addr alignment error
#define FAPI_STATUS_BANK_NUM_ERROR              0x00000007 // bank number error
#define FAPI_STATUS_NOSUPPORT_ERROR             0x00000008 // Flash API is not supported
#define FAPI_STATUS_UNPROTECT_ERROR             0x00000009 // can't unprotect for saved setting

/*!
 *  @brief      Initialize a PSA connection to the Secure Flash Service
 *
 *  @pre        None
 *
 *  @return     A value greater than 0 indicates the value of a successfuly
 *              established connection handle. On failure a PSA_ERROR code
 *              will be returned.
 */
int_fast16_t FlashOpen(void);

/*!
 *  @brief      End the PSA connection to the Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @return     void
 */
void FlashClose(void);

/*!
 *  @brief      Call FlashProtectionGet API using Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @param[in]  ui32SectorAddress    The start address of the sector to protect.
 *
 *  @return     Returns the sector protection value (0 or 1) on success or FAPI_STATUS
 *              code on failure. If a PSA error occurs this will take precedence and
 *              a PSA_ERROR code will be returned instead.
 */
uint32_t FlashProtectionGet(uint32_t ui32SectorAddress);

/*!
 *  @brief      Call FlashSectorErase API using Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @param[in]  ui32SectorAddress    The start address of the sector to erase.
 *
 *  @return     Returns an FAPI_STATUS status code based on status of erase operation.
 *              If a PSA error occurs this will take precedence and a PSA_ERROR code
 *              will be returned instead.
 */
uint32_t FlashSectorErase(uint32_t ui32SectorAddress);

/*!
 *  @brief      Call FlashProgram API using Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @param[in]  pui8DataBuffer    Pointer to the data to be programmed.
 *  @param[in]  ui32Address       Starting address in flash to be programmed.
 *  @param[in]  ui32Count         Number of bytes to be programmed.
 *
 *  @return     Returns an FAPI_STATUS status code based on status of flash operation.
 *              If a PSA error occurs this will take precedence and a PSA_ERROR code
 *              will be returned instead.
 */
uint32_t FlashProgram(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count);

/*!
 *  @brief      Call FlashProgram4X API using Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @param[in]  pui8DataBuffer    Pointer to the data to be programmed.
 *  @param[in]  ui32Address       Starting address in flash to be programmed.
 *  @param[in]  ui32Count         Number of bytes to be programmed.
 *
 *  @return     Returns an FAPI_STATUS status code based on status of flash operation.
 *              If a PSA error occurs this will take precedence and a PSA_ERROR code
 *              will be returned instead.
 */
uint32_t FlashProgram4X(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count);

/*!
 *  @brief      Call FlashSectorSizeGet API using Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @return     Returns the size of Flash Sector on success or an FAPI_STATUS status
 *              code on failure. Values greater than FAPI_STATUS_UNPROTECT_ERROR will
 *              be considered a valid Flash Sector size. If a PSA error occurs this
 *              will take precedence and a PSA_ERROR code will be returned instead.
 */
uint32_t FlashSectorSizeGet(void);

/*!
 *  @brief      Call FlashSizeGet API using Secure Flash Service
 *
 *  @pre        FlashOpen() was called.
 *
 *  @return     Returns the size of Flash on success or an FAPI_STATUS status
 *              code on failure. Values greater than FAPI_STATUS_UNPROTECT_ERROR will
 *              be considered a valid Flash size. If a PSA error occurs this
 *              will take precedence and a PSA_ERROR code will be returned instead.
 */
uint32_t FlashSizeGet(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_nvs_flash_FlashCC26X4_ns_H__ */