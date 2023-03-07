/***************************************************************************//**
 * @file
 * @brief File System - Fat Long File Name (Lfn) Operations
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

#ifndef  FS_FAT_XFN_PRIV_H
#define  FS_FAT_XFN_PRIV_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <fs/source/core/fs_core_unicode_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>

#include  <fs/source/sys/fat/fs_fat_vol_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_FN_CHK_FLAGS_NONE                       DEF_BIT_NONE
#define  FS_FAT_FN_CHK_FLAGS_HAS_INVALID_CHAR           DEF_BIT_00
#define  FS_FAT_FN_CHK_FLAGS_EXT_IS_LOWER_CASE          DEF_BIT_01
#define  FS_FAT_FN_CHK_FLAGS_NAME_IS_LOWER_CASE         DEF_BIT_02
#define  FS_FAT_FN_CHK_FLAGS_IS_MIXED_CASE              DEF_BIT_03
#define  FS_FAT_FN_CHK_FLAGS_NAME_IS_TOO_LONG           DEF_BIT_04
#define  FS_FAT_FN_CHK_FLAGS_EXT_IS_TOO_LONG            DEF_BIT_05
#define  FS_FAT_FN_CHK_FLAGS_IS_SFN_COMPATIBLE          DEF_BIT_06

#define  FS_FAT_LFN_CHAR_EMPTY                          0xFFFFu
#define  FS_FAT_LFN_CHAR_FINAL                          0x0000u

#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
#define  FS_FAT_DIRENT_LFN_NBR_CHARS                      13u
#define  FS_FAT_MAX_PATH_NAME_LEN                        256u
#define  FS_FAT_MAX_FILE_NAME_LEN                        255u

#else
#define  FS_FAT_MAX_PATH_NAME_LEN                        256u
#define  FS_FAT_MAX_FILE_NAME_LEN                         12u
#endif

#define  FS_FAT_SFN_NAME_MAX_NBR_CHAR                      8u
#define  FS_FAT_SFN_EXT_MAX_NBR_CHAR                       3u
#define  FS_FAT_SFN_NAME_PLUS_EXT_MAX_NBR_CHAR            (FS_FAT_SFN_NAME_MAX_NBR_CHAR + FS_FAT_SFN_EXT_MAX_NBR_CHAR)
#define  FS_FAT_SFN_MAX_STEM_LEN                           6u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_UTF8_EN == DEF_ENABLED)
typedef CPU_WCHAR FS_FAT_LFN_CHAR;
#else
typedef CPU_CHAR FS_FAT_LFN_CHAR;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FS_FAT_LFN_SFN_Alloc(FS_FAT_VOL     *p_fat_vol,
                          CPU_CHAR       *p_name,
                          CPU_INT32U     *p_name_8_3,
                          FS_FAT_SEC_NBR dir_sec,
                          RTOS_ERR       *p_err);

CPU_INT08U FS_FAT_LFN_ChkSumCalc(CPU_INT08U *name_8_3);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_LFN_DirEntriesCreate(FS_FAT_VOL             *p_fat_vol,
                                 CPU_CHAR               *p_name,
                                 CPU_INT08U             chk_sum,
                                 FS_FAT_SEC_BYTE_POS    *p_start_pos,
                                 FS_CACHE_WR_JOB_HANDLE *p_entry_wr_job_handle,
                                 RTOS_ERR               *p_err);
#endif

FS_FLAGS FS_FAT_LFN_NameChk(CPU_CHAR *p_name);

void FS_FAT_LFN_NameParse(void     *p_dir_entry,
                          CPU_CHAR *p_name);

FS_FILE_NAME_LEN FS_FAT_LFN_MultiByteLenGet(void *p_dir_entry);

CPU_BOOLEAN FS_FAT_LFN_NameParseCmp(void     *p_dir_entry,
                                    CPU_CHAR *p_name);

CPU_INT32S FS_FAT_LFN_StrCmpIgnoreCase_N(CPU_CHAR         *p1_str,
                                         CPU_WCHAR        *p2_str,
                                         FS_FILE_NAME_LEN len_max);

CPU_CHAR *FS_FAT_LFN_PathSegPosGet(CPU_CHAR   *p_str,
                                   CPU_SIZE_T pos);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
