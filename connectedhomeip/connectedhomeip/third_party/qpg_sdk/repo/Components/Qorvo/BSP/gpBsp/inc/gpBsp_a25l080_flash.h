#ifndef _GPBSP_A25L080_FLASH_H_
#define _GPBSP_A25L080_FLASH_H_

/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpBsp_a25l080_flash.h
 *   Board support package for external a25l080 spi flash chip
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    SPI flash chip definition
 *****************************************************************************/
#if   defined(GP_DIVERSITY_QPG6105DK_B01)
#define MSPI_GPIO_SSN   1
#else
#define MSPI_GPIO_SSN   24
#endif
#define FLASH_ADDRESS_SIZE  3
#define SPI_FLASH_PAGE_SIZE 256UL
#define SPI_FLASH_SECTOR_SIZE 0x1000UL
#define SPI_FLASH_BLOCK_SIZE 0x10000UL
#define SPI_FLASH_SIZE    0x100000
#define SPI_FLASH_SECTORS_COUNT    (SPI_FLASH_SIZE / SPI_FLASH_SECTOR_SIZE)

/* define command id's */
/* read identification */
#define CMD_RDID    0x9F
/* read status register */
#define CMD_RDSR    0x05
/* Write enable */
#define CMD_WREN    0x06
/* Write disable */
#define CMD_WRDI    0x04
/* Read data bytes */
#define CMD_READ    0x03
/* page program */
#define CMD_WRITE   0x02
/* Bulk erase */
#define CMD_BE  0xC7
/* Sector erase */
#define CMD_SE  0x20

#endif
