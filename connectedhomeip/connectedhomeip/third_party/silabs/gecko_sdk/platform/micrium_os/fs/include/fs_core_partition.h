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

/****************************************************************************************************//**
 * @addtogroup FS_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_PARTITION_H_
#define  FS_CORE_PARTITION_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_blk_dev.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) Table 5.3 of the book "File System Forensic Analysis, Carrier Brian, 2005" gives various
 *               values used in the partition type field of DOS partitions.  The book explained that some
 *               operating systems do not rely on this marker during file system type determination.
 *               Others, such as Microsoft Windows, do. This page gives also the list of partition
 *               identifiers for PCs: http://www.win.tue.nl/~aeb/partitions/partition_types-1.html.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_PARTITION_NBR_VOID   ((FS_PARTITION_NBR)-1)

//                                                                 See Note #1.
#define  FS_PARTITION_TYPE_EMPTY                        0x00u
#define  FS_PARTITION_TYPE_FAT12_CHS                    0x01u
#define  FS_PARTITION_TYPE_FAT16_16_32MB                0x04u
#define  FS_PARTITION_TYPE_CHS_MICROSOFT_EXT            0x05u
#define  FS_PARTITION_TYPE_FAT16_CHS_32MB_2GB           0x06u
#define  FS_PARTITION_TYPE_FAT32_CHS                    0x0Bu
#define  FS_PARTITION_TYPE_FAT32_LBA                    0x0Cu
#define  FS_PARTITION_TYPE_FAT16_LBA_32MB_2GB           0x0Eu
#define  FS_PARTITION_TYPE_LBA_MICROSOFT_EXT            0x0Fu
#define  FS_PARTITION_TYPE_HID_FAT12_CHS                0x11u
#define  FS_PARTITION_TYPE_HID_FAT16_16_32MB_CHS        0x14u
#define  FS_PARTITION_TYPE_HID_FAT16_CHS_32MB_2GB       0x15u
#define  FS_PARTITION_TYPE_HID_CHS_FAT32                0x1Bu
#define  FS_PARTITION_TYPE_HID_LBA_FAT32                0x1Cu
#define  FS_PARTITION_TYPE_HID_FAT16_LBA_32MB_2GB       0x1Eu
#define  FS_PARTITION_TYPE_NTFS                         0x07u
#define  FS_PARTITION_TYPE_MICROSOFT_MBR                0x42u
#define  FS_PARTITION_TYPE_SOLARIS_X86                  0x82u
#define  FS_PARTITION_TYPE_LINUX_SWAP                   0x82u
#define  FS_PARTITION_TYPE_LINUX                        0x83u
#define  FS_PARTITION_TYPE_HIBERNATION_A                0x84u
#define  FS_PARTITION_TYPE_LINUX_EXT                    0x85u
#define  FS_PARTITION_TYPE_NTFS_VOLSETA                 0x86u
#define  FS_PARTITION_TYPE_NTFS_VOLSETB                 0x87u
#define  FS_PARTITION_TYPE_HIBERNATION_B                0xA0u
#define  FS_PARTITION_TYPE_HIBERNATION_C                0xA1u
#define  FS_PARTITION_TYPE_FREE_BSD                     0xA5u
#define  FS_PARTITION_TYPE_OPEN_BSD                     0xA6u
#define  FS_PARTITION_TYPE_MAX_OSX                      0xA8u
#define  FS_PARTITION_TYPE_NET_BSD                      0xA9u
#define  FS_PARTITION_TYPE_MAC_OSX_BOOT                 0xABu
#define  FS_PARTITION_TYPE_BSDI                         0xB7u
#define  FS_PARTITION_TYPE_BSDI_SWAP                    0xB8u
#define  FS_PARTITION_TYPE_EFI_GPT_DISK                 0xEEu
#define  FS_PARTITION_TYPE_EFI_SYS_PART                 0xEFu
#define  FS_PARTITION_TYPE_VMWARE_FILE_SYS              0xFBu
#define  FS_PARTITION_TYPE_VMWARE_SWAP                  0xFCu

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_partition_info {
  FS_LB_NBR  StartSec;                                          ///< Sector number where the partition starts.
  FS_LB_QTY  SecCnt;                                            ///< Number of sectors composing the partition.
  CPU_INT08U Type;                                              ///< Partition type found in DOS partition table entry.
} FS_PARTITION_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void FSPartition_Query(FS_BLK_DEV_HANDLE blk_dev_handle,
                       FS_PARTITION_NBR  partition_nbr,
                       FS_PARTITION_INFO *p_partition_info,
                       RTOS_ERR          *p_err);

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)

FS_PARTITION_NBR FSPartition_CntGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                                    RTOS_ERR          *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
FS_PARTITION_NBR FSPartition_Add(FS_BLK_DEV_HANDLE blk_dev_handle,
                                 FS_LB_QTY         partition_sec_cnt,
                                 RTOS_ERR          *p_err);

void FSPartition_Init(FS_BLK_DEV_HANDLE blk_dev_handle,
                      FS_LB_QTY         partition_sec_cnt,
                      RTOS_ERR          *p_err);
#endif
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
