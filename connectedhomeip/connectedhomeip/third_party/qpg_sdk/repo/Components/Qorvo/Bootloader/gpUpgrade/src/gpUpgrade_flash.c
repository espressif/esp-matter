/*
 *   Copyright (c) 2021, Qorvo Inc
 *
 *   Upgrade functionality
 *   Implementation of gpUpgrade flash actions
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_UPGRADE

#include "gpHal.h"
#include "gpUtils.h"
#include "gpUpgrade.h"

#include "gpUpgrade_defs.h"

#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
#include "gpExtStorage.h"
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
static gpUpgrade_FlashLoadSource_t gpUpgrade_FlashLoadSource = gpUpgrade_FlashLoadSourceExternal;
#endif

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
/** @brief This function sets the flash load source
*
*   @param loadSource The source from which to load content
*/
void gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSource_t loadSource)
{
    gpUpgrade_FlashLoadSource = loadSource;
}

/** @brief This function gets the flash load source
*
*   @return The source from which content is loaded
*/
gpUpgrade_FlashLoadSource_t gpUpgrade_GetFlashLoadSource(void)
{
    return gpUpgrade_FlashLoadSource;
}
#endif


/** @brief This function erases flash sector based on image size
*
*   @param address startaddress from which to erase
*   @param numSect sector number to be erased
*
*   @return true if erased
*/
Bool gpUpgrade_FlashErase(FlashPtr address, UInt16 numSect)
{
    UInt16   i;

    /* Erase space sector by sector */
    for (i = 0; i < numSect; i++)
    {
        // Call upon a verified erase first
        if (gpHal_FlashEraseSector(address) != gpHal_FlashError_Success)
        {
            //if it fails, run an additional wipe
            gpHal_FlashEraseSectorNoVerify(address);
        }
        address += FLASH_SECTOR_SIZE;
    }
    return true;
}


/** @brief This functions loads image blocks from the image memory
*   Image memory staroage may be either internal flash or external SPI flash
*
*   @param addr     Address of the flash memory
*   @param len      Total size of the data to be loaded from memory
*   @param *pData   Pointer to data in the memory
*
*   @return gpUpgrade_Status_t status of load image
*/
gpUpgrade_Status_t gpUpgrade_FlashLoad(UInt32 addr, UInt16 len, UInt8* pData)
{
    UInt8 maxret = 4;
    while(--maxret)
    {
#if (defined(GP_COMP_EXTSTORAGE) && !defined(GP_UPGRADE_DIVERSITY_USE_INTSTORAGE))
        if(gpUpgrade_FlashLoadSource == gpUpgrade_FlashLoadSourceExternal)
        {
            if(gpExtStorage_ReadBlock(addr, len, pData) == gpExtStorage_Success)
            {
                return gpUpgrade_StatusSuccess;
            }
        }
        else
#endif
        {
            if(gpHal_FlashRead(addr, len, pData) == gpHal_FlashError_Success)
            {
                return gpUpgrade_StatusSuccess;
            }
        }
    }
    return gpUpgrade_StatusLoadImageFailed;
}

#if (!defined(GP_UPGRADE_DIVERSITY_COMPRESSION) || defined(GP_DIVERSITY_GPHAL_K8E))

/** @brief This function is used to program blocks read from image memory to the internal flash program region
*
*   @param addr     Address to flash memory
*   @param len      Length of the block
*   @param *pdata   Pointer to data blocks
*
*   @return gpUpgrade_Status_t status of program image
*/
gpUpgrade_Status_t gpUpgrade_FlashProgram(FlashPtr addr, UInt16 len, UInt32* pData)
{
    UInt16 wlen = 0;
    gpHal_FlashError_t status;
    do
    {
        if ( FLASH_IN_PAGE(addr, len) )
        {
            wlen = len;
        }
        else
        {
            wlen = FLASH_PAGE_SIZE - ((addr) % FLASH_PAGE_SIZE);
        }

        status = gpHal_FlashWrite(addr, (wlen+3)/4, (UInt32*)pData);
        if(status != gpHal_FlashError_Success)
        {
            return gpUpgrade_StatusFailedProgramError;
        }
        len -= wlen;
        pData += wlen;
        addr += wlen;

    }while(len);

    return gpUpgrade_StatusSuccess;
}

/** @brief This function allows comparing the image memory with the internal flash program region.
*
*   @param progrAddr   Address to the internal program
*   @param imgAddr     Address to the image memory (internal or external)
*   @param imageSize   Length of the image
*
*   @return status of verification
*/
Bool gpUpgrade_FlashCompare(FlashPtr progAddr, UInt32 imgAddr, UInt32 imageSize)
{
    UInt32 sz = 0;
    UInt8 imgData[IMG_MAX_BLOCK_LEN];
    UInt8 progData[IMG_MAX_BLOCK_LEN];
    UInt16 imgBlockLen = 0;

    do
    {
        imgBlockLen = min(IMG_MAX_BLOCK_LEN, (imageSize - sz));

        if(gpUpgrade_StatusSuccess == gpUpgrade_FlashLoad(imgAddr, imgBlockLen, imgData))
        {
            gpHal_FlashRead(progAddr, imgBlockLen, progData);
            if(MEMCMP(imgData, progData, imgBlockLen) != 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        sz += imgBlockLen;
        imgAddr += imgBlockLen;
        progAddr += imgBlockLen;

    }while(sz < imageSize);

    return true;
}

/** @brief This function installs new image to the application region
*
*   @param progAddr     Address to the program region
*   @param imgMemAddr   Memory address
*   @param imgMemSz     Size of image
*
*   @return gpUpgrade_Status_t status of install image
*/
gpUpgrade_Status_t gpUpgrade_FlashInstallImage(FlashPtr progAddr, UInt32 imgMemAddr, UInt32 imgMemSz)
{
    UInt32 sz = 0;
    UInt16 imgBlockLen = IMG_MAX_BLOCK_LEN;
    UInt32 imgData[IMG_MAX_BLOCK_LEN/sizeof(UInt32)];

    do
    {
        // Always write in 256K blocks, unless end of array reached
        if ((imgMemSz - sz) < IMG_MAX_BLOCK_LEN)
        {
            imgBlockLen = imgMemSz - sz;
#if defined(GP_DIVERSITY_GPHAL_K8E)
            // always 16-byte alignment
            imgBlockLen += ((imgBlockLen % 16) == 0) ? 0 : 16 - imgBlockLen % 16;
#endif
        }

        if(gpUpgrade_StatusSuccess == gpUpgrade_FlashLoad(imgMemAddr, imgBlockLen, (UInt8*)imgData))
        {
            if(gpUpgrade_StatusSuccess == gpUpgrade_FlashProgram(progAddr, imgBlockLen, imgData))
            {
                imgMemAddr += imgBlockLen;
                progAddr += imgBlockLen;
            }
            else
            {
                return gpUpgrade_StatusFailedProgramError;
            }
        }
        else
        {
            return gpUpgrade_StatusLoadImageFailed;
        }

        sz += imgBlockLen;

    }while (sz < imgMemSz);

    return gpUpgrade_StatusSuccess;
}
#endif // !GP_APP_DIVERSITY_USE_FLASH_REMAPPING && (!GP_UPGRADE_DIVERSITY_COMPRESSION || GP_DIVERSITY_GPHAL_K8C || GP_DIVERSITY_GPHAL_K8D || GP_DIVERSITY_GPHAL_K8E)
