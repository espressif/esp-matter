/***************************************************************************//**
 * @file
 * @brief File System - Fat Journal Operations
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

#ifndef  FS_FAT_JOURNAL_PRIV_H_
#define  FS_FAT_JOURNAL_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>

#include "fs_fat_vol_priv.h"
#include "fs_fat_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/kal.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_JOURNAL_STATE_NONE                       DEF_BIT_00
#define  FS_FAT_JOURNAL_STATE_OPEN                       DEF_BIT_01
#define  FS_FAT_JOURNAL_STATE_START                      DEF_BIT_02
#define  FS_FAT_JOURNAL_STATE_REPLAY                     DEF_BIT_03

#define  FS_FAT_JOURNAL_FILE_LEN                         (6u * 1024u)

#define  FS_FAT_JOURNAL_FILE_WR_LOG_MAX_SIZE             134u
#define  FS_FAT_JOURNAL_FILE_EXTEND_LOG_MAX_SIZE         134u
#define  FS_FAT_JOURNAL_FILE_TRUNCATE_LOG_MAX_SIZE       FS_FAT_JOURNAL_FILE_LEN
#define  FS_FAT_JOURNAL_FILE_CREATE_LOG_MAX_SIZE         24u
#define  FS_FAT_JOURNAL_DIR_CREATE_LOG_MAX_SIZE          35u
#define  FS_FAT_JOURNAL_ENTRY_DELETE_LOG_MAX_SIZE        FS_FAT_JOURNAL_FILE_LEN
#define  FS_FAT_JOURNAL_ENTRY_RENAME_LOG_MAX_SIZE        FS_FAT_JOURNAL_FILE_LEN

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERNAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FS_FAT_JournalOpen(FS_FAT_VOL *p_vol,
                        RTOS_ERR   *p_err);

void FS_FAT_JournalClose(FS_FAT_VOL *p_vol,
                         RTOS_ERR   *p_err);

void FS_FAT_JournalClrReset(FS_FAT_VOL *p_vol,
                            RTOS_ERR   *p_err);

void FS_FAT_JournalEnterTopLvlOp(FS_FAT_VOL *p_fat_vol,
                                 CPU_SIZE_T size_reqd,
                                 RTOS_ERR   *p_err);

void FS_FAT_JournalEnterClusChainAlloc(FS_FAT_VOL      *p_vol,
                                       FS_FAT_CLUS_NBR start_clus,
                                       CPU_BOOLEAN     is_new,
                                       RTOS_ERR        *p_err);

void FS_FAT_JournalEnterClusChainDel(FS_FAT_VOL      *p_vol,
                                     FS_FAT_CLUS_NBR start_clus,
                                     FS_FAT_CLUS_NBR nbr_clus,
                                     CPU_BOOLEAN     del_first,
                                     RTOS_ERR        *p_err);

void FS_FAT_JournalEnterEntryCreate(FS_FAT_VOL          *p_vol,
                                    FS_FAT_SEC_BYTE_POS *p_dir_start_pos,
                                    FS_FAT_SEC_BYTE_POS *p_dir_end_pos,
                                    RTOS_ERR            *p_err);

void FS_FAT_JournalEnterEntryUpdate(FS_FAT_VOL          *p_vol,
                                    FS_FAT_SEC_BYTE_POS dir_start_pos,
                                    FS_FAT_SEC_BYTE_POS dir_end_pos,
                                    RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
