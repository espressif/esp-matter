/******************************************************************************

 @file  ext_flash.c

 @brief Implementation for JEDEC compatible Flash

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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
*  Includes
* ------------------------------------------------------------------------------
*/

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/gpio.h)

#include "bsp.h"
#include "bsp_spi.h"
#include "ext_flash.h"

/* -----------------------------------------------------------------------------
*  Constants and macros
* ------------------------------------------------------------------------------
*/

#define SPI_BIT_RATE              4000000

/* Instruction codes */

#define BLS_CODE_PROGRAM          0x02 /**< Page Program */
#define BLS_CODE_READ             0x03 /**< Read Data */
#define BLS_CODE_READ_STATUS      0x05 /**< Read Status Register */
#define BLS_CODE_WRITE_ENABLE     0x06 /**< Write Enable */
#define BLS_CODE_SECTOR_ERASE     0x20 /**< Sector Erase */
#define BLS_CODE_MDID             0x90 /**< Manufacturer Device ID */

#define BLS_CODE_DP               0xB9 /**< Power down */
#define BLS_CODE_RDP              0xAB /**< Power standby */

/* Erase instructions */

#define BLS_CODE_ERASE_4K         0x20 /**< Sector Erase */
#define BLS_CODE_ERASE_32K        0x52
#define BLS_CODE_ERASE_64K        0xD8
#define BLS_CODE_ERASE_ALL        0xC7 /**< Mass Erase */

/* Bitmasks of the status register */

#define BLS_STATUS_SRWD_BM        0x80
#define BLS_STATUS_BP_BM          0x0C
#define BLS_STATUS_WEL_BM         0x02
#define BLS_STATUS_WIP_BM         0x01

#define BLS_STATUS_BIT_BUSY       0x01 /**< Busy bit of the status register */

/* Part specific constants */

#define BLS_PROGRAM_PAGE_SIZE     256
#define BLS_ERASE_SECTOR_SIZE     4096

/* -----------------------------------------------------------------------------
*  Private data
* ------------------------------------------------------------------------------
*/

// Supported flash devices
static const ExtFlashInfo_t flashInfo[] =
{
    {
        .manfId = 0xC2,         // Macronics MX25R1635F
        .devId = 0x15,
        .deviceSize = 0x200000  // 2 MByte (16 Mbit)
    },
    {
        .manfId = 0xC2,         // Macronics MX25R8035F
        .devId = 0x14,          //
        .deviceSize = 0x100000  // 1 MByte (8 Mbit)
    },
    {
        .manfId = 0xEF,         // WinBond W25X40CL
        .devId = 0x12,
        .deviceSize = 0x080000  // 512 KByte (4 Mbit)
    },
    {
        .manfId = 0xEF,         // WinBond W25X20CL
        .devId = 0x11,
        .deviceSize = 0x040000  // 256 KByte (2 Mbit)
    },
    {
        .manfId = 0x0,
        .devId = 0x0,
        .deviceSize = 0x0
    }
};

// Flash information
static const ExtFlashInfo_t *pFlashInfo = NULL;
static uint8_t infoBuf[2];

/* -----------------------------------------------------------------------------
*  Private function prototypes
* ------------------------------------------------------------------------------
*/
static int extFlashWaitReady(void);
static int extFlashWaitPowerDown(void);


/* -----------------------------------------------------------------------------
*  Public Functions
* ------------------------------------------------------------------------------
*/

/*******************************************************************************
* @fn          extFlashSelect
*
* @brief       Select the external flash on the SensorTag
*
* @param       none
*
* @return      none
*/
static void extFlashSelect(void)
{
    GPIO_clearDio(BSP_IOID_FLASH_CS);
}

/*******************************************************************************
* @fn          extFlashDeselect
*
* @brief       Deselect the external flash on the SensorTag
*
* @param       none
*
* @return      none
*/
static void extFlashDeselect(void)
{
    GPIO_setDio(BSP_IOID_FLASH_CS);
}

/*******************************************************************************
* @fn       extFlashInfo
*
* @brief    Get information about the mounted flash
*
* @param    none
*
* @return   return flash info record (all fields are zero if not found)
*******************************************************************************/
const ExtFlashInfo_t *extFlashInfo(void)
{
    return (pFlashInfo);
}

/*******************************************************************************
* @fn       extFlashPowerDown
*
* @brief    Put the device in power save mode. No access to data; only
*           the status register is accessible.
*
* @param    none
*
* @return   Returns true if transactions succeed
*******************************************************************************/
static bool extFlashPowerDown(void)
{
    uint8_t cmd;
    bool success;

    cmd = BLS_CODE_DP;
    extFlashSelect();
    success = bspSpiWrite(&cmd,sizeof(cmd)) == 0;
    extFlashDeselect();

    return (success);
}

/******************************************************************************
* @fn       extFlashPowerStandby
*
* @brief    Take device out of power save mode and prepare it for normal operation
*
* @param    none
*
* @return   Returns true if command successfully written
******************************************************************************/
static bool extFlashPowerStandby(void)
{
    uint8_t cmd;
    bool success;

    cmd = BLS_CODE_RDP;
    extFlashSelect();
    success = bspSpiWrite(&cmd,sizeof(cmd)) == 0;
    extFlashDeselect();

    if (success)
    {
        volatile uint16_t i = 400;

        // Waking up of the device is manufacturer dependent.
        // for a Winond chip-set, once the request to wake up the flash has been
        // send, CS needs to stay high at least 3us (for Winbond part)
        // for chip-set like Macronix, it can take up to 35us.
        for (i; i > 0; i--);

        if (extFlashWaitReady() != 0)
        {
            success = false;
        }
    }

    return (success);
}

/**
* Read flash information (manufacturer and device ID)
* @return True when successful.
*/
static bool extFlashReadInfo(void)
{
    int ret;
    const uint8_t wbuf[] = { BLS_CODE_MDID, 0xFF, 0xFF, 0x00 };

    extFlashSelect();

    ret = bspSpiWrite(wbuf, sizeof(wbuf));
    if (ret)
    {
        extFlashDeselect();
        return (false);
    }

    ret = bspSpiRead(infoBuf, sizeof(infoBuf));
    extFlashDeselect();

    return (ret == 0);
}

/**
* Verify the flash part.
* @return True when successful.
*/
static bool extFlashVerifyPart(void)
{
    if (!extFlashReadInfo())
    {
        return (false);
    }

    pFlashInfo = flashInfo;
    while (pFlashInfo->deviceSize > 0)
    {
        if (infoBuf[0] == pFlashInfo->manfId && infoBuf[1] == pFlashInfo->devId)
        {
            break;
        }
        pFlashInfo++;
    }

    return (pFlashInfo->deviceSize > 0);
}

/**
* Wait till previous erase/program operation completes.
* @return Zero when successful.
*/
static int extFlashWaitReady(void)
{
    const uint8_t wbuf[1] = { BLS_CODE_READ_STATUS };
    int ret;

    /* Throw away garbage */
    extFlashSelect();
    bspSpiFlush();
    extFlashDeselect();

    for (;;)
    {
        uint8_t buf;

        extFlashSelect();
        bspSpiWrite(wbuf, sizeof(wbuf));
        ret = bspSpiRead(&buf,sizeof(buf));

        extFlashDeselect();

        if (ret)
        {
            /* Error */
            return (-2);
        }
        if (!(buf & BLS_STATUS_BIT_BUSY))
        {
            /* Now ready */
            break;
        }
    }

    return (0);
}


/**
* Wait until the part has entered power down (JDEC readout fails)
* @return Zero when successful.
*/
static int extFlashWaitPowerDown(void)
{
    uint8_t i;

    i = 0;
    while (i<10)
    {
        if (!extFlashVerifyPart())
        {
            return (0);
        }
        i++;
    }

    return (-1);
}

/**
* Enable write.
* @return Zero when successful.
*/
static int extFlashWriteEnable(void)
{
    const uint8_t wbuf[] = { BLS_CODE_WRITE_ENABLE };

    extFlashSelect();
    int ret = bspSpiWrite(wbuf,sizeof(wbuf));
    extFlashDeselect();

    if (ret)
    {
        return (-3);
    }

    return (0);
}

/* See ext_flash.h file for description */
bool extFlashOpen(void)
{
    bool f;

    bspSpiOpen(SPI_BIT_RATE, BSP_SPI_CLK_FLASH);

    /* GPIO pin configuration */
    IOCPinTypeGpioOutput(BSP_IOID_FLASH_CS);

    /* Default output to clear chip select */
    extFlashDeselect();

    /* Put the part is standby mode */
    f = extFlashPowerStandby();

    if (f)
    {
        f = extFlashVerifyPart();
    }
    else /* if (!f) */
    {
        extFlashClose();
    }

    return (f);
}

/* See ext_flash.h file for description */
void extFlashClose(void)
{
    // Put the part in low power mode
    extFlashPowerDown();
    extFlashWaitPowerDown();

    bspSpiClose();
}

/* See ext_flash.h file for description */
bool extFlashRead(size_t offset, size_t length, uint8_t *buf)
{
    uint8_t wbuf[4];

    /* Wait till previous erase/program operation completes */
    int ret = extFlashWaitReady();
    if (ret)
    {
        return false;
    }

    /* SPI is driven with very low frequency (1MHz < 33MHz fR spec)
    * in this temporary implementation.
    * and hence it is not necessary to use fast read. */
    wbuf[0] = BLS_CODE_READ;
    wbuf[1] = (offset >> 16) & 0xff;
    wbuf[2] = (offset >> 8) & 0xff;
    wbuf[3] = offset & 0xff;

    extFlashSelect();

    if (bspSpiWrite(wbuf, sizeof(wbuf)))
    {
        /* failure */
        extFlashDeselect();
        return (false);
    }

    ret = bspSpiRead(buf, length);

    extFlashDeselect();

    return (ret == 0);
}

/* See ext_flash.h file for description */
bool extFlashWrite(size_t offset, size_t length, const uint8_t *buf)
{
    uint8_t wbuf[4];

    while (length > 0)
    {
        /* Wait till previous erase/program operation completes */
        int ret = extFlashWaitReady();
        if (ret)
        {
            return (false);
        }

        ret = extFlashWriteEnable();
        if (ret)
        {
            return (false);
        }

        size_t ilen; /* interim length per instruction */

        ilen = BLS_PROGRAM_PAGE_SIZE - (offset % BLS_PROGRAM_PAGE_SIZE);
        if (length < ilen)
        {
            ilen = length;
        }

        wbuf[0] = BLS_CODE_PROGRAM;
        wbuf[1] = (offset >> 16) & 0xff;
        wbuf[2] = (offset >> 8) & 0xff;
        wbuf[3] = offset & 0xff;

        offset += ilen;
        length -= ilen;

        /* Up to 100ns CS hold time (which is not clear
        * whether it's application only in between reads)
        * is not imposed here since above instructions
        * should be enough to delay
        * as much. */
        extFlashSelect();


        if (bspSpiWrite(wbuf, sizeof(wbuf)))
        {
            /* failure */
            extFlashDeselect();
            return (false);
        }

        if (bspSpiWrite(buf,ilen))
        {
            /* failure */
            extFlashDeselect();
            return (false);
        }
        buf += ilen;
        extFlashDeselect();
    }

    return (true);
}

/* See ext_flash.h file for description */
bool extFlashErase(size_t offset, size_t length)
{
    /* Note that Block erase might be more efficient when the floor map
    * is well planned for OTA but to simplify for the temporary implemetation,
    * sector erase is used blindly. */
    uint8_t wbuf[4];
    size_t i, numsectors;

    wbuf[0] = BLS_CODE_SECTOR_ERASE;

    {
        size_t endoffset = offset + length - 1;
        offset = (offset / BLS_ERASE_SECTOR_SIZE) * BLS_ERASE_SECTOR_SIZE;
        numsectors = (endoffset - offset + BLS_ERASE_SECTOR_SIZE - 1) /
                      BLS_ERASE_SECTOR_SIZE;
    }

    for (i = 0; i < numsectors; i++)
    {
        /* Wait till previous erase/program operation completes */
        int ret = extFlashWaitReady();
        if (ret)
        {
            return (false);
        }

        ret = extFlashWriteEnable();
        if (ret)
        {
            return (false);
        }

        wbuf[1] = (offset >> 16) & 0xff;
        wbuf[2] = (offset >> 8) & 0xff;
        wbuf[3] = offset & 0xff;

        extFlashSelect();

        if (bspSpiWrite(wbuf, sizeof(wbuf)))
        {
            /* failure */
            extFlashDeselect();
            return (false);
        }
        extFlashDeselect();

        offset += BLS_ERASE_SECTOR_SIZE;
    }

    return (true);
}

/* See ext_flash.h file for description */
bool extFlashTest(void)
{
    bool ret;

    ret = extFlashOpen();
    if (ret)
    {
        extFlashClose();
    }

    return (ret);
}
