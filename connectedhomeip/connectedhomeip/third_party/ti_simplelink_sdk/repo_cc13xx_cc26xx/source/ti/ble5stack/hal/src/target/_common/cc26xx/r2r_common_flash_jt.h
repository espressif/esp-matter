/******************************************************************************

 @file  r2r_common_flash_jt.h

 @brief This file contains the defines for every Common function which can be
        mapped to either itself (for Flash-Only build), or to jump table offset
        in flash (ROM build). The latter can be used to relocate any function
        to flash in the event that software needs to be replaced.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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

#ifndef R2R_COMMON_FLASH_JT_H
#define R2R_COMMON_FLASH_JT_H

#if defined( COMMON_ROM_BUILD )

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"

/*******************************************************************************
 * EXTERNS
 */

// Common ROM's RAM table for pointers to ICall functions and flash jump tables.
// Note: This linker imported symbol is treated as a variable by the compiler.
// 0: iCall Dispatch Function Pointer
// 1: iCall Enter Critical Section Function Pointer
// 2: iCall Leave Critical Section Function Pointer
// 3: R2F Flash Jump Table Pointer
// 4: R2R Flash Jump Table Pointer
extern uint32 COMMON_RAM_BASE_ADDR[];

/*******************************************************************************
 * CONSTANTS
 */

// Common ROM's RAM table offset to R2R flash jump table pointer.
#define COMMON_ROM_RAM_TABLE_R2R           4

// Defines used for the flash jump table routines that are not part of build.
// Note: Any change to this table must accompany a change to Flash_JT[]!
#define R2R_JT_LOC_CROM                    (uint32 *)(&COMMON_RAM_BASE_ADDR[0]+COMMON_ROM_RAM_TABLE_R2R)

#define R2R_JT_BASE_CROM                   (*((uint32 **)R2R_JT_LOC_CROM))
#define R2R_JT_OFFSET_CROM(index)          (*(R2R_JT_BASE_CROM+(index)))

#define MAP_MB_Init                        ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(0))
#define MAP_MB_EnableInts                  ((void          (*) (uint32, uint32, uint32))                                                           R2R_JT_OFFSET_CROM(1))
#define MAP_MB_DisableInts                 ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(2))
#define MAP_MB_ClearInts                   ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(3))
#define MAP_MB_EnableHWInts                ((void          (*) (uint32))                                                                           R2R_JT_OFFSET_CROM(4))
#define MAP_MB_ReadMailboxStatus           ((uint32        (*) (void))                                                                             R2R_JT_OFFSET_CROM(5))
#define MAP_MB_SendCommand                 ((uint8         (*) (uint32))                                                                           R2R_JT_OFFSET_CROM(6))
#define MAP_MB_SendCommandSynch            ((uint8         (*) (uint32))                                                                           R2R_JT_OFFSET_CROM(7))
#define MAP_MB_RegisterIsrCback            ((void          (*) (uint8, mbIntCback_t))                                                              R2R_JT_OFFSET_CROM(8))
#define MAP_MB_FwDebugDump                 ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(9))
//
#define MAP_mbCmdAckIsr                    ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(10))
#define MAP_mbCpe0Isr                      ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(11))
#define MAP_mbCpe1Isr                      ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(12))
#define MAP_mbHwIsr                        ((void          (*) (void))                                                                             R2R_JT_OFFSET_CROM(13))
//
#define MAP_RFHAL_InitRfHal                ((uint8         (*) (void))                                                                             R2R_JT_OFFSET_CROM(14))
#define MAP_RFHAL_AllocRatChan             ((uint8         (*) (void))                                                                             R2R_JT_OFFSET_CROM(15))
#define MAP_RFHAL_FreeRatChan              ((uint8         (*) (uint8))                                                                            R2R_JT_OFFSET_CROM(16))
#define MAP_RFHAL_MapRatChansToInt         ((uint32        (*) (void))                                                                             R2R_JT_OFFSET_CROM(17))
#define MAP_RFHAL_RegisterRatChanCallback  ((uint8         (*) (uint8, ratChanCBack_t))                                                            R2R_JT_OFFSET_CROM(18))
#define MAP_RFHAL_AddTxDataEntry           ((uint8         (*) (dataEntryQ_t *, dataEntry_t *, uint8))                                             R2R_JT_OFFSET_CROM(19))
#define MAP_RFHAL_InitDataQueue            ((void          (*) (dataEntryQ_t *))                                                                   R2R_JT_OFFSET_CROM(20))
#define MAP_RFHAL_FreeNextTxDataEntry      ((void          (*) (dataEntryQ_t *))                                                                   R2R_JT_OFFSET_CROM(21))
#define MAP_RFHAL_AllocDataEntryQueue      ((dataEntryQ_t *(*) (dataEntry_t *, dataEntry_t *))                                                     R2R_JT_OFFSET_CROM(22))
#define MAP_RFHAL_FreeDataEntryQueue       ((void          (*) (dataEntryQ_t *))                                                                   R2R_JT_OFFSET_CROM(23))
#define MAP_RFHAL_GetNextDataEntry         ((dataEntry_t * (*) (dataEntryQ_t *))                                                                   R2R_JT_OFFSET_CROM(24))
#define MAP_RFHAL_GetTempDataEntry         ((dataEntry_t * (*) (dataEntryQ_t *))                                                                   R2R_JT_OFFSET_CROM(25))
#define MAP_RFHAL_NextDataEntryDone        ((void          (*) (dataEntryQ_t *))                                                                   R2R_JT_OFFSET_CROM(26))
#define MAP_RFHAL_AllocDataEntry           ((uint8 *       (*) (uint8, uint8, uint16))                                                             R2R_JT_OFFSET_CROM(27))
#define MAP_RFHAL_FreeDataEntry            ((void          (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(28))
#define MAP_RFHAL_BuildRingBuffer          ((uint8 *       (*) (uint8, uint8, uint8, uint16))                                                      R2R_JT_OFFSET_CROM(29))
#define MAP_RFHAL_BuildDataEntryRingBuffer ((dataEntry_t * (*) (uint8, uint8, uint16, uint8))                                                      R2R_JT_OFFSET_CROM(30))
#define MAP_RFHAL_BuildLinkedBuffer        ((uint8 *       (*) (uint8, uint8, uint8, uint16))                                                      R2R_JT_OFFSET_CROM(31))
//AES
#define MAP_ssp_KeyInit_Sw                  ((void         (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(32))
#define MAP_sspAesEncrypt_Sw                ((void         (*) (uint8 *, uint8 *))                                                                 R2R_JT_OFFSET_CROM(33))
#define MAP_sspKeyExpansion_Sw              ((void         (*) (uint8 *, uint8 *))                                                                 R2R_JT_OFFSET_CROM(34))
#define MAP_sspAesEncryptKeyExp_Sw          ((void         (*) (uint8 *, uint8 *))                                                                 R2R_JT_OFFSET_CROM(35))
#define MAP_sspAesEncryptBasic_Sw           ((void         (*) (uint8 *, uint8 *))                                                                 R2R_JT_OFFSET_CROM(36))
#define MAP_RoundKey_Sw                     ((void         (*) (uint8 *, uint8))                                                                   R2R_JT_OFFSET_CROM(37))
#define MAP_AddRoundKeySubBytes_Sw          ((void         (*) (uint8 *, uint8 *))                                                                 R2R_JT_OFFSET_CROM(38))
#define MAP_ShiftRows_Sw                    ((void         (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(39))
#define MAP_MixColumns_Sw                   ((void         (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(40))
#define MAP_FFMult_Sw                       ((uint8        (*) (uint8, uint8))                                                                     R2R_JT_OFFSET_CROM(41))
#define MAP_sspAesDecrypt_Sw                ((void         (*) (uint8 *, uint8 *))                                                                 R2R_JT_OFFSET_CROM(42))
#define MAP_InvAddRoundKey_Sw               ((void         (*) (uint8, uint8 *, uint8 *, uint8 *))                                                 R2R_JT_OFFSET_CROM(43))
#define MAP_InvRoundKey_Sw                  ((void         (*) (uint8 *, uint8, uint8 *))                                                          R2R_JT_OFFSET_CROM(44))
#define MAP_InvSubBytes_Sw                  ((void         (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(45))
#define MAP_InvShiftRows_Sw                 ((void         (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(46))
#define MAP_InvMixColumns_Sw                ((void         (*) (uint8 *))                                                                          R2R_JT_OFFSET_CROM(47))
//CCM
#define MAP_SSP_CCM_Auth_Sw                 ((uint8        (*) (uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8*, uint8))         R2R_JT_OFFSET_CROM(48))
#define MAP_SSP_CCM_Encrypt_Sw              ((uint8        (*) (uint8, uint8 *, uint8 *, uint16, uint8 *, uint8 *, uint8))                         R2R_JT_OFFSET_CROM(49))
#define MAP_SSP_CCM_Decrypt_Sw              ((uint8        (*) (uint8, uint8 *, uint8 *, uint16, uint8 *, uint8 *, uint8))                         R2R_JT_OFFSET_CROM(50))
#define MAP_SSP_CCM_InvAuth_Sw              ((uint8        (*) (uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8))        R2R_JT_OFFSET_CROM(51))
#define MAP_SSP_CCM_Auth_Encrypt_Sw         ((uint8        (*) (uint8, uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8)) R2R_JT_OFFSET_CROM(52))
#define MAP_SSP_CCM_InvAuth_Decrypt_Sw      ((uint8        (*) (uint8, uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 * ,uint8 *, uint8)) R2R_JT_OFFSET_CROM(53))
#define MAP_SSP_CTR_Decrypt_Sw              ((uint8        (*) (uint8 *, uint16, uint8 *, uint8 *, uint8 *))                                       R2R_JT_OFFSET_CROM(54))
#define MAP_SSP_CTR_Encrypt_Sw              ((uint8        (*) (uint8 *, uint16, uint8 *, uint8 *, uint8 *))                                       R2R_JT_OFFSET_CROM(55))
//AES-CCM
#define MAP_AesCcm_getNumBlocks_Sw          ((uint16       (*) (uint16))                                                                           R2R_JT_OFFSET_CROM(56))
#define MAP_Aes_encrypt_Sw                  ((void         (*) (uint8 *, uint16, uint8 *))                                                         R2R_JT_OFFSET_CROM(57))
#define MAP_Aes_decrypt_Sw                  ((void         (*) (uint8 *, uint16, uint8 *))                                                         R2R_JT_OFFSET_CROM(58))
#define MAP_AesCcm_authEncrypt_Sw           ((signed char  (*) (uint8, uint8, uint8 *, uint8 *, uint8, uint8 *, uint8, uint8 *, uint8 *, uint8))   R2R_JT_OFFSET_CROM(59))
#define MAP_AesCcm_decryptAuth_Sw           ((signed char  (*) (uint8, uint8, uint8 *, uint8 *, uint8, uint8 *, uint8, uint8 *, uint8 *, uint8))   R2R_JT_OFFSET_CROM(60))
#else // no R2R flash JT

#define MAP_MB_Init                         MB_Init
#define MAP_MB_EnableInts                   MB_EnableInts
#define MAP_MB_DisableInts                  MB_DisableInts
#define MAP_MB_ClearInts                    MB_ClearInts
#define MAP_MB_EnableHWInts                 MB_EnableHWInts
#define MAP_MB_ReadMailboxStatus            MB_ReadMailboxStatus
#define MAP_MB_SendCommand                  MB_SendCommand
#define MAP_MB_SendCommandSynch             MB_SendCommandSynch
#define MAP_MB_RegisterIsrCback             MB_RegisterIsrCback
#define MAP_MB_FwDebugDump                  MB_FwDebugDump
#define MAP_mbCmdAckIsr                     mbCmdAckIsr
#define MAP_mbCpe0Isr                       mbCpe0Isr
#define MAP_mbCpe1Isr                       mbCpe1Isr
#define MAP_mbHwIsr                         mbHwIsr
//
#define MAP_RFHAL_InitRfHal                 RFHAL_InitRfHal
#define MAP_RFHAL_AllocRatChan              RFHAL_AllocRatChan
#define MAP_RFHAL_FreeRatChan               RFHAL_FreeRatChan
#define MAP_RFHAL_MapRatChansToInt          RFHAL_MapRatChansToInt
#define MAP_RFHAL_RegisterRatChanCallback   RFHAL_RegisterRatChanCallback
#define MAP_RFHAL_AddTxDataEntry            RFHAL_AddTxDataEntry
#define MAP_RFHAL_InitDataQueue             RFHAL_InitDataQueue
#define MAP_RFHAL_FreeNextTxDataEntry       RFHAL_FreeNextTxDataEntry
#define MAP_RFHAL_AllocDataEntryQueue       RFHAL_AllocDataEntryQueue
#define MAP_RFHAL_FreeDataEntryQueue        RFHAL_FreeDataEntryQueue
#define MAP_RFHAL_GetNextDataEntry          RFHAL_GetNextDataEntry
#define MAP_RFHAL_GetTempDataEntry          RFHAL_GetTempDataEntry
#define MAP_RFHAL_NextDataEntryDone         RFHAL_NextDataEntryDone
#define MAP_RFHAL_AllocDataEntry            RFHAL_AllocDataEntry
#define MAP_RFHAL_FreeDataEntry             RFHAL_FreeDataEntry
#define MAP_RFHAL_BuildRingBuffer           RFHAL_BuildRingBuffer
#define MAP_RFHAL_BuildDataEntryRingBuffer  RFHAL_BuildDataEntryRingBuffer
#define MAP_RFHAL_BuildLinkedBuffer         RFHAL_BuildLinkedBuffer
//AES
#define MAP_ssp_KeyInit_Sw                  ssp_KeyInit_Sw
#define MAP_sspAesEncrypt_Sw                sspAesEncrypt_Sw
#define MAP_sspKeyExpansion_Sw              spKeyExpansion_Sw
#define MAP_sspAesEncryptKeyExp_Sw          sspAesEncryptKeyExp_Sw
#define MAP_sspAesEncryptBasic_Sw           sspAesEncryptBasic_Sw
#define MAP_RoundKey_Sw                     RoundKey_Sw
#define MAP_AddRoundKeySubBytes_Sw          AddRoundKeySubBytes_Sw
#define MAP_ShiftRows_Sw                    ShiftRows_Sw
#define MAP_MixColumns_Sw                   MixColumns_Sw
#define MAP_FFMult_Sw                       FFMult_Sw
#define MAP_sspAesDecrypt_Sw                sspAesDecrypt_Sw
#define MAP_InvAddRoundKey_Sw               InvAddRoundKey_Sw
#define MAP_InvRoundKey_Sw                  InvRoundKey_Sw
#define MAP_InvSubBytes_Sw                  InvSubBytes_Sw
#define MAP_InvShiftRows_Sw                 InvShiftRows_Sw
#define MAP_InvMixColumns_Sw                InvMixColumns_Sw
//CCM
#define MAP_SSP_CCM_Auth_Sw                 SSP_CCM_Auth_Sw
#define MAP_SSP_CCM_Encrypt_Sw              SSP_CCM_Encrypt_Sw
#define MAP_SSP_CCM_Decrypt_Sw              SSP_CCM_Decrypt_Sw
#define MAP_SSP_CCM_InvAuth_Sw              SSP_CCM_InvAuth_Sw
#define MAP_SSP_CCM_Auth_Encrypt_Sw         SSP_CCM_Auth_Encrypt_Sw
#define MAP_SSP_CCM_InvAuth_Decrypt_Sw      SSP_CCM_InvAuth_Decrypt_Sw
#define MAP_SSP_CTR_Decrypt_Sw              SSP_CTR_Decrypt_Sw
#define MAP_SSP_CTR_Encrypt_Sw              SSP_CTR_Encrypt_Sw
//AES-CCM
#define MAP_AesCcm_getNumBlocks_Sw          AesCcm_getNumBlocks_Sw
#define MAP_Aes_encrypt_Sw                  Aes_encrypt_Sw
#define MAP_Aes_decrypt_Sw                  Aes_decrypt_Sw
#define MAP_AesCcm_authEncrypt_Sw           AesCcm_authEncrypt_Sw
#define MAP_AesCcm_decryptAuth_Sw           AesCcm_decryptAuth_Sw
#endif // USE_COMMON_ROM || !FLASH_ONLY_BUILD

#endif /* R2R_COMMON_FLASH_JT_H */

