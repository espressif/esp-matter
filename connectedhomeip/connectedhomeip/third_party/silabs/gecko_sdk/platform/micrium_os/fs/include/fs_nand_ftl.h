/***************************************************************************//**
 * @file
 * @brief File System - NAND Ftl Operations
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
 * @note     (1) Supports NAND-type Flash memory devices, including :
 *             - (a) Parallel NAND Flash.
 *                 - (1) Small-page (512-B) SLC Devices.
 *                 - (2) Large-page (2048-, 4096-, 8096-B) SLC Devices.
 *                 - (3) Some MLC Devices.
 *
 * @note     (2) Supported media MUST have the following characteristics :
 *             - (a) Medium organized into units (called blocks) which are erased at the same time.
 *             - (b) When erased, all bits are set to 1.
 *             - (c) Only an erase operation can change a bit from 0 to 1.
 *             - (d) Each block divided into smaller units called pages: each page has a data area
 *                   as well as a spare area. 16 bytes spare area are required for each 512 bytes
 *                   sector in the page.
 *
 * @note     (3) Supported media TYPICALLY have the following characteristics :
 *             - (a) A  program operation takes much longer than a read    operation.
 *             - (b) An erase   operation takes much longer than a program operation.
 *             - (c) The number of erase operations per block is limited.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_NAND_FTL_H_
#define  FS_NAND_FTL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/include/fs_nand.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_NAND_CFG_SEC_SIZE_AUTO         0u
#define  FS_NAND_CFG_BLK_CNT_AUTO          0u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       PHYSICAL PARAMETERS DATA TYPE
 *******************************************************************************************************/

typedef CPU_INT08U FS_NAND_UB_QTY;
typedef CPU_INT16U FS_NAND_SEC_PER_BLK_QTY;
typedef CPU_INT08U FS_NAND_ASSOC_BLK_QTY;

/*
 ********************************************************************************************************
 *                               NAND FLASH DEVICE CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef struct fs_nand_ftl_cfg {
  FS_LB_SIZE      SecSize;                                      ///< Sec size in octets.
  FS_NAND_BLK_QTY BlkCnt;                                       ///< Total blk cnt.
  FS_NAND_BLK_QTY BlkIxFirst;                                   ///< Ix of first blk to be used.
  FS_NAND_UB_QTY  UB_CntMax;                                    ///< Max nbr of Update Blocks.
  CPU_INT08U      RUB_MaxAssoc;                                 ///< Max assoc of Random Update Blocks.
  CPU_INT08U      AvailBlkTblEntryCntMax;                       ///< Nbr of entries in avail blk tbl.
} FS_NAND_FTL_CFG;

/*
 ********************************************************************************************************
 *                                       NAND STAT AND ERR CTRS
 *******************************************************************************************************/

#if ((FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED) \
  || (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED))
typedef struct fs_nand_ctrs {
#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // -------------------- STAT CTRS ---------------------

  CPU_INT32U StatMetaSecCommitCtr;                              ///< Nbr of meta sec commits.
  CPU_INT32U StatSUB_MergeCtr;                                  ///< Nbr of SUB merges done.
  CPU_INT32U StatRUB_MergeCtr;                                  ///< Nbr of RUB full merges done.
  CPU_INT32U StatRUB_PartialMergeCtr;                           ///< Nbr of RUB partial merges done.

  CPU_INT32U StatBlkRefreshCtr;                                 ///< Nbr of blk refreshes done.
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // --------------------- ERR CTRS ---------------------
  CPU_INT16U ErrRefreshDataLoss;                                ///< Nbr of unrefreshable/lost data sectors.
#endif
} FS_NAND_CTRS;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_NAND_FTL_CFG FS_NAND_CfgDflt;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void FS_NAND_FTL_ConfigureLowParams(FS_MEDIA_HANDLE       media_handle,
                                    const FS_NAND_FTL_CFG *p_nand_cfg,
                                    RTOS_ERR              *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
