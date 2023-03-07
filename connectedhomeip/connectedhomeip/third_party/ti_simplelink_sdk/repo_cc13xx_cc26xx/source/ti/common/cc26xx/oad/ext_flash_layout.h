/******************************************************************************

 @file  ext_flash_layout.h

 @brief Contains common structures and defines related to external flash
        layout as it for the OAD usecase

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

/** ============================================================================
 *  @defgroup EXT_FLASH_LAYOUT OAD External Flash Layout
 *  @brief Contains common structures and defines related to external flash
            layout as it for the OAD usecase
 *
 *  @{
 *
 *  # Header Include #
 *  The external flash layout file should be included in an OAD enabled
 *  application as follows:
 *  @code
 *  #include <common/cc26xx/oad/ext_flash_layout.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  The OAD external flash header is used to build up a table of all images that
 *  currently reside in external flash, and gives the OAD user application a way
 *  to easily collect information about the images. More information about the
 *  external flash header and the external flash layout can be found in the
 *  Over the Air Download (OAD) section of your protocol stack's User Guide.
 *
 *  @file  ext_flash_layout.h
 *
 *  @brief Contains common structures and defines related to external flash
            layout as it for the OAD usecase
 */

#ifndef EXT_FLASH_LAYOUT_H
#define EXT_FLASH_LAYOUT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <common/cc26xx/oad/oad_image_header.h>

/*!
 * Magic number used by entries in the image header table at the beginning of
 * external flash. Note this is different from @ref OAD_IMG_ID_VAL so that
 * the application can determine the difference between an entry in the header
 * table and the start of an OAD image
 *
 * \note This is only used by off-chip OAD
 */
#define OAD_EFL_MAGIC               {'O', 'A', 'D', ' ', 'N', 'V', 'M', '1'}

/*!
 * Length of external flash magic string, this is the length of
 * @ref OAD_EFL_MAGIC_SZ
 */
#define OAD_EFL_MAGIC_SZ            8

/*!
 * Address at which the table of external flash image structures start
 * Each record is populated by @ref ExtImageInfo_t
 */
#define EFL_ADDR_META               0x00000

/*!
 * Size of each entry in the external flash image header table
 * Entries in the table are page aligned to allow for easy erasing
 */
#define EFL_SIZE_META               EFL_PAGE_SIZE

/*!
 * Flag to indicate that the candidate external flash image header page is
 * invalid
 */
#define EFL_META_PG_INVALID         0xFF

/*!
 * The page in which the external flash image header of the factory image
 * resides
 */
#define EFL_FACT_IMG_META_PG        0

/*!
 * The address of the external flash header for the factory image
 */
#define EFL_ADDR_META_FACT_IMG      EFL_ADDR_META

/*!
 * Size of core header portion to copy
 */
#define EFL_META_COPY_SZ            offsetof(ExtImageInfo_t, fixedHdr.rfu)  +  \
                                    sizeof(((ExtImageInfo_t){0}).fixedHdr.rfu)

/*!
 * Length of @ref ExtImageInfo_t structure in bytes
 */
#define EFL_METADATA_LEN            sizeof(ExtImageInfo_t)

/*!
 * Offset of the image address in external flash into the @ref ExtImageInfo_t
 * structure
 */
#define EFL_IMG_STR_ADDR_OFFSET     offsetof(ExtImageInfo_t, extFlAddr)

/*!
 * Offset of the timestamp/counter into the @ref ExtImageInfo_t structure
 */
#define EFL_META_COUNTER_OFFSET     offsetof(ExtImageInfo_t, counter)

/*!
 * External flash image header used to populate the table of image headers in
 * external flash.
 */
TYPEDEF_STRUCT_PACKED
{
    imgFixedHdr_t        fixedHdr;      //!< This is the core image header
    uint32_t             extFlAddr;     //!< Location of the image in ext flash
    uint32_t             counter;       //!< Timestamp/counter of image
} ExtImageInfo_t;

/** @} End EXT_FLASH_LAYOUT */

#ifdef __cplusplus
}
#endif


#endif /* EXT_FLASH_LAYOUT_H */
