/******************************************************************************

 @file  oad_efl_dbg.c

 @brief This module provides functionality for debugging external flash via UART
        for off-chip OAD applications.

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

#ifdef OAD_DEBUG

/*********************************************************************
 * INCLUDES
 */
#include <common/cc26xx/oad/oad_image_header.h>
#include <common/cc26xx/oad/ext_flash_layout.h>
#include <common/cc26xx/flash_interface/flash_interface.h>
#include <common/cc26xx/oad/oad_image_header.h>
#include <common/cc26xx/menu/two_btn_menu.h>

#include <ti/sysbios/knl/Task.h>
#include <ti_drivers_config.h>
#include "bcomdef.h"

#include <ti/display/DisplayUart.h>
#include <ti/display/AnsiColor.h>

#include "oad_efl_dbg.h"
#include "oad.h"

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * CONSTANTS
 */
#define EFL_NUM_META                    4
#define APP_ROW_1                       TBM_ROW_APP

#define MAX_LINES                       42

#define SEP_STR   "************************"
#define TITLE_STR "EXT FL Meta*************"

#ifdef BOARD_DISPLAY_USE_UART_ANSI
    #define CRC_VALID_STR (ANSI_COLOR(FG_GREEN) "CRC_VALID" ANSI_COLOR(ATTR_RESET))
    #define CRC_INVALID_STR (ANSI_COLOR(FG_RED) "CRC_INVALID" ANSI_COLOR(ATTR_RESET))
    #define NEED_CP_STR (ANSI_COLOR(FG_RED) "NEED_COPY" ANSI_COLOR(ATTR_RESET))
    #define CP_DONE_STR (ANSI_COLOR(FG_GREEN) "COPY_DONE" ANSI_COLOR(ATTR_RESET))
    #define DEFAULT_STR (ANSI_COLOR(FG_YELLOW) "DEFAULT_STATE" ANSI_COLOR(ATTR_RESET))
#else
    #define CRC_VALID_STR  "CRC_VALID"
    #define CRC_INVALID_STR "CRC_INVALID"
    #define NEED_CP_STR  "NEED_COPY"
    #define CP_DONE_STR  "COPY_DONE"
    #define DEFAULT_STR  "DEFAULT_STATE"
#endif // BOARD_DISPLAY_USE_UART_ANSI

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern Display_Handle dispHandle;

/*********************************************************************
 * LOCAL VARIABLES
 */

static const uint8_t hdrID[] = OAD_EFL_MAGIC;

// Placeholder for meta headers
static ExtImageInfo_t eflMetaHdr;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void displayMetadata(uint8_t startingRow, uint8_t imgIdx);
static void displayMetaList(uint8_t startingRow, uint8_t maxMeta);
static void eraseAll(uint8_t startingRow, uint8_t maxMeta);
static void eraseSingle(uint8_t startingRow, uint8_t metaIdx);
static void createFactoryImg(void);
static void printMetaDataStruct(ExtImageInfo_t *meta, uint8_t imgNum,
                                uint8_t startingRow);
static uint8_t eraseExtFlRegion(uint16_t startPage, uint16_t endPage);
static bool pageIsUsed(uint8_t page);
static bool eraseImage(uint8_t startingRow, uint8_t metaIdx);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
   
/*********************************************************************
 * @fn      OadEflDbg_displayMetadata
 *
 * @brief   Print external flash metadata
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
bool OadEflDbg_displayMetadata(uint8_t index)
{
    Display_clearLines(dispHandle, APP_ROW_1, MAX_LINES);
    displayMetadata(APP_ROW_1, index);
    return(true);
}

/*********************************************************************
 * @fn      OadEflDbg_displayMetaList
 *
 * @brief   Displays external flash metadata list
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
bool OadEflDbg_displayMetaList(uint8_t index)
{
    Display_clearLines(dispHandle, APP_ROW_1, MAX_LINES);
    displayMetaList(APP_ROW_1, EFL_NUM_META);
    return(true);
}

/*********************************************************************
 * @fn      OadEflDbg_eraseAll
 *
 * @brief   Erase all images and metadata pages
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
bool OadEflDbg_eraseAll(uint8_t index)
{
    Display_clearLines(dispHandle, APP_ROW_1, MAX_LINES);
    eraseAll(APP_ROW_1, EFL_NUM_META);
    return(true);
}

/*********************************************************************
 * @fn      OadEflDbg_eraseAllMeta
 *
 * @brief   Erase selected meta pages
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
bool OadEflDbg_eraseImg(uint8_t index)
{
    Display_clearLines(dispHandle, APP_ROW_1, MAX_LINES);
    eraseSingle(APP_ROW_1, index);
    return(true);
}

/*********************************************************************
 * @fn      OadEflDbg_createFactoryImg
 *
 * @brief   Copies on-chip flash image to create factory image
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
bool OadEflDbg_createFactoryImg(uint8_t index)
{
   (void)index;  // not used
   Display_clearLines(dispHandle, APP_ROW_1, MAX_LINES);
   createFactoryImg();
   return(true);
}


/*********************************************************************
 * PRIVATE FUNCTIONS
 */

/*********************************************************************
 * @fn      displayMetadata
 *
 * @brief   Displays external flash metadata
 *
 * @param   startingRow - Start row for displaying information
 * @param   maxMeta - Maximum meta pages to search
 *
 * @return  None
 */
static void displayMetadata(uint8_t startingRow, uint8_t imgIdx)
{
    uint8_t page = imgIdx;
    if(flash_open() != true) // Open flash
    { 
        Display_printf(dispHandle, startingRow, 0, "Unable to open the flash device");
        return;
    }
    
    Display_printf(dispHandle, startingRow, 0, "Metadata page: %d",
                    imgIdx);
    Display_printf(dispHandle, startingRow++, 0, SEP_STR);
    
    // Check if this page is used or not
    if(pageIsUsed(page))
    {
        //Read in the entire header
        readFlashPg(page, 0, (uint8_t *)&eflMetaHdr, sizeof(ExtImageInfo_t));

        // Print out the struct and the members of interest
        printMetaDataStruct(&eflMetaHdr, page, startingRow);
    }
    else
    {
        Display_printf(dispHandle, startingRow++, 0, SEP_STR);
        Display_printf(dispHandle, startingRow++, 0, 
                       "Image Num: %d Unused", page);
    }
    flash_close();
}

/*********************************************************************
 * @fn      displayMetaList
 *
 * @brief   Print external flash metadata list
 *
 * @param   startingRow - Start row for displaying information
 * @param   maxMeta - Maximum meta pages to search
 *
 * @return  None
 */
static void displayMetaList(uint8_t startingRow, uint8_t maxMeta)
{
    if(flash_open() != true) // Open flash
    { 
        Display_printf(dispHandle, startingRow, 0, "Unable to open the flash device");
        return;
    }
    
    // Print header
    Display_printf(dispHandle, startingRow, 0, SEP_STR);
    Display_printf(dispHandle, startingRow++, 0, TITLE_STR);
    Display_printf(dispHandle, startingRow++, 0, "Num Images: %d",
                    maxMeta);
    Display_printf(dispHandle, startingRow++, 0, SEP_STR);

    for(uint8_t page = 0; page < maxMeta; ++page)
    {
        // Check if this page is used or not
        if(pageIsUsed(page))
        {
            //Read in the entire header
            readFlashPg(page, 0, (uint8_t *)&eflMetaHdr,
                        sizeof(ExtImageInfo_t));
            //Display information
            Display_printf(dispHandle, startingRow++, 0, SEP_STR);
            Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "Metadata page no", page);            
            Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "imgType", eflMetaHdr.fixedHdr.imgType);
            Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "imgNo", eflMetaHdr.fixedHdr.imgNo);
            Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "len", eflMetaHdr.fixedHdr.len);
            Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "crcstat", eflMetaHdr.fixedHdr.crcStat);
            Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "copystat", eflMetaHdr.fixedHdr.imgCpStat);
        }
        else
        { 
             Display_printf(dispHandle, startingRow++, 0, SEP_STR);
             Display_printf(dispHandle, startingRow++, 0,
                            "Image Num: %d Unused", page);
        }
    }
    flash_close();
}

/*********************************************************************
 * @fn      eraseAll
 *
 * @brief   Erase all images and their meta pages
 *
 * @param   startingRow - Start row for displaying information
 * @param   maxMeta - Maximum meta pages to search
 *
 * @return  None
 */
static void eraseAll(uint8_t startingRow, uint8_t maxMeta)
{
    if(flash_open() != true) // Open flash
    { 
        Display_printf(dispHandle, startingRow, 0, "Unable to open the flash device");
        return;
    }
    
    for(uint8_t page = 0; page < (maxMeta + 1); ++page)
    {
        eraseSingle(startingRow, page);
    }
    Display_printf(dispHandle, startingRow, 0, "Successfully erased all images");
    flash_close();
}

/*********************************************************************
 * @fn      eraseSingle
 *
 * @brief   Erase selected meta page
 *
 * @param   startingRow - Start row for displaying information
 * @param   maxMeta - Maximum meta pages to search
 *
 * @return  None
 */
static void eraseSingle(uint8_t startingRow, uint8_t metaIdx)
{
    if(flash_open() != true) // Open flash
    { 
        Display_printf(dispHandle, startingRow, 0, "Unable to open the flash device");
        return;
    }
    if(pageIsUsed(metaIdx))
    {
        eraseImage(startingRow, metaIdx);
    }
    else
    {
        Display_printf(dispHandle, startingRow++, 0,"Metadata Index: %d is Empty", metaIdx);
    }
    flash_close();
}

/*********************************************************************
 * @fn      createFactoryImg
 *
 * @brief   Copies on-chip flash image to create factory image
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
static void createFactoryImg(void)
{
   if(oadCreateFactoryImageBackup())
   {
       Display_printf(dispHandle, APP_ROW_1, 0, "Factory image creation failed");
   }
   else
   {
       Display_printf(dispHandle, APP_ROW_1, 0, "Succussfully created factory image");
   }
}

/*********************************************************************
 * @fn      printMetaDataStruct
 *
 * @brief   Prints and formats meta struct for display
 *
 * @param   meta - pointer to the struct
 * @param   imgNum - image number
 * @param   startingRow - Start row for displaying information
 *
 * @return  None
 */
static void printMetaDataStruct(ExtImageInfo_t *meta, uint8_t imgNum, 
                                uint8_t startingRow)
{
    uint8_t swVerSTR[OAD_SW_VER_LEN + 1];
    memcpy(swVerSTR, meta->fixedHdr.softVer, OAD_SW_VER_LEN);
    swVerSTR[OAD_SW_VER_LEN] = 0;

    /*
     * Print out metadata, format is "Member name: value"
     * Appropriate format specifiers are used depdning on the type of
     * of member variable. On some fields, ANSI beautifying is attempted
     */
    Display_printf(dispHandle, startingRow++, 0,
                    "Image Num: %d", imgNum);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "crc32", meta->fixedHdr.crc32);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "bimVer", meta->fixedHdr.bimVer);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "metaVer", meta->fixedHdr.metaVer);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%04x", "techType", meta->fixedHdr.techType);

    // Format copy status
    if(meta->fixedHdr.imgCpStat == NEED_COPY)
    {
        Display_printf(dispHandle, startingRow++, 0, "%s: %s", "imgCpStat",
                        NEED_CP_STR);
    }
    else
    {
        Display_printf(dispHandle, startingRow++, 0, "%s: %s", "imgCpStat",
                        CP_DONE_STR);
    }

    // Format CRC stat field
    if(meta->fixedHdr.crcStat == CRC_VALID)
    {
        Display_printf(dispHandle, startingRow++, 0, "%s: %s", "crcStat",
                        CRC_VALID_STR);
    }
    else if(meta->fixedHdr.crcStat == CRC_INVALID)
    {
        Display_printf(dispHandle, startingRow++, 0, "%s: %s", "crcStat",
                        CRC_INVALID_STR);
    }
    else
    {
        Display_printf(dispHandle, startingRow++, 0, "%s: %s", "crcStat",
                        DEFAULT_STR);
    }

    // Print remaining fields, see comment above for info
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "imgType", meta->fixedHdr.imgType);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%02x", "imgNo", meta->fixedHdr.imgNo);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "len", meta->fixedHdr.len);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "prgEntry", meta->fixedHdr.prgEntry);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: %s", "softVer", swVerSTR);
    Display_printf(dispHandle, startingRow++, 0, "%s: 0x%08x", "imgEndAddr",
                    meta->fixedHdr.imgEndAddr);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%04x", "hdrLen", meta->fixedHdr.hdrLen);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "extFlAddr", meta->extFlAddr);
    Display_printf(dispHandle, startingRow++, 0,
                    "%s: 0x%08x", "counter", meta->counter);
    Display_printf(dispHandle, startingRow++, 0, SEP_STR);
}

/*********************************************************************
 * @fn      pageIsUsed
 *
 * @brief   Determine if a meta pg is used or not
 *
 * @param   page - Page to check
 *
 * @return  true - page has valid meta
 *          false - page unused
 */
static bool pageIsUsed(uint8_t page)
{
    bool isUSed = false;
    // Buffer to store strings
    uint8_t readBuf[OAD_EFL_MAGIC_SZ];

    // Read imageID into buffer
    readFlashPg(page, 0, readBuf, OAD_IMG_ID_LEN);

    if(0 == memcmp(readBuf, hdrID, OAD_IMG_ID_LEN))
    {
        isUSed = true;
    }
    else
    {
        isUSed = false;
    }

    return (isUSed);
}

/*********************************************************************
 * @fn      eraseImage
 *
 * @brief   Determine if a meta pg is used or not
 *
 * @param   metaIdx - metadata index staoring image information
 *
 * @return  true - page has valid meta
 *          false - page unused
 */
static bool eraseImage(uint8_t startingRow, uint8_t metaIdx)
{
    bool status = true;
    
    //Read in the image header
    readFlashPg(metaIdx, 0, (uint8_t *)&eflMetaHdr, sizeof(ExtImageInfo_t));

    // Calculate the start and end pages of the image;
    uint32_t imgStartAddr = eflMetaHdr.extFlAddr;
    uint16_t imgStartPage = EXT_FLASH_PAGE(imgStartAddr);
    uint16_t imgEndPage = imgStartPage + (eflMetaHdr.fixedHdr.len / EFL_PAGE_SIZE);
    if(eflMetaHdr.fixedHdr.len & EXTFLASH_PAGE_MASK)
    {
        imgEndPage++;
    }

    // Erase region is non-inclusive, increase by one
    imgEndPage++;

    // First try to erase the meta page
    if(eraseFlashPg(metaIdx) == FLASH_SUCCESS)
    {
        // If meta pg erased successfully, then continue erasing the region
        eraseExtFlRegion(imgStartPage, imgEndPage);
        Display_printf(dispHandle, startingRow++, 0, "Successfully erased metadata index: %d", metaIdx);      
    }
    else
    {
        Display_printf(dispHandle, startingRow++, 0, "Unable to erase flash metadata: %d", metaIdx);    
        status = false;
    }
    return status;
}       

/*********************************************************************
 * @fn      eraseExtFlRegion
 *
 * @brief   Erase region of external flash
 *
 * @param   startPage - Page to start erase
 * @param   endPage - Page to end erase, NOT inclusive
 *
 * @return  OADExtFlDebug_success - region erased successfully
 *          OADExtFlDebug_flashFail - Ext Flash reported error
 */
static uint8_t eraseExtFlRegion(uint16_t startPage, uint16_t endPage)
{
    /*
     * Iterate through pages erasing external flash. If error, kick out early
     * Note endPage is not erased, range is [startPage, endPage)
     */
    for(uint16_t page = startPage; page < endPage; ++page)
    {
        if(eraseFlashPg(page) != FLASH_SUCCESS)
        {
            return(FAILURE);
        }
    }
    return(SUCCESS);
}

#endif //OAD_DEBUG

/*********************************************************************
*********************************************************************/

