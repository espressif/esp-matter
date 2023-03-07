/***************************************************************************//**
 * @file
 * @brief File System - NOR FTL Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @addtogroup FS_STORAGE
 * @{
 ********************************************************************************************************
 * @note     (1) Supports NOR-type Flash memory devices, including :
 *             - (a) Parallel NOR Flash.
 *             - (b) Serial NOR Flash (SPI and QSPI).
 *
 * @note      (2) Supported media MUST have the following characteristics :
 *             - (a) Medium organized into units (called blocks) which are erased at the same time.
 *             - (b) When erased, all bits are 1.
 *             - (c) Only an erase operation can change a bit from a 0 to a 1.
 *             - (d) Any bit  can be individually programmed  from a 1 to a 0.
 *             - (e) Any word can be individually accessed (read or programmed).
 *
 * @note     (3) Supported media TYPICALLY have the following characteristics :
 *             - (a) A program operation takes much longer than a read    operation.
 *             - (b) An erase  operation takes much longer than a program operation.
 *             - (c) The number of erase operations per block is limited.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NOR_FTL_H_
#define  FS_NOR_FTL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/include/fs_nor.h>
#include  <fs/include/fs_blk_dev.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NOR_FTL_ERASE_CNT_DIFF_TH_MIN                  5u
#define  FS_NOR_FTL_ERASE_CNT_DIFF_TH_MAX               1000u

#define  FS_NOR_FTL_PCT_RSVD_MIN                           5u
#define  FS_NOR_FTL_PCT_RSVD_MAX                          35u

#define  FS_NOR_FTL_PCT_RSVD_SEC_ACTIVE_MAX               90u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                               NOR FLASH DEVICE CONFIGURATION DATA TYPE
 *
 * @brief   NOR Flash Device Configuration
 *
 * @note     (1) The user must specify a valid configuration structure with configuration data.
 *               - (a) 'RegionNbr' MUST specify the block region which will be used for the file system area.
 *                       Block regions are enumerated by the physical-layer driver; for more information, see
 *                       the physical-layer driver header file.  (On monolithic devices, devices with only one
 *                       block region, this MUST be 0.)
 *               - (b) 'StartOffset' MUST specify
 *                   - (1) ... the absolute start address of the file system area in the flash memory, for
 *                           a parallel flash.
 *                   - (2) ... the offset of the start of the file system area in the flash, for a serial
 *                           flash.
 *                   @n
 *                   The address specified by 'AddrStart' MUST lie within the region 'RegionNbr' (see Note
 *                   #1a).
 *               - (c) 'PctRsvd' MUST specify the percentage of sectors on the flash that will be
 *                   reserved for extra-file system storage (to improve efficiency).  This value must
 *                   be between 5% & 35%, except if 0 is specified whereupon the default will be used (10%).
 *               - (d) 'EraseCntDiffTh' MUST specify the difference between minimum & maximum erase counts
 *                   that will trigger passive wear-leveling.  This value must be between 5 & 100, except
 *                   if 0 is specified whereupon the default will be used (20).
 *               - (e) 'DevSize' MUST specify the number of octets that will belong to the file system area.
 *               - (f) 'SecSize' MUST specify the sector size for the low-level flash format (either 512,
 *                   1024, 2048 or 4096).
 *******************************************************************************************************/

typedef struct fs_nor_ftl_cfg {
  CPU_INT08U RegionNbr;                                         ///< Block region within flash.
  CPU_ADDR   StartOffset;                                       ///< Start address of data within flash.
  CPU_INT08U PctRsvd;                                           ///< Percentage of device area reserved.
  CPU_INT16U EraseCntDiffTh;                                    ///< Erase count difference threshold.
  CPU_INT32U DevSize;                                           ///< Size of flash, in octets.
  FS_LB_SIZE SecSize;                                           ///< Sector size of low-level formatted flash.
} FS_NOR_FTL_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void FS_NOR_FTL_ConfigureLowParams(FS_MEDIA_HANDLE      media_handle,
                                   const FS_NOR_FTL_CFG *p_cfg,
                                   RTOS_ERR             *p_err);

void FS_NOR_FTL_LowCompact(FS_BLK_DEV_HANDLE blk_dev_handle,
                           RTOS_ERR          *p_err);

void FS_NOR_FTL_LowDefrag(FS_BLK_DEV_HANDLE blk_dev_handle,
                          RTOS_ERR          *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
