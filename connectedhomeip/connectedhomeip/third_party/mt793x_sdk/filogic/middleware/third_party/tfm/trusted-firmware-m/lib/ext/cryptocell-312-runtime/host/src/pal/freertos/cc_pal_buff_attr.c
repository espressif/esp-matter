/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/
#include "cc_pal_buff_attr.h"
#include "cc_hal_plat.h"
#include "dx_id_registers.h"
#include "dx_crys_kernel.h"
#include "cc_regs.h"
#ifdef ARCH_V8M
#include <arm_cmse.h>
#endif
/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private Functions ******************************/

/************************ Public Functions ******************************/
#ifdef ARCH_V8M
CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer,     /*!< [in] Address of the buffer to map. */
                                  size_t              buffSize,         /*!< [in] Buffer size in bytes. */
                                  uint8_t             buffType,         /* ! [in] Input for read / output for write */
                                  uint8_t             *pBuffNs          /*!< [out] HNONSEC buffer attribute (0 for secure, 1 for non-secure) */
)
{
    cmse_address_info_t addInfo;
    CC_UNUSED_PARAM(buffSize);
    CC_UNUSED_PARAM(buffType);
    *pBuffNs = DATA_BUFFER_IS_SECURE;

    /* It is assumed that an object is allocated in a single MPU (Memory Protection Unit) region,
     * so it is not needed to check the entire length of the buffer. */
    addInfo = cmse_TT((unsigned char*)pDataBuffer);
    if (addInfo.flags.secure == 0x00) {
        *pBuffNs = DATA_BUFFER_IS_NONSECURE;
    } else {
    }

    return CC_OK;
}
#else
CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer,     /*!< [in] Address of the buffer to map. */
                                  size_t              buffSize,         /*!< [in] Buffer size in bytes. */
                                  uint8_t             buffType,         /* ! [in] Input for read / output for write */
                                  uint8_t             *pBuffNs          /*!< [out] HNONSEC buffer attribute (0 for secure, 1 for non-secure) */
)
{
    CC_UNUSED_PARAM(pDataBuffer);
    CC_UNUSED_PARAM(buffSize);
    CC_UNUSED_PARAM(buffType);

    *pBuffNs = DATA_BUFFER_IS_SECURE;

    return CC_OK;
}
#endif
