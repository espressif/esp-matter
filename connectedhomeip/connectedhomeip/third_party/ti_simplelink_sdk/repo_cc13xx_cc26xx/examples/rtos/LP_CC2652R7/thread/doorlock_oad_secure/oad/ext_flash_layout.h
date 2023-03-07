/******************************************************************************

 @file  ext_flash_layout.h

 @brief Contains a layout plan for the external flash of the SensorTag

 
 Group: CMCU, LPC
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

#ifndef EXT_FLASH_LAYOUT_H
#define EXT_FLASH_LAYOUT_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "oad_image_header.h"

#define OAD_EFL_MAGIC               {'O', 'A', 'D', ' ', 'N', 'V', 'M', '1'}
#define OAD_EFL_MAGIC_SZ            8

#define EFL_NUM_FACT_IMAGES         2

#define EFL_MAX_IMG_SZ              0x25000
#define EFL_APP_IMG_SZ              0x10000

#define EFL_MAX_META                4

#define EFL_USR_AREA_ADDR           EFL_SIZE_META*EFL_MAX_META
#define EFL_USR_AREA_SZ             EFL_IMG_SPACE_START - EFL_USR_AREA_ADDR
#define EFL_IMG_SPACE_START         EFL_APP_IMG_SZ
#define EFL_IMG_SPACE_END           (EFL_FACT_IMG_1_ADDR - 1)

// Image information (meta-data)
#define EFL_ADDR_META               0x00000
#define EFL_SIZE_META               EFL_PAGE_SIZE
#define EFL_META_PG_INVALID         0xFF
#define EFL_FACT_IMG_META_PG        0

#define EFL_META_COPY_SZ            offsetof(ExtImageInfo_t, fixedHdr.rfu)  +            \
                                    sizeof(((ExtImageInfo_t){0}).fixedHdr.rfu)

#define EFL_ADDR_META_FACT_IMG      EFL_ADDR_META    /* Factory image metadata address */

#define EFL_METADATA_LEN            sizeof(ExtImageInfo_t)  /* Metadata header length in bytes */

#define EFL_IMG_STR_ADDR_OFFSET     offsetof(ExtImageInfo_t, extFlAddr)    /* External flash storage address offset in words */
#define EFL_META_COUNTER_OFFSET     offsetof(ExtImageInfo_t, counter)
   
 typedef struct __attribute__((packed))
{
    imgFixedHdr_t        fixedHdr;
    uint32_t             extFlAddr;
    uint32_t             counter;
} ExtImageInfo_t;

#ifdef __cplusplus
}
#endif


#endif /* EXT_FLASH_LAYOUT_H */
