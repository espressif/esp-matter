/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 */

/** @file "gpUpgrade_Flash.h"
 *
 *  Upgrade functionality
 *
 *  Declarations of the private functions within gpUpgrade for flash access.
*/

#ifndef _GPUPGRADE_FLASH_H_
#define _GPUPGRADE_FLASH_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpUpgrade.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

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
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_COMP_EXTSTORAGE)
/** @brief This function sets the flash load source
*
*   @param loadSource The source from which to load content
*/
void gpUpgrade_SetFlashLoadSource(gpUpgrade_FlashLoadSource_t loadSource);

/** @brief This function gets the flash load source
*
*   @return The source from which content is loaded
*/
gpUpgrade_FlashLoadSource_t gpUpgrade_GetFlashLoadSource(void);
#endif


/** @brief This function erases flash sector based on image size
*
*   @param address startaddress from which to erase
*   @param numSect sector number to be erased
*
*   @return true if erased
*/
Bool gpUpgrade_FlashErase(FlashPtr address, UInt16 numSect);


/** @brief This functions loads image blocks from the image memory
*   Image memory staroage may be either internal flash or external SPI flash
*
*   @param addr     Address of the flash memory
*   @param len      Total size of the data to be loaded from memory
*   @param *pData   Pointer to data in the memory
*
*   @return gpUpgrade_Status_t status of load image
*/
gpUpgrade_Status_t gpUpgrade_FlashLoad(UInt32 addr, UInt16 len, UInt8* pData);

#if (!defined(GP_UPGRADE_DIVERSITY_COMPRESSION) || defined(GP_DIVERSITY_GPHAL_K8E))

/** @brief This function is used to program blocks read from image memory to the internal flash program region
*
*   @param addr     Address to flash memory
*   @param len      Length of the block
*   @param *pdata   Pointer to data blocks
*
*   @return gpUpgrade_Status_t status of program image
*/
gpUpgrade_Status_t gpUpgrade_FlashProgram(FlashPtr addr, UInt16 len, UInt32* pData);

/** @brief This function allows comparing the image memory with the internal flash program region.
*
*   @param progrAddr   Address to the internal program
*   @param imgAddr     Address to the image memory (internal or external)
*   @param imageSize   Length of the image
*
*   @return status of verification
*/
Bool gpUpgrade_FlashCompare(FlashPtr progrAddr, UInt32 imgAddr, UInt32 imageSize);

/** @brief This function installs new image to the application region
*
*   @param progAddr     Address to the program region
*   @param imgMemAddr   Memory address
*   @param imgMemSz     Size of image
*
*   @return gpUpgrade_Status_t status of install image
*/
gpUpgrade_Status_t gpUpgrade_FlashInstallImage(FlashPtr progAddr, UInt32 imgMemAddr, UInt32 imgMemSz);

#endif // !GP_APP_DIVERSITY_USE_FLASH_REMAPPING && GP_UPGRADE_DIVERSITY_COMPRESSION

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPUPGRADE_FLASH_H_
