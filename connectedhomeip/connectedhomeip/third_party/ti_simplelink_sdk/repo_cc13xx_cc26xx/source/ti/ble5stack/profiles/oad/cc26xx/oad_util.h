/******************************************************************************

 @file  oad_util.h

 @brief This file contains OAD functions that can be shared between
        persistent application and user applications running either the
        oad_reset_service or the oad service.

        Note: these functions should be used in a TI-RTOS application
        with a BLE-Stack running

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef OAD_UTIL_H
#define OAD_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <profiles/oad/cc26xx/oad.h>
#include <common/cc26xx/sha2/SHA2CC26XX.h>
 /*********************************************************************
 * @fn      OADUtil_signCommand
 *
 * @brief   Verify the signer of an OAD command
 *
 * @param   payload - pointer to command payload
 * @param   len - length of payload
 * @param   inputECCWorkzone - pointer to ECC Workzone RAM
 * @param   inputSHAWorkzone - pointer to SHA Workzone RAM
 *
 * @return  OAD_SUCCESS or OAD_AUTH_FAIL
 */
#ifdef SECURITY
extern uint8_t OADUtil_signCommandECDSA(securityHdr_t * secHdr,
                                        signPld_ECDSA_P256_t *signPld,
                                        uint8_t *payload, uint16_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* OAD_UTIL_H */
