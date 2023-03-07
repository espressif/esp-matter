/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _APP_H_
#define _APP_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
#define EXAMPLE_QSPI            QSPI
#define EXAMPLE_FLASHC          FLASHC
#define FLASH_PAGE_SIZE         256U                             /*!< 256 bytes */
#define FLASH_SECTOR_SIZE       4096U                            /*!< 4 Kbytes */
#define FLASH_SIZE              (4U * 0x100000U)                 /*!< 4 Mbytes */
#define FLASH_ERASE_ADDR_OFFSET (FLASH_SIZE - FLASH_SECTOR_SIZE) /* Test on last sector */

/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
/*${prototype:end}*/

#endif /* _APP_H_ */
