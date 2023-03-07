/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** ============================================================================
 *  @file       ExtFlash.c
 *
 *  @brief      External flash storage implementation.
 *  ============================================================================
 */

/* -----------------------------------------------------------------------------
*  Includes
* ------------------------------------------------------------------------------
*/
#include "ti_drivers_config.h"
#include "ExtFlash.h"
#include "string.h"
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/ssi.h)

/* -----------------------------------------------------------------------------
*  Constants and macros
* ------------------------------------------------------------------------------
*/

/*
 * Implementation for JEDEC compatible Flash
 *
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

/* Manufacturer IDs */
#define MF_MACRONIX               0xC2
#define MF_WINBOND                0xEF

/* -----------------------------------------------------------------------------
*  Private functions
* ------------------------------------------------------------------------------
*/
static bool Spi_open(uint32_t bitRate);
static void Spi_close(void);
static void Spi_flash(void);
static int Spi_read(uint8_t *buf, size_t length);
static int Spi_write(const uint8_t *buf, size_t length);
static int ExtFlash_waitReady(void);
static int ExtFlash_powerDown(void);

/* -----------------------------------------------------------------------------
*  Local variables
* ------------------------------------------------------------------------------
*/
static PIN_Config BoardFlashPinTable[] =
{
    Board_SPI_FLASH_CS | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MIN, /* Ext. flash chip select */

    PIN_TERMINATE
};

static PIN_Handle hFlashPin = NULL;
static PIN_State pinState;

// Supported flash devices
static ExtFlashInfo_t flashInfo[] =
{
    {
        .manfId = MF_MACRONIX,  // Macronics
        .devId = 0x15,          // MX25R1635F
        .deviceSize = 0x200000  // 2 MByte (16 Mbit)
    },
    {
        .manfId = MF_MACRONIX,  // Macronics
        .devId = 0x14,          // MX25R8035F
        .deviceSize = 0x100000  // 1 MByte (8 Mbit)
    },
    {
        .manfId = MF_WINBOND,   // WinBond
        .devId = 0x12,          // W25X40CL
        .deviceSize = 0x080000  // 512 KByte (4 Mbit)
    },
    {
        .manfId = MF_WINBOND,   // WinBond
        .devId = 0x11,          // W25X20CL
        .deviceSize = 0x040000  // 256 KByte (2 Mbit)
    },
    {
        .manfId = 0x0,
        .devId = 0x0,
        .deviceSize = 0x0
    }
};

// Flash information
static ExtFlashInfo_t *pFlashInfo = NULL;
static uint8_t infoBuf[2];

// SPI interface
static SPI_Handle spiHandle = NULL;
static SPI_Params spiParams;

/* -----------------------------------------------------------------------------
*  Functions
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
    PIN_setOutputValue(hFlashPin,Board_SPI_FLASH_CS,Board_FLASH_CS_ON);
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
    PIN_setOutputValue(hFlashPin,Board_SPI_FLASH_CS,Board_FLASH_CS_OFF);
}

/*******************************************************************************
* @fn       ExtFlash_info
*
* @brief    Get information about the mounted flash
*
* @param    none
*
* @return   return flash info record (all fields are zero if not found)
*******************************************************************************/
ExtFlashInfo_t *ExtFlash_info(void)
{
    return pFlashInfo;
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
    success = Spi_write(&cmd,sizeof(cmd)) == 0;
    extFlashDeselect();

    return success;
}

/*******************************************************************************
* @fn       extFlashPowerStandby
*
* @brief    Take device out of power save mode and prepare it for normal operation
*
* @param    none
*
* @return   Returns true if command successfully written
*******************************************************************************/
static bool extFlashPowerStandby(void)
{
    uint8_t cmd;
    bool success;

    cmd = BLS_CODE_RDP;
    extFlashSelect();
    success = Spi_write(&cmd,sizeof(cmd)) == 0;
    extFlashDeselect();

    if (success)
    {
        if (ExtFlash_waitReady() != 0)
        {
            success = false;
        }
    }

    return success;
}

/**
* Read flash information (manufacturer and device ID)
* @return True when successful.
*/
static bool ExtFlash_readInfo(void)
{
    const uint8_t wbuf[] = { BLS_CODE_MDID, 0xFF, 0xFF, 0x00 };

    extFlashSelect();

    int ret = Spi_write(wbuf, sizeof(wbuf));
    if (ret)
    {
        extFlashDeselect();
        return false;
    }

    ret = Spi_read(infoBuf, sizeof(infoBuf));
    extFlashDeselect();

    return ret == 0;
}

/**
* Verify the flash part.
* @return True when successful.
*/

static bool extFlashVerifyPart(void)
{
    if (!ExtFlash_readInfo())
    {
        return false;
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

    return pFlashInfo->deviceSize > 0;
}

/**
* Wait till previous erase/program operation completes.
* @return Zero when successful.
*/
static int ExtFlash_waitReady(void)
{
    const uint8_t wbuf[1] = { BLS_CODE_READ_STATUS };
    int ret;

    /* Throw away all garbage */
    extFlashSelect();
    Spi_flash();
    extFlashDeselect();

    for (;;)
    {
        uint8_t buf;

        extFlashSelect();
        Spi_write(wbuf, sizeof(wbuf));
        ret = Spi_read(&buf,sizeof(buf));
        extFlashDeselect();

        if (ret)
        {
            /* Error */
            return -2;
        }
        if (!(buf & BLS_STATUS_BIT_BUSY))
        {
            /* Now ready */
            break;
        }
    }

    return 0;
}

/**
* Wait until the part has entered power down (JDEC readout fails).
* @return Zero when successful.
*/
static int ExtFlash_powerDown(void)
{
    uint8_t i;

    i = 0;
    while (i<10)
    {
        if (!ExtFlash_readInfo())
        {
            return 0;
        }
        i++;
    }

    return -1;
}

/**
* Enable write.
* @return Zero when successful.
*/
static int ExtFlash_writeEnable(void)
{
    const uint8_t wbuf[] = { BLS_CODE_WRITE_ENABLE };

    extFlashSelect();
    int ret = Spi_write(wbuf,sizeof(wbuf));
    extFlashDeselect();

    if (ret)
    {
        return -3;
    }
    return 0;
}


/* See ExtFlash.h file for description */
bool ExtFlash_open(void)
{
    bool f;

    hFlashPin = PIN_open(&pinState, BoardFlashPinTable);

    if (hFlashPin == NULL)
    {
        return false;
    }

    /* Initialise SPI. Subsequent calls will do nothing. */
    SPI_init();

    /* Make sure SPI is available */
    f = Spi_open(SPI_BIT_RATE);

    if (f)
    {
        /* Put the part is standby mode */
        f = extFlashPowerStandby();

        if (f)
        {
            /* Verify manufacturer and device ID */
            f = extFlashVerifyPart();
        }

        if (!f)
        {
            ExtFlash_close();
        }
    }

    return f;
}

/* See ExtFlash.h file for description */
void ExtFlash_close(void)
{
    if (hFlashPin != NULL)
    {
        // Put the part in low power mode
        extFlashPowerDown();
        if (pFlashInfo->manfId == MF_WINBOND)
        {
            ExtFlash_powerDown();
        }

        /* Make sure SPI lines have a defined state */
        Spi_close();

        PIN_close(hFlashPin);
        hFlashPin = NULL;
    }
}

/* See ExtFlash.h file for description */
bool ExtFlash_read(size_t offset, size_t length, uint8_t *buf)
{
    uint8_t wbuf[4];

    /* Wait till previous erase/program operation completes */
    int ret = ExtFlash_waitReady();
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

    if (Spi_write(wbuf, sizeof(wbuf)))
    {
        /* failure */
        extFlashDeselect();
        return false;
    }

    ret = Spi_read(buf, length);

    extFlashDeselect();

    return ret == 0;
}

/* See ExtFlash.h file for description */
bool ExtFlash_write(size_t offset, size_t length, const uint8_t *buf)
{
    uint8_t wbuf[4];

    while (length > 0)
    {
        /* Wait till previous erase/program operation completes */
        int ret = ExtFlash_waitReady();
        if (ret)
        {
            return false;
        }

        ret = ExtFlash_writeEnable();
        if (ret)
        {
            return false;
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

        if (Spi_write(wbuf, sizeof(wbuf)))
        {
            /* failure */
            extFlashDeselect();
            return false;
        }

        if (Spi_write(buf,ilen))
        {
            /* failure */
            extFlashDeselect();
            return false;
        }
        buf += ilen;
        extFlashDeselect();
    }

    return true;
}

/* See ExtFlash.h file for description */
bool ExtFlash_erase(size_t offset, size_t length)
{
    /* Note that Block erase might be more efficient when the floor map
    * is well planned for OTA but to simplify for the temporary implementation,
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
        int ret = ExtFlash_waitReady();
        if (ret)
        {
            return false;
        }

        ret = ExtFlash_writeEnable();
        if (ret)
        {
            return false;
        }

        wbuf[1] = (offset >> 16) & 0xff;
        wbuf[2] = (offset >> 8) & 0xff;
        wbuf[3] = offset & 0xff;

        extFlashSelect();

        if (Spi_write(wbuf, sizeof(wbuf)))
        {
            /* failure */
            extFlashDeselect();
            return false;
        }
        extFlashDeselect();

        offset += BLS_ERASE_SECTOR_SIZE;
    }

    return true;
}

/* See ExtFlash.h file for description */
bool ExtFlash_test(void)
{
    bool ret;

    ret = ExtFlash_open();
    if (ret)
    {
        ExtFlash_close();
    }

    return ret;
}

/*******************************************************************************
*
*   SPI interface
*
*******************************************************************************/

/*******************************************************************************
* @fn          Spi_write
*
* @brief       Write to an SPI device
*
* @param       buf - pointer to data buffer
* @param       len - number of bytes to write
*
* @return      '0' if success, -1 if failed
*/
static int Spi_write(const uint8_t *buf, size_t len)
{
    SPI_Transaction masterTransaction;

    masterTransaction.count  = len;
    masterTransaction.txBuf  = (void*)buf;
    masterTransaction.arg    = NULL;
    masterTransaction.rxBuf  = NULL;

    return SPI_transfer(spiHandle, &masterTransaction) ? 0 : -1;
}


/*******************************************************************************
* @fn          Spi_read
*
* @brief       Read from an SPI device
*
* @param       buf - pointer to data buffer
* @param       len - number of bytes to write
*
* @return      '0' if success, -1 if failed
*/
static int Spi_read(uint8_t *buf, size_t len)
{
    SPI_Transaction masterTransaction;

    masterTransaction.count = len;
    masterTransaction.txBuf = NULL;
    masterTransaction.arg = NULL;
    masterTransaction.rxBuf = buf;

    return SPI_transfer(spiHandle, &masterTransaction) ? 0 : -1;
}


/*******************************************************************************
* @fn          Spi_open
*
* @brief       Open the RTOS SPI driver
*
* @param       bitRate - transfer speed in bits/sec
*
* @return      true if success
*/
static bool Spi_open(uint32_t bitRate)
{
    /*  Configure SPI as master */
    SPI_Params_init(&spiParams);
    spiParams.bitRate = bitRate;
    spiParams.mode = SPI_MASTER;
    spiParams.transferMode = SPI_MODE_BLOCKING;

    /* Attempt to open SPI. */
    spiHandle = SPI_open(Board_SPI0, &spiParams);

    return spiHandle != NULL;
}

/*******************************************************************************
* @fn          Spi_close
*
* @brief       Close the RTOS SPI driver
*
* @return      none
*/
static void Spi_close(void)
{
    if (spiHandle != NULL)
    {
        // Close the RTOS driver
        SPI_close(spiHandle);
        spiHandle = NULL;
    }
}


/*******************************************************************************
* @fn          Spi_flash
*
* @brief       Get rid of garbage from the slave
*
* @param       none
*
* @return      none
*/
static void Spi_flash(void)
{
    /* make sure SPI hardware module is done  */
    while(SSIBusy(((SPICC26XXDMA_HWAttrsV1*)spiHandle->hwAttrs)->baseAddr))
    { };
}
