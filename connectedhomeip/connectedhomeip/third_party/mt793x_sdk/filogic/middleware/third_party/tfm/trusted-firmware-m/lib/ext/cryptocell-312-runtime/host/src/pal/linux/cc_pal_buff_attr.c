/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/
#include "cc_pal_types.h"
#include "cc_pal_buff_attr.h"
/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private Functions ******************************/

/************************ Public Functions ******************************/

CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer,     /*!< [in] Address of the buffer to map. */
                                  size_t              buffSize,         /*!< [in] Buffer size in bytes. */
                                  uint8_t             buffType,         /* ! [in] Input for read / output for write */
                                  uint8_t             *pBuffNs           /*!< [out] HNONSEC buffer attribute (0 for secure, 1 for non-secure) */
                                  )
{
    CC_UNUSED_PARAM(pDataBuffer);
    CC_UNUSED_PARAM(buffSize);
    CC_UNUSED_PARAM(buffType);

    *pBuffNs = DATA_BUFFER_IS_SECURE;

    return CC_OK;
}
