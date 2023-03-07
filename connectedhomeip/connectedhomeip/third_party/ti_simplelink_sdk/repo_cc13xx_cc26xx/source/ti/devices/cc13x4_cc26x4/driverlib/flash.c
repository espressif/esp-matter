/******************************************************************************
*  Filename:       flash.c
*  Revised:        $Date$
*  Revision:       $Revision$
*
*  Description:    Driver for on chip Flash.
*
*  Copyright (c) 2015 - 2021, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include "../inc/hw_types.h"
#include "../inc/hw_ccfg.h"
#include "flash.h"
#include "chipinfo.h"

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  FlashPowerModeSet
    #define FlashPowerModeSet               NOROM_FlashPowerModeSet
    #undef  FlashPowerModeGet
    #define FlashPowerModeGet               NOROM_FlashPowerModeGet
    #undef  FlashProtectionSet
    #define FlashProtectionSet              NOROM_FlashProtectionSet
    #undef  FlashProtectionGet
    #define FlashProtectionGet              NOROM_FlashProtectionGet
    #undef  FlashProtectionSave
    #define FlashProtectionSave             NOROM_FlashProtectionSave
    #undef  FlashSectorErase
    #define FlashSectorErase                NOROM_FlashSectorErase
    #undef  FlashProgram
    #define FlashProgram                    NOROM_FlashProgram
    #undef  FlashProgram4X
    #define FlashProgram4X                  NOROM_FlashProgram4X
    #undef  FlashProgramNowait
    #define FlashProgramNowait              NOROM_FlashProgramNowait
    #undef  FlashEfuseReadRow
    #define FlashEfuseReadRow               NOROM_FlashEfuseReadRow
    #undef  FlashDisableSectorsForWrite
    #define FlashDisableSectorsForWrite     NOROM_FlashDisableSectorsForWrite
#endif


//*****************************************************************************
//
// Defines for accesses to the security control in the customer configuration
// area in flash CCFG area (at begining)
//
//*****************************************************************************
#define CCFG_OFFSET_SECURITY   CCFG_O_BL_CONFIG
#define CCFG_OFFSET_SECT_PROT  CCFG_O_CCFG_WEPROT_31_0_BY2K
#define CCFG_SIZE_SECURITY     0x00000018
#define CCFG_SIZE_SECT_PROT    0x00000004

#define FLASH_2T_WAITSTATE_VAL      (0x5)

//*****************************************************************************
//
// Default values for security control in customer configuration area in flash
// top sector. Below is layout
//      CCFG_O_BL_CONFIG
//      CCFG_O_ERASE_CONF
//      CCFG_O_ERASE_CONF_1
//      CCFG_O_CCFG_TI_OPTION
//      CCFG_O_CCFG_TAP_DAP_0
//      CCFG_O_CCFG_TAP_DAP_1


//*****************************************************************************
const uint8_t g_pui8CcfgDefaultSec[] = {0xFF, 0xFF, 0xFF, 0xC5,
                                        0xFF, 0xFF, 0xFF, 0xFF,
                                        0xFF, 0xFF, 0xFF, 0xFF,
                                        0xC5, 0xC5, 0xFF, 0xFF,
                                        0xC5, 0xC5, 0xC5, 0xFF,
                                        0xC5, 0xC5, 0xC5, 0xFF
                                       };

typedef uint32_t (* FlashPrgPointer_t) (uint8_t *, uint32_t, uint32_t);

typedef uint32_t (* FlashSectorErasePointer_t) (uint32_t);

//*****************************************************************************
//
// Function prototypes for static functions
//
//*****************************************************************************
static void IssueFsmCommand(tFlashStateCommandsType eCommand);



//*****************************************************************************
//
// Set power mode
//
//*****************************************************************************
void
FlashPowerModeSet(uint32_t ui32PowerMode, uint32_t ui32BankGracePeriod,
                  uint32_t ui32PumpGracePeriod)
{
    // depreciaed
    return ;
}

//*****************************************************************************
//
// Get current configured power mode
//
//*****************************************************************************
uint32_t
FlashPowerModeGet(void)
{
    // depreciated
    return (FAPI_STATUS_NOSUPPORT_ERROR);
}
//*****************************************************************************
//
// Set sector protection
//
//*****************************************************************************
void
FlashProtectionSet(uint32_t ui32SectorAddress, uint32_t ui32ProtectMode)
{
    uint32_t ui32SectorNumber;
    uint32_t ui32Mask;
    uint32_t ui32SetMask;
    uint32_t region;

    // Check the arguments.
    // only Bank 0 first 32 sectors are protected
    // for CCFG which is in non-main region
    // sector address should be aligned on sector size

    if ((ui32SectorAddress & (FlashSectorSizeGet() - 1)) != 00)
    {
        //return FAPI_STATUS_ADDRESS_ALIGN_ERROR;
        return;
    }

    if (ui32SectorAddress < (FLASHMEM_BASE + FlashSectorSizeGet() * 32))
    {
        // address is in the main region
        // FLASH_O_WEPROT_B0_31_0_BY1 only first 32 sectors are static protected
        region = NVMNW_CMDCTL_REGIONSEL_MAIN ;

        ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();
        ui32Mask = 1 << (ui32SectorNumber);
    }
    else if ( ( ui32SectorAddress >= (CCFG_BASE_DEFAULT) ) &&
             ( ui32SectorAddress < (CCFG_BASE_DEFAULT + FlashSectorSizeGet() ) ) )
    {
        // the address is CCFG in non-main region
        // non main region only one sector for each region
        region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32Mask = FLASH_WEPROT_AUX_BY1_WEPROT_B0_CCFG_BY1_M;
    }
    else
    {   // address is wrong
        //return FAPI_STATUS_ADDRESS_ERROR;
        return;
    }

    if(ui32ProtectMode == FLASH_WRITE_PROTECT)
    {   // set the corresponding bit
        ui32SetMask = ui32Mask;
    }
    else
    {   // clear the corresponding bit
        ui32SetMask = 0;
    }

    if (region == NVMNW_CMDCTL_REGIONSEL_MAIN )
    {   // clear the bit based on mask and set the bit
        HWREG(FLASH_BASE + FLASH_O_WEPROT_B0_31_0_BY1) &= ~ui32Mask;
        HWREG(FLASH_BASE + FLASH_O_WEPROT_B0_31_0_BY1) |= ui32SetMask;
    }
    else
    {   // clear the bit based on mask and set the bit
        HWREG(FLASH_BASE + FLASH_O_WEPROT_AUX_BY1) &= ~ui32Mask;
        HWREG(FLASH_BASE + FLASH_O_WEPROT_AUX_BY1) |= ui32SetMask;
    }

    return;
}

//*****************************************************************************
//
// Get sector protection
//
//*****************************************************************************
uint32_t
FlashProtectionGet(uint32_t ui32SectorAddress)
{
    uint32_t ui32SectorProtect;
    uint32_t ui32SectorNumber;
    uint32_t ui32Mask,ui32result;
    uint32_t region;

    // Check the arguments.
    // only Bank 0 first 32 sectors are protected
    // for CCFG which is in non-main region
    // sector address should be aligned on sector size
    // inverted logic: in CCFG

    if ((ui32SectorAddress & (FlashSectorSizeGet() - 1)) != 00)
    {
        return FAPI_STATUS_ADDRESS_ALIGN_ERROR;
    }

    if (ui32SectorAddress < (FLASHMEM_BASE + FlashSectorSizeGet() * 32))
    {
        // address is in the main region
        // FLASH_O_WEPROT_B0_31_0_BY1 only first 32 sectors are static protected
        region = NVMNW_CMDCTL_REGIONSEL_MAIN ;

        ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();
        ui32Mask = 1 << (ui32SectorNumber);
    }
    else if ( ( ui32SectorAddress >= (CCFG_BASE_DEFAULT) ) &&
            ( ui32SectorAddress < (CCFG_BASE_DEFAULT + FlashSectorSizeGet() ) ) )
    {
        // the address is CCFG in non-main region
        // non main region only one sector for each region
        region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32Mask = FLASH_WEPROT_AUX_BY1_WEPROT_B0_CCFG_BY1_M;
    }
    else
    {   // address is wrong
        return FAPI_STATUS_ADDRESS_ERROR;
    }

    if (region == NVMNW_CMDCTL_REGIONSEL_MAIN )
    {
        ui32result = HWREG(FLASH_BASE + FLASH_O_WEPROT_B0_31_0_BY1) & ui32Mask;
    }
    else
    {
        ui32result = HWREG(FLASH_BASE + FLASH_O_WEPROT_AUX_BY1) & ui32Mask;
    }

    if (ui32result)
    {
        ui32SectorProtect = FLASH_WRITE_PROTECT;
    }
    else
    {
        ui32SectorProtect = FLASH_NO_PROTECT;
    }

    return ui32SectorProtect;
}

//*****************************************************************************
//
// Save sector protection to make it permanent
//
//*****************************************************************************
uint32_t
FlashProtectionSave(uint32_t ui32SectorAddress)
{
    uint32_t ui32SectorProtect;
    uint32_t ui32SectorNumber;
    uint32_t ui32Mask;
    uint32_t ui32ErrorReturn;
    uint32_t ui32CcfgSectorAddr;
    uint32_t ccfgVal;

    // Check the arguments.
    // only Bank 0 first 32 sectors are protected
    // for CCFG which is in non-main region
    // sector address should be aligned on sector size

    if ((ui32SectorAddress & (FlashSectorSizeGet() - 1)) != 00)
    {
        return FAPI_STATUS_ADDRESS_ALIGN_ERROR;
    }

    if (ui32SectorAddress < (FLASHMEM_BASE + FlashSectorSizeGet() * 32))
    {
        // address is in the main region
        // FLASH_O_WEPROT_B0_31_0_BY1 only first 32 sectors are static protected

        ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();
        ui32Mask = 1 << (ui32SectorNumber);

        ui32CcfgSectorAddr = CCFG_BASE_DEFAULT + CCFG_O_CCFG_WEPROT_31_0_BY2K;
    }
    else if ( ( ui32SectorAddress >= (CCFG_BASE_DEFAULT) ) &&
            ( ui32SectorAddress < (CCFG_BASE_DEFAULT + FlashSectorSizeGet() ) ) )
    {
        // the address is CCFG in non-main region
        // non main region only one sector for each region
        ui32Mask = CCFG_ERASE_CONF_1_WEPROT_CCFG_N_M;

        ui32CcfgSectorAddr = CCFG_BASE_DEFAULT + CCFG_O_ERASE_CONF_1;
    }
    else
    {   // address is wrong
        return FAPI_STATUS_ADDRESS_ERROR;
    }

    // read from Flash static protection register
    ui32SectorProtect = FlashProtectionGet(ui32SectorAddress) ;
    ui32ErrorReturn = FAPI_STATUS_UNPROTECT_ERROR ;
    // CCFG change
    //  unprotected (1) ==> protect (0) (allow)
    //  protected (0)   ==> protect (0) (no chnage with FlashProgram call)
    //  unprotected (1) ==> unprotect (1) (no action, no FlashProgram call)
    //  protected (0)   ==> unprotect (1) (no possible without erase, no Flashprogram Call)

    if(ui32SectorProtect == FLASH_WRITE_PROTECT)
    {
        // Find sector number for specified sector. (conversion)
        //  write 1: unprotected
        //  write 0: protected
        //  from unprotected mode ==> protected mode
        //  from protected mode ==> unprotected mode (need reset ??)
        //
        //  only change one bit from unprotec (CCFG value=1 ) to protect (CCFG value =0)

        // read from the CCFG
        ccfgVal = HWREG(ui32CcfgSectorAddr);
        // clear the bit to protetc the sector
        ccfgVal &= (~ui32Mask);
        ui32SectorProtect = ~ui32Mask;

        // write the cleared bit back to CCFG
        // need to reset to take effect
        ui32ErrorReturn = FlashProgram((uint8_t*)&ccfgVal, ui32CcfgSectorAddr,
                                       CCFG_SIZE_SECT_PROT);
    }

    // Return status.
    return(ui32ErrorReturn);
}

//*****************************************************************************
//
// Erase a flash sector
//
//*****************************************************************************
uint32_t
FlashSectorErase(uint32_t ui32SectorAddress)
{
    // The below code part constitutes the variant of the FlashSectorErase()
    // function that is located in ROM. The source code of this variant is not
    // visible in internal or external driverlib. The source code is
    // only compiled during a ROM build.
    // The two above code parts (seperated by compile switches) constitute wrapper
    // functions which both call this ROM variant of the function.
    // The ROM variant is called by referrencing it directly through the ROM API table.
    uint32_t ui32ErrorReturn;
    uint32_t ui32SectorNumber;
    uint32_t ui32NoOfSectorsBank0;
    uint32_t ui32BankNum;
    uint32_t ui32Region;
    uint32_t bank0Size;

    if ((ui32SectorAddress & (FlashSectorSizeGet() - 1)) != 00)
    {
        return FAPI_STATUS_ADDRESS_ALIGN_ERROR;
    }

    if  ( ui32SectorAddress < (FLASHMEM_BASE + FlashSizeGet()) )
    {
        bank0Size = ((HWREG(NVMNW_BASE + NVMNW_O_BANK0INFO0) &
                       NVMNW_BANK0INFO0_MAINSIZE_M) >>
                       NVMNW_BANK0INFO0_MAINSIZE_S) * FlashSectorSizeGet();
        // address is in the main region
        ui32Region = NVMNW_CMDCTL_REGIONSEL_MAIN ;

        ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();
    }
    else if ( ( ui32SectorAddress >= (CCFG_BASE_DEFAULT) ) &&
            ( ui32SectorAddress < (CCFG_BASE_DEFAULT + FlashSectorSizeGet() ) ) )
    {
        // the address is CCFG in non-main region, bank 0
        ui32Region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32SectorNumber = (ui32SectorAddress - CCFG_BASE_DEFAULT) / FlashSectorSizeGet();
        ui32BankNum = 0;
    }
    else if ( ( ui32SectorAddress >= (FCFG1_BASE) ) &&
            ( ui32SectorAddress < (FCFG1_BASE + FlashSectorSizeGet() ) ) )
    {
        // the address is FCFG1 in non-main region, bank 1
        ui32Region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32SectorNumber = (ui32SectorAddress - FCFG1_BASE) / FlashSectorSizeGet();
        ui32BankNum = 1;
    }
    else
    {   // address is wrong
        return FAPI_STATUS_ADDRESS_ERROR;
    }

    // Clear the Status register.
    IssueFsmCommand(FAPI_CLEAR_STATUS);

    if ( ui32Region == NVMNW_CMDCTL_REGIONSEL_MAIN )
    {
        // for main flash region
        // Unprotect sector to be erased.
        // sectorNum = sectorAddr / sectorSize
        // if sectorNum < 256 (number of sector in bank0)
        //  set bankNum =0
        // else set bankNum =1
        //
        // if sectorNum<32, it is the first 32 sector, it is 2K protect, use the rgister A
        //      registerA = 1<<sectorNum
        // if sector>=32 and sector<256, select bank0, registerB
        //      registerB = 1<< ( (sectorNum-32)/8 + 4)
        //  if (sector>256)
        //      select Bank=1, registerB = 1<< (sectorNum-256)/8
        //
        ui32NoOfSectorsBank0 = (HWREG(NVMNW_BASE + NVMNW_O_BANK0INFO0) &
                                NVMNW_BANK0INFO0_MAINSIZE_M) >>
                                NVMNW_BANK0INFO0_MAINSIZE_S;

        if (ui32SectorNumber < ui32NoOfSectorsBank0)
        {
            ui32BankNum = 0;
        }
        else
        {
            ui32BankNum = 1;
        }

        // set up Dynamic Write/Erase Protect to unprotect mode (clear the bit)
        if ( ui32SectorNumber < 32)
        {
            HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTA) &= ~(1 << ui32SectorNumber);
        }
        else if ( ui32SectorNumber < ui32NoOfSectorsBank0)
        {
            HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTB) &=  ~( 1 << ((ui32SectorNumber-32)/8 + 4));
        }
        else
        {
            HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTB) &= ~( 1 << ((ui32SectorNumber - ui32NoOfSectorsBank0)/8));
        }

        // set up the command address, translate to Flash word adress
        // address is relative to each bank, translate to Flash word address
        HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = ( ( ui32SectorAddress - FLASHMEM_BASE ) & (bank0Size -1) ) >> 4 ;
    }
    else
    {
        // for NON-MAIN CCFG /FCFG1 region
        // set up Dynamic Write/Erase Protect to unprotect mode (clear the bit)
        HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTNM) &=  ~( 1 << (ui32SectorNumber) );

        // set up the command address, translate to Flash word adress
        // address is relative to each bank, translate to Flash word address
        if ( ui32BankNum == 0 )
        {   // CCFG bank 0
            HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32SectorAddress - CCFG_BASE_DEFAULT )>> 4;
        }
        else
        {   // FCFG1 bank 1
            HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32SectorAddress - FCFG1_BASE )>> 4;
        }
    }

    // set up control register
    /*
        19   Stair Step Erase =0
        18   erase mask disable 0
        17   Program mask disable (0)
        16   ECC override (no ECC) 0
        15   address translate overrid = 1
        14   post verify enable (1)
        13   preverify enable (1)
        12:9 regionsel = (main region) /Non-main
        8:4  bank selection = 0 /1
        3:0  N/A (only for mode change)

    */
    HWREG(NVMNW_BASE + NVMNW_O_CMDCTL) = NVMNW_CMDCTL_BANKSEL_BANK0 <<(ui32BankNum) |
                                         ui32Region |
                                         NVMNW_CMDCTL_ADDRXLATEOVR_OVERRIDE |
                                         NVMNW_CMDCTL_POSTVEREN_ENABLE |
                                         NVMNW_CMDCTL_PREVEREN_ENABLE ;

    // Issue the sector erase command to the FSM.
    IssueFsmCommand(FAPI_ERASE_SECTOR);

    // Wait for erase to finish.
    while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
    {
    }

    // Update status.
    ui32ErrorReturn = FlashCheckFsmForError();

    // check if the it is CCFG region
    if ( ( ui32Region == NVMNW_CMDCTL_REGIONSEL_NONMAIN ) &&
         ( ui32BankNum == 0 ) &&
         ( ui32ErrorReturn == FAPI_STATUS_SUCCESS ) )
    {
        // program security data to default vaues in the customer configuration
        // are within the flash CCFG sector

        // Program security data to default values in the customer configuration
        // area within the flash CCFG.
        ui32ErrorReturn = FlashProgram((uint8_t *)g_pui8CcfgDefaultSec,
                                 (ui32SectorAddress + CCFG_OFFSET_SECURITY),
                                 CCFG_SIZE_SECURITY);

    }
    // Return status of operation.
    return(ui32ErrorReturn);
}


static uint32_t FlashCopyOneFlashWordToHW(uint8_t *pBuf,uint32_t FlashAddr, uint32_t startIdx,uint32_t endIdx)
{
    uint32_t dataBuf[FLASHWORDSIZE_ONE/4];
    uint32_t i,wordIdx,byteIdx;
    uint32_t byteEnableVal =0;
    uint32_t cmdDataOffset;

    // init the dataBuf to zero
    for (i = 0; i < (FLASHWORDSIZE_ONE/4); i++)
    {
        dataBuf[i] = 0x00000000;
    }

    // build data buffer from byte address to Flash word address
    // build the byte enable value
    for (i = startIdx; i <= endIdx; i++)
    {
        byteEnableVal |= 1 << i ;
        wordIdx = i >> FLASHWORD_SHIFTBIT ;
        byteIdx = i & FLASHWORD_MASKBIT ;

        dataBuf[wordIdx] |= (*pBuf) << ( 8 * byteIdx );
        /* next data byte */
        pBuf++;
    }

    /* based on the Flash addr to figure the cdata index register
       1. convert the system byte address to flash word address (>>4)
       2. based on the flash word address to find command data index (last 2 bit)
       3. cmddataOffset will be 0,16(0x10),32(0x20),48(0x30)
    */
    cmdDataOffset = ((FlashAddr >> 4) & 0x3 ) << 4 ;

    /* write the data buffer to Flash Command data buffer */
    HWREG(NVMNW_BASE + NVMNW_O_CMDDATA0 + cmdDataOffset ) = dataBuf[0];
    HWREG(NVMNW_BASE + NVMNW_O_CMDDATA1 + cmdDataOffset ) = dataBuf[1];
    HWREG(NVMNW_BASE + NVMNW_O_CMDDATA2 + cmdDataOffset ) = dataBuf[2];
    HWREG(NVMNW_BASE + NVMNW_O_CMDDATA3 + cmdDataOffset ) = dataBuf[3];

    /* update the byte enable register (mask) */
    HWREG(NVMNW_BASE + NVMNW_O_CMDBYTEN ) = byteEnableVal;

    return (endIdx - startIdx + 1);
}

static uint32_t FlashCopyMultiFlashWordToHW(uint8_t *pBuf,uint32_t FlashAddr,uint32_t numBytes)
{
    uint32_t wordIdx;
    uint32_t flashdata=0;
    uint32_t cmdDataOffset;

    // double check the Flash address is 32 or 64 byte aligned

    /* based on the Flash addr to figure the cdata index register
       1. convert the system byte address to flash word address (>>4)
       2. based on the flash word address to find command data index (last 2 bit)
       3. cmddataOffset will be 0,32(0x20), 64 (0x40)
    */
    cmdDataOffset = ((FlashAddr >> 4) & 0x3 ) << 4 ;

    // in 2X/4X program mode, Byte Enable register is required
    // to set to 0xFFFF
    HWREG(NVMNW_BASE + NVMNW_O_CMDBYTEN ) = 0xFFFF;

    // build data buffer from byte address to Flash word address
    for (wordIdx = 0; wordIdx < (numBytes >> 2); wordIdx++)
    {
        flashdata = ( pBuf[3] << 24 ) |
                    ( pBuf[2] << 16 ) |
                    ( pBuf[1] << 8  ) |
                    ( pBuf[0] ) ;
        HWREG(NVMNW_BASE + NVMNW_O_CMDDATA0 + cmdDataOffset + (4 * wordIdx)) = flashdata ;

        /* next 4 data byte */
        pBuf += 4;
    }

    return (numBytes);
}


//*****************************************************************************
//
// Programs unprotected main bank flash sectors
//
//*****************************************************************************
uint32_t
FlashProgram(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count)
{
    // The below code part constitutes the variant of the FlashProgram() function
    // that is located in ROM. The source code of this variant is not visible in
    // internal or external driverlib. The source code is only compiled during a ROM
    // build.
    // The two above code parts (seperated by compile switches) constitute wrapper
    // functions which both call this ROM variant of the function.
    // The ROM variant is called by referrencing it directly through the ROM API table.
    uint32_t ui32StartIndex;
    uint32_t ui32StopIndex;
    uint8_t ui8BankWidth;
    uint8_t ui8NoOfBytes;
    uint32_t ui32ErrorReturn;
    uint32_t ui32SectorNum;
    uint32_t ui32BankNum;
    uint32_t ui32Region;
    uint32_t bank0Size;
    uint32_t ui32NoOfSectorsBank0;
    tFlashStateCommandsType StateMachineCmd;
    uint32_t sectorSize;

    sectorSize = FlashSectorSizeGet();

    // Check the arguments.
    // onlt main region and non-main CCFG
    ui32BankNum = 0;
    if  ( (ui32Address + ui32Count) <= (FLASHMEM_BASE + FlashSizeGet()) )
    {
        bank0Size = ((HWREG(NVMNW_BASE + NVMNW_O_BANK0INFO0) &
                       NVMNW_BANK0INFO0_MAINSIZE_M) >>
                       NVMNW_BANK0INFO0_MAINSIZE_S) * sectorSize;

        if (ui32Address >= bank0Size)
            ui32BankNum = 1;

        // address is in the main region
        ui32Region = NVMNW_CMDCTL_REGIONSEL_MAIN ;

        ui32NoOfSectorsBank0 = (HWREG(NVMNW_BASE + NVMNW_O_BANK0INFO0) &
                                NVMNW_BANK0INFO0_MAINSIZE_M) >>
                                NVMNW_BANK0INFO0_MAINSIZE_S;
    }
    else if ( ( ui32Address >= (CCFG_BASE_DEFAULT) ) &&
              ( (ui32Address + ui32Count) <= (CCFG_BASE_DEFAULT + sectorSize) ) )
    {
        // the address is CCFG in non-main region (bank 0)
        ui32Region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32BankNum = 0;

    }
    else if ( ( ui32Address >= (FCFG1_BASE) ) &&
              ( (ui32Address + ui32Count) <= (FCFG1_BASE + sectorSize) ) )
    {
        // the address is FCFG1 in non-main region, ( bank 1 )
        ui32Region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32BankNum = 1;

    }
    else
    {   // address is wrong
        return FAPI_STATUS_ADDRESS_ERROR;
    }

    // Set the status to indicate success.
    ui32ErrorReturn = FAPI_STATUS_SUCCESS;

    // Find flash bank width in number of bytes.
    ui8BankWidth = (((HWREG(NVMNW_BASE + NVMNW_O_GBLINFO1) &
                      NVMNW_GBLINFO1_DATAWIDTH_M ) >>
                      NVMNW_GBLINFO1_DATAWIDTH_S) >> 3);

    // dataIndex =0 for all modes (1X,2X,4X)
    HWREG(NVMNW_BASE + NVMNW_O_CMDDATAINDEX) = 0x0000;

    while ( ui32Count)
    {
        // Clear the Status register.
        IssueFsmCommand(FAPI_CLEAR_STATUS);

        // set up WEPROT register
        // based on the flash address to figure out WEPROT register for each state machine command
        if ( ui32Region == NVMNW_CMDCTL_REGIONSEL_MAIN)
        {   // for main region
            // based on the flash address, get the sector number on fly
            ui32SectorNum = (ui32Address - FLASHMEM_BASE) / sectorSize;

            // for main flash region
            // Unprotect sector to be erased.
            // sectorNum = sectorAddr / sectorSize
            // if sectorNum < 256 (number of sector in bank0)
            //  set bankNum =0
            // else set bankNum =1
            //
            // if sectorNum<32, it is the first 32 sector, it is 2K protect, use the rgister A
            //      registerA = 1<<sectorNum
            // if sector>=32 and sector<256, select bank0, registerB
            //      registerB = 1<< ( (sectorNum-32)/8 + 4)
            //  if (sector>256)
            //      select Bank=1, registerB = 1<< (sectorNum-256)/8
            //

            // set up Dynamic Write/Erase Protect to unprotect mode (clear the bit)
            if ( ui32SectorNum < 32)
            {
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTA) &= ~(1 << ui32SectorNum);
            }
            else if ( ui32SectorNum < ui32NoOfSectorsBank0)
            {
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTB) &=  ~( 1 << ((ui32SectorNum-32)/8 + 4));
            }
            else
            {
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTB) &= ~( 1 << ((ui32SectorNum - ui32NoOfSectorsBank0)/8));
            }

            // set up the command address, translate to Flash word adress
            // address is relative to each bank, translate to Flash word address
            HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32Address & ( bank0Size -1) )>> 4;
        }
        else
        {   // for NON-main region (CCFG) and FCFG1
            if ( ui32BankNum == 0)
            {
                // the address is CCFG in non-main region
                ui32SectorNum = (ui32Address - CCFG_BASE_DEFAULT) / sectorSize;
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTNM) &=  ~( 1 << (ui32SectorNum ));

                // set up the command address, translate to Flash word adress
                // address is relative to each bank, translate to Flash word address
                HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32Address - CCFG_BASE_DEFAULT )>> 4;
            }
            else
            {
                // the address is FCFG1 in non-main region
                ui32SectorNum = (ui32Address - FCFG1_BASE) / sectorSize;
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTNM) &=  ~( 1 << (ui32SectorNum ));

                // set up the command address, translate to Flash word adress
                // address is relative to each bank, translate to Flash word address
                HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32Address - FCFG1_BASE )>> 4;

            }
        }

        // set up NW control register
        /*
            21   enable data verification for 1T only
            19   Stair Step Erase =0
            18   erase mask disable 0
            17   Program mask disable (0)
            16   ECC override (no ECC) 0
            15   address translate overrid =1
            14   post verify enable (1)
            13   preverify enable (1)
            12:9 regionsel = 1 (main region)/Non-Main
            8:4  bank selection = 0 or 1
            3:0  N/A (only for mode change)
        */

        HWREG(NVMNW_BASE + NVMNW_O_CMDCTL) = (NVMNW_CMDCTL_BANKSEL_BANK0 <<(ui32BankNum)) |
                                              NVMNW_CMDCTL_DATAVEREN |
                                              (ui32Region) |
                                              NVMNW_CMDCTL_ADDRXLATEOVR_OVERRIDE |
                                              NVMNW_CMDCTL_POSTVEREN_ENABLE |
                                              NVMNW_CMDCTL_PREVEREN_ENABLE ;

        // based on the ui32Address alignment and ui32Count to figure oue
        // 1. if ui32Address is 4X Flashword aligned and ui32Count>=4X flashword
        //      goto 4X program mode
        // 2. if ui32Address is 2X Flashword aligned and ui32Count>=2X flashword
        //      goto 2X program mode
        // 3. otherwise
        //      goto 1X program mode (default mode)

        if ( (( ui32Address & ( FLASHWORDSIZE_FOUR -1 ) ) == 0x00 ) &&
             ( ui32Count >= FLASHWORDSIZE_FOUR ) )
        {   // 4X program mode
            ui8NoOfBytes = FlashCopyMultiFlashWordToHW (pui8DataBuffer,ui32Address,FLASHWORDSIZE_FOUR);

            StateMachineCmd = FAPI_PROGRAM_FOURWORD;
        }
        else if ( (( ui32Address & ( FLASHWORDSIZE_TWO -1 ) ) == 0x00 ) &&
                  ( ui32Count >= FLASHWORDSIZE_TWO ) )
        {   // 2X program mode
            ui8NoOfBytes = FlashCopyMultiFlashWordToHW (pui8DataBuffer,ui32Address,FLASHWORDSIZE_TWO);

            StateMachineCmd = FAPI_PROGRAM_TWOWORD;
        }
        else
        {   // 1X program mode

            // Setup the start position within the write data registers.
            ui32StartIndex = ui32Address & (uint32_t)(ui8BankWidth - 1);

            // setup number of bytes to program
            ui8NoOfBytes = ui8BankWidth - ui32StartIndex;
            if(ui8NoOfBytes > ui32Count)
            {
                ui8NoOfBytes = ui32Count;
            }

            // Setup the stop position within the write data registers.
            ui32StopIndex = ui32StartIndex + (uint32_t)(ui8NoOfBytes - 1);

            // Write each byte to the Flash Data registers
            FlashCopyOneFlashWordToHW(pui8DataBuffer,ui32Address,
                                       ui32StartIndex,ui32StopIndex);
            StateMachineCmd = FAPI_PROGRAM_ONEWORD;
        }

        // Issue the Program command to the FSM.
        IssueFsmCommand(StateMachineCmd);

        // Wait until the word has been programmed.
        while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
        {
        }

        // Exit if an access violation occurred.
        ui32ErrorReturn = FlashCheckFsmForError();
        if(ui32ErrorReturn != FAPI_STATUS_SUCCESS)
        {
            break;
        }

        // Prepare for next data burst.
        ui32Count       -= ui8NoOfBytes;
        ui32Address     += ui8NoOfBytes;
        pui8DataBuffer  += ui8NoOfBytes;

        // Check if next data burst require change of target bank
        if( (ui32Region == NVMNW_CMDCTL_REGIONSEL_MAIN) &&
            (ui32BankNum == 0) && (ui32Address >= bank0Size))
        {
            // Change target bank.
            ui32BankNum = 1;
        }

    }

    // Return status of operation.
    return(ui32ErrorReturn);
}

uint32_t FlashProgram4X(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count) {
    // The below code part constitutes the variant of the FlashProgram() function
    // that is located in ROM. The source code of this variant is not visible in
    // internal or external driverlib. The source code is only compiled during a ROM
    // build.
    // The two above code parts (seperated by compile switches) constitute wrapper
    // functions which both call this ROM variant of the function.
    // The ROM variant is called by referrencing it directly through the ROM API table.
    uint32_t ui32StartIndex;
    uint32_t ui32StopIndex;
    uint8_t ui8BankWidth;
    uint8_t ui8NoOfBytes;
    uint32_t ui32ErrorReturn;
    uint32_t ui32SectorNum;
    uint32_t ui32BankNum;
    uint32_t ui32Region;
    uint32_t bank0Size;
    uint32_t ui32NoOfSectorsBank0;
    tFlashStateCommandsType StateMachineCmd;
    uint32_t sectorSize;

    sectorSize = FlashSectorSizeGet();

    // Check the arguments.
    // onlt main region and non-main CCFG
    ui32BankNum = 0;
    if  ( (ui32Address + ui32Count) <= (FLASHMEM_BASE + FlashSizeGet()) )
    {   // get bank-0 size
        bank0Size = ((HWREG(NVMNW_BASE + NVMNW_O_BANK0INFO0) &
                       NVMNW_BANK0INFO0_MAINSIZE_M) >>
                       NVMNW_BANK0INFO0_MAINSIZE_S) * sectorSize;

        if (ui32Address >= bank0Size)
            ui32BankNum = 1;

        // address is in the main region
        ui32Region = NVMNW_CMDCTL_REGIONSEL_MAIN ;

        ui32NoOfSectorsBank0 = (HWREG(NVMNW_BASE + NVMNW_O_BANK0INFO0) &
                                NVMNW_BANK0INFO0_MAINSIZE_M) >>
                                NVMNW_BANK0INFO0_MAINSIZE_S;
    }
    else if ( ( ui32Address >= (CCFG_BASE_DEFAULT) ) &&
             ( (ui32Address + ui32Count) <= (CCFG_BASE_DEFAULT + sectorSize ) ) )
    {
        // the address is CCFG in non-main region
        ui32Region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32BankNum = 0;

    }
    else if ( ( ui32Address >= (FCFG1_BASE) ) &&
         ( (ui32Address + ui32Count) <= (FCFG1_BASE + sectorSize) ) )
    {
        // the address is FCFG1 in non-main region, ( bank 1 )
        ui32Region = NVMNW_CMDCTL_REGIONSEL_NONMAIN ;
        ui32BankNum = 1;

    }
    else
    {   // address is wrong
        return FAPI_STATUS_ADDRESS_ERROR;
    }

    // Set the status to indicate success.
    ui32ErrorReturn = FAPI_STATUS_SUCCESS;

    // Find flash bank width in number of bytes.
    ui8BankWidth = (((HWREG(NVMNW_BASE + NVMNW_O_GBLINFO1) &
                      NVMNW_GBLINFO1_DATAWIDTH_M ) >>
                      NVMNW_GBLINFO1_DATAWIDTH_S) >> 3);

    // dataIndex =0 for all modes (1X,2X,4X)
    HWREG(NVMNW_BASE + NVMNW_O_CMDDATAINDEX) = 0x0000;

    while ( ui32Count)
    {
        // Clear the Status register.
        IssueFsmCommand(FAPI_CLEAR_STATUS);

        // set up WEPROT register
        // based on the flash address to figure out WEPROT register for each state machine command
        if ( ui32Region == NVMNW_CMDCTL_REGIONSEL_MAIN)
        {   // for main region
            // based on the flash address, get the sector number on fly
            ui32SectorNum = (ui32Address - FLASHMEM_BASE) / sectorSize;

            // for main flash region
            // Unprotect sector to be erased.
            // sectorNum = sectorAddr / sectorSize
            // if sectorNum < 256 (number of sector in bank0)
            //  set bankNum =0
            // else set bankNum =1
            //
            // if sectorNum<32, it is the first 32 sector, it is 2K protect, use the rgister A
            //      registerA = 1<<sectorNum
            // if sector>=32 and sector<256, select bank0, registerB
            //      registerB = 1<< ( (sectorNum-32)/8 + 4)
            //  if (sector>256)
            //      select Bank=1, registerB = 1<< (sectorNum-256)/8
            //

            // set up Dynamic Write/Erase Protect to unprotect mode (clear the bit)
            if ( ui32SectorNum < 32)
            {
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTA) &= ~(1 << ui32SectorNum);
            }
            else if ( ui32SectorNum < ui32NoOfSectorsBank0)
            {
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTB) &=  ~( 1 << ((ui32SectorNum-32)/8 + 4));
            }
            else
            {
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTB) &= ~( 1 << ((ui32SectorNum - ui32NoOfSectorsBank0)/8));
            }

            // set up the command address, translate to Flash word adress
            // address is relative to each bank, translate to Flash word address
            HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32Address & ( bank0Size -1) )>> 4;
        }
        else
        {   // for NON-main region (CCFG)
            // the address is CCFG in non-main region
            if ( ui32BankNum ==0 )
            {
                ui32SectorNum = (ui32Address - CCFG_BASE_DEFAULT) / sectorSize;
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTNM) &=  ~( 1 << (ui32SectorNum));

                // set up the command address, translate to Flash word adress
                // address is relative to each bank, translate to Flash word address
                HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32Address - CCFG_BASE_DEFAULT )>> 4;
            }
            else
            {
                // the address is FCFG1 in non-main region
                ui32SectorNum = (ui32Address - FCFG1_BASE) / sectorSize;
                HWREG(NVMNW_BASE + NVMNW_O_CMDWEPROTNM) &=  ~( 1 << (ui32SectorNum ));

                // set up the command address, translate to Flash word adress
                // address is relative to each bank, translate to Flash word address
                HWREG(NVMNW_BASE + NVMNW_O_CMDADDR ) = (ui32Address - FCFG1_BASE )>> 4;

            }
        }

        // set up NW control register
        /*
            21   enable the data verification for 1T only
            19   Stair Step Erase =0
            18   erase mask disable 0
            17   Program mask disable (0)
            16   ECC override (no ECC) 0
            15   address translate overrid =1
            14   post verify enable (1)
            13   preverify enable (1)
            12:9 regionsel = 1 (main region)/Non-Main
            8:4  bank selection = 0 or 1
            3:0  N/A (only for mode change)
        */

        HWREG(NVMNW_BASE + NVMNW_O_CMDCTL) = (NVMNW_CMDCTL_BANKSEL_BANK0 <<(ui32BankNum)) |
                                              NVMNW_CMDCTL_DATAVEREN |
                                              (ui32Region) |
                                              NVMNW_CMDCTL_ADDRXLATEOVR_OVERRIDE |
                                              NVMNW_CMDCTL_POSTVEREN_ENABLE |
                                              NVMNW_CMDCTL_PREVEREN_ENABLE ;

        // based on the ui32Address alignment and ui32Count to figure oue
        // 1. if ui32Address is 4X Flashword aligned and ui32Count>=4X flashword
        //      goto 4X program mode
        // 2. if ui32Address is 2X Flashword aligned and ui32Count>=2X flashword
        //      goto 2X program mode
        // 3. otherwise
        //      goto 1X program mode (default mode)

        if ( (( ui32Address & ( FLASHWORDSIZE_FOUR -1 ) ) == 0x00 ) &&
             ( ui32Count >= FLASHWORDSIZE_FOUR ) )
        {   // 4X program mode
            ui8NoOfBytes = FlashCopyMultiFlashWordToHW (pui8DataBuffer,ui32Address,FLASHWORDSIZE_FOUR);

            StateMachineCmd = FAPI_PROGRAM_FOURWORD;
        }
        else if ( (( ui32Address & ( FLASHWORDSIZE_TWO -1 ) ) == 0x00 ) &&
                  ( ui32Count >= FLASHWORDSIZE_TWO ) )
        {   // 2X program mode
            ui8NoOfBytes = FlashCopyMultiFlashWordToHW (pui8DataBuffer,ui32Address,FLASHWORDSIZE_TWO);

            StateMachineCmd = FAPI_PROGRAM_TWOWORD;
        }
        else
        {   // 1X program mode

            // Setup the start position within the write data registers.
            ui32StartIndex = ui32Address & (uint32_t)(ui8BankWidth - 1);

            // setup number of bytes to program
            ui8NoOfBytes = ui8BankWidth - ui32StartIndex;
            if(ui8NoOfBytes > ui32Count)
            {
                ui8NoOfBytes = ui32Count;
            }

            // Setup the stop position within the write data registers.
            ui32StopIndex = ui32StartIndex + (uint32_t)(ui8NoOfBytes - 1);

            // Write each byte to the Flash Data registers
            FlashCopyOneFlashWordToHW(pui8DataBuffer,ui32Address,
                                       ui32StartIndex,ui32StopIndex);
            StateMachineCmd = FAPI_PROGRAM_ONEWORD;
        }

        // Issue the Program command to the FSM.
        IssueFsmCommand(StateMachineCmd);

        // Wait until the word has been programmed.
        while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
        {
        }

        // Exit if an access violation occurred.
        ui32ErrorReturn = FlashCheckFsmForError();
        if(ui32ErrorReturn != FAPI_STATUS_SUCCESS)
        {
            break;
        }

        // Prepare for next data burst.
        ui32Count       -= ui8NoOfBytes;
        ui32Address     += ui8NoOfBytes;
        pui8DataBuffer  += ui8NoOfBytes;

        // Check if next data burst require change of target bank
        if( (ui32Region == NVMNW_CMDCTL_REGIONSEL_MAIN) &&
            (ui32BankNum == 0) && (ui32Address >= bank0Size))
        {
            // Change target bank.
            ui32BankNum = 1;
        }

    }

    // Return status of operation.
    return(ui32ErrorReturn);
}


//*****************************************************************************
//
// Starts programming within unprotected main bank flash sector and returns
//
//*****************************************************************************
uint32_t
FlashProgramNowait(uint32_t ui32StartAddress, uint8_t *pui8DataBuffer,
                   uint8_t ui8NoOfBytes)
{
    // Return the function status.
    return (FAPI_STATUS_NOSUPPORT_ERROR);
}

//*****************************************************************************
//
// Reads efuse data from specified row
//
//*****************************************************************************
bool
FlashEfuseReadRow(uint32_t *pui32EfuseData, uint32_t ui32RowAddress)
{
    bool bStatus;

    // Make sure the clock for the efuse is enabled
    HWREG(FLASH_BASE + FLASH_O_CFG) &= ~FLASH_CFG_DIS_EFUSECLK;

    // Set timing for EFUSE read operations.
    HWREG(FLASH_BASE + FLASH_O_EFUSEREAD) |= ((5 << FLASH_EFUSEREAD_READCLOCK_S) &
            FLASH_EFUSEREAD_READCLOCK_M);

    // Clear status register.
    HWREG(FLASH_BASE + FLASH_O_EFUSEERROR) = 0;

    // Select the FuseROM block 0.
    HWREG(FLASH_BASE + FLASH_O_EFUSEADDR) = 0x00000000;

    // Start the read operation.
    HWREG(FLASH_BASE + FLASH_O_EFUSE) =
        (DUMPWORD_INSTR << FLASH_EFUSE_INSTRUCTION_S) |
        (ui32RowAddress & FLASH_EFUSE_DUMPWORD_M);

    // Wait for operation to finish.
    while(!(HWREG(FLASH_BASE + FLASH_O_EFUSEERROR) & FLASH_EFUSEERROR_DONE))
    {
    }

    // Check if error reported.
    if(HWREG(FLASH_BASE + FLASH_O_EFUSEERROR) & FLASH_EFUSEERROR_CODE_M)
    {
        // Set error status.
        bStatus = 1;

        // Clear data.
        *pui32EfuseData = 0;
    }
    else
    {
        // Set ok status.
        bStatus = 0;

        // No error. Get data from data register.
        *pui32EfuseData = HWREG(FLASH_BASE + FLASH_O_DATALOWER);
    }

    // Disable the efuse clock to conserve power
    HWREG(FLASH_BASE + FLASH_O_CFG) |= FLASH_CFG_DIS_EFUSECLK;

    // Return the data.
    return(bStatus);

}


//*****************************************************************************
//
// Disables all sectors for erase and programming on the active bank
//
//*****************************************************************************
void
FlashDisableSectorsForWrite(void)
{
    // depreciated
    return;
}

//*****************************************************************************
//
//!
//! Issues a command to the Flash State Machine.
//!
//! \param eCommand specifies the FSM command.
//!
//! Issues a command to the Flash State Machine.
//!
//! \return None
//
//*****************************************************************************
static void
IssueFsmCommand(tFlashStateCommandsType eCommand)
{
    // Check the arguments.
    ASSERT(
        eCommand == FAPI_ERASE_SECTOR    || eCommand == FAPI_ERASE_BANK ||
        eCommand == FAPI_PROGRAM_FOURWORD || eCommand == FAPI_CLEAR_STATUS ||
        eCommand == FAPI_PROGRAM_ONEWORD  || eCommand == FAPI_PROGRAM_TWOWORD ||
        eCommand == FAPI_READVERIFY );

    // write to NW COMMAND Type register
    HWREG(NVMNW_BASE + NVMNW_O_CMDTYPE) = eCommand;

    // write to NW Execute register
    HWREG(NVMNW_BASE + NVMNW_O_CMDEXEC) = NVMNW_CMDEXEC_VAL_EXECUTE;

}
