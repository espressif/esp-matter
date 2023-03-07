/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SECURE_BOOT_STAGE_DEFS_H
#define _SECURE_BOOT_STAGE_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file contains all of the definitions and structures used for the run-time Secure Boot.
*/

#include "cc_pal_mem.h"
#include "cc_crypto_boot_defs.h"
#include "cc_sec_defs.h"
#include "secureboot_parser_gen_defs.h"
#include "secureboot_general_hwdefs.h"
#include "bsv_crypto_driver.h"
#include "bsv_crypto_api.h"
#include "crypto_driver.h"
#include "bsv_hw_defs.h"
#include "sbrt_int_func.h"
#include "mbedtls_cc_mng_int.h"
#include "pka_hw_defs.h"

extern unsigned long gCcRegBase;

/* ROM ==> RT */
#define CC_BSV_CHIP_MANUFACTURE_LCS CC_MNG_LCS_CM
#define CC_BSV_DEVICE_MANUFACTURE_LCS   CC_MNG_LCS_DM
#define CC_BSV_SECURE_LCS       CC_MNG_LCS_SEC_ENABLED
#define CC_BSV_RMA_LCS          CC_MNG_LCS_RMA

#define UTIL_MemCopy(pDst, pSrc ,size)          \
    CC_PalMemCopy(pDst, pSrc, size)
#define UTIL_MemSet(pBuff, val, size )          \
    CC_PalMemSet(pBuff, val, size)
#define UTIL_MemCmp(pBuff1, pBuff2, size)       \
    SBRT_MemCmp(pBuff1, pBuff2, size)
#define UTIL_ReverseMemCopy(pDst, pSrc, size)       \
    SBRT_ReverseMemCopy(pDst, pSrc, size)
#define UTIL_ReverseBuff(pBuff, size)           \
    SBRT_ReverseMemCopy(pBuff, pBuff, size)

#define _CCSbImageLoadAndVerify(preHashflashRead_func, preHashUserContext, hwBaseAddress, isLoadFromFlash, isVerifyImage, cryptoMode, keyType, AESIv, pSwRecSignedData, pSwRecNoneSignedData, workspace_ptr, workspaceSize) \
    SBRT_ImageLoadAndVerify(preHashflashRead_func, preHashUserContext, hwBaseAddress, isLoadFromFlash, isVerifyImage, cryptoMode, keyType, AESIv, pSwRecSignedData, pSwRecNoneSignedData, workspace_ptr, workspaceSize)

#define _RSA_PSS_Verify(hwBaseAddress, mHash, pN, pNp, pSign)   \
    SBRT_RSA_PSS_Verify(hwBaseAddress, mHash, pN, pNp, pSign)

#define CC_BsvLcsGet(hwBaseAddress, pLcs) \
    SBRT_LcsGet(hwBaseAddress, pLcs)

#define SB_HAL_WRITE_REGISTER(addr,val) \
        ((*((volatile uint32_t*)(gCcRegBase + addr))) = (unsigned long)(val))
#define SB_HAL_READ_REGISTER(addr,val)  \
        ((val) = (*((volatile uint32_t*)(gCcRegBase + addr))))

#define SB_HalClearInterruptBit(hwBaseAddress, data) \
    SBRT_HalClearInterruptBit(hwBaseAddress, data)

#define SB_HalMaskInterrupt(hwBaseAddress, data) \
    SBRT_HalMaskInterrupt(hwBaseAddress, data)

#define SB_HalWaitInterrupt(hwBaseAddress, data) \
    SBRT_HalWaitInterrupt(hwBaseAddress, data)

#define CC_BsvOTPWordRead(hwBaseAddress, otpAddress, pOtpWord) \
    SBRT_OTPWordRead(hwBaseAddress, otpAddress, pOtpWord)

#define CC_BsvSwVersionGet(hwBaseAddress, keyIndex, swVersion)  \
    SBRT_SwVersionGet(hwBaseAddress, keyIndex, swVersion)

#define CC_BsvPubKeyHashGet(hwBaseAddress, keyIndex, hashedPubKey, hashResultSizeWords) \
    SBRT_PubKeyHashGet(hwBaseAddress, keyIndex, hashedPubKey, hashResultSizeWords)

#define CC_BsvSHA256(hwBaseAddress, pDataIn, dataSize, hashBuff) \
    SBRT_SHA256(hwBaseAddress, pDataIn, dataSize, hashBuff)

#define BsvCryptoImageInit(hwBaseAddress, mode, keyType) \
    SBRT_CryptoImageInit(hwBaseAddress, mode, keyType)

#define BsvCryptoImageUpdate(hwBaseAddress, mode, keyType, pCtrStateBuf, pDataIn, pDataOut, dataSize, hashBuff, isLoadIV) \
    SBRT_CryptoImageUpdate(hwBaseAddress, mode, keyType, pCtrStateBuf, pDataIn, pDataOut, dataSize, hashBuff, isLoadIV)

#define BsvCryptoImageFinish(hwBaseAddress, mode, hashBuff) \
    SBRT_CryptoImageFinish(hwBaseAddress, mode, hashBuff)

#undef min
#define min(a, b) \
    CC_MIN(a, b)

#ifdef BIG__ENDIAN
#define UTIL_REVERT_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#else
#define UTIL_REVERT_UINT32_BYTES( val ) (val)
#endif

#ifdef BIG__ENDIAN
#define UTIL_INVERSE_UINT32_BYTES( val )    (val)
#else
#define UTIL_INVERSE_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#endif

/* rsa_hwdefs => pka_hw_defs */
#define RSA_PKA_MAX_COUNT_OF_REGS_SIZES         PKA_NUM_OF_PKA_LEN_IDS_REGS
#define RSA_PKA_BIG_WORD_SIZE_IN_BITS           CC_PKA_WORD_SIZE_IN_BITS
#define RSA_PKA_BIG_WORD_SIZE_IN_32_BIT_WORDS       PKA_WORD_SIZE_IN_32BIT_WORDS
#define RSA_PKA_EXTRA_BITS                  PKA_EXTRA_BITS
#define   PkaModExp                 PKA_OPCODE_ID_MODEXP
#define RSA_PKA_SRAM_REGS_MEM_OFFSET_WORDS      CC_SRAM_PKA_BASE_ADDRESS
#define RSA_HW_PKI_PKA_N_NP_T0_T1_REG_DEFAULT_VAL   PKA_N_NP_T0_T1_REG_DEFAULT_VAL

#define RSA_PKA_FullOpCode( Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag ) \
    PKA_SET_FULL_OPCODE(Opcode,LenID,IsAImmed,OpA,IsBImmed,OpB,ResDiscard,Res,Tag )
#define RSA_PKA_WAIT_ON_PKA_PIPE_READY(VirtualHwBaseAddr) \
    CC_UNUSED_PARAM(VirtualHwBaseAddr); \
    PKA_WAIT_ON_PKA_PIPE_READY()
#define RSA_PKA_WAIT_ON_PKA_DONE(VirtualHwBaseAddr) \
    CC_UNUSED_PARAM(VirtualHwBaseAddr); \
    PKA_WAIT_ON_PKA_DONE()
#define RSA_PKA_ReadRegSize(SizeBits, EntryNum, VirtualHwBaseAddr) \
    CC_UNUSED_PARAM(VirtualHwBaseAddr); \
    PKA_GET_REG_SIZE(SizeBits, EntryNum)
#define RSA_HW_PKI_HW_LOAD_BLOCK_TO_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
    CC_UNUSED_PARAM(VirtualHwBaseAddr); \
    PKA_HW_LOAD_BLOCK_TO_PKA_MEM(Addr, ptr, SizeWords)
#define RSA_HW_PKI_HW_CLEAR_PKA_MEM( VirtualHwBaseAddr , Addr , SizeWords ) \
    CC_UNUSED_PARAM(VirtualHwBaseAddr); \
    PKA_HW_CLEAR_PKA_MEM(Addr, SizeWords)
#define RSA_HW_PKI_HW_READ_BLOCK_FROM_PKA_MEM( VirtualHwBaseAddr , Addr , ptr , SizeWords ) \
    CC_UNUSED_PARAM(VirtualHwBaseAddr); \
    PKA_HW_READ_BLOCK_FROM_PKA_MEM(Addr, ptr, SizeWords)
#define RSA_PKA_GetRegAddress(VirtReg, VirtualHwBaseAddr) \
    (*((volatile uint32_t*)(gCcRegBase + CC_REG_OFFSET(CRY_KERNEL, MEMORY_MAP0) + 4*(VirtReg))))


#ifdef __cplusplus
}
#endif

#endif


