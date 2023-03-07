/***************************************************************************//**
 * @file
 * @brief File System - Fat Entry Operations
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_FAT_ENTRY_H_
#define  FS_FAT_ENTRY_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CFG ------------------------
#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_vol_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>
#include  <fs/source/sys/fat/fs_fat_dirent_priv.h>
#include  <fs/source/sys/fat/fs_fat_priv.h>
//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/core/fs_core_entry_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>

#include  "sl_sleeptimer.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Next entry get callback modes.
#define  FS_FAT_NEXT_ENTRY_GET_MODE_PARSE          0u
#define  FS_FAT_NEXT_ENTRY_GET_MODE_FIND           1u

//                                                                 Next entry get callback entry types.
#define  FS_FAT_NEXT_ENTRY_GET_TYPE_SFN           DEF_BIT_00
#define  FS_FAT_NEXT_ENTRY_GET_TYPE_LFN           DEF_BIT_01
#define  FS_FAT_NEXT_ENTRY_GET_TYPE_FILE          DEF_BIT_02
#define  FS_FAT_NEXT_ENTRY_GET_TYPE_DIR           DEF_BIT_03
#define  FS_FAT_NEXT_ENTRY_GET_TYPE_VOL_LABEL     DEF_BIT_04
#define  FS_FAT_NEXT_ENTRY_GET_TYPE_ALL          (FS_FAT_NEXT_ENTRY_GET_TYPE_SFN         \
                                                  | FS_FAT_NEXT_ENTRY_GET_TYPE_LFN       \
                                                  | FS_FAT_NEXT_ENTRY_GET_TYPE_VOL_LABEL \
                                                  | FS_FAT_NEXT_ENTRY_GET_TYPE_FILE      \
                                                  | FS_FAT_NEXT_ENTRY_GET_TYPE_DIR)

#define  FS_FAT_DTE_FIELD_NONE           DEF_BIT_NONE
#define  FS_FAT_DTE_FIELD_ATTR           DEF_BIT_00
#define  FS_FAT_DTE_FIELD_SIZE           DEF_BIT_01
#define  FS_FAT_DTE_FIELD_FIRST_CLUS     DEF_BIT_02
#define  FS_FAT_DTE_FIELD_CREATION_TIME  DEF_BIT_03
#define  FS_FAT_DTE_FIELD_WR_TIME        DEF_BIT_04
#define  FS_FAT_DTE_FIELD_ACCESS_TIME    DEF_BIT_05
#define  FS_FAT_DTE_FIELD_ALL           (FS_FAT_DTE_FIELD_ATTR            \
                                         | FS_FAT_DTE_FIELD_SIZE          \
                                         | FS_FAT_DTE_FIELD_FIRST_CLUS    \
                                         | FS_FAT_DTE_FIELD_CREATION_TIME \
                                         | FS_FAT_DTE_FIELD_WR_TIME       \
                                         | FS_FAT_DTE_FIELD_ACCESS_TIME)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ FAT ENTRY INFO ------------------
typedef struct fs_fat_entry_info {
  FS_FILE_SIZE    Size;                                         // File size (in octets). Always 0 for a dir.
  FS_FAT_CLUS_NBR FirstClusNbr;                                 // Entry's first dir tbl clus.
  FS_FAT_TIME     DateCreate;                                   // Creation date.
  FS_FAT_TIME     TimeCreate;                                   // Creation time.
  FS_FAT_TIME     DateAccess;                                   // Last access data.
  FS_FAT_TIME     DateWr;                                       // Last modification date.
  FS_FAT_TIME     TimeWr;                                       // Last modification time.
  FS_FLAGS        Attrib;                                       // Entry's attributes (e.g. rd-only, dir, root dir, ...)
} FS_FAT_ENTRY_INFO;

//                                                                 ------------------ FAT DATE TIME -------------------
typedef struct fs_fat_date_time {
  FS_FAT_DATE Date;
  FS_FAT_TIME Time;
} FS_FAT_DATE_TIME;

typedef enum fs_fat_dte_type {
  FS_FAT_DTE_TYPE_NONE,
  FS_FAT_DTE_TYPE_SFN,
  FS_FAT_DTE_TYPE_LFN,
  FS_FAT_DTE_TYPE_INVALID_LFN,
  FS_FAT_DTE_TYPE_VOL_LABEL,
  FS_FAT_DTE_TYPE_ERASED,
  FS_FAT_DTE_TYPE_FREE
} FS_FAT_DTE_TYPE;

typedef struct fs_fat_sfn_dte_info {
  FS_FAT_DTE_TYPE Type;
  CPU_BOOLEAN     HasLFN;
  CPU_BOOLEAN     ChkSumMatch;
} FS_FAT_SFN_DTE_INFO;

typedef struct fs_fat_lfn_dte_info {
  FS_FAT_DTE_TYPE Type;
  CPU_BOOLEAN     IsLast;
  CPU_INT08U      SeqNo;
} FS_FAT_LFN_DTE_INFO;

/********************************************************************************************************
 *                                       DIRECTORY ENTRIES CALLBACK TYPES
 *******************************************************************************************************/

typedef FS_FAT_DIRENT_BROWSE_OUTCOME (*FS_FAT_ON_DTE_CB)     (FS_FAT_VOL          *p_fat_vol,
                                                              FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                              void                *p_arg,
                                                              RTOS_ERR            *p_err);

typedef FS_FAT_DIRENT_BROWSE_OUTCOME (*FS_FAT_ON_DTE_ACQUIRE_CB)     (void                *p_dirent,
                                                                      FS_FAT_SEC_BYTE_POS *p_cur_pos,
                                                                      void                *p_dte_info,
                                                                      void                *p_arg);

/********************************************************************************************************
 *                                   DIRECTORY ENTRIES CALLBACK DATA TYPES
 *******************************************************************************************************/

//                                                                 ------------ ENTRY PARSE CALLBACK DATA -------------
typedef struct fs_fat_entry_parse_cb_data {
  CPU_CHAR            *NamePtr;
  CPU_SIZE_T          NameBufSize;
  FS_FILE_NAME_LEN    NameLen;
  FS_FAT_ENTRY_INFO   *EntryInfoPtr;
  FS_FAT_SEC_BYTE_POS StartDirentPos;
  FS_FAT_SEC_BYTE_POS EndDirentPos;
  CPU_BOOLEAN         Done;
  CPU_BOOLEAN         Ovf;
  CPU_SIZE_T          CurNamePos;
} FS_FAT_ENTRY_PARSE_CB_DATA;

//                                                                 --------- ENTRY FIND BY NAME CALLBACK DATA ---------
typedef struct fs_fat_entry_find_by_name_cb_data {
  CPU_INT32U          *Name83Ptr;
  FS_FAT_SEC_BYTE_POS StartDirentPos;
  FS_FAT_SEC_BYTE_POS EndDirentPos;
  FS_FAT_ENTRY_INFO   *EntryInfoPtr;
  CPU_BOOLEAN         EntryFound;
  CPU_BOOLEAN         HasNameMatch;
#if (FS_FAT_CFG_LFN_EN == DEF_ENABLED)
  CPU_CHAR            *NamePtr;
#endif
} FS_FAT_ENTRY_FIND_BY_NAME_CB_DATA;

//                                                                 ------ ENTRY FIND BY FIRST CLUS CALLBACK DATA ------
typedef struct fs_fat_entry_find_by_first_clus_cb_data {
  FS_FAT_CLUS_NBR     FirstClusNbr;
  FS_FAT_SEC_BYTE_POS StartDirentPos;
  CPU_BOOLEAN         EntryFound;
} FS_FAT_ENTRY_FIND_BY_FIRST_CLUS_CB_DATA;

//                                                                 ----------- DIR EMPTY CHK CALLBACK DATA ------------
typedef struct fs_fat_dir_chk_empty_cb_data {
  FS_FAT_DIR_ENTRY_QTY FreeDirEntryCnt;
} FS_FAT_DIR_CHK_EMPTY_CB_DATA;

//                                                                 ----------- ENTRY TIME SET CALLBACK DATA -----------
typedef struct fs_fat_entry_time_set_cb_data {
  sl_sleeptimer_date_t DataTimePtr;
  CPU_INT08U    TimeType;
  CPU_BOOLEAN   Done;
} FS_FAT_ENTRY_TIME_SET_CB_DATA;

//                                                                 ---------- ENTRY ATTRIB SET CALLBACK DATA ----------
typedef struct fs_fat_entry_attrib_set_cb_data {
  FS_FLAGS    Attrib;
  CPU_BOOLEAN Done;
} FS_FAT_ENTRY_ATTRIB_SET_CB_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               SYS SPECIFIC POSITION ENCODING & DECODING
 *******************************************************************************************************/
//                                                                 ------------------- POS ENCODING -------------------
#define  FS_FAT_POS_DATA_ENCODE(sec_byte_pos, pos_data) \
  do {                                                  \
    (pos_data) = (sec_byte_pos).SecNbr;                 \
    (pos_data) <<= 32u;                                 \
    (pos_data) |= (sec_byte_pos).SecOffset;             \
  } while (0)

//                                                                 ------------------- POS DECODING -------------------
#define  FS_FAT_POS_DATA_DECODE(pos_data, sec_byte_pos) \
  do {                                                  \
    (sec_byte_pos).SecOffset = (pos_data) & 0xFFFFFFFF; \
    (sec_byte_pos).SecNbr = ((pos_data) >> 32u);        \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   SYSTEM DRIVER FUNCTION PROTOTYPES
 *******************************************************************************************************/

void FS_FAT_EntryQuery(FS_VOL           *p_vol,
                       FS_SYS_POS       entry_pos_data,
                       FS_ENTRY_INFO    *p_entry_info,
                       CPU_CHAR         *p_name,
                       FS_FILE_NAME_LEN *p_name_len,
                       CPU_SIZE_T       name_buf_size,
                       RTOS_ERR         *p_err);

FS_SYS_POS FS_FAT_EntryLookup(FS_VOL     *p_vol,
                              FS_SYS_POS start_sys_pos,
                              CPU_CHAR   *p_entry_name,
                              RTOS_ERR   *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryAttribSet(FS_VOL     *p_vol,
                           FS_SYS_POS entry_sys_pos,
                           FS_FLAGS   attrib,
                           RTOS_ERR   *p_err);

FS_SYS_POS FS_FAT_EntryCreate(FS_VOL      *p_vol,
                              FS_SYS_POS  parent_sys_pos,
                              CPU_CHAR    *p_entry_name,
                              CPU_BOOLEAN is_dir,
                              RTOS_ERR    *p_err);

void FS_FAT_EntryDel(FS_VOL     *p_vol,
                     FS_SYS_POS entry_sys_pos,
                     RTOS_ERR   *p_err);

void FS_FAT_EntryRename(FS_VOL      *p_vol,
                        FS_SYS_POS  old_entry_sys_pos,
                        FS_SYS_POS  new_entry_sys_pos,
                        FS_SYS_POS  new_entry_parent_sys_pos,
                        CPU_CHAR    *p_new_name,
                        CPU_BOOLEAN exist,
                        RTOS_ERR    *p_err);

void FS_FAT_EntryTimeSet(FS_VOL               *p_vol,
                         FS_SYS_POS           entry_sys_pos,
                         sl_sleeptimer_date_t *p_time,
                         CPU_INT08U           time_type,
                         RTOS_ERR             *p_err);
#endif

FS_LB_NBR FS_FAT_EntryFirstLbGet(FS_VOL     *p_vol,
                                 FS_SYS_POS entry_sys_pos,
                                 RTOS_ERR   *p_err);

FS_LB_NBR FS_FAT_NextLbGet(FS_VOL    *p_vol,
                           FS_LB_NBR cur_lb_nbr,
                           RTOS_ERR  *p_err);

FS_ENTRY_NODE *FS_FAT_EntryNodeAlloc(FS_FLAGS entry_type,
                                     RTOS_ERR *p_err);

void FS_FAT_EntryNodeFree(FS_ENTRY_NODE *p_entry_node);

/********************************************************************************************************
 *                                   INTERNAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

CPU_BOOLEAN FS_FAT_EntryFind(FS_FAT_VOL          *p_fat_vol,
                             CPU_CHAR            *p_name,
                             FS_FAT_ENTRY_INFO   *p_entry_info,
                             FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                             FS_FAT_SEC_BYTE_POS *p_entry_end_pos,
                             RTOS_ERR            *p_err);

void FS_FAT_EntryCreateInternal(FS_FAT_VOL             *p_fat_vol,
                                FS_FAT_SEC_BYTE_POS    *p_entry_start_pos,
                                FS_FAT_SEC_BYTE_POS    *p_entry_end_pos,
                                CPU_CHAR               *p_entry_name,
                                FS_FAT_SEC_NBR         dir_first_sec,
                                CPU_BOOLEAN            is_dir,
                                FS_CACHE_WR_JOB_HANDLE *p_entry_cache_blk_handle,
                                RTOS_ERR               *p_err);

void FS_FAT_EntryParse(FS_FAT_VOL          *p_fat_vol,
                       FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                       FS_FAT_SEC_BYTE_POS *p_entry_end_pos,
                       FS_FAT_ENTRY_INFO   *p_entry_info,
                       CPU_CHAR            *p_name,
                       FS_FILE_NAME_LEN    *p_name_len,
                       CPU_SIZE_T          name_buf_size,
                       RTOS_ERR            *p_err);

CPU_BOOLEAN FS_FAT_SFN_EntryFind(FS_FAT_VOL          *p_fat_vol,
                                 CPU_INT32U          *p_name_8_3,
                                 FS_FAT_SEC_BYTE_POS *p_dir_pos,
                                 RTOS_ERR            *p_err);

void FS_FAT_DirTblBrowseAcquire(FS_FAT_VOL               *p_fat_vol,
                                FS_FAT_SEC_BYTE_POS      *p_dte_pos,
                                FS_FAT_ON_DTE_ACQUIRE_CB on_dte_acquire_cb,
                                void                     *p_arg,
                                FS_FLAGS                 acquire_mode,
                                FS_CACHE_WR_JOB_HANDLE   *p_entry_cache_blk_handle,
                                RTOS_ERR                 *p_err);

FS_FAT_SEC_NBR FS_FAT_EntryDirTblGet(FS_FAT_VOL          *p_fat_vol,
                                     FS_FAT_SEC_BYTE_POS *p_entry_pos,
                                     RTOS_ERR            *p_err);

CPU_BOOLEAN FS_FAT_NextEntryParse(FS_FAT_VOL          *p_fat_vol,
                                  FS_FAT_SEC_BYTE_POS *p_entry_start_pos,
                                  FS_FAT_SEC_BYTE_POS *p_entry_end_pos,
                                  FS_FAT_ENTRY_INFO   *p_entry_info,
                                  CPU_CHAR            *p_entry_name,
                                  FS_FILE_NAME_LEN    *p_name_len,
                                  CPU_SIZE_T          name_buf_size,
                                  RTOS_ERR            *p_err);

void FS_FAT_EntryInfoGet(FS_ENTRY_INFO     *p_entry_info,
                         FS_FAT_ENTRY_INFO *p_fat_entry_info);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_EntryDelInternal(FS_FAT_VOL             *p_fat_vol,
                             FS_FAT_SEC_BYTE_POS    *p_dte_start_pos,
                             FS_CACHE_WR_JOB_HANDLE *p_entry_blk_handle,
                             RTOS_ERR               *p_err);

void FS_FAT_EntryUpdate(FS_FAT_VOL             *p_fat_vol,
                        FS_FAT_SEC_BYTE_POS    *p_entry_end_pos,
                        FS_FAT_ENTRY_INFO      *p_entry_info,
                        CPU_BOOLEAN            get_date,
                        FS_FLAGS               fields,
                        FS_CACHE_WR_JOB_HANDLE *p_last_entry_cache_blk_handle,
                        RTOS_ERR               *p_err);

void FS_FAT_DirEntriesErase(FS_FAT_VOL             *p_fat_vol,
                            FS_FAT_SEC_BYTE_POS    *p_dir_start_pos,
                            FS_FAT_SEC_BYTE_POS    *p_dir_end_pos,
                            FS_CACHE_WR_JOB_HANDLE *p_entry_cache_blk_handle,
                            RTOS_ERR               *p_err);
#endif

void FS_FAT_DirTblBrowse(FS_FAT_VOL          *p_fat_vol,
                         FS_FAT_SEC_BYTE_POS *p_dte_start_pos,
                         FS_FAT_ON_DTE_CB    on_dte_cb,
                         void                *p_arg,
                         RTOS_ERR            *p_err);

void FS_FAT_VolLabelGet(FS_VOL     *p_vol,
                        CPU_CHAR   *label,
                        CPU_SIZE_T len_max,
                        RTOS_ERR   *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_VolLabelSet(FS_VOL   *p_vol,
                        CPU_CHAR *label,
                        RTOS_ERR *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
