/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_dmpu
 @{
 */

/*!
 @file
 @brief This file contains all of the OEM production library APIs, their enums
 and definitions.
 */

#ifndef _DMPU_H
#define _DMPU_H

#include "cc_pal_types_plat.h"
#include "cc_prod.h"

/************************ Defines ******************************/

/*! The size of the OEM production library workspace in bytes. This workspace
is required by the library for internal use. */
#define DMPU_WORKSPACE_MINIMUM_SIZE  1536

/*! The size of the Hbk1 buffer in words. */
#define DMPU_HBK1_SIZE_IN_WORDS  4

/*! The size of the Hbk buffer in words. */
#define DMPU_HBK_SIZE_IN_WORDS  8

/************************ Enums ********************************/

/*! The type of the unique data. */
typedef enum {
    /*! The device uses Hbk1. */
        DMPU_HBK_TYPE_HBK1 = 1,
    /*! The device uses a full Hbk. */
        DMPU_HBK_TYPE_HBK = 2,
    /*! Reserved. */
        DMPU_HBK_TYPE_RESERVED  = 0x7FFFFFFF,
} CCDmpuHBKType_t;

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/

/*!
  The device use of the Hbk buffer.

  If the device uses Hbk0 and Hbk1, then the Hbk1 field is used.
  Otherwise, the Hbk field is used.
 */
typedef union {
        /*! The Hbk1 buffer, if used by the device. */
        uint8_t hbk1[DMPU_HBK1_SIZE_IN_WORDS*CC_PROD_32BIT_WORD_SIZE];
        /*! The full 256-bit Hbk buffer. */
        uint8_t hbk[DMPU_HBK_SIZE_IN_WORDS*CC_PROD_32BIT_WORD_SIZE];
} CCDmpuHbkBuff_t;



/*! The OEM production library input defines .*/
typedef struct {
        /*! The type of Hbk: Hbk1 - 128 bits. Hbk - 256 bits. */
        CCDmpuHBKType_t   hbkType;
        /*! The Hbk buffer. */
        CCDmpuHbkBuff_t   hbkBuff;
        /*! The Kcp asset type: Not used, Plain-asset, or Package. */
        CCAssetType_t     kcpDataType;
        /*! The Kcp buffer, if \p kcpDataType is Plain-asset or package. */
        CCAssetBuff_t     kcp;
        /*! The Kce asset type: Not used, Plain-asset, or Package. */
        CCAssetType_t     kceDataType;
        /*! The Kce buffer, if \p kceDataType is Plain-asset or package. */
        CCAssetBuff_t     kce;
        /*! The minimal SW version of the OEM. */
        uint32_t          oemMinVersion;
        /*! The default DCU lock bits of the OEM. */
        uint32_t          oemDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE];
}CCDmpuData_t;



/************************ Functions *****************************/

/*!
 @brief This function burns all OEM assets into the OTP of the device.

 The user must perform a power-on-reset (PoR) to trigger LCS change to Secure.

 @return \c CC_OK on success.
 @return A non-zero value from cc_prod_error.h on failure.
*/
CIMPORT_C CCError_t  CCProd_Dmpu(
        /*! [in] The base address of CrytoCell HW registers. */
        unsigned long   ccHwRegBaseAddr,
        /*! [in] A pointer to the defines structure of the OEM. */
        CCDmpuData_t    *pDmpuData,
        /*! [in] The base address of the workspace for internal use. */
        unsigned long   workspaceBaseAddr,
        /*! [in] The size of provided workspace. Must be at least
        \p DMPU_WORKSPACE_MINIMUM_SIZE. */
        uint32_t        workspaceSize
) ;


/*!
 @}
 */
#endif  //_DMPU_H
