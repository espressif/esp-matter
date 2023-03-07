/***************************************************************************//**
 * @file
 * @brief Micro SD card driver for the EFM32GG_DK3750 development kit.
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

#ifndef __MICROSD_H
#define __MICROSD_H

#include "em_device.h"
#include "microsdconfig.h"
#include "em_gpio.h"
#include "integer.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup MicroSd
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for MMC/SDC command */
#define CMD0      (0)         /**< GO_IDLE_STATE */
#define CMD1      (1)         /**< SEND_OP_COND */
#define ACMD41    (41 | 0x80) /**< SEND_OP_COND (SDC) */
#define CMD8      (8)         /**< SEND_IF_COND */
#define CMD9      (9)         /**< SEND_CSD */
#define CMD10     (10)        /**< SEND_CID */
#define CMD12     (12)        /**< STOP_TRANSMISSION */
#define ACMD13    (13 | 0x80) /**< SD_STATUS (SDC) */
#define CMD16     (16)        /**< SET_BLOCKLEN */
#define CMD17     (17)        /**< READ_SINGLE_BLOCK */
#define CMD18     (18)        /**< READ_MULTIPLE_BLOCK */
#define CMD23     (23)        /**< SET_BLOCK_COUNT */
#define ACMD23    (23 | 0x80) /**< SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24     (24)        /**< WRITE_BLOCK */
#define CMD25     (25)        /**< WRITE_MULTIPLE_BLOCK */
#define CMD41     (41)        /**< SEND_OP_COND (ACMD) */
#define CMD55     (55)        /**< APP_CMD */
#define CMD58     (58)        /**< READ_OCR */

void      MICROSD_Init(void);
void      MICROSD_Deinit(void);

int       MICROSD_Select(void);
void      MICROSD_Deselect(void);

void      MICROSD_PowerOn(void);
void      MICROSD_PowerOff(void);

int       MICROSD_BlockRx(uint8_t *buff, uint32_t btr);
int       MICROSD_BlockTx(const uint8_t *buff, uint8_t token);

uint8_t   MICROSD_SendCmd(uint8_t cmd, DWORD arg);
uint8_t   MICROSD_XferSpi(uint8_t data);

void      MICROSD_SpiClkFast(void);
void      MICROSD_SpiClkSlow(void);

bool      MICROSD_TimeOutElapsed(void);
void      MICROSD_TimeOutSet(uint32_t msec);

#ifdef __cplusplus
}
#endif

/** @} (end group MicroSd) */
/** @} (end group kitdrv) */

#endif
