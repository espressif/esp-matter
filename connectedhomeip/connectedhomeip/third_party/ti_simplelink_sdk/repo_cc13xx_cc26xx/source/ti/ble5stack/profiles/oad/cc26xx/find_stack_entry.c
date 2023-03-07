/******************************************************************************

 @file  find_stack_entry.c

 @brief This module finds th entry point of stack application from stack image
 header.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
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

/* -----------------------------------------------------------------------------
 *                                          Includes
 * -----------------------------------------------------------------------------
 */
#include <common/cc26xx/flash_interface/flash_interface.h>
#include <common/cc26xx/bim/bim_util.h>

/* -----------------------------------------------------------------------------
 *                                          Externs
 * -----------------------------------------------------------------------------
 */
extern const imgHdr_t _imgHdr;

/*********************************************************************
 * @fn      findStackBoundaryAddr
 *
 * @brief   This function is used to find the active address of the app/stack
 *          boundary for on-chip OAD applications
 *
 * @return  stackBoundaryAddr - The address of the stack boundary information
 */
uint32_t findStackBoundaryAddr()
{
    imgHdr_t *imgHdr;
    bool search = true;
    bool searchFromUsrApp = true;

    /*
     * To be efficient, start search at flash page starting at flash page before
     * the start of persistent image for on-chip OAD and for offchip from the
     * start of the flash.
     */
    int16_t flashPageNum = (_imgHdr.imgPayload.startAddr > 0)?(FLASH_PAGE(_imgHdr.imgPayload.startAddr) - 1):0;

    if(_imgHdr.fixedHdr.imgType == OAD_IMG_TYPE_PERSISTENT_APP)
    {
        searchFromUsrApp = false;
    }

    /* Search all flash pages for the stack header */
    do
    {
        /* Read image header */
        imgHdr = (imgHdr_t *)FLASH_ADDRESS(flashPageNum, 0);

        /* Check imageID bytes */
        if (imgIDCheck(&imgHdr->fixedHdr) == true)
        {
            /* Check that we found a stack */
            if((imgHdr->fixedHdr.imgType == OAD_IMG_TYPE_STACK) /* matching image type */
               &&
               (imgHdr->fixedHdr.crcStat != CRC_INVALID)) /* Valid CRC */
            {
                return (flashPageNum * INTFLASH_PAGE_SIZE);
            }
        }
        if(!searchFromUsrApp)
        {
            search = (flashPageNum-- >= 0);
        }
        else
        {
            search = (flashPageNum++ < MAX_ONCHIP_FLASH_PAGES);
        }

    }while(search);

    return (0xFFFFFFFF);
}

/*******************************************************************************
*/
