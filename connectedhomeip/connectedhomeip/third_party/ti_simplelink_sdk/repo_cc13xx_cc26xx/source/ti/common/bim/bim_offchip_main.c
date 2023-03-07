/******************************************************************************

 @file  bim_offchip_main.c

 @brief This module contains the definitions for the main functionality of a
        Boot  Image Manager.

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

/*******************************************************************************
 *                                          Includes
 */
#include <stdint.h>
#include <string.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/watchdog.h)
#include DeviceFamily_constructPath(inc/hw_prcm.h)

#include "common/cc26xx/oad/ext_flash_layout.h"
#include "common/cc26xx/crc/crc32.h"
#include "common/flash/no_rtos/extFlash/ext_flash.h"
#include "common/cc26xx/flash_interface/flash_interface.h"
#include "common/cc26xx/bim/bim_util.h"
#include "common/cc26xx/oad/oad_image_header.h"
#include "common/cc26xx/oad/oad_image_header.h"
#ifdef __IAR_SYSTEMS_ICC__
#include <intrinsics.h>
#endif

#ifdef DEBUG
#include "common/cc26xx/debug/led_debug.h"
#endif

#if defined (SECURITY)
#include "sign_util.h"
#if defined(DeviceFamily_CC26X2) || defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC13X2X7) || defined(DeviceFamily_CC26X2X7)
#include "sha2_driverlib.h"
#else
#include DeviceFamily_constructPath(driverlib/rom_sha256.h)
#endif /* DeviceFamily_CC26X2 || DeviceFamily_CC13X2 || DeviceFamily_CC13X2X7 || DeviceFamily_CC26X2X7 */
#endif
/*******************************************************************************
 *                                          Constants
 */

#define IMG_HDR_FOUND                  -1
#define EMPTY_METADATA                 -2

#define SUCCESS                         0
#define FAIL                           ~0x0

#define ONCHIP_COPY_CHUNK_SIZE          256            /* Max number of bytes to copy on on-chip flash */

#if defined (SECURITY)
#define SHA_BUF_SZ                      EFL_PAGE_SIZE
#endif

/*******************************************************************************
 * LOCAL VARIABLES
 */
static uint32_t intFlashPageSize;                   /* Size of internal flash page */

#if (defined(SECURITY))

#if defined(__IAR_SYSTEMS_ICC__)
__no_init uint8_t shaBuf[SHA_BUF_SZ];
#elif defined(__TI_COMPILER_VERSION__) || defined(__clang__)
uint8_t shaBuf[SHA_BUF_SZ];
#endif

/* Cert element stored in flash where public keys in Little endian format*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(_secureCertElement, ".cert_element")
#pragma RETAIN(_secureCertElement)
const certElement_t _secureCertElement =
#elif __clang__
const certElement_t _secureCertElement __attribute__((section(".cert_element"))) =
#elif  defined(__IAR_SYSTEMS_ICC__)
#pragma location=".cert_element"
const certElement_t _secureCertElement @ ".cert_element" =
#endif
{
  .version    = SECURE_SIGN_TYPE,
  .len        = SECURE_CERT_LENGTH,
  .options    = SECURE_CERT_OPTIONS,
  .signerInfo = {0xb0,0x17,0x7d,0x51,0x1d,0xec,0x10,0x8b},
  .certPayload.eccKey.pubKeyX = {0xd8,0x51,0xbc,0xa2,0xed,0x3d,0x9e,0x19,0xb7,0x33,0xa5,0x2f,0x33,0xda,0x05,0x40,0x4d,0x13,0x76,0x50,0x3d,0x88,0xdf,0x5c,0xd0,0xe2,0xf2,0x58,0x30,0x53,0xc4,0x2a},
  .certPayload.eccKey.pubKeyY = {0xb9,0x2a,0xbe,0xef,0x66,0x5f,0xec,0xcf,0x56,0x16,0xcc,0x36,0xef,0x2d,0xc9,0x5e,0x46,0x2b,0x7c,0x3b,0x09,0xc1,0x99,0x56,0xd9,0xaf,0x95,0x81,0x63,0x23,0x7b,0xe7}
 };

uint32_t eccWorkzone[SECURE_FW_ECC_NIST_P256_WORKZONE_LEN_IN_BYTES + SECURE_FW_ECC_BUF_TOTAL_LEN(SECURE_FW_ECC_NIST_P256_KEY_LEN_IN_BYTES)*5] = {0};
uint8_t headerBuf[HDR_LEN_WITH_SECURITY_INFO];

#endif

/*******************************************************************************
 * EXTERN FUNCTIONS
 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static int8_t Bim_copyImage(uint32_t imgStart, uint32_t imgLen, uint32_t dstAddr);
static int8_t isLastMetaData(uint8_t flashPageNum);
static uint32_t Bim_findImageStartAddr(uint32_t efImgAddr, uint32_t imgLen);
static uint8_t Bim_EraseOnchipFlashPages(uint32_t startAddr, uint32_t imgLen, uint32_t pageSize);
static int8_t checkImagesExtFlash(void);
static uint8_t checkImagesIntFlash(uint8_t flashPageNum);
static bool Bim_revertFactoryImage(void);
static void Bim_checkImages(void);

#if defined(SECURITY)
uint8_t Bim_payloadVerify(uint8_t ver, uint32_t cntr, uint32_t payloadlen,
                         uint8_t  *dataPayload, uint8_t *signPayload,
                         ecdsaSigVerifyBuf_t *ecdsaSigVerifyBuf);
static bool Bim_checkForSecSegmntIntFlash(uint32_t startAddr, uint32_t imgLen);
static bool    Bim_checkForSecSegmnt(uint32_t eflStartAddr, uint32_t imgLen);
static uint8_t Bim_verifyImage(uint32_t eflStartAddr, uint8_t *shaBuffer);
static uint8_t Bim_verifyImageIntFlash(uint32_t startAddr, uint8_t *shaBuffer);
static int8_t Bim_authenticateImage(uint32_t flStrAddr, uint32_t imgLen, bool isExtFlash);

#endif //#if defined (SECURITY)

/*******************************************************************************
 * @fn     Bim_copyImage
 *
 * @brief  Copies firmware image into the executable flash area.
 *
 * @param  imgStart - starting address of image in external flash.
 * @param  imgLen   - size of image in 4 byte blocks.
 * @param  dstAddr  - destination address within internal flash.
 *
 * @return Zero/SUCCESS when successful. FAIL, otherwise.
 */
static int8_t Bim_copyImage(uint32_t imgStart, uint32_t imgLen, uint32_t dstAddr)
{
    uint_fast16_t page = dstAddr/intFlashPageSize;
    uint_fast16_t lastPage;

    lastPage = (uint_fast16_t) ((dstAddr + imgLen - 1) / intFlashPageSize);

    if(dstAddr & 3)
    {
        /* Not an aligned address */
        return(FAIL);
    }

    if(page > lastPage || lastPage > MAX_ONCHIP_FLASH_PAGES)
    {
        return(FAIL);
    }

    /* Erase the pages needed to be copied */
    if(Bim_EraseOnchipFlashPages(dstAddr, imgLen, intFlashPageSize) != SUCCESS)
    {
        return(FAIL);
    }

    uint8_t buf[ONCHIP_COPY_CHUNK_SIZE];
    uint16_t byteCnt = ONCHIP_COPY_CHUNK_SIZE;

    while (imgLen > 0)
    {
        if(imgLen < byteCnt)
        {
            byteCnt = imgLen;
        }

        /* Read word from external flash */
        if(!extFlashRead(imgStart, byteCnt, (uint8_t *)&buf))
        {
            /* read failed */
            return(FAIL);
        }

        /* Write word to internal flash */
        if(writeFlash(dstAddr, buf, byteCnt) != FLASH_SUCCESS)
        {
            /* Program failed */
            return(FAIL);
        }

        imgStart += byteCnt;
        dstAddr += byteCnt;
        imgLen -= byteCnt;
    }
    /* Do not close external flash driver here just return */
    return(SUCCESS);
}

/*******************************************************************************
 * @fn     isLastMetaData
 *
 * @brief  Copies the internal application + stack image to the external flash
 *         to serve as the permanent resident image in external flash.
 *
 * @param  flashPageNum - Flash page number to start search for external flash
 *         metadata
 *
 * @return flashPageNum - Valid flash page number if metadata is found.
 *         IMG_HDR_FOUND - if metadat starting form specified flash page not found
 *         EMPTY_METADATA - if it is empty flashif metadata not found.
 */
static int8_t isLastMetaData(uint8_t flashPageNum)
{
    /* Read flash to find OAD image identification value */
    imgFixedHdr_t imgHdr;
    uint8_t readNext = true;
    do
    {
        extFlashRead(EXT_FLASH_ADDRESS(flashPageNum, 0),  OAD_IMG_ID_LEN, (uint8_t *)&imgHdr.imgID[0]);

        /* Check imageID bytes */
        if(metadataIDCheck(&imgHdr) == true)  /* External flash metadata found */
        {
            return(flashPageNum);
        }
        else if(imgIDCheck(&imgHdr) == true)   /* Image metadata found which indicate end of external flash metadata pages*/
        {
           return(IMG_HDR_FOUND);
        }
        flashPageNum +=1;
        if(flashPageNum >= MAX_OFFCHIP_METADATA_PAGES) /* End of flash reached, Note:practically it would never go till end if there a factory image exits */
        {
            readNext = false;
        }

    }while(readNext);

    return(EMPTY_METADATA);
}


/*******************************************************************************
 * @fn     Bim_findImageStartAddr
 *
 * @brief  Checks for stored images on the on-chip flash. If valid image is
 * found to be copied, it executable it.
 *
 * @param  efImgAddr - image start address on external flash
 * @param  imgLen    - length of the image
 *
 * @return - start address of the image if image payload segment is found else
 *          INVALID_ADDR
 */
static uint32_t Bim_findImageStartAddr(uint32_t efImgAddr, uint32_t imgLen)
{
    uint8_t buff[12];
    uint32_t startAddr = INVALID_ADDR;
    uint32_t offset = OAD_IMG_HDR_LEN;
    uint32_t len = SEG_HDR_LEN;
    uint32_t flashAddrBase = efImgAddr;
    do
    {
        /* Read flash to find segment header of the first segment */
        if(extFlashRead((flashAddrBase + offset), len, &buff[0]))
        {
            /* Check imageID bytes */
            if(buff[0] == IMG_PAYLOAD_SEG_ID)
            {
                startAddr = *((uint32_t *)(&buff[8]));
                break;
            }
            else /* some segment other than image payload */
            {
                /* get segment payload length and keep iterating */
                uint32_t payloadlen = *((uint32_t *)&buff[4]);
                if(payloadlen == 0) /* We have problem here break to avoid spinning in loop */
                {
                    break;
                }
                else
                {
                    offset += payloadlen;
                }
            }
        }
        else
        {
            break;
        }
    }while(offset < imgLen);

    return startAddr;
}

/*******************************************************************************
 * @fn     Bim_EraseOnchipFlashPages
 *
 * @brief  It Erases the onchip flash pages.
 *
 * @param  startAddr - Image start address on on-chip flash
 * @param  imgLen    - image length
 * @param  pageSize  - flash page size
 *
 * @return - SUCCESS on successful erasure else
 *           FAIL
 */
static uint8_t Bim_EraseOnchipFlashPages(uint32_t startAddr, uint32_t imgLen, uint32_t pageSize)
{
    int8_t status = SUCCESS;

    uint8_t startPage = startAddr/pageSize;
    uint8_t numFlashPages = imgLen/pageSize;
    if(0 != (imgLen % pageSize))
    {
        numFlashPages += 1;
    }

    // Erase the correct amount of pages
    for(uint8_t page=startPage; page<(startPage + numFlashPages); ++page)
    {
        uint8_t flashStat = eraseFlashPg(page);
        if(flashStat == FLASH_FAILURE)
        {
            // If we fail to pre-erase, then halt the OAD process
            status = FAIL;
            break;
        }
    }
    return status;
}

/*******************************************************************************
 * @fn     checkImagesExtFlash
 *
 * @brief  Checks for stored images on external flash. If valid image is found
 * to be copied, it copies the image and if the image is executable it will jump
 * to execute.
 *
 * @param  None
 *
 * @return No return if image is found to be copied and copied successfully else
 *         FAIL - If flash open fails or no image found on external flash
 *             number if metadata is found else
 *         SUCCESS - if no valid external flash metadata not found indicating the image to be copied.
 */
static int8_t checkImagesExtFlash(void)
{
    ExtImageInfo_t metadataHdr;
    int8_t flashPageNum = 0;

    uint8_t status;
    uint32_t eFlStrAddr; /* will be populated on reading a VALID meta data */
    int8_t securityStatus = FAIL;

    /* Initialize external flash driver. */
    if(!extFlashOpen())
    {
        return(FAIL);
    }

    /* Read flash to find OAD external flash metadata identification value  and check for external
    flash bytes */
    while((flashPageNum = isLastMetaData(flashPageNum)) > -1)
    {
        /* Read whole metadata header */
        extFlashRead(EXT_FLASH_ADDRESS(flashPageNum, 0), EFL_METADATA_LEN, (uint8_t *)&metadataHdr);

        /* check BIM and Metadata version */
        if((metadataHdr.fixedHdr.imgCpStat != NEED_COPY) ||
           (metadataHdr.fixedHdr.bimVer != BIM_VER  || metadataHdr.fixedHdr.metaVer != META_VER) ||
           (metadataHdr.fixedHdr.crcStat == CRC_INVALID))  /* Invalid CRC */
        {
            flashPageNum += 1; /* increment flash page number */
            continue;          /* Continue search on next flash page */
        }

        /* get start address of the image in external Flash */
        eFlStrAddr = metadataHdr.extFlAddr;

        /*
         * Verify the start address and the img is within external flash bounds
         */
        if ((eFlStrAddr + metadataHdr.fixedHdr.len) > EFL_FLASH_SIZE)
        {
            flashPageNum += 1; /* increment flash page number */
            continue;          /* Continue search on next flash page */
        }

#if (defined(SECURITY))
        /* check for sign verification on external flash image */
        securityStatus = Bim_authenticateImage(eFlStrAddr, metadataHdr.fixedHdr.len, true);
#else
        securityStatus = SUCCESS;
#endif /* #if defined(SECURITY) */


        if((metadataHdr.fixedHdr.imgCpStat == NEED_COPY) && (securityStatus == SUCCESS))
        {
            /* On CC26XXR2 platform only stack application needs to be copied
               Do the image copy */
           status = COPY_DONE;

           /* Now read image's start address from image stored on external flash */
           imgFixedHdr_t imgFxdHdr;

           /* Read whole image header, to find the image start address */
           extFlashRead(eFlStrAddr, OAD_IMG_HDR_LEN, (uint8_t *)&imgFxdHdr);

           /* Copy image on internal flash after passing certain checks */

           /* Get image start address from image */
           uint32_t startAddr = INVALID_ADDR;
           startAddr = Bim_findImageStartAddr(eFlStrAddr, (uint32_t)imgFxdHdr.len);

           /* Not a valid image: continue search in the next flash page  */
           if((startAddr == INVALID_ADDR) ||
                   (imgFxdHdr.prgEntry < startAddr) ||
                   (imgFxdHdr.prgEntry > (startAddr + imgFxdHdr.len)))
           {
               flashPageNum += 1; /* increment flash page number */
               continue;          /* Continue search on next flash page */
           }

           /*
            * NOTE: During debugging image length wouldn't available, as it is updated
            * by python script during post build process, for now this implementation
            * catering only for contiguous image's, so image length will be calculate
            * by subtracting the image end address by image start address.
            */
#ifdef DEBUG
            imgFxdHdr.len = imgFxdHdr.imgEndAddr - startAddr + 1;
#endif

            /* Copy image to internal flash */
            uint8_t retVal = Bim_copyImage(eFlStrAddr, imgFxdHdr.len, startAddr);

            /* Update copy status in the meta header */
            extFlashWrite(EXT_FLASH_ADDRESS(flashPageNum, IMG_COPY_STAT_OFFSET), 1, (uint8_t *)&status);

            /* If image copy is successful */
            if(retVal == SUCCESS)
            {
                /* update image copy status and calculate the
                the CRC of the copied
                and update it's CRC status. CRC_STAT_OFFSET
                */

                uint32_t crc32 = CRC32_calc(FLASH_PAGE(startAddr), intFlashPageSize, 0, imgFxdHdr.len, false);
                if(crc32 == imgFxdHdr.crc32) // if crc matched then update its status in the copied image
                {
                    status = CRC_VALID;
                    /* update status in meta data */
                    extFlashWrite(FLASH_ADDRESS(flashPageNum, CRC_STAT_OFFSET),
                                    sizeof(status), &status);

                    /* update status in internal flash */
                    writeFlashPg(FLASH_PAGE(startAddr), CRC_STAT_OFFSET,
                                    &status, sizeof(status));

#if (defined(SECURITY))
                    /* check for sign verification on internal flash image */
                    securityStatus = Bim_authenticateImage(startAddr, metadataHdr.fixedHdr.len, false);
#else
                    securityStatus = SUCCESS;
#endif /* #if defined(SECURITY) */

                    /* If it is executable Image jump to execute it */
                    if (((imgFxdHdr.imgType == OAD_IMG_TYPE_APP) ||
                        (imgFxdHdr.imgType == OAD_IMG_TYPE_PERSISTENT_APP) ||
                        (imgFxdHdr.imgType == OAD_IMG_TYPE_APP_STACK) ||
                        (imgFxdHdr.imgType == OAD_IMG_TYPE_APPSTACKLIB)) &&
                        (securityStatus == SUCCESS))
                    {
                        /* close external Flash gracefully before jumping */
                        extFlashClose();
                        jumpToPrgEntry(imgFxdHdr.prgEntry);
                    }
                }
                else /* CRC check failed */
                {
                    status = CRC_INVALID;
                    extFlashWrite((eFlStrAddr + CRC_STAT_OFFSET), 1, &status);
                }

                status = COPY_DONE;
                extFlashWrite((eFlStrAddr + IMG_COPY_STAT_OFFSET), 1, (uint8_t *)&status);

            } /* if(retVal == SUCCESS) wrt Bim_copyImage() */
        }/* if(metadataHdr.fixedHdr.imgCpStat == NEED_COPY) */

        /* Unable to find valid executable image, continue to check for a valid image by going to next page */
        flashPageNum += 1;

    }  /* end of  while((retVal = isLastMetaData(flashPageNum)) > -1) */

    extFlashClose();
    return(SUCCESS);
}

/*******************************************************************************
 * @fn     checkImagesIntFlash
 *
 * @brief  Checks for stored images on the on-chip flash. If valid image is
 * found to be copied, it executable it.
 *
 * @param  flashPageNum - Flash page number to start searching for imageType.
 * @param  imgType - Image type to look for.
 *
 * @return - No return if image is found else
 *           0 - If intended image is not found.
 */
static uint8_t checkImagesIntFlash(uint8_t flashPageNum)
{
    imgFixedHdr_t imgHdr;
    uint32_t startAddr = INVALID_ADDR;

    do
    {
        startAddr = FLASH_ADDRESS(flashPageNum, 0);

        /* Read flash to find OAD image identification value */
        readFlash((uint32_t)startAddr, &imgHdr.imgID[0], OAD_IMG_ID_LEN);

        /* Check imageID bytes */
        if(imgIDCheck(&imgHdr) == true)
        {
            /* Read whole image header */
            readFlash((uint32_t)startAddr, (uint8_t *)&imgHdr, OAD_IMG_HDR_LEN);

            /* If application is neither executable user application or merged app_stack */
            if(!(OAD_IMG_TYPE_APP == imgHdr.imgType ||
                 OAD_IMG_TYPE_APP_STACK == imgHdr.imgType  ||
                 OAD_IMG_TYPE_APPSTACKLIB == imgHdr.imgType))
            {
                continue;
            }

            if((imgHdr.len == 0) || (imgHdr.len == 0xFFFFFFFF) ||
               (imgHdr.len > (MAX_ONCHIP_FLASH_PAGES*INTFLASH_PAGE_SIZE)))
            {
                continue;
            }

            /* Not a valid image: continue search in the next flash page  */
            if((startAddr == INVALID_ADDR) ||
                    (imgHdr.prgEntry < startAddr) ||
                    (imgHdr.prgEntry > (startAddr + imgHdr.len)))
            {
                continue;          /* Continue search on next flash page */
            }


            /* Invalid metadata version */
            if((imgHdr.bimVer != BIM_VER  || imgHdr.metaVer != META_VER) ||
               /* Invalid CRC */
               (imgHdr.crcStat == CRC_INVALID))
            {
                continue;
            }
            else if(imgHdr.crcStat == DEFAULT_STATE) /* CRC not calculated */
            {



/* If DEBUG is enabled, skip the crc checking and updating the crc status.
 * as crc wouldn't have been calculated at the first place. Instead, directly
 * jump to the entry point of the image.
 * If DEBUG is disabled, do crc checks as below, before executing the image.
 */
#ifndef DEBUG
                uint8_t  crcstat = CRC_VALID;

                /* Calculate the CRC over the data buffer and update status */
                uint32_t crc32 = CRC32_calc(flashPageNum, intFlashPageSize, 0, imgHdr.len, false);

                /* Check if calculated CRC matched with the image header */
                if(crc32 != imgHdr.crc32)
                {
                    /* Update CRC status */
                    crcstat = CRC_INVALID;
                }

                writeFlash((uint32_t)FLASH_ADDRESS(flashPageNum, CRC_STAT_OFFSET), (uint8_t *)&crcstat, 1);
                if(crc32 == imgHdr.crc32)
                {
#endif
#if (defined(SECURITY))
                   // Check the authenticity of the image
                   if(Bim_authenticateImage((uint32_t)startAddr, imgHdr.len, false) == SUCCESS)
                   {
#endif
                       jumpToPrgEntry(imgHdr.prgEntry);  /* No return from here */
#if (defined(SECURITY))
                   }
#endif

#ifndef DEBUG
                }
#endif

            } /* if (imgHdr.crcStat == DEFAULT_STATE) */
            else if(imgHdr.crcStat == CRC_VALID)
            {
#if (defined(SECURITY))
                // Check the authenticity of the image
                if(Bim_authenticateImage((uint32_t)startAddr, imgHdr.len, false) == SUCCESS)
                {
#endif
                jumpToPrgEntry(imgHdr.prgEntry);  /* No return from here */
#if (defined(SECURITY))
                }
#endif
            }

        } /* if (imgIDCheck(&imgHdr) == true) */

    } while(flashPageNum++ < (MAX_ONCHIP_FLASH_PAGES -1));  /* last flash page contains CCFG */

    return(0);
}

/*******************************************************************************
 * @fn     Bim_revertFactoryImage
 *
 * @brief  It copies the factory image from external flash to on-chip and executes.
 *
 * @param  None.
 *
 * @return - No return if image is copied succsfully else
 *           false
 */
static bool Bim_revertFactoryImage(void)
{
    ExtImageInfo_t metadataHdr;

    /* Initialize external flash driver. */
    if(!extFlashOpen())
    {
        return(false);
    }
    // Read First metadata page for getting factory image information
    extFlashRead(EFL_ADDR_META_FACT_IMG, EFL_METADATA_LEN, (uint8_t *)&metadataHdr);

    /* check BIM and Metadata version */
    if(metadataHdr.fixedHdr.crcStat != CRC_VALID)  /* Invalid CRC */
    {
        return false;
    }

    uint32_t eFlStrAddr = metadataHdr.extFlAddr;

    /*
     * Verify the start address and the img is within external flash bounds
     */
    if ((eFlStrAddr + metadataHdr.fixedHdr.len) > EFL_FLASH_SIZE)
    {
        return false;
    }

    uint32_t startAddr = Bim_findImageStartAddr(eFlStrAddr, metadataHdr.fixedHdr.len);

    /* Not a valid image */
    if((startAddr == INVALID_ADDR) ||
           (metadataHdr.fixedHdr.prgEntry < startAddr) ||
           (metadataHdr.fixedHdr.prgEntry > (startAddr + metadataHdr.fixedHdr.len)))
    {
       return false;
    }


    if(Bim_copyImage(eFlStrAddr, metadataHdr.fixedHdr.len, startAddr) == SUCCESS)
    {

        // Calculate the CRC of the copied on-chip image to make sure copy is successful
        uint32_t crc32 = CRC32_calc(FLASH_PAGE(startAddr), intFlashPageSize, 0, metadataHdr.fixedHdr.len, false);
        uint8_t status = CRC_INVALID;
        if(crc32 == metadataHdr.fixedHdr.crc32) // if crc matched then update its status in the copied image
        {
            status = CRC_VALID;

            /* Update the CRC status of the copied image at CRC_STAT_OFFSET */
            writeFlashPg(FLASH_PAGE(startAddr), CRC_STAT_OFFSET, &status, sizeof(status));

            // Also check the authenticity of the image
#if defined(SECURITY)
            if(Bim_authenticateImage(startAddr, metadataHdr.fixedHdr.len, false) == SUCCESS)
            {
#endif
            /* Jump to program entry to execute it */
            jumpToPrgEntry(metadataHdr.fixedHdr.prgEntry);
#if defined(SECURITY)
            }
#endif
        }
    }
    extFlashClose();
    return false;
}

/*******************************************************************************
 * @fn     Bim_checkImages
 *
 * @brief  Check for stored images on external flash needed to be copied and
 *         execute. If there is no image to be copied, execute on-chip image.
 *
 * @param  none
 *
 * @return none
 */
static void Bim_checkImages(void)
{
#ifndef DEBUG

    /* Find executable on offchip flash; Validate it before copying to internal flash
     * and then execute it */
    checkImagesExtFlash();

    /* In no valid image has been found in external flash, then
     * try to find a valid executable image on on-chip flash and execute */
    checkImagesIntFlash(0);

    /* BIM is not able find any valid application, either on off-chip
     * or on-chip flash. Try to revert to Factory image */
    Bim_revertFactoryImage();

#else /* ifdef DEBUG */

    int8_t retVal = 0;

    /* First try finding a valid image on off-chip flash */
    if( !(retVal = checkImagesExtFlash() ) )
    {
        /* Could not find valid image on off-chip Flash.
         * Try finding application image on on-chip flash and jump to it */
       checkImagesIntFlash(0);
    }

    /* BIM is not able find any valid application, either on off-chip
     * or on-chip flash. Try to revert to Factory image */
    Bim_revertFactoryImage();

    /* No Valid image is found on off-chip or on-chip flash */
    if( retVal == -1)
    {
        /* Light RED LED to indicate an Error */
        powerUpGpio();
        lightRedLed();

        /* spin in while loop so that context is preserved for debug */
        while(1);
    }

#endif

    /* if it reached there is a problem */
    /* TBD put device to sleep and wait for button press interrupt */
}

#if (defined(SECURITY))

/*******************************************************************************
 * @fn      Bim_payloadVerify
 *
 * @brief   Function in BIM to verify the payload of an OTA command.
 *
 * @param   ver - version of the security algorithm
 *          cntr - time-stamp /counter value use to verify the payload
 *          payloadlen - payload length in bytes
 *          dataPayload - pointer to data payload to be verified
 *          signPayload - pointer to sign payload
 *          eccPayloadWorkzone - pointer to the workzone used to verify the command
 *          shaPayloadWorkzone - pointer to the workzone used to generate a hash of the command
 *
 *
 * @return  Zero when successful. Non-zero, otherwise..
 */
uint8_t Bim_payloadVerify(uint8_t ver, uint32_t cntr, uint32_t payloadlen, uint8_t  *dataPayload, uint8_t *signPayload, ecdsaSigVerifyBuf_t *ecdsaSigVerifyBuf)
{
    signPld_ECDSA_P256_t *signPld = (signPld_ECDSA_P256_t*)signPayload;

    uint8_t *sig1 = signPld->signature;
    uint8_t *sig2 = &signPld->signature[32];
    uint8_t status = FAIL;
    uint8_t verifyStatus = FAIL;
    uint8_t *finalHash = ecdsaSigVerifyBuf->tempWorkzone;

    memset(ecdsaSigVerifyBuf->tempWorkzone, 0, sizeof(ECDSA_SHA_TEMPWORKZONE_LEN));

    if (ver == 1)
    {
#if defined(DeviceFamily_CC26X2) || defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC13X2X7) || defined(DeviceFamily_CC26X2X7)
        SHA2_open();
        SHA2_addData(dataPayload, payloadlen);
        SHA2_finalize(finalHash);
        SHA2_close();
#else
        SHA256_Workzone sha256_workzone;
        SHA256_init(&sha256_workzone);
        SHA256_full(&sha256_workzone, finalHash, dataPayload, payloadlen);
#endif /* DeviceFamily_CC26X2 || DeviceFamily_CC13X2 || DeviceFamily_CC13X2X7 || DeviceFamily_CC26X2X7 */

        // First verify signerInfo
        verifyStatus = verifyCertElement(signPld->signerInfo);
        if(verifyStatus != SUCCESS)
        {
          return FAIL;
        }

        verifyStatus = bimVerifyImage_ecc(_secureCertElement.certPayload.eccKey.pubKeyX,
                                          _secureCertElement.certPayload.eccKey.pubKeyY,
                                           finalHash, sig1, sig2,
                                           ecdsaSigVerifyBuf->eccWorkzone,
                                           (ecdsaSigVerifyBuf->tempWorkzone + ECDSA_KEY_LEN));
        if(verifyStatus == SECURE_FW_ECC_STATUS_VALID_SIGNATURE)
        {
            status = SUCCESS;
        }
    }
    return status;
}//end of function

/*******************************************************************************
 * @fn         Bim_checkForSecSegmntIntFlash
*
*  @brief      Check for Security Segment. Reads through the headers in the .bin
*              file. If a security header is found the function checks to see if
*              the header has a populated segment.
*
*  @param       startAddr - The start address in external flash of the binary image
*  @param       imgLen     - Length of the image
*
*  @return      0  - security not found
*  @return      1  - security found
*
*/
static bool Bim_checkForSecSegmntIntFlash(uint32_t startAddr, uint32_t imgLen)
{
    bool securityFound = false;
    uint8_t endOfSegment = 0;
    uint8_t segmentType = DEFAULT_STATE;
    uint32_t segmentLength = 0;
    uint32_t searchAddr =  startAddr+OAD_IMG_HDR_LEN;

    while(!endOfSegment)
    {
        readFlash(searchAddr, &segmentType, 1);


        if(segmentType == IMG_SECURITY_SEG_ID)
        {
            /* In this version of BIM, the security header will ALWAYS be present
               But the payload will sometimes not be there. If this finds the
               header, the payload is also checked for existance. */
            searchAddr += SIG_OFFSET;
            uint32_t sigVal = 0;
            readFlash(searchAddr, (uint8_t *)&sigVal, sizeof(uint32_t));

            if(sigVal != 0) //Indicates the presence of a signature
            {
                endOfSegment = 1;
                securityFound = true;
            }
            else
            {
                break;
            }
        }
        else
        {
            searchAddr += SEG_LEN_OFFSET;
            if((searchAddr + sizeof(uint32_t)) > (startAddr + imgLen))
            {
                break;
            }
            readFlash(searchAddr, (uint8_t *)&segmentLength, sizeof(uint32_t));

            searchAddr += (segmentLength - SEG_LEN_OFFSET);
            if((searchAddr) > (startAddr + imgLen))
            {
                break;
            }
        }
    }
    return securityFound;
}//end of function

/*******************************************************************************
 * @fn         Bim_checkForSecSegmnt
*
*  @brief      Check for Security Segment. Reads through the headers in the .bin
*              file. If a security header is found the function checks to see if
*              the header has a populated segment.
*
*  @param       eFlStrAddr - The start address in external flash of the binary image
*
*  @return      0  - security not found
*  @return      1  - security found
*
*/
static bool Bim_checkForSecSegmnt(uint32_t eFlStrAddr, uint32_t imgLen)
{
    bool securityFound = false;
    uint8_t endOfSegment = 0;
    uint8_t segmentType = DEFAULT_STATE;
    uint32_t segmentLength = 0;
    uint32_t searchAddr =  eFlStrAddr+OAD_IMG_HDR_LEN;

    while(!endOfSegment)
    {
        extFlashRead(searchAddr, 1, &segmentType);

        if(segmentType == IMG_SECURITY_SEG_ID)
        {
            /* In this version of BIM, the security header will ALWAYS be present
               But the paylond will sometimes not be there. If this finds the
               header, the payload is also checked for existance. */
            searchAddr += SIG_OFFSET;
            uint32_t sigVal = 0;
            extFlashRead(searchAddr, sizeof(uint32_t), (uint8_t *)&sigVal);
            if(sigVal != 0) //Indicates the presence of a signature
            {
                endOfSegment = 1;
                securityFound = true;
            }
            else
            {
                break;
            }
        }
        else
        {
            searchAddr += SEG_LEN_OFFSET;
            if((searchAddr + sizeof(uint32_t)) > (eFlStrAddr + imgLen))
            {
                break;
            }
            extFlashRead(searchAddr, sizeof(uint32_t), (uint8_t *)&segmentLength);
            searchAddr += (segmentLength - SEG_LEN_OFFSET);
            if((searchAddr) > (eFlStrAddr + imgLen))
            {
                break;
            }
        }
    }

    return securityFound;
}//end of function

/*******************************************************************************
 * @fn      Bim_verifyImage
 *
 * @brief   Verifies the image stored on external flash using ECDSA-SHA256
 *
 * @param   eflStartAddr - external flash address of the image to be verified.
 *
 * @return  Zero when successful. Non-zero, otherwise..
 */
static uint8_t Bim_verifyImage(uint32_t eflStartAddr, uint8_t *shaBuffer)
{
    uint8_t verifyStatus = (uint8_t)FAIL;

    /* clear the ECC work zone Buffer */
    uint32_t *eccPayloadWorkzone = eccWorkzone;
    memset(eccPayloadWorkzone, 0, sizeof(eccWorkzone));

    /* Read in the offchip header to get the image signature */
    extFlashRead(eflStartAddr, HDR_LEN_WITH_SECURITY_INFO, headerBuf);

    // First verify signerInfo
    verifyStatus = verifyCertElement(&headerBuf[SEG_SIGERINFO_OFFSET]);
    if(verifyStatus != SUCCESS)
    {
      return verifyStatus;
    }

    // Get the hash of the image
    uint8_t *finalHash;

    // Verify the hash
    finalHash = computeSha2Hash(eflStartAddr, shaBuffer, SHA_BUF_SZ, true);

    if(!finalHash || (*finalHash == 0x00))
    {
        verifyStatus = (uint8_t)FAIL;
        return verifyStatus;
    }

    // Create temp buffer used for ECDSA sign verify, it should 6*ECDSA_KEY_LEN
    uint8_t tempWorkzone[ECDSA_SHA_TEMPWORKZONE_LEN];
    memset(tempWorkzone, 0, ECDSA_SHA_TEMPWORKZONE_LEN);

    verifyStatus = bimVerifyImage_ecc(_secureCertElement.certPayload.eccKey.pubKeyX,
                                      _secureCertElement.certPayload.eccKey.pubKeyY,
                                       finalHash,
                                       &headerBuf[SEG_SIGNR_OFFSET],
                                       &headerBuf[SEG_SIGNS_OFFSET],
                                       eccWorkzone,
                                       tempWorkzone);

    if(verifyStatus == SECURE_FW_ECC_STATUS_VALID_SIGNATURE)
    {
       verifyStatus = SUCCESS;
    }
    return verifyStatus;

}//end of function

/*******************************************************************************
 * @fn      Bim_verifyImageIntFlash
 *
 * @brief   Verifies the image stored on internal flash using ECDSA-SHA256
 *
 * @param   startAddr - internal flash address of the image to be verified.
 * @param   shaBuf    - address SHA buffer
 *
 * @return  Zero when successful. Non-zero, otherwise..
 */
static uint8_t Bim_verifyImageIntFlash(uint32_t startAddr, uint8_t *shaBuffer)
{
    uint8_t intFlshSignVrfyStatus = (uint8_t)FAIL;

    /* Calculate the SHA256 of the image */
    /* Hash the onchip image */
    uint8_t *dataHash;
    dataHash = computeSha2Hash(startAddr, shaBuffer, SHA_BUF_SZ, false);

    if(!dataHash || (*dataHash == 0x00))
    {
      return intFlshSignVrfyStatus;
    }

    /* Read in the onchip header to get the image signature */
    CRC32_memCpy(headerBuf, (void *)startAddr, HDR_LEN_WITH_SECURITY_INFO);

    /* Verify the image signature using public key, image hash, and signature */
    // Create temp buffer used for ECDSA sign verify, it should 6*ECDSA_KEY_LEN
    uint8_t tempWorkzone[ECDSA_SHA_TEMPWORKZONE_LEN];
    intFlshSignVrfyStatus = bimVerifyImage_ecc( _secureCertElement.certPayload.eccKey.pubKeyX,
                                                _secureCertElement.certPayload.eccKey.pubKeyY,
                                                dataHash,
                                                &headerBuf[SEG_SIGNR_OFFSET],
                                                &headerBuf[SEG_SIGNS_OFFSET],
                                                eccWorkzone,
                                                tempWorkzone);
    if(intFlshSignVrfyStatus != SECURE_FW_ECC_STATUS_VALID_SIGNATURE)
    {
        return FAIL;
    }
    return SUCCESS;
}

 /*******************************************************************************
 * @fn      Bim_authenticateImage
 *
 * @brief   Verifies  if the image spasses authentication using ECDSA-SHA256
 *
 * @param   flStrAddr -  start address on flash of the image to be verified.
 * @param   imgLen    - length of the image
 * @param   isExtFlash - is image stored on external flash or onchip flash
 *
 * @return  FAIL  when unsuccessfulS, SUCCESS otherwise..
 */
static int8_t Bim_authenticateImage(uint32_t flStrAddr, uint32_t imgLen, bool isExtFlash)
{
    int8_t imgSignVrfyStatus = FAIL;

    /* Ensure the imgLen is valid */
    if ((imgLen == 0x00) ||
        (isExtFlash && (imgLen > EFL_FLASH_SIZE)) ||
        (!isExtFlash && (imgLen > (MAX_ONCHIP_FLASH_PAGES * INTFLASH_PAGE_SIZE))))
    {
        return FAIL;
    }

    /* check if security segment is present or not in the image */
    uint8_t securityPresence = 0;
    if (isExtFlash)
    {
        securityPresence = Bim_checkForSecSegmnt(flStrAddr, imgLen);
    }
    else
    {
        securityPresence = Bim_checkForSecSegmntIntFlash(flStrAddr, imgLen);
    }

    if (!securityPresence)
    {
        return FAIL;
    }


    /* Calculate the SHA256 of the image */
    uint8_t readSecurityByte[SEC_VERIF_STAT_OFFSET + 1];

    /* Read in the header to check if the signature has already been denied */
    if (isExtFlash)
    {
        extFlashRead(flStrAddr,(SEC_VERIF_STAT_OFFSET + 1), readSecurityByte);
        if (readSecurityByte[SEC_VERIF_STAT_OFFSET] != VERIFY_FAIL)
        {
            imgSignVrfyStatus = (int8_t)Bim_verifyImage(flStrAddr, (uint8_t *)shaBuf);

            /* If the signature is invalid, mark the image as invalid */
            if ((uint8_t)imgSignVrfyStatus != SUCCESS)
            {
                readSecurityByte[SEC_VERIF_STAT_OFFSET] = VERIFY_FAIL;
                extFlashWrite((flStrAddr+SEC_VERIF_STAT_OFFSET), 1,
                               &readSecurityByte[SEC_VERIF_STAT_OFFSET]);
            }
        }
    }
    else  // on-chip flash
    {
        readFlash(flStrAddr, (uint8_t *)&readSecurityByte, (SEC_VERIF_STAT_OFFSET + 1));
        if (readSecurityByte[SEC_VERIF_STAT_OFFSET] != VERIFY_FAIL)
        {
            imgSignVrfyStatus = Bim_verifyImageIntFlash(flStrAddr, (uint8_t *)shaBuf);

            /* If the signature is invalid, mark the image as invalid */
            if ((uint8_t)imgSignVrfyStatus != SUCCESS)
            {
                readSecurityByte[SEC_VERIF_STAT_OFFSET] = VERIFY_FAIL;
                writeFlash((flStrAddr+SEC_VERIF_STAT_OFFSET), 
                            &readSecurityByte[SEC_VERIF_STAT_OFFSET], 1);
            }
        }
    }
    return imgSignVrfyStatus;
}
#endif // #if (defined(SECURITY))

/*******************************************************************************
 * @fn          main
 *
 * @brief       C-code main function.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
int main(void)
{
#ifdef __IAR_SYSTEMS_ICC__
  __set_CONTROL(0);
#endif

/* CAUTION: Never enable the macro "FLASH_DEVICE_ERASE" if the device
 * needs to boot to new image after doing a successful OAD.
 * Enabling this macro would mean that the newly downloaded OAD Image
 * will get erased from External Flash and then the BIM will boot to
 * the already existent (older) Internal Flash image.
 */

#ifdef FLASH_DEVICE_ERASE
    /* It will take a couple of seconds to search through External Flash
     * and erase External Flash. */
    extFlashOpen();

    const ExtFlashInfo_t *pExtFlashInfo = extFlashInfo();

    /* Read meta data page to see if it finds any starting from the first one */
    int8_t response = isLastMetaData(0);

    if(response != EMPTY_METADATA)
    {
        /* Erase flash */
        extFlashErase(0, pExtFlashInfo->deviceSize);

        /* Read metadata again */
        response = isLastMetaData(0);
    }

#ifdef DEBUG

    powerUpGpio();
    if(response != EMPTY_METADATA)
    {
        /* Blink Red LED to indicate flash erase in progress */
        blinkLed(RED_LED, 20, 50);
    }
    else
    {
        /* Blink Green LED if flash erase is complete  */
        blinkLed(GREEN_LED, 20, 50);
    }
    powerDownGpio();

#endif /* DEBUG */

    extFlashClose();

#endif /* FLASH_DEVICE_ERASE */

    intFlashPageSize = FlashSectorSizeGet();

    Bim_checkImages();

    /* If we get here, that means there is an issue: no valid image found.
     * Set the device to the lowest power state. Does not return.
     */
    setLowPowerMode();

    return(0);
}

/**************************************************************************************************
*/
