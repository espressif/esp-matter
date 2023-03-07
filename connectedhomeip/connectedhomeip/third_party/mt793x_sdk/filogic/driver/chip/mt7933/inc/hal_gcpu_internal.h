/*
 * MediaTek Inc. (C) 2020. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#ifndef __HAL_TRNG_INTERNAL_H__
#define __HAL_TRNG_INTERNAL_H__

#include "reg_base.h"
#include "type_def.h"
#include "hal_cache.h"
#include "hal_clock.h"
#include "hal_spm.h"


#define GCPU_DEBUG 0
#define AES_GCM_SUPPORT_NOT_ONE_PASS_MODE 1

#define GCPU_WORKBUF_SIZE           (1024)
#define GCPU_WORKBUF_IV_SIZE        (256)
#define GCPU_WORKBUF_AAD_SIZE       (256)

#define AES_CTR                         0x1E
#define AES_DPAK                        0x23
#define AES_EPAK                        0x24
#define AES_DCBC                        0x26
#define AES_ECBC                        0x27
#define AES_DGCM                        0x39
#define AES_EGCM                        0x38
#define AESPK_DPAK                      0x7A
#define AESPK_EPAK                      0x7B
#define AESPK_DCBC                      0x7C
#define AESPK_ECBC                      0x7D

#define TDES_DMA_D                      0x52
#define TDES_DMA_E                      0x53
#define TDES_CBC_D                      0x54
#define TDES_CBC_E                      0x55


#define SHA_1                          0x40 /* /< SHA-1 Algorithm */
#define MD5                            0x42 /* /< MD5 Algorithm */
#define SHA_256                        0x41 /* /< SHA-256 Algorithm */
#define SHA_224                        0x43 /* /< SHA-224 Algorithm */

#define LD_EF_KEY                      0x70 /* /< Load E_Fuse Key */

#define SHA1_BLOCK_SIZE      64 /**<  512 bits =  64 bytes */
#define SHA224_BLOCK_SIZE    64 /**<  512 bits =  64 bytes */
#define SHA256_BLOCK_SIZE    64 /**<  512 bits =  64 bytes */
#define MD5_BLOCK_SIZE       64 /**<  512 bits =  64 bytes */

#define SHA1_DIGEST_SIZE     20 /**<  160 bits = 20 bytes */
#define SHA224_DIGEST_SIZE   28 /**<  224 bits = 28 bytes */
#define SHA256_DIGEST_SIZE   32 /**<  256 bits = 32 bytes */
#define MD5_DIGEST_SIZE      16 /**<  128 bits = 16 bytes */

typedef enum {
    GCPU_RET_OK = 0,
    E_GCPU_PARAM_WRONG = 1,
    E_GCPU_OPERATE_FORBID = 2,
    E_GCPU_NO_MEM = 3,
    E_GCPU_INVALID_ARG = 4,
    E_GCPU_MEM_MAP_FAIL = 5,
    E_GCPU_HW_FAIL = 6,
} GCPU_RET_T;

typedef enum {
    GCPU_EF_LDKEY_ID_HRK_0 = 0,
    GCPU_EF_LDKEY_ID_HRK_1 = 1,
} GCPU_EF_LDKEY_ID_T;

/*!
 * @brief Cmd parameter structure for LD_EF_KEY
 */
typedef struct _EF_PARAM_LDKEY {
    GCPU_EF_LDKEY_ID_T eId;      /*!< [IN] E-Fuse key id */
    uint16_t u2DstSlotHandle;    /*!< [IN] Target slot handle. (NOTE: Support secure slot only) */
    uint32_t key_len;        /*!< [IN] Key length */
} EF_Param_LDKEY;

/* GCPU Cmd Data Struct */
typedef struct _GCPU_HAL_CMD_T_ {
    unsigned long u4SrcBufVAddr;    /* /< Src Data Buffer Virtual Addr */
    unsigned long u4DstBufVAddr;    /* /< Dst Data Buffer Virtual Addr */

    unsigned int u4GcpuCmdMode;     /* /< Gcpu Cmd Mode */
    unsigned long u4SrcBufAddr;     /* /< Src Data Buffer Addr */
    unsigned long u4DstBufAddr;     /* /< Dst Data Buffer Addr */
    unsigned int u4PacketNum;       /* /< Opt Packet Number */
    unsigned int u4DataLength;      /* /< Opt Data Length */
    unsigned int u4Mode;            /* /< Opt Mode */

    unsigned int u4CmdPtr[5];       /* /< Cmd Ptr (0: Key0 Ptr, 1: Key1 Ptr, 2: Result Ptr) */
    unsigned int u4Param0[20];      /* /< Opt Key0 Data */
    unsigned int u4Param1[20];      /* /< Opt Key1 Data */
    unsigned int u4Result[20];      /* /< Opt Result */

    unsigned char fgNeedSetData;    /* /< Need set data before command exec */
} GCPU_HAL_CMD_T;

/* GCPU_HAL_CMD_T2 for AES_GCM */
typedef struct _GCPU_HAL_CMD_T2_ {
    unsigned int u4GcpuCmdMode;     /* /< Gcpu Cmd Mode */
    unsigned long u4SrcBufAddr;     /* /< Src Data Buffer Addr */
    unsigned long u4DstBufAddr;     /* /< Dst Data Buffer Addr */
    unsigned long u4IVBufAddr;      /* /< IV Data Buffer Addr */
    unsigned long u4AadBufAddr;     /* /< AAD Data Buffer Addr */
    unsigned int u4DatLen;          /* /< Data length (in unit of bits) */
    unsigned int u4IVLen;           /* /< IV length (in unit of bits) */
    unsigned int u4AADLen;          /* /< AAD length (in unit of bits) */
    unsigned int u4Mode;            /* /< key len */
    unsigned int u4CalculationMode;
    unsigned int u4PacketMode;
    unsigned int u4CmdPtr[5];       /* /< Cmd Ptr (0: Key0 Ptr, 1: Key1 Ptr, 2: Result Ptr) */
} GCPU_HAL_CMD_T2;

/*!
 * @brief Cmd parameter structure for TDES_CBC_D, TDES_CBC_E, TDES_DMA_D, TDES_DMA_E
 *
 * Key value can use pbKey or u2KeySlotHandle according to uFlag
 * for TDES_CBC_D, u2KeySlotHandle must not be a secure slot
 */
typedef struct _TDES_PARAM {
    unsigned long  u4SrcSa;                 /*!< [IN] Source DRAM Address */
    unsigned long  u4DstSa;                 /*!< [IN] Destination DRAM Address */
    UINT32 u4Len;                     /*!< [IN] data length (in unit of byte) */
    BYTE *pbKey;                     /*!< [IN] Key value DRAM address. Len: same with key length */
    BYTE *pbIV;                       /*!< [IN] initial vector for CBC. Len: 64bits */
    BYTE *pbFB;                       /*!< [OUT] Feedback value for the next block. This can be set as the initial value of the next consecutive block. Len: 64bits*/
    UINT8 uKeyLen;                  /*!< [IN] Key length. 0:64bits, 1:128bits, 2:192bits, 3:40bits */
} TDES_Param;

/*!
 * @brief Cmd parameter structure for AES_ECB, AES_CBC, AES_CTR
 */
typedef struct _AES_PARAM {
    unsigned long u4SrcSa;                          /*!< [IN] Source DRAM Address */
    unsigned long u4DstSa;                          /*!< [IN] Destination DRAM Address */
    unsigned int u4DatLen;                         /*!< [IN] Data transfer length (in unit of byte) */
    unsigned char *pbKey;                             /*!< [IN] Key value DRAM address. Len: should be same with key length */
    unsigned char *pbIV;                               /*!< [IN] Initial vector value DRAM address. Len: 128bits */
    unsigned char *pbIVResult;                              /*!< [OUT] FeedBack data value DRAM address. Len: 128bits (AES_CBC:pbXOR)*/
    unsigned char uKeyLen;                           /*!< [IN] Key Length. 0: 128bit, 1: 192bit, 2: 256bit */
} AES_Param;

/*!
 * @brief Cmd parameter structure for AES_GCM
 */
typedef struct _AES_GCM_PARAM {
    unsigned long u4SrcSa;                          /*!< [IN] Source DRAM Address */
    unsigned long u4DstSa;                          /*!< [IN] Destination DRAM Address */
    unsigned long u4IV;                             /*!< [IN] IV DRAM Address */
    unsigned long u4AAD;                            /*!< [IN] AAD DRAM Address */
    unsigned int u4DatLen;                          /*!< [IN] Data length (in unit of bits) */
    unsigned int u4IVLen;                           /*!< [IN] IV length (in unit of bits) */
    unsigned int u4AADLen;                          /*!< [IN] AAD length (in unit of bits) */
    unsigned char *pbKey;                           /*!< [IN] Key value DRAM address. Len: should be same with key length */
    unsigned char *pbTag;                           /*!< [OUT] FeedBack tag value DRAM address. Len: 128bits (AES_CBC:pbXOR)*/
    unsigned char *pbIVResult;                      /*!< [OUT] CTRUPDATE. Len: 128bits*/
    unsigned char uKeyLen;                          /*!< [IN] Key Length. 0: 128bit, 1: 192bit, 2: 256bit */
} AES_GCM_Param;

/*!
 * @brief Cmd parameter structure for AESPK_DCBC, AESPK_ECBC
 *
 * Key value use tKeySlotHandle
 * IV,XOR using DRAM address, just for CBC
 */
typedef struct _AESPK_PARAM_PAK {
    unsigned long u4SrcSa;         /*!< [IN] Source DRAM Address */
    unsigned long u4DstSa;         /*!< [IN] Destination DRAM Address */
    uint32_t u4DatLen;             /*!< [IN] Data transfer length (in unit of byte) */
    uint16_t tKeySlotHandle;        /*!< [IN] Key Slot Handle. Slot size should be 128 bits */
    uint8_t *pbIV;                 /*!< [IN] Initial vector value DRAM address. Len: 128bits */
    uint8_t *pbXOR;                /*!< [OUT] XOR data value DRAM address. Len: 128bits */
    uint32_t uMode;                /*!< [IN] Key source selection, should be 1 */
} AESPK_Param_PAK;

typedef struct _SHA_PARAM {
    unsigned long u4SrcSa;                         /*!< [IN] Source DRAM Address */
    UINT32 u4DatLen;                        /*!< [IN] Data transfer length (in unit of byte) */
    UINT64 u8BitCnt;                        /*!< [IN] Bit Count for previouse data, for first packet, it should be zero */
    BOOL   fgFirstPacket;                   /*!< [IN] TRUE: The content contains the first packet. FALSE: The content does not contain the first packet. */
    BOOL   fgLastPacket;                    /*!< [IN] TRUE: The content contains the last packet. FALSE: The content does not contain the last packet. */
    BYTE  *pbIniHash;                       /*!< [IN] Initial Hash value DRAM address. should not be set for first packet */
    BYTE  *pbResHash;                       /*!< [OUT] Result Hash value DRAM address. */
    UINT32 iv_count;
} SHA_Param;
/*!
 * @brief Cmd parameter structure for MD5
 */
typedef struct _MD5_PARAM {
    unsigned long u4SrcSa;                          /*!< [IN] Source DRAM Address */
    UINT32 u4DatLen;                        /*!< [IN] Data transfer length (in unit of byte) */
    UINT64 u8BitCnt;                        /*!< [IN] Bit Count for previouse data, for first packet, it should be zero */
    BOOL fgFirstPacket;                    /*!< [IN] TRUE: The content contains the first packet. FALSE: The content does not contain the first packet. The initial value has to be set. */
    BOOL   fgLastPacket;                    /*!< [IN] TRUE: The content contains the last packet. FALSE: The content does not contain the last packet. */
    BYTE *pbIniHash;                        /*!< [IN] Initial Hash value DRAM address. Len: 128 bits */
    BYTE *pbResHash;                       /*!< [OUT] Result Hash value DRAM address. Len: 128 bits */
} MD5_Param;

/* Note:
   pre-device key--> load from efuse HUK
   GCPU_AESPK_D/EPAK -> aes_pk_ecb
   GCPU_AESPK_D/ECBC -> aes_pk_cbc
*/

/* gcpu_init() should be called first */
int gcpu_init(void);
GCPU_RET_T gcpu_exe_cmd(unsigned int u4Cmd, void *pvParam);
void gcpu_clock_enable(void);
void gcpu_clock_disable(void);
void gcpu_hw_init(void);
#endif /* #ifndef __HAL_TRNG_INTERNAL_H__ */
