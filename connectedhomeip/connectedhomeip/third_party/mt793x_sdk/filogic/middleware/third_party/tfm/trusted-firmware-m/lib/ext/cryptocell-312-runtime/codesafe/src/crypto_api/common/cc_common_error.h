/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_COMMON_ERROR_H
#define _CC_COMMON_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* CC COMMON module errors. Base address - 0x00F00D00 */

#define CC_COMMON_INIT_HW_SEM_CREATION_FAILURE    (CC_COMMON_MODULE_ERROR_BASE + 0x0UL)
#define CC_COMMON_DATA_IN_POINTER_INVALID_ERROR   (CC_COMMON_MODULE_ERROR_BASE + 0x4UL)
#define CC_COMMON_DATA_SIZE_ILLEGAL         (CC_COMMON_MODULE_ERROR_BASE + 0x5UL)
#define CC_COMMON_DATA_OUT_DATA_IN_OVERLAP_ERROR  (CC_COMMON_MODULE_ERROR_BASE + 0x6UL)
#define CC_COMMON_DATA_OUT_POINTER_INVALID_ERROR  (CC_COMMON_MODULE_ERROR_BASE + 0x7UL)
#define CC_COMMON_OUTPUT_BUFF_SIZE_ILLEGAL      (CC_COMMON_MODULE_ERROR_BASE + 0x9UL)

#define CC_COMMON_TST_UTIL_CHUNK_SIZE_SMALL_ERROR (CC_COMMON_MODULE_ERROR_BASE + 0x10UL)
#define CC_COMMON_ERROR_IN_SAVING_LLI_DATA_ERROR  (CC_COMMON_MODULE_ERROR_BASE + 0x11UL)


#define CC_COMMON_TST_UTIL_LLI_ENTRY_SIZE_TOO_SMALL_ERROR   (CC_COMMON_MODULE_ERROR_BASE + 0x12UL)
#define CC_COMMON_TST_CSI_DATA_SIZE_EXCEED_ERROR            (CC_COMMON_MODULE_ERROR_BASE + 0x13UL)
#define CC_COMMON_TST_CSI_MODULE_ID_OUT_OF_RANGE            (CC_COMMON_MODULE_ERROR_BASE + 0x14UL)
#define CC_COMMON_TST_CSI_MEMORY_MAPPING_ERROR              (CC_COMMON_MODULE_ERROR_BASE + 0x15UL)

#define CC_COMMON_TERM_HW_SEM_DELETE_FAILURE                (CC_COMMON_MODULE_ERROR_BASE + 0x16UL)

#define CC_COMMON_TST_UTIL_NOT_INTEGER_CHAR_ERROR           (CC_COMMON_MODULE_ERROR_BASE + 0x17UL)
#define CC_COMMON_TST_UTIL_BUFFER_IS_SMALL_ERROR            (CC_COMMON_MODULE_ERROR_BASE + 0x18UL)
#define CC_COMMON_POINTER_NOT_ALIGNED_ERROR                 (CC_COMMON_MODULE_ERROR_BASE + 0x19UL)


/************************ Enums ********************************/


/************************ Typedefs  ****************************/


/************************ Structs  ******************************/


/************************ Public Variables **********************/


/************************ Public Functions **********************/

#ifdef __cplusplus
}
#endif

#endif


