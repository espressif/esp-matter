/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_cmpu
 @{
 */

/*!
 @file
 @brief This file contains all of the ICV production library APIs, their
 enums and definitions.
*/


#ifndef _CMPU_H
#define _CMPU_H

#include "cc_pal_types_plat.h"
#include "cc_prod.h"

/************************ Defines ******************************/

/*! The size of the ICV production library workspace in bytes. This workspace
is needed by the library for internal use. */
#define CMPU_WORKSPACE_MINIMUM_SIZE  4096

/*! The size of the ICV production library unique buffer in bytes:
Hbk0 or user data. */
#define PROD_UNIQUE_BUFF_SIZE   16
/************************ Enums ********************************/

/*! The unique data type. */
typedef enum {
        /*! The device uses the unique data as Hbk0. */
        CMPU_UNIQUE_IS_HBK0 = 1,
        /*! The device uses the unique data as a random value. Hbk0 is not
        used for the device. */
        CMPU_UNIQUE_IS_USER_DATA = 2,
        /*! Reserved. */
        CMPU_UNIQUE_RESERVED    = 0x7FFFFFFF,
} CCCmpuUniqueDataType_t;

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/

/*!
  @brief The device use of the unique buffer.

  If the device uses Hbk0, then the \p hbk0 field is used.
  Otherwise, a random buffer for the \p userData field is used.
 */
typedef union {
        /*! The Hbk0 buffer, if used by the device. */
        uint8_t hbk0[PROD_UNIQUE_BUFF_SIZE];
        /*! Any random value, if Hbk0 is not used by the device. */
        uint8_t userData[PROD_UNIQUE_BUFF_SIZE];
} CCCmpuUniqueBuff_t;


/*! The ICV production library input options. */
typedef struct {
        /*! The unique data type: Hbk0 or a random user-defined data. */
        CCCmpuUniqueDataType_t  uniqueDataType;
        /*! The unique data buffer. */
        CCCmpuUniqueBuff_t      uniqueBuff;
        /*! The Kpicv asset type. Allowed values are: Not used,
        Plain-asset, or Package. */
        CCAssetType_t           kpicvDataType;
        /*! The Kpicv buffer, if its type is Plain-asset or Package. */
        CCAssetBuff_t           kpicv;
        /*! The asset type of the Kceicv. Allowed values are: Not used,
        Plain-asset, or Package. */
        CCAssetType_t           kceicvDataType;
        /*! The Kceicv buffer, if its type is Plain-asset or Package. */
        CCAssetBuff_t           kceicv;
        /*! The minimal SW version of the ICV. Valid only if Hbk0 is used. */
        uint32_t                icvMinVersion;
        /*! The ICV configuration word. */
        uint32_t                icvConfigWord;
        /*! The default DCU lock bits of the ICV. Valid only if Hbk0 is used.*/
        uint32_t                icvDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE];
}CCCmpuData_t;



/************************ Functions *****************************/

/*!
 @brief This function burns all ICV assets into the OTP of the device.

 The user must perform a power-on-reset (PoR) to trigger LCS change to DM LCS.

 @return \c CC_OK on success.
 @return A non-zero value from cc_prod_error.h on failure.
*/
CIMPORT_C CCError_t  CCProd_Cmpu(
        /*! [in] The base address of CrytoCell HW registers. */
        unsigned long    ccHwRegBaseAddr,
        /*! [in] A pointer to the ICV defines structure. */
        CCCmpuData_t     *pCmpuData,
        /*! [in] The base address of the workspace for internal use. */
        unsigned long    workspaceBaseAddr,
        /*! [in] The size of the provided workspace. Must be at least
        #CMPU_WORKSPACE_MINIMUM_SIZE. */
        uint32_t         workspaceSize
                                    );


/*!
 @}
 */
#endif  //_CMPU_H
