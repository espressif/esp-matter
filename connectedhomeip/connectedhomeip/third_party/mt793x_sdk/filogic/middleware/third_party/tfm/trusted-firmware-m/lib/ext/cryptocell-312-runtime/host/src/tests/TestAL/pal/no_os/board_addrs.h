/*
 * Copyright (c) 2001-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _BOARD_ADDRS_
#define _BOARD_ADDRS_

/* Stored in TestAL only for no_os
 * In other cases, these are stored in kernel's directory */

#define MPS2_PLUS_MEM_SYSTEM_RAM            0x20000000
#define MPS2_PLUS_MEM_SYSTEM_RAM_LEN        0x007FFFFF

#define MPS2_PLUS_MEM_PSRAM                 0x21000000
#define MPS2_PLUS_MEM_PSRAM_LEN             0x21FFFFFF /* 16MB */

#define MPS2_PLUS_MEM_UNMANAGED_BASE_ADDR   MPS2_PLUS_MEM_PSRAM
#define MPS2_PLUS_MEM_UNMANAGED_AREA_LEN    0x001FFFFF /* 2MB */

#define MPS2_PLUS_MEM_DMA_BASE_ADDR         0x21200000
#define MPS2_PLUS_MEM_DMA_AREA_LEN          0x00DFFFFF /* 14MB */

/******************************************************************************/
/* board_addrs.h definitions are supposed to be used by other projects.
 * Some of them are supplied after compilation (e.g. TestAL).
 * The following getter functions were created in order to allow the ability
 * to change definitions values without forcing additional compilation
 * of the dependent projects.
 */
/******************************************************************************/
/*
 * @brief This function returns the DMA base address, i.e. the start address
 * of the DMA region.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - MPS2_PLUS_MEM_DMA_BASE_ADDR.
 */
unsigned long MPS2_GetDMAbaseAddr(void);

/******************************************************************************/
/*
 * @brief This function returns the DMA region length.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - MPS2_PLUS_MEM_DMA_AREA_LEN.
 */
unsigned long MPS2_GetDMAAreaLen(void);

/******************************************************************************/
/*
 * @brief This function returns the unmanaged base address.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - MPS2_PLUS_MEM_UNMANAGED_BASE_ADDR.
 */
unsigned long MPS2_GetUnmanagedbaseAddr(void);

/******************************************************************************/
/*
 * @brief This function returns the unmanaged region length.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - MPS2_PLUS_MEM_UNMANAGED_AREA_LEN.
 */
unsigned long MPS2_GetUnmanagedAreaLen(void);

#endif // _BOARD_ADDRS_
