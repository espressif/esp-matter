/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "qspi_nor_flash_ops.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
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

/*******************************************************************************
 * Variables
 *****************************************************************************/

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

void FLASH_PowerDown(QSPI_Type *base, bool enable)
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
        /* Set instruction to power down flash. */
        QSPI_SetInstruction(base, FLASH_INS_CODE_PD);
    }
    else
    {
        /* Set instruction to relase flash from power down. */
        QSPI_SetInstruction(base, FLASH_INS_CODE_RPD_DI);
    }

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);
}

/* Set flash write enable / disable  */
void FLASH_WriteEnable(QSPI_Type *base, bool enable)
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
void FLASH_WriteEnableVolatileStatusReg(QSPI_Type *base)
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

/* Reset Flash Continuous Dual Read mode */
void FLASH_ResetFastReadDual(QSPI_Type *base)
{
    qspi_header_count_config_t config;

    config.instructionCnt = kQSPI_InstructionCnt2Byte;
    config.addressCnt     = kQSPI_AddressCnt0Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt1Byte;
    config.dummyCnt       = kQSPI_DummyCnt0Byte;

    /* Clear QSPI1 FIFO */
    QSPI_FlushFIFO(base);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinDual);

    /* Set QSPI address pin mode */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
    QSPI_SetHeaderCount(base, &config);

    /* Set read mode. */
    QSPI_SetReadMode(base, 0x00U);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x00U);

    /* Set instruction for fast read dual mode reset. */
    QSPI_SetInstruction(base, FLASH_INS_CODE_FDRST);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);
}

/* Reset Flash Continuous Quad Read mode */
void FLASH_ResetFastReadQuad(QSPI_Type *base)
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
uint8_t FLASH_GetStatusReg(QSPI_Type *base, uint8_t index)
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
status_t FLASH_WriteStatusReg(QSPI_Type *base, uint16_t status)
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

/* Check sector is blank or not */
status_t FLASH_IsSectorBlank(QSPI_Type *base, uint32_t sectorNumber)
{
    status_t status = kStatus_Success;
    uint32_t sectorAddress;
    uint32_t data;
    uint32_t lastSector = (g_flashConfig->chipSize / g_flashConfig->sectorSize) - 1U;

    if (!(sectorNumber > lastSector))
    {
        /* Get sector start address */
        sectorAddress = sectorNumber * g_flashConfig->sectorSize;

        for (uint32_t count = 0; count < g_flashConfig->sectorSize; count += 0x04U)
        {
            data = FLASH_WordRead(base, kFLASH_FastRead, sectorAddress + count);
            if (data != 0xFFFFFFFF)
            {
                status = kStatus_Fail;
                break;
            }
        }
    }
    else
    {
        status = kStatus_Fail;
    }

    return status;
}

/* Check flash is blank or not */
status_t FLASH_IsBlank(QSPI_Type *base)
{
    status_t status = kStatus_Success;
    uint32_t flashData;
    uint32_t i;
    uint32_t maxWordAddr;

    maxWordAddr = g_flashConfig->chipSize >> 2;

    for (i = 0; i < maxWordAddr; i++)
    {
        flashData = FLASH_WordRead(base, kFLASH_NormalRead, i << 2);
        if (flashData != 0xFFFFFFFF)
        {
            status = kStatus_Fail;
            break;
        }
    }

    return status;
}

/* Get the flash busy status */
bool FLASH_GetBusyStatus(QSPI_Type *base)
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

/* Set flash protection mode. */
status_t FLASH_SetProtectionMode(QSPI_Type *base, flash_protection_mode_type_t mode)
{
    status_t status            = kStatus_Fail;
    volatile uint32_t localCnt = 0;

    /* Enable flash write */
    FLASH_WriteEnable(base, true);

    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* Set instruction */
    QSPI_SetInstruction(base, FLASH_INS_CODE_WSR1);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Write protection mode (SEC, TB, BP2, BP1, BP0) */
    QSPI_WriteByte(base, ((uint8_t)mode & 0x7FU));

    /* Write protection mode (CMP) */
    QSPI_WriteByte(base, (((uint8_t)mode & 0x80U) >> 1));

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);

    while (localCnt++ < 100000U)
    {
        /* Check flash busy status */
        if (FLASH_GetBusyStatus(base) == false)
        {
            status = kStatus_Success;
            break;
        }
    }

    return status;
}

/* Whole flash erase */
status_t FLASH_EraseAll(QSPI_Type *base)
{
    status_t status            = kStatus_Fail;
    volatile uint32_t localCnt = 0;

    /* Enable flash write */
    FLASH_WriteEnable(base, true);

    /* Set address pin. */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set data pin. */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* Set instruction */
    QSPI_SetInstruction(base, FLASH_INS_CODE_CE);

    /* Set QSPI write */
    QSPI_StartTransfer(base, kQSPI_Write);

    /* Stop QSPI transfer */
    QSPI_StopTransfer(base);

    while (localCnt++ < 0xFFFFFFF)
    {
        /* Check flash busy status */
        if (FLASH_GetBusyStatus(base) == false)
        {
            status = kStatus_Success;
            break;
        }
    }

    return status;
}

/* Flash sector erase */
status_t FLASH_SectorErase(QSPI_Type *base, uint32_t sectorNumber)
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
status_t FLASH_Block32KErase(QSPI_Type *base, uint32_t blockNumber)
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
status_t FLASH_Block64KErase(QSPI_Type *base, uint32_t blockNumber)
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
status_t FLASH_Erase(QSPI_Type *base, uint32_t startAddr, uint32_t endAddr)
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

/* Read a word from specified flash address */
uint32_t FLASH_WordRead(QSPI_Type *base, flash_read_mode_type_t mode, uint32_t address)
{
    uint32_t data = 0x00U;
    FLASH_Read(base, mode, address, (uint8_t *)&data, 4U);
    return data;
}

/* Read a byte from specified flash address */
uint8_t FLASH_ByteRead(QSPI_Type *base, flash_read_mode_type_t mode, uint32_t address)
{
    uint8_t data = 0x00U;
    FLASH_Read(base, mode, address, (uint8_t *)&data, 1U);
    return data;
}

/* Program flash within a page */
status_t FLASH_PageProgram(
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
status_t FLASH_Program(
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

/* Program a word to specified flash address */
status_t FLASH_WordProgram(QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint32_t data)
{
    return FLASH_Program(base, mode, address, (uint8_t *)&data, 4U);
}

/* Program a byte to specified flash address */
status_t FLASH_ByteProgram(QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint8_t data)
{
    return FLASH_Program(base, mode, address, (uint8_t *)&data, 1U);
}

/* Get flash unique ID */
uint64_t FLASH_GetUniqueID(QSPI_Type *base)
{
    uint64_t uniqueID;
    qspi_header_count_config_t config;

    /* Configure header count. */
    config.instructionCnt = kQSPI_InstructionCnt1Byte;
    config.addressCnt     = kQSPI_AddressCnt1Byte;
    config.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config.dummyCnt       = kQSPI_DummyCnt3Byte;

    /* Clear QSPI FIFO */
    QSPI_FlushFIFO(base);

    /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter*/
    QSPI_SetHeaderCount(base, &config);

    /* Set data in counter */
    QSPI_SetDataInCnt(base, 0x08U);

    /* Set address */
    QSPI_SetAddress(base, 0x00U);

    /* Read Unique ID number */
    QSPI_SetInstruction(base, FLASH_INS_CODE_RUID);

    /* Set QSPI address pin mode */
    QSPI_SetAddressPin(base, kQSPI_AddrPinSingle);

    /* Set QSPI data pin mode */
    /* Set QSPI data pin mode */
    QSPI_SetDatePin(base, kQSPI_DataPinSingle);

    /* QSPI one-byte length mode */
    QSPI_UpdateTansferLength(base, kQSPI_TransferLength1Byte);

    /* Set QSPI read */
    QSPI_StartTransfer(base, kQSPI_Read);

    uniqueID = QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);
    uniqueID <<= 8;
    uniqueID |= QSPI_ReadByte(base);

    /* Disable QSPI */
    QSPI_SetSSEnable(base, false);

    return uniqueID;
}

/* Get JEDEC ID */
uint32_t FLASH_GetJEDECID(QSPI_Type *base)
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

status_t FLASH_SetConfig(uint32_t jedecID)
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

void FLASH_Init(QSPI_Type *base)
{
    qspi_config_t config;

    /*Get QSPI default settings and configure the qspi */
    QSPI_GetDefaultConfig(&config);

    QSPI_Init(base, &config);
}
