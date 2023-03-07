/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_NOR_FLASH_OPS_H_
#define _FSL_NOR_FLASH_OPS_H_

#include "fsl_qspi.h"

/*!
 * @addtogroup qspi_nor_flash_ops_driver
 * @{
 */

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

/*! @brief Flash protection type */
typedef enum
{
    kFLASH_ProtectNone        = 0x00, /*!< None protection */
    kFLASH_ProtectUpper64KB   = 0x04, /*!< Protect upper 64KB   0x0F0000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper128KB  = 0x08, /*!< Protect upper 128KB  0x0E0000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper256KB  = 0x0C, /*!< Protect upper 256KB  0x0C0000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper512KB  = 0x10, /*!< Protect upper 512KB  0x080000 ~ 0x0FFFFF */
    kFLASH_ProtectLower64KB   = 0x24, /*!< Protect lower 64KB   0x000000 ~ 0x00FFFF */
    kFLASH_ProtectLower128KB  = 0x28, /*!< Protect lower 128KB  0x000000 ~ 0x01FFFF */
    kFLASH_ProtectLower256KB  = 0x2C, /*!< Protect lower 256KB  0x000000 ~ 0x03FFFF */
    kFLASH_ProtectLower512KB  = 0x30, /*!< Protect lower 512KB  0x000000 ~ 0x07FFFF */
    kFLASH_ProtectAll         = 0x14, /*!< Protect all          0x000000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper4KB    = 0x44, /*!< Protect upper 4KB    0x0FF000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper8KB    = 0x48, /*!< Protect upper 8KB    0x0FE000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper16KB   = 0x4C, /*!< Protect upper 16KB   0x0FC000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper32KB   = 0x50, /*!< Protect upper 32KB   0x0F8000 ~ 0x0FFFFF */
    kFLASH_ProtectLower4KB    = 0x64, /*!< Protect lower 4KB    0x000000 ~ 0x000FFF */
    kFLASH_ProtectLower8KB    = 0x68, /*!< Protect lower 8KB    0x000000 ~ 0x001FFF */
    kFLASH_ProtectLower16KB   = 0x6C, /*!< Protect lower 16KB   0x000000 ~ 0x003FFF */
    kFLASH_ProtectLower32KB   = 0x70, /*!< Protect lower 32KB   0x000000 ~ 0x007FFF */
    kFLASH_ProtectLower960KB  = 0x84, /*!< Protect lower 960KB  0x000000 ~ 0x0EFFFF */
    kFLASH_ProtectLower896KB  = 0x88, /*!< Protect lower 896KB  0x000000 ~ 0x0DFFFF */
    kFLASH_ProtectLower768KB  = 0x8C, /*!< Protect lower 960KB  0x000000 ~ 0x0BFFFF */
    kFLASH_ProtectUpper960KB  = 0xA4, /*!< Protect upper 960KB  0x010000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper896KB  = 0xA8, /*!< Protect upper 896KB  0x020000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper768KB  = 0xAC, /*!< Protect upper 768KB  0x040000 ~ 0x0FFFFF */
    kFLASH_ProtectLower1020KB = 0xC4, /*!< Protect lower 1020KB 0x000000 ~ 0x0FEFFF */
    kFLASH_ProtectLower1016KB = 0xC8, /*!< Protect lower 1016KB 0x000000 ~ 0x0FDFFF */
    kFLASH_ProtectLower1008KB = 0xCC, /*!< Protect lower 1008KB 0x000000 ~ 0x0FBFFF */
    kFLASH_ProtectLower992KB  = 0xD0, /*!< Protect lower 992KB  0x000000 ~ 0x0F7FFF */
    kFLASH_ProtectUpper1020KB = 0xE4, /*!< Protect upper 1020KB 0x001000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper1016KB = 0xE8, /*!< Protect upper 1016KB 0x002000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper1008KB = 0xEC, /*!< Protect upper 1008KB 0x004000 ~ 0x0FFFFF */
    kFLASH_ProtectUpper992KB  = 0xF0, /*!< Protect upper 992KB  0x008000 ~ 0x0FFFFF */
} flash_protection_mode_type_t;

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

/******************************************************************************
 * API
 *****************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Flash power down.
 *
 * @param base QSPI base pointer
 * @param enable enable/disable flash power down
 */
void FLASH_PowerDown(QSPI_Type *base, bool enable);

/*!
 * @brief Set flash write enable/disable.
 *
 * @param base QSPI base pointer
 * @param enable enable/disable write operation
 */
void FLASH_WriteEnable(QSPI_Type *base, bool enable);

/*!
 * @brief Write enable for volatile status register.
 *
 * @param base QSPI base pointer
 */
void FLASH_WriteEnableVolatileStatusReg(QSPI_Type *base);

/*!
 * @brief Reset Flash Continuous Dual Read mode.
 *
 * @param base QSPI base pointer
 */
void FLASH_ResetFastReadDual(QSPI_Type *base);

/*!
 * @brief Reset Flash Continuous Quad Read mode.
 *
 * @param base QSPI base pointer
 */
void FLASH_ResetFastReadQuad(QSPI_Type *base);

/*!
 * @brief Get the flash status register value.
 *
 * @param base QSPI base pointer
 * @param index 1 is for status register1, 2 is for status register2
 * @return return status register value
 */
uint8_t FLASH_GetStatusReg(QSPI_Type *base, uint8_t index);

/*!
 * @brief Write flash status register.
 *
 * @param base QSPI base pointer
 * @param status status register write value
 * @return return return status
 */
status_t FLASH_WriteStatusReg(QSPI_Type *base, uint16_t status);

/*!
 * @brief Check sector is blank or not.
 *
 * @param base QSPI base pointer
 * @param sectorNumber sector number
 * @return return return status
 */
status_t FLASH_IsSectorBlank(QSPI_Type *base, uint32_t sectorNumber);

/*!
 * @brief Check flash is blank or not.
 *
 * @param base QSPI base pointer
 * @return return return status
 */
status_t FLASH_IsBlank(QSPI_Type *base);

/*!
 * @brief Get the flash busy status.
 *
 * @param base QSPI base pointer
 * @return return busy or not
 */
bool FLASH_GetBusyStatus(QSPI_Type *base);

/*!
 * @brief Set flash protection mode.
 *
 * @param base QSPI base pointer
 * @param mode protection mode
 */
status_t FLASH_SetProtectionMode(QSPI_Type *base, flash_protection_mode_type_t mode);

/*!
 * @brief Whole flash erase.
 *
 * @param base QSPI base pointer
 */
status_t FLASH_EraseAll(QSPI_Type *base);

/*!
 * @brief Flash sector erase.
 *
 * @param base QSPI base pointer
 * @param sectorNumber sector number
 * @return return erase status
 */
status_t FLASH_SectorErase(QSPI_Type *base, uint32_t sectorNumber);

/*!
 * @brief Flash block 32K erase.
 *
 * @param base QSPI base pointer
 * @param blockNumber block number
 * @return return erase status
 */
status_t FLASH_Block32KErase(QSPI_Type *base, uint32_t blockNumber);

/*!
 * @brief Flash block 64K erase.
 *
 * @param base QSPI base pointer
 * @param blockNumber block number
 * @return return erase status
 */
status_t FLASH_Block64KErase(QSPI_Type *base, uint32_t blockNumber);

/*!
 * @brief Erase specfied address of the flash.
 *
 * @param base QSPI base pointer
 * @param startAddr start address
 * @param endAddr end address
 * @return return erase status
 */
status_t FLASH_Erase(QSPI_Type *base, uint32_t startAddr, uint32_t endAddr);

/*!
 * @brief Read flash from specified address to buffer.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @param buffer buffer to hold data read from flash
 * @param size data size to be read out from flash
 */
uint32_t FLASH_Read(QSPI_Type *base, flash_read_mode_type_t mode, uint32_t address, uint8_t *buffer, uint32_t size);

/*!
 * @brief Read a word from specified flash address.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @return return a word from flash
 */
uint32_t FLASH_WordRead(QSPI_Type *base, flash_read_mode_type_t mode, uint32_t address);

/*!
 * @brief Read a byte from specified flash address.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @return return a byte from flash
 */
uint8_t FLASH_ByteRead(QSPI_Type *base, flash_read_mode_type_t mode, uint32_t address);

/*!
 * @brief Write flash within a page.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @param buffer buffer data to be programmed to flash
 * @param size data size to be programmed to flash
 * @return return program status
 */
status_t FLASH_PageProgram(
    QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint8_t *buffer, uint32_t size);

/*!
 * @brief Program flash with any address and size.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @param buffer buffer data to be programmed to flash
 * @param size data size to be programmed to flash
 * @return return program status
 */
status_t FLASH_Program(
    QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint8_t *buffer, uint32_t size);

/*!
 * @brief Program a word to specified flash address.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @param buffer buffer data to be programmed to flash
 * @param size data size to be programmed to flash
 * @return return program status
 */
status_t FLASH_WordProgram(QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint32_t data);

/*!
 * @brief Program a byte to specified flash address.
 *
 * @param base QSPI base pointer
 * @param mode reading mode to be set
 * @param address address to be read
 * @param buffer buffer data to be programmed to flash
 * @param size data size to be programmed to flash
 * @return return program status
 */
status_t FLASH_ByteProgram(QSPI_Type *base, flash_program_mode_type_t mode, uint32_t address, uint8_t data);

/*!
 * @brief Get flash unique ID.
 *
 * @param base QSPI base pointer
 * @return return unique ID
 */
uint64_t FLASH_GetUniqueID(QSPI_Type *base);

/*!
 * @brief Get JEDEC ID.
 *
 * @param base QSPI base pointer
 * @return return JEDEC ID
 */
uint32_t FLASH_GetJEDECID(QSPI_Type *base);

/*!
 * @brief Set flash device configuration base on JEDEC ID.
 *
 * @param base QSPI base pointer
 * @return return status
 */
status_t FLASH_SetConfig(uint32_t jedecID);

/*!
 * @brief Get flash device configuration.
 *
 * @return return lash device configuration.
 */
const struct FlashDeviceConfig *FLASH_GetConfig(void);

/*!
 * @brief QSPI flash initilization.
 *
 * @param base QSPI base pointer
 */
void FLASH_Init(QSPI_Type *base);

/* @}*/
#endif /* _FSL_NOR_FLASH_OPS_H_*/
