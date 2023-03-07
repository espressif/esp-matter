/***************************************************************************//**
 * @file
 * @brief Nandflash driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __NANDFLASH_H
#define __NANDFLASH_H

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup NANDFlash
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define NAND256W3A_SPARESIZE       16 /**< Spare area size of Numonyx NAND256W3A. */

/* "Standard" byte positions in the spare area. */
#define NAND_SPARE_BADBLOCK_POS    5 /**< Spare area position of bad-block marker. */
#define NAND_SPARE_ECC0_POS        6 /**< Spare area position of ECC byte 0 (LSB). */
#define NAND_SPARE_ECC1_POS        7 /**< Spare area position of ECC byte 1.       */
#define NAND_SPARE_ECC2_POS        8 /**< Spare area position of ECC byte 2 (MSB). */

/** @brief NANDFLASH status enumerator. */
typedef enum {
  NANDFLASH_STATUS_OK         = 0,        /**< No errors detected.                      */
  NANDFLASH_INVALID_DEVICE    = -1,       /**< Invalid (unsupported) flash device.      */
  NANDFLASH_INVALID_ADDRESS   = -2,       /**< Invalid nand flash address.              */
  NANDFLASH_WRITE_ERROR       = -3,       /**< Nand flash write error, block is "bad".  */
  NANDFLASH_ECC_ERROR         = -4,       /**< Illegal ECC value read from spare area.  */
  NANDFLASH_ECC_UNCORRECTABLE = -5,       /**< Uncorrectable data error in page.        */
  NANDFLASH_INVALID_SETUP     = -6,       /**< Invalid parameter to NANDFLASH_Init().   */
  NANDFLASH_NOT_INITIALIZED   = -7,       /**< Nand module has not been initialized.    */
} NANDFLASH_Status_TypeDef;

/** @brief NANDFLASH device information structure. */
typedef struct {
  uint32_t baseAddress;                     /**< The device base address in cpu memory map.   */
  uint8_t  manufacturerCode;                /**< The device manufacturer code.                */
  uint8_t  deviceCode;                      /**< The device ID .                              */
  uint32_t deviceSize;                      /**< Total device size in bytes.                  */
  uint32_t pageSize;                        /**< Device page size in bytes.                   */
  uint32_t spareSize;                       /**< Device page spare size in bytes.             */
  uint32_t blockSize;                       /**< Device block size in bytes.                  */
  uint32_t ecc;                             /**< Result of ECC generation from last read/written page. */
  uint8_t  spare[NAND256W3A_SPARESIZE];     /**< Spare area content from last read page or spare operation. */
  int      dmaCh;                           /**< The DMA channel used, -1 if DMA is not used. */
} NANDFLASH_Info_TypeDef;

bool  NANDFLASH_AddressValid(uint32_t addr);
int   NANDFLASH_CopyPage(uint32_t dstAddr, uint32_t srcAddr);
NANDFLASH_Info_TypeDef *
NANDFLASH_DeviceInfo(void);
int   NANDFLASH_EccCorrect(uint32_t generatedEcc, uint32_t readEcc, uint8_t *data);
int   NANDFLASH_EraseBlock(uint32_t address);
int   NANDFLASH_Init(int dmaCh);
int   NANDFLASH_MarkBadBlock(uint32_t address);
int   NANDFLASH_ReadPage(uint32_t address, uint8_t *buffer);
int   NANDFLASH_ReadSpare(uint32_t address, uint8_t *buffer);
int   NANDFLASH_WritePage(uint32_t address, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

/** @} (end group NANDFlash) */
/** @} (end group kitdrv) */

#endif /* __NANDFLASH_H */
