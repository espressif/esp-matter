/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>

#include "mflash_drv.h"
#include "board.h"
#include "fsl_qspi.h"
#include "fsl_flashc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLASH_INS_CODE_WE     0x06 /*!< Write enable */
#define FLASH_INS_CODE_WE_VSR 0x50 /*!< Write enable for volatile status register */
#define FLASH_INS_CODE_WD     0x04 /*!< Write disable */
#define FLASH_INS_CODE_RSR1   0x05 /*!< Read status register 1 */
#define FLASH_INS_CODE_RSR2   0x35 /*!< Read status register 2 */
#define FLASH_INS_CODE_WSR1   0x01 /*!< Write status register 1 */
#define FLASH_INS_CODE_WSR2   0x31 /*!< Write status register 2 */
#define FLASH_INS_CODE_PP     0x02 /*!< Page program */
#define FLASH_INS_CODE_QPP    0x32 /*!< Quad page program */
#define FLASH_INS_CODE_QPP_MX 0x38 /*!< Quad page program for Macronix chip*/
#define FLASH_INS_CODE_SE     0x20 /*!< Sector(4k) erase */
#define FLASH_INS_CODE_BE_32K 0x52 /*!< Block(32k) erase */
#define FLASH_INS_CODE_BE_64K 0xD8 /*!< Block(64k) erase */
#define FLASH_INS_CODE_CE     0xC7 /*!< Chip erase */
#define FLASH_INS_CODE_PD     0xB9 /*!< Power down */

#define FLASH_INS_CODE_RD      0x03   /*!< Read data */
#define FLASH_INS_CODE_FR      0x0B   /*!< Fast read */
#define FLASH_INS_CODE_FRDO    0x3B   /*!< Fast read dual output */
#define FLASH_INS_CODE_FRQO    0x6B   /*!< Fast read quad output */
#define FLASH_INS_CODE_FRDIO   0xBB   /*!< Fast read dual IO */
#define FLASH_INS_CODE_FRQIO   0xEB   /*!< Fast read quad IO */
#define FLASH_INS_CODE_WFRQIO  0xE7   /*!< Word Fast read quad IO, A0 must be zero */
#define FLASH_INS_CODE_OWFRQIO 0xE3   /*!< Octal word Fast read quad IO, A[3:0] must be zero */
#define FLASH_INS_CODE_FDRST   0xFFFF /*!< Fast dual read mode reset */
#define FLASH_INS_CODE_FQRST   0xFF   /*!< Fast quad read mode reset */

#define FLASH_INS_CODE_RPD_DI   0xAB /*!< Release power down or device ID */
#define FLASH_INS_CODE_RUID     0x4B /*!< Read unique ID number */
#define FLASH_INS_CODE_JEDEC_ID 0x9F /*!< Read JEDEC ID number */

/*! @brief Flash read type */
typedef enum
{
    kFLASH_NormalRead,              /*!< Normal read mode */
    kFLASH_FastRead,                /*!< Fast read mode */
    kFLASH_FastReadDualOUT,         /*!< Fast read dual output mode */
    kFLASH_FastReadDaulIO,          /*!< Fast read dual IO mode */
    kFLASH_FastReadQuadOUT,         /*!< Fast read quad output mode */
    kFLASH_FastReadQuadIO,          /*!< Fast read quad IO mode */
    kFLASH_WordFastReadQuadIO,      /*!< Word fast read quad IO mode */
    kFLASH_OctalWordFastReadQuadIO, /*!< Octal word fast read quad IO mode */
} flash_read_mode_type_t;

/*! @brief Flash program type */
typedef enum
{
    kFLASH_ProgramNormal, /*!< Normal page program mode */
    kFLASH_ProgramQuad,   /*!< Quad page program mode   */
} flash_program_mode_type_t;

#define KILO_BYTE 0x400    /*!< 1KB */
#define MEGA_BYTE 0x100000 /*!< 1MB */

#define FLASH_32K_BLOCK_SIZE 0x8000  /*!< 32KB */
#define FLASH_64K_BLOCK_SIZE 0x10000 /*!< 64KB */

/*! Get the page number according to the address with the page space */
#define FLASH_PAGE_NUM(addr) ((addr) >> 8)

#define FLASH_NAME_SIZE 16

/*! @brief Flash device configuration structure */
struct FlashDeviceConfig
{
    char name[FLASH_NAME_SIZE]; /*!< Flash name. */
    uint32_t jedecId;           /*!< JEDEC ID. */
    uint32_t chipSize;          /*!< Flash memory size. */
    uint32_t sectorSize;        /*!< Flash memory sector size. */
    uint32_t blockSize;         /*!< Flash memory block size. */
    uint16_t pageSize;          /*!< Flash memory page size. */
};

const struct FlashDeviceConfig flashDevList[] = {
    {"W25Q80BL", 0xef4014, 1 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"W25Q16CL", 0xef4015, 2 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"W25Q32BV", 0xef4016, 4 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"W25Q32JV", 0xef4016, 4 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"W25Q64CV", 0xef4017, 8 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"W25Q128BV", 0xef4018, 16 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"GD25Q16B", 0xc84015, 2 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"GD25Q16C", 0xc84015, 2 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"GD25Q32C", 0xc84016, 4 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"GD25LQ32C", 0xc86016, 4 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"GD25Q127C", 0xc84018, 16 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25L8035E", 0xc22014, 1 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25L3233F", 0xc22016, 4 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25L6433F", 0xc22017, 8 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25L12835F", 0xc22018, 16 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R512F", 0xc22810, 64 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R1035F", 0xc22811, 128 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R2035F", 0xc22812, 256 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R4035F", 0xc22813, 512 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R8035F", 0xc22814, 1 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R1635F", 0xc22815, 2 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R3235F", 0xc22816, 4 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25R6435F", 0xc22817, 8 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25V512F", 0xc22310, 64 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25V1035F", 0xc22311, 128 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25V2035F", 0xc22312, 256 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25V4035F", 0xc22313, 512 * KILO_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25V8035F", 0xc22314, 1 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
    {"MX25V1635F", 0xc22315, 2 * MEGA_BYTE, 4 * KILO_BYTE, 64 * KILO_BYTE, 256},
};

#define FLASH_WEL_BIT_SET   0x02U
#define FLASH_WEL_BIT_CLEAR 0x00U

#ifndef FLASH_DEFAULT_INDEX
#define FLASH_DEFAULT_INDEX (0x0C)
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static status_t FLASH_CheckWriteEnableLatch(QSPI_Type *base);
static uint8_t FLASH_GetStatusReg(QSPI_Type *base, uint8_t index);
static bool FLASH_GetBusyStatus(QSPI_Type *base);

/*******************************************************************************
 * Variables
 *****************************************************************************/
/* Flash address offset from AHB access. */
static uint32_t mflashOffset;
static bool mflashInitialized;

static const struct FlashDeviceConfig *g_flashConfig = &flashDevList[FLASH_DEFAULT_INDEX];

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t FLASH_CheckWriteEnableLatch(QSPI_Type *base)
{
    status_t status        = kStatus_Success;
    uint32_t statusReg1Val = 0x00U;

    /*Check WEL bit is cleared or not. */
    statusReg1Val = FLASH_GetStatusReg(base, 0x01U);
    if ((statusReg1Val & FLASH_WEL_BIT_SET) == FLASH_WEL_BIT_SET)
    {
        status = kStatus_Fail;
    }

    return status;
}

/* Set flash write enable / disable  */
static void FLASH_WriteEnable(QSPI_Type *base, bool enable)
{
    qspi_header_count_config_t config;

    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI1 FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x00U);

    if (enable == true)
    {
        /* Set instruction for write enable. */
        QSPI_SetInstruction(base, FLASH_INS_CODE_WE);
    }
    else
    {
        /* Set instruction for write disable. */
        QSPI_SetInstruction(base, FLASH_INS_CODE_WD);
    }

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);
}

/* Write enable for volatile status register */
static void FLASH_WriteEnableVolatileStatusReg(QSPI_Type *base)
{
    qspi_header_count_config_t config;

    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI1 FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x00U);

    /* Set instruction for volatile status register. */
    QSPI_SetInstruction(base, FLASH_INS_CODE_WE_VSR);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);
}

/* Reset Flash Continuous Quad Read mode */
static void FLASH_ResetFastReadQuad(QSPI_Type *base)
{
    qspi_header_count_config_t config;

    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt1Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI1 FIFO */
    QSPI_FlushFIFO(base);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinQuad);

    /* Set QSPI address pin mode */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set read mode. */
    QSPI_SetReadMode(base, 0x00U);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x00U);

    /* Set instruction for fast read quad mode reset. */
    QSPI_SetInstruction(base, FLASH_INS_CODE_FQRST);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);
}

/* Get the flash status register value */
static uint8_t FLASH_GetStatusReg(QSPI_Type *base, uint8_t index)
{
    uint8_t status;
    qspi_header_count_config_t config;

    /* Configure header count. */
    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI1 FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x01U);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    if (index == 0x01U)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_RSR1);
    }
    else if (index == 0x02U)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_RSR2);
    }

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI read */
    QSPI_StartTransfer(base, kQSPI_Read);

    /* Get flash busy status */
    status = QSPI_ReadByte(base);

    /* Disable QSPI */
    QSPI_SetSSEnable(base, false);

    return status;
}

/* Write flash status register */
static status_t FLASH_WriteStatusReg(QSPI_Type *base, uint16_t status)
{
    volatile uint32_t localCnt = 0;
    uint8_t byte;

    /* 0xC2 is for macronix. */
    if (((g_flashConfig->jedecId >> 16) & 0xFF) == 0xc2)
    {
        FLASH_WriteEnable(base, true);
    }
    else
    {
        /* Enable flash write */
        FLASH_WriteEnableVolatileStatusReg(base);
    }

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* Set instruction */
    QSPI_SetInstruction(base, FLASH_INS_CODE_WSR1);

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Write status[7:0] */
    byte = status & 0xFF;
    QSPI_WriteByte(base, byte);

    if ((g_flashConfig->jedecId != 0xc84016) && (g_flashConfig->jedecId != 0xc84018) &&
        (((g_flashConfig->jedecId >> 16) & 0xFF) != 0xc2))
    {
        /* Write status[15:8] */
        byte = (status >> 8) & 0xFF;
        QSPI_WriteByte(base, byte);
    }

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);

    while (localCnt++ < 100000)
    {
        /* Check flash busy status */
        if (FLASH_GetBusyStatus(base) == false)
        {
            if ((g_flashConfig->jedecId != 0xc84016) && (g_flashConfig->jedecId != 0xc84018))
            {
                return kStatus_Success;
            }
            else
            {
                break;
            }
        }
    }

    /* Following code is specific to GigaDevice Flash GD25Q32C
       only and is written as per its datasheet */
    if ((g_flashConfig->jedecId == 0xc84016) || (g_flashConfig->jedecId == 0xc84018))
    {
        /* Write enable for volatile status register */
        FLASH_WriteEnableVolatileStatusReg(base);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinSingle);

        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_WSR2);

        /* QSPI one-byte length mode */
        QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

        /* Set QSPI write */
        QSPI_StartTransfer(base, kQSPI_Write);

        /* Write status[15:8] */
        byte = (status >> 8) & 0xFF;
        QSPI_WriteByte(base, byte);

        /* Stop QSPI transfer */
        QSPI_StopTransfer(base);

        localCnt = 0;
        while (localCnt++ < 100000)
        {
            /* Check flash busy status */
            if (FLASH_GetBusyStatus(base) == false)
            {
                return kStatus_Success;
            }
        }
    }

    return kStatus_Fail;
}

/* Get the flash busy status */
static bool FLASH_GetBusyStatus(QSPI_Type *base)
{
    bool status;
    qspi_header_count_config_t config;

    /* Configure header count. */
    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI1 FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x01U);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* Set instruction */
    QSPI_SetInstruction(base, FLASH_INS_CODE_RSR1);

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI read */
    QSPI_StartTransfer(base, kQSPI_Read);

    /* Get flash busy status */
    status = (QSPI_ReadByte(base) & 0x01U) ? true : false;

    /* Disable QSPI */
    QSPI_SetSSEnable(base, false);

    return status;
}

/* Flash sector erase */
static status_t FLASH_SectorErase(QSPI_Type *base, uint32_t sectorNumber)
{
    status_t status = kStatus_Fail;
    uint32_t sectorAddress;
    uint32_t lastSector        = (g_flashConfig->chipSize / g_flashConfig->sectorSize) - 0x01U;
    volatile uint32_t localCnt = 0U;

    if (!(sectorNumber > lastSector))
    {
        /* Enable flash write */
        FLASH_WriteEnable(base, true);

        /* Get start address for sector to be erased */
        sectorAddress = sectorNumber * g_flashConfig->sectorSize;

        /* Set address counter */
        QSPI_SetAddressCnt(base, kQSPI_AddressCnt3Byte);

        /* Set address pin. */
        QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

        /* Set data pin. */
        QSPI_SetDatePin(base, kQSPI_DataPinSingle);

        /* Set address */
        QSPI_SetAddress(base, sectorAddress);

        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_SE);

        /* Set QSPI write */
        QSPI_StartTransfer(base, kQSPI_Write);

        /* Stop QSPI transfer */
        QSPI_StopTransfer(base);

        while (localCnt++ < 1000000)
        {
            /* Check flash busy status */
            if (FLASH_GetBusyStatus(base) == false)
            {
                status = kStatus_Success;
                break;
            }
        }
    }

    /*Check WEL bit is cleared or not. */
    if (kStatus_Success != FLASH_CheckWriteEnableLatch(base))
    {
        status = kStatus_Fail;
    }

    return status;
}

/* Flash 32KB block erase */
static status_t FLASH_Block32KErase(QSPI_Type *base, uint32_t blockNumber)
{
    status_t status = kStatus_Fail;
    uint32_t blockAddress;
    volatile uint32_t localCnt = 0;
    uint32_t last32KBlock      = (g_flashConfig->chipSize / FLASH_32K_BLOCK_SIZE) - 1;

    if (!(blockNumber > last32KBlock))
    {
        /* Enable flash write */
        FLASH_WriteEnable(base, true);

        /* Get start address of the block to be erased */
        blockAddress = blockNumber * FLASH_32K_BLOCK_SIZE;

        /* Set address counter */
        QSPI_SetAddressCnt(base, kQSPI_AddressCnt3Byte);

        /* Set address pin. */
        QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

        /* Set data pin. */
        QSPI_SetDatePin(base, kQSPI_DataPinSingle);

        /* Set address */
        QSPI_SetAddress(base, blockAddress);

        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_BE_32K);

        /* Set QSPI write */
        QSPI_StartTransfer(base, kQSPI_Write);

        /* Stop QSPI transfer */
        QSPI_StopTransfer(base);

        while (localCnt++ < 2000000)
        {
            /* Check flash busy status */
            if (FLASH_GetBusyStatus(base) == false)
            {
                status = kStatus_Success;
                break;
            }
        }
    }

    /*Check WEL bit is cleared or not. */
    if (kStatus_Success != FLASH_CheckWriteEnableLatch(base))
    {
        status = kStatus_Fail;
    }

    return status;
}

/* Flash 64KB block erase */
static status_t FLASH_Block64KErase(QSPI_Type *base, uint32_t blockNumber)
{
    status_t status = kStatus_Fail;
    uint32_t blockAddress;
    volatile uint32_t localCnt = 0;
    uint32_t last64KBlock      = (g_flashConfig->chipSize / FLASH_64K_BLOCK_SIZE) - 1;

    if (!(blockNumber > last64KBlock))
    {
        /* Enable flash write */
        FLASH_WriteEnable(base, true);

        /* Get start address of the block to be erased */
        blockAddress = blockNumber * FLASH_64K_BLOCK_SIZE;

        /* Set address counter */
        QSPI_SetAddressCnt(base, kQSPI_AddressCnt3Byte);

        /* Set address pin. */
        QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

        /* Set data pin. */
        QSPI_SetDatePin(base, kQSPI_DataPinSingle);

        /* Set address */
        QSPI_SetAddress(base, blockAddress);

        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_BE_64K);

        /* Set QSPI write */
        QSPI_StartTransfer(base, kQSPI_Write);

        /* Stop QSPI transfer */
        QSPI_StopTransfer(base);

        while (localCnt++ < 2000000)
        {
            /* Check flash busy status */
            if (FLASH_GetBusyStatus(base) == false)
            {
                status = kStatus_Success;
                break;
            }
        }
    }

    /*Check WEL bit is cleared or not. */
    if (kStatus_Success != FLASH_CheckWriteEnableLatch(base))
    {
        status = kStatus_Fail;
    }

    return status;
}

/* Erase specfied address of the flash */
static status_t FLASH_Erase(QSPI_Type *base, uint32_t startAddr, uint32_t endAddr)
{
    status_t ret;
    uint32_t sectorNumber, blockNumber, length, validStart;

    length = endAddr - startAddr + 1;

    while (length != 0)
    {
        if ((startAddr & (FLASH_64K_BLOCK_SIZE - 1)) == 0 &&
            length > (FLASH_64K_BLOCK_SIZE - g_flashConfig->sectorSize))
        {
            /* Address is a multiple of 64K and length is > (64K block -4K sector)
             * So directly erase 64K from this address */
            blockNumber = startAddr / FLASH_64K_BLOCK_SIZE;
            ret         = FLASH_Block64KErase(base, blockNumber);
            endAddr     = startAddr + FLASH_64K_BLOCK_SIZE;
        }
        else if ((startAddr & (FLASH_32K_BLOCK_SIZE - 1)) == 0 &&
                 length > (FLASH_32K_BLOCK_SIZE - g_flashConfig->sectorSize))
        {
            /* Address is a multiple of 32K and length is > (32K block -4K sector)
             * So directly erase 32K from this address */
            blockNumber = startAddr / FLASH_32K_BLOCK_SIZE;
            ret         = FLASH_Block32KErase(base, blockNumber);
            endAddr     = startAddr + FLASH_32K_BLOCK_SIZE;
        }
        else
        {
            /* Find 4K aligned address and erase 4K sector */
            validStart   = startAddr - (startAddr & (g_flashConfig->sectorSize - 1));
            sectorNumber = validStart / g_flashConfig->sectorSize;
            ret          = FLASH_SectorErase(base, sectorNumber);
            endAddr      = validStart + g_flashConfig->sectorSize;
        }

        /* If erase operation fails then return error */
        if (ret != kStatus_Success)
        {
            return kStatus_Fail;
        }

        /* Calculate the remaining length that is to be erased yet */
        if (length < (endAddr - startAddr))
        {
            length = 0;
        }
        else
        {
            length -= (endAddr - startAddr);
        }

        startAddr = endAddr;
    }

    return kStatus_Success;
}

/* Read flash from specified address to buffer */
uint32_t FLASH_Read(QSPI_Type *base, flash_read_mode_type_t mode, uint32_t address, uint8_t *buffer, uint32_t size)
{
    uint32_t readBytes   = 0x00U;
    uint16_t statusWrite = 0x00U;
    uint16_t statusReg1  = 0x00U;
    uint16_t statusReg2  = 0x00U;

    qspi_header_count_config_t config;

    /* Configure header count. */
    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt3Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Enable quad mode. */
    if ((mode == kFLASH_FastReadQuadOUT) || (mode == kFLASH_FastReadQuadIO) || (mode == kFLASH_WordFastReadQuadIO) ||
        (mode == kFLASH_OctalWordFastReadQuadIO))
    {
        /* Get status register 1. */
        statusReg1 = FLASH_GetStatusReg(base, 0x01U);

        if (((g_flashConfig->jedecId >> 16) & 0xFF) != 0xc2U)
        {
            /* Get status register 2. */
            statusReg2  = FLASH_GetStatusReg(base, 0x02U);
            statusWrite = ((statusReg2 << 8) | statusReg1) | 0x0200U;
            FLASH_WriteStatusReg(base, statusWrite);
        }
        else if ((statusReg1 & 0x40U) != 0x40U)
        {
            statusWrite = statusReg1 | 0x40U;
            FLASH_WriteStatusReg(base, statusWrite);
        }
    }

    /* Clear QSPI FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set read mode */
    QSPI_SetReadMode(base, 0x00U);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, size);

    /* Set address */
    QSPI_SetAddress(base, address);

    /* Set QSPI address pin mode */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    if (mode == kFLASH_NormalRead)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_RD);
    }
    else if (mode == kFLASH_FastRead)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_FR);

        /* Set dummy counter */
        QSPI_SetDummyCnt(base, kQSPI_DummyCnt1Byte);
    }
    else if (mode == kFLASH_FastReadDualOUT)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_FRDO);

        /* Set dummy counter */
        QSPI_SetDummyCnt(base, kQSPI_DummyCnt1Byte);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinDual);
    }
    else if (mode == kFLASH_FastReadDaulIO)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_FRDIO);

        /* Set read mode counter */
        QSPI_SetReadModeCnt(base, kQSPI_ReadModeCnt1Byte);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinDual);

        /* Set QSPI address pin mode */
        QSPI_SetAddressPin(base, kQSPI_AddrPinAsData);
    }
    else if (mode == kFLASH_FastReadQuadOUT)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_FRQO);

        /* Set dummy counter */
        QSPI_SetDummyCnt(base, kQSPI_DummyCnt1Byte);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinQuad);
    }
    else if (mode == kFLASH_FastReadQuadIO)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_FRQIO);

        /* Set read mode counter */
        QSPI_SetReadModeCnt(base, kQSPI_ReadModeCnt1Byte);

        /* Set dummy counter */
        QSPI_SetDummyCnt(base, kQSPI_DummyCnt2Byte);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinQuad);

        /* Set QSPI address pin mode */
        QSPI_SetAddressPin(base, kQSPI_AddrPinAsData);
    }
    else if (mode == kFLASH_WordFastReadQuadIO)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_WFRQIO);

        /* Set read mode counter */
        QSPI_SetReadModeCnt(base, kQSPI_ReadModeCnt1Byte);

        /* Set dummy counter */
        QSPI_SetDummyCnt(base, kQSPI_DummyCnt1Byte);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinQuad);

        /* Set QSPI address pin mode */
        QSPI_SetAddressPin(base, kQSPI_AddrPinAsData);
    }
    else if (mode == kFLASH_OctalWordFastReadQuadIO)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_OWFRQIO);

        /* Set read mode counter */
        QSPI_SetReadModeCnt(base, kQSPI_ReadModeCnt1Byte);

        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinQuad);

        /* Set QSPI address pin mode */
        QSPI_SetAddressPin(base, kQSPI_AddrPinAsData);
    }
    else
    {
        ; /* Intentional empty for MISTA C-2012 rule 15.7. */
    }

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI read */
    QSPI_StartTransfer(base, kQSPI_Read);

    for (uint32_t i = 0x00U; i < size; i++)
    {
        buffer[i] = QSPI_ReadByte(base);
        readBytes++;
    }

    /* Disable QSPI */
    QSPI_SetSSEnable(base, false);

    return readBytes;
}

/* Program flash within a page */
static status_t FLASH_PageProgram(
    QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint8_t *buffer, uint32_t size)
{
    status_t status            = kStatus_Fail;
    volatile uint32_t localCnt = 0x00U;
    uint16_t statusWrite       = 0x00U;
    uint16_t statusReg1Val     = 0x00U;
    uint16_t statusReg2Val     = 0x00U;

    qspi_header_count_config_t config;

    /* Configure header count. */
    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt3Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    if (mode == kFLASH_ProgramQuad)
    {
        /* Get status register 1. */
        statusReg1Val = FLASH_GetStatusReg(base, 0x01U);

        if (((g_flashConfig->jedecId >> 16) & 0xFFU) != 0xC2U)
        {
            /* Get status register 2. */
            statusReg2Val = FLASH_GetStatusReg(base, 0x02U);
            statusWrite   = ((statusReg2Val << 8) | statusReg1Val) | 0x0200U;
            FLASH_WriteStatusReg(base, statusWrite);
        }
        else if ((statusReg1Val & 0x40U) != 0x40U)
        {
            statusWrite = statusReg1Val | 0x40U;
            FLASH_WriteStatusReg(base, statusWrite);
        }
    }

    /* Check address validity */
    if ((FLASH_PAGE_NUM(address + size - 1) > FLASH_PAGE_NUM(address)) || size == 0)
    {
        return kStatus_Fail;
    }

    /* Enable flash write */
    FLASH_WriteEnable(base, true);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set QSPI address pin mode */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* Set address */
    QSPI_SetAddress(base, address);

    if (mode == kFLASH_ProgramNormal)
    {
        /* Set instruction */
        QSPI_SetInstruction(base, FLASH_INS_CODE_PP);
    }
    else if (mode == kFLASH_ProgramQuad)
    {
        /* Set QSPI data pin mode */
        QSPI_SetDatePin(base, kQSPI_DataPinQuad);

        if (((g_flashConfig->jedecId >> 16) & 0xFFU) == 0xC2U)
        {
            /* Set QSPI address pin mode */
            QSPI_SetAddressPin(base, kQSPI_AddrPinAsData);
            /* Set instruction */
            QSPI_SetInstruction(base, FLASH_INS_CODE_QPP_MX);
        }
        else
        {
            /* Set instruction */
            QSPI_SetInstruction(base, FLASH_INS_CODE_QPP);
        }
    }

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    for (uint32_t i = 0x00U; i < size; i++)
    {
        QSPI_WriteByte(base, buffer[i]);
    }

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);

    while (localCnt++ < 1000000)
    {
        /* Check flash busy status */
        if (FLASH_GetBusyStatus(base) == false)
        {
            status = kStatus_Success;
            break;
        }
    }

    statusReg1Val = FLASH_GetStatusReg(base, 0x01U);

    /* Check WEL bit is cleared or not. */
    if ((statusReg1Val & FLASH_WEL_BIT_SET) == FLASH_WEL_BIT_SET)
    {
        status = kStatus_Fail;
    }

    return status;
}

/* Program flash with any address and size */
static status_t FLASH_Program(
    QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint8_t *buffer, uint32_t size)
{
    uint8_t *programBuf;
    uint32_t begPgNum;
    uint32_t endPgNum;
    uint32_t step;
    uint32_t currentAddr;
    uint32_t endPgAddr;
    status_t status = kStatus_Fail;

    programBuf  = buffer;
    currentAddr = address;

    /* Get page number of start address */
    begPgNum = FLASH_PAGE_NUM(address);
    /* Get page number of end address */
    endPgNum = FLASH_PAGE_NUM(address + size - 1);

    /* Both start address and end address are within the same page */
    if (begPgNum == endPgNum)
    {
        return (FLASH_PageProgram(base, mode, address, buffer, size));
    }
    /* Start address and end address are not in the same page */
    else
    {
        /* For first page */
        endPgAddr = (g_flashConfig->pageSize * (FLASH_PAGE_NUM(address) + 1U) - 1U);
        step      = endPgAddr - address + 1;
        status    = FLASH_PageProgram(base, mode, address, programBuf, step);
        if (status == kStatus_Fail)
        {
            return kStatus_Fail;
        }

        programBuf += step;
        currentAddr += step;

        for (uint32_t i = begPgNum + 1U; i <= endPgNum; i++)
        {
            /* For last page */
            if (i == endPgNum)
            {
                step = (address + size) & 0xFF;

                /* If step is 0, the last page has 256 bytes data to be writen ( size of data is 0x100 ) */
                if (step == 0)
                {
                    step = 0x100;
                }

                return (FLASH_PageProgram(base, mode, currentAddr, programBuf, step));
            }
            else
            {
                status = FLASH_PageProgram(base, mode, currentAddr, programBuf, g_flashConfig->pageSize);
                if (status == kStatus_Fail)
                {
                    return kStatus_Fail;
                }

                programBuf += g_flashConfig->pageSize;
                currentAddr += g_flashConfig->pageSize;
            }
        }
    }

    return status;
}

/* Get JEDEC ID */
static uint32_t FLASH_GetJEDECID(QSPI_Type *base)
{
    uint32_t jedecID = 0;
    qspi_header_count_config_t config;

    /* Configure header count. */
    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter*/
    QSPI_SetHeaderCount(base, &config);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x03U);

    /* Read JEDEC ID */
    QSPI_SetInstruction(base, FLASH_INS_CODE_JEDEC_ID);

    /* Set QSPI address pin mode */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI read */
    QSPI_StartTransfer(base, kQSPI_Read);

    jedecID = QSPI_ReadByte(base);
    jedecID <<= 8;
    jedecID |= QSPI_ReadByte(base);
    jedecID <<= 8;
    jedecID |= QSPI_ReadByte(base);

    /* Disable QSPI */
    QSPI_SetSSEnable(base, false);

    return jedecID;
}

static const struct FlashDeviceConfig *FLASH_GetConfigFromJedecID(uint32_t jedecID)
{
    uint32_t count = sizeof(flashDevList) / sizeof(struct FlashDeviceConfig);

    for (uint32_t i = 0U; i < count; i++)
    {
        if (flashDevList[i].jedecId == jedecID)
        {
            return &flashDevList[i];
        }
    }

    return NULL;
}

static status_t FLASH_SetConfig(uint32_t jedecID)
{
    status_t status = kStatus_Fail;
    const struct FlashDeviceConfig *cfg;

    cfg = FLASH_GetConfigFromJedecID(jedecID);

    if (NULL != cfg)
    {
        g_flashConfig = cfg;
        status        = kStatus_Success;
    }

    return status;
}

const struct FlashDeviceConfig *FLASH_GetConfig(void)
{
    return g_flashConfig;
}

static void FLASH_Init(QSPI_Type *base)
{
    qspi_config_t config;

    /*Get QSPI default settings and configure the qspi */
    QSPI_GetDefaultConfig(&config);

    QSPI_Init(base, &config);
}

static status_t FLASH_EnableQSPI(void)
{
    return FLASHC_EnableQSPIPad(FLASHC);
}

static void FLASH_EnableFlashC(void)
{
    assert(g_flashConfig != NULL);

    FLASHC_EnableFLASHCPad(FLASHC, kFLASHC_HardwareCmdFastReadQuadIO, g_flashConfig->jedecId);
}

/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    uint32_t jedecID = 0x00U;
    status_t status;
    uint32_t primask;

    if (mflashInitialized)
    {
        return kStatus_Success;
    }

    assert((FLASHC->FCCR & FLASHC_FCCR_FLASHC_PAD_EN_MASK) != 0U);

    if ((FLASHC->FCACR & FLASHC_FCACR_OFFSET_EN_MASK) == 0U)
    {
        mflashOffset = 0U;
    }
    else
    {
        mflashOffset = FLASHC->FAOFFR;
    }

    /* During QSPI operation, no interrupt allowed in XIP case. */
    primask = DisableGlobalIRQ();

    FLASH_EnableQSPI();

    FLASH_Init(QSPI);

    FLASH_ResetFastReadQuad(QSPI);

    /*Get JEDEC ID. */
    jedecID = FLASH_GetJEDECID(QSPI);

    status = FLASH_SetConfig(jedecID);
    if (status == kStatus_Success)
    {
        mflashInitialized = true;
    }

    FLASH_EnableFlashC();

    EnableGlobalIRQ(primask);

    return status;
}

/* Calling wrapper for 'mflash_drv_sector_erase_internal'.
 * Erase one sector starting at 'sector_addr' - must be sector aligned.
 */
int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    status_t status;
    uint32_t primask;

    if (0 == mflash_drv_is_sector_aligned(sector_addr))
        return kStatus_InvalidArgument;

    /* During flash operation, no interrupt allowed in XIP case. */
    primask = DisableGlobalIRQ();

    FLASH_EnableQSPI();

    status = FLASH_Erase(QSPI, sector_addr, sector_addr + MFLASH_SECTOR_SIZE - 1U);

    FLASH_EnableFlashC();

    EnableGlobalIRQ(primask);

    return status;
}

/* Calling wrapper for 'mflash_drv_page_program_internal'.
 * Write 'data' to 'page_addr' - must be page aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_page_program(uint32_t page_addr, uint32_t *data)
{
    status_t status;
    uint32_t primask;

    if (0 == mflash_drv_is_page_aligned(page_addr))
        return kStatus_InvalidArgument;

    /* During flash operation, no interrupt allowed in XIP case. */
    primask = DisableGlobalIRQ();

    FLASH_EnableQSPI();

    status = FLASH_PageProgram(QSPI, kFLASH_ProgramQuad, page_addr, (uint8_t *)data, MFLASH_PAGE_SIZE);

    FLASH_EnableFlashC();

    EnableGlobalIRQ(primask);

    return status;
}

/* API - Read data, access by FLASHC. */
int32_t mflash_drv_read(uint32_t addr, uint32_t *buffer, uint32_t len)
{
    status_t status;
    uint32_t primask;

    /* Ensure current flash access via FLASHC. */
    assert((FLASHC->FCCR & FLASHC_FCCR_FLASHC_PAD_EN_MASK) != 0U);
    /* If physical address is larger than mflashOffset, access from FLASHC directly. */
    if (addr > mflashOffset)
    {
        memcpy(buffer, (void *)(addr + MFLASH_BASE_ADDRESS - mflashOffset), len);
        status = kStatus_Success;
    }
    else
    {
        /* During flash operation, no interrupt allowed in XIP case. */
        primask = DisableGlobalIRQ();

        FLASH_EnableQSPI();

        status = (FLASH_Read(QSPI, kFLASH_FastReadQuadIO, addr, (uint8_t *)(void *)buffer, len) == len) ?
                     kStatus_Success :
                     kStatus_Fail;

        FLASH_EnableFlashC();

        EnableGlobalIRQ(primask);
    }

    return status;
}

/* API - Write data */
int32_t mflash_drv_write(uint32_t addr, uint32_t *buffer, uint32_t len)
{
    status_t status;
    uint32_t primask;

    /* Only allow buffer in SRAM, flash address cannot be used in QSPI operation. */
    assert(((uint32_t)buffer & 0x1F000000U) != 0x1F000000U);

    /* During flash operation, no interrupt allowed in XIP case. */
    primask = DisableGlobalIRQ();

    FLASH_EnableQSPI();

    status = FLASH_Program(QSPI, kFLASH_ProgramQuad, addr, (uint8_t *)buffer, len);

    FLASH_EnableFlashC();

    EnableGlobalIRQ(primask);

    return status;
}

/* API - Erase data */
int32_t mflash_drv_erase(uint32_t addr, uint32_t len)
{
    status_t status;
    uint32_t primask;

    /* During flash operation, no interrupt allowed in XIP case. */
    primask = DisableGlobalIRQ();

    FLASH_EnableQSPI();

    status = FLASH_Erase(QSPI, addr, addr + len - 1U);

    FLASH_EnableFlashC();

    EnableGlobalIRQ(primask);

    return status;
}

/* API - Get pointer to FLASH region */
void *mflash_drv_phys2log(uint32_t addr, uint32_t len)
{
    assert(addr > mflashOffset);
    return (void *)(addr + MFLASH_BASE_ADDRESS - mflashOffset);
}

/* API - Get pointer to FLASH region */
uint32_t mflash_drv_log2phys(void *ptr, uint32_t len)
{
    if ((uint32_t)ptr < MFLASH_BASE_ADDRESS)
        return kStatus_InvalidArgument;

    return ((uint32_t)ptr + mflashOffset - MFLASH_BASE_ADDRESS);
}
