/***************************************************************************//**
 * @file
 * @brief File System - Core Partition Operations
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_PARTITION_PRIV_H_
#define  FS_CORE_PARTITION_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/include/fs_core_partition.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_PARTITION_DOS_TBL_ENTRY_SIZE                  16u

#define  FS_PARTITION_DOS_OFF_ENTRY_1                    446u
#define  FS_PARTITION_DOS_OFF_SIG                        510u

#define  FS_PARTITION_DOS_OFF_BOOT_FLAG_1               (FS_PARTITION_DOS_OFF_ENTRY_1 +  0u)
#define  FS_PARTITION_DOS_OFF_START_CHS_ADDR_1          (FS_PARTITION_DOS_OFF_ENTRY_1 +  1u)
#define  FS_PARTITION_DOS_OFF_PARTITION_TYPE_1          (FS_PARTITION_DOS_OFF_ENTRY_1 +  4u)
#define  FS_PARTITION_DOS_OFF_END_CHS_ADDR_1            (FS_PARTITION_DOS_OFF_ENTRY_1 +  5u)
#define  FS_PARTITION_DOS_OFF_START_LBA_1               (FS_PARTITION_DOS_OFF_ENTRY_1 +  8u)
#define  FS_PARTITION_DOS_OFF_SIZE_1                    (FS_PARTITION_DOS_OFF_ENTRY_1 + 12u)

#define  FS_PARTITION_DOS_BOOT_FLAG                     0x80u

#define  FS_PARTITION_DOS_NAME_EXT                     "Extended Partition"
#define  FS_PARTITION_DOS_NAME_FAT12                   "FAT12"
#define  FS_PARTITION_DOS_NAME_FAT16                   "FAT16"
#define  FS_PARTITION_DOS_NAME_FAT32                   "FAT32"
#define  FS_PARTITION_DOS_NAME_OTHER                   "Other"

#define  FS_PARTITION_DOS_CHS_SECTORS_PER_TRK           63u
#define  FS_PARTITION_DOS_CSH_HEADS_PER_CYLINDER        255u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSPartition_Update(FS_BLK_DEV_HANDLE blk_dev_handle,
                        FS_PARTITION_NBR  partition_nbr,
                        CPU_INT08U        partition_type,
                        RTOS_ERR          *p_err);
#endif
#endif

CPU_BOOLEAN FSPartition_Find(FS_BLK_DEV_HANDLE blk_dev_handle,
                             FS_PARTITION_NBR  partition_nbr,
                             FS_PARTITION_INFO *p_partition_info,
                             RTOS_ERR          *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_PARTITION_EN
#error  "FS_CORE_CFG_PARTITION_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_PARTITION_EN != DEF_ENABLED) \
  && (FS_CORE_CFG_PARTITION_EN != DEF_DISABLED))
#error  "FS_CORE_CFG_PARTITION_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
