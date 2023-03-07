/***************************************************************************//**
 * @file
 * @brief File System - Fat Internal Types
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

#ifndef  FS_FAT_TYPE_PRIV_H
#define  FS_FAT_TYPE_PRIV_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT32U FS_FAT_CLUS_NBR;                             // Number of clusters/cluster index.
typedef CPU_INT32U FS_FAT_SEC_NBR;                              // Number of sectors/sector index.
typedef FS_LB_SIZE FS_FAT_SEC_SIZE;
typedef CPU_INT16U FS_FAT_DATE;                                 // FAT date.
typedef CPU_INT16U FS_FAT_TIME;                                 // FAT time.
typedef CPU_INT08U FS_FAT_DIR_ENTRY_QTY;                        // Quantity of directory entries.
typedef CPU_INT32U FS_FAT_FILE_SIZE;                            // Size of file, in octets.

typedef struct fs_fat_sec_byte_pos {
  FS_FAT_SEC_NBR  SecNbr;
  FS_FAT_SEC_SIZE SecOffset;
} FS_FAT_SEC_BYTE_POS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
