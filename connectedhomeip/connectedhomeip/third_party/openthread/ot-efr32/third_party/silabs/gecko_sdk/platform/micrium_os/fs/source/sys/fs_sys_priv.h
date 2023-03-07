/***************************************************************************//**
 * @file
 * @brief File System System Driver Management
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

#ifndef  FS_SYS_PRIV_H_
#define  FS_SYS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_err.h>

#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_partition.h>

#include  <cpu/include/cpu.h>

#include  "sl_sleeptimer.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           SYSTEM INFO DATA TYPE
 *******************************************************************************************************/

typedef enum fs_sys_type {
  FS_SYS_TYPE_FAT
} FS_SYS_TYPE;

typedef CPU_INT64U FS_SYS_POS;

typedef struct fs_sys_info {
  FS_LB_QTY BadSecCnt;                                          // Nbr of bad  sectors on volume.
  FS_LB_QTY FreeSecCnt;                                         // Nbr of free sectors on volume.
  FS_LB_QTY UsedSecCnt;                                         // Nbr of used sectors on volume.
  FS_LB_QTY TotSecCnt;                                          // Tot nbr of  sectors on volume.
  CPU_CHAR  *SysTypeStrPtr;                                     // String identifying file system type.
  void      *DataInfoPtr;                                       // Information specific to file system type.
} FS_SYS_INFO;

struct fs_file_desc;
struct fs_file_node;
struct fs_dir_desc;
struct fs_dir_node;
struct fs_vol;
struct fs_dev;
struct fs_entry_info;
//                                                                 *INDENT-OFF*
typedef const struct fs_sys_api {
  CPU_BOOLEAN (*BootSecChk)(FS_BLK_DEV_HANDLE blk_dev_handle,
                            FS_PARTITION_INFO *p_partition_info,
                            FS_LB_NBR         mbr_sec,
                            RTOS_ERR          *p_err);

  struct fs_vol *(*VolAlloc)(RTOS_ERR *p_err);

  void (*VolFree)(struct fs_vol *p_vol);

  struct fs_file_desc *(*FileDescAlloc)(RTOS_ERR *p_err);

  struct fs_file_node *(*FileNodeAlloc)(RTOS_ERR *p_err);

  void (*FileDescFree)(struct fs_file_desc *p_desc);

  void (*FileNodeFree)(struct fs_file_node *p_node);

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  struct fs_dir_desc *(*DirDescAlloc)(RTOS_ERR *p_err);

  struct fs_dir_node *(*DirNodeAlloc)(RTOS_ERR *p_err);

  void (*DirDescFree)(struct fs_dir_desc *p_desc);

  void (*DirNodeFree)(struct fs_dir_node *p_node);
#endif

  void (*VolOpen)(struct fs_vol *p_vol,
                  FS_FLAGS      open_opt,
                  RTOS_ERR      *p_err);

  void (*VolClose)(struct fs_vol *p_vol,
                   RTOS_ERR      *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)

  void (*VolSync)(struct fs_vol *p_vol,
                  RTOS_ERR      *p_err);

  void (*VolFmt)(FS_BLK_DEV_HANDLE blk_dev_handle,
                 FS_PARTITION_NBR  partition_nbr,
                 FS_LB_NBR         partition_sec_start,
                 FS_LB_QTY         partition_sec_cnt,
                 void              *p_sys_cfg,
                 RTOS_ERR          *p_err);
#endif

  void (*VolLabelGet)(struct fs_vol *p_vol,
                      CPU_CHAR      *label,
                      CPU_SIZE_T    len_max,
                      RTOS_ERR      *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  void (*VolLabelSet)(struct fs_vol *p_vol,
                      CPU_CHAR      *label,
                      RTOS_ERR      *p_err);
#endif

  void (*VolQuery)(struct fs_vol *p_vol,
                   FS_SYS_INFO   *p_info,
                   RTOS_ERR      *p_err);

  FS_SYS_POS (*EntryLookup)(struct fs_vol *p_vol,
                            FS_SYS_POS    start_sys_pos,
                            CPU_CHAR      *p_entry_name,
                            RTOS_ERR      *p_err);

  void (*FileNodeInit)(struct fs_file_node *p_file_node,
                       FS_SYS_POS          entry_sys_pos,
                       RTOS_ERR            *p_err);

  void (*FileDescInit)(struct fs_file_desc *p_file_desc,
                       RTOS_ERR            *p_err);

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  void (*DirNodeInit)(struct fs_dir_node *p_dir_node,
                      FS_SYS_POS         entry_sys_pos,
                      RTOS_ERR           *p_err);

  void (*DirDescInit)(struct fs_dir_desc *p_dir_desc,
                      RTOS_ERR           *p_err);
#endif

  void (*EntryQuery)(struct fs_vol        *p_vol,
                     FS_SYS_POS           entry_pos,
                     struct fs_entry_info *p_entry_info,
                     CPU_CHAR             *p_name,
                     FS_FILE_NAME_LEN     *p_name_len,
                     CPU_SIZE_T           name_buf_size,
                     RTOS_ERR             *p_err);

                                                                // ----------------- ENTRY FUNCTIONS ------------------
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)

  FS_SYS_POS (*EntryCreate)(struct fs_vol *p_vol,
                            FS_SYS_POS    parent_sys_pos,
                            CPU_CHAR      *name_entry,
                            CPU_BOOLEAN   is_dir,
                            RTOS_ERR      *p_err);

  void (*EntryDel)(struct fs_vol *p_vol,
                   FS_SYS_POS    entry_sys_pos,
                   RTOS_ERR      *p_err);

  void (*EntryRename)(struct fs_vol *p_vol,
                      FS_SYS_POS    old_entry_sys_pos,
                      FS_SYS_POS    new_entry_sys_pos,
                      FS_SYS_POS    new_entry_parent_sys_pos,
                      CPU_CHAR      *p_new_name,
                      CPU_BOOLEAN   exist,
                      RTOS_ERR      *p_err);

  void (*EntryAttribSet)(struct fs_vol *p_vol,
                         FS_SYS_POS    entry_sys_pos,
                         FS_FLAGS      attrib,
                         RTOS_ERR      *p_err);

  void (*EntryTimeSet)(struct fs_vol        *p_vol,
                       FS_SYS_POS           entry_sys_pos,
                       sl_sleeptimer_date_t *p_time,
                       CPU_INT08U           time_type,
                       RTOS_ERR             *p_err);
#endif

  CPU_SIZE_T (*FileRd)(struct fs_file_desc *p_file_desc,
                       CPU_SIZE_T          pos,
                       CPU_INT08U          *p_dest,
                       CPU_SIZE_T          size,
                       RTOS_ERR            *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  CPU_SIZE_T (*FileWr)(struct fs_file_desc *p_file_desc,
                       CPU_SIZE_T          pos,
                       CPU_INT08U          *p_src,
                       CPU_SIZE_T          size,
                       RTOS_ERR            *p_err);

  void (*FileTruncate)(struct fs_vol       *p_vol,
                       struct fs_file_node *p_file_node,
                       FS_SYS_POS          entry_pos_data,
                       FS_FILE_SIZE        size,
                       RTOS_ERR            *p_err);

  void (*FileExtend)(struct fs_vol *p_vol,
                     FS_SYS_POS    entry_pos_data,
                     FS_FILE_SIZE  size,
                     RTOS_ERR      *p_err);

  void (*FileSync)(struct fs_file_node *p_file_node,
                   RTOS_ERR            *p_err);
#endif

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)

  CPU_BOOLEAN (*DirRd)(struct fs_dir_desc   *p_dir_desc,
                       FS_SYS_POS           *p_cur_pos_data,
                       struct fs_entry_info *p_entry_info,
                       CPU_CHAR             *p_name,
                       CPU_SIZE_T           name_buf_size,
                       RTOS_ERR             *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  CPU_BOOLEAN (*DirChkEmpty)(struct fs_vol *p_vol,
                             FS_SYS_POS    *p_dirent_pos_data,
                             RTOS_ERR      *p_err);
#endif
#endif

  FS_LB_NBR (*EntryFirstLbGet)(struct fs_vol *p_vol,
                               FS_SYS_POS    entry_pos,
                               RTOS_ERR      *p_err);

  FS_LB_NBR (*NextLbGet)(struct fs_vol *p_vol,
                         FS_LB_NBR     cur_lb_nbr,
                         RTOS_ERR      *p_err);

  FS_SYS_POS VoidEntryPos;

  FS_SYS_POS RootDirPosData;

  FS_LB_NBR  VoidDataLbNbr;
} FS_SYS_API;
//                                                                 *INDENT-ON*
#if (FS_TEST_FAT_JNL_EN == DEF_ENABLED)
typedef enum fs_test_fat_jnl_test_num {
  INIT_TEST = 0u,
  FSVOL_OPEN = 1u,
  FSFILE_OPEN = 10u,
  FSDIR_OPEN = 11u,
  FSFILE_WR1 = 12u,
  FSFILE_WR2 = 13u,
  FSENTRY_RENAME_FILE1 = 14u,
  FSENTRY_RENAME_FILE2 = 15u,
  FSENTRY_RENAME_FILE3 = 16u,
  FSENTRY_RENAME_FILE4 = 17u,
  FSFILE_TRUNCATE_SHORTEN1 = 18U,
  FSFILE_TRUNCATE_SHORTEN2 = 19u,
  FSFILE_TRUNCATE_SHORTEN3 = 20u,
  FSFILE_TRUNCATE_EXTEND = 21u,
  FSENTRY_DEL_FILE1 = 22u,
  FSENTRY_DEL_FILE2 = 23u,
  FSENTRY_DEL_FILE3 = 24u,
  FSENTRY_TIMESET1 = 25u,
  FSENTRY_TIMESET2 = 26u,
  FSENTRY_ATTRIBSET1 = 27u,
  FSENTRY_ATTRIBSET2 = 28u,
  FSFILE_COPY = 29u,
  END_OF_TEST = 30u
} FS_TEST_FAT_JNL_TEST_NUM;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern FS_SYS_API const * const FSSys_API_Tbl[];

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
