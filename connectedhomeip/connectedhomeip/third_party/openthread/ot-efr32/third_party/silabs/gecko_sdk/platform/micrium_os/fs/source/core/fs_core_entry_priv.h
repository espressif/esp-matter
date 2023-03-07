/***************************************************************************//**
 * @file
 * @brief File System - Core Directory Operations
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

#ifndef  FS_CORE_ENTRY_PRIV_H_
#define  FS_CORE_ENTRY_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_dir.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/include/fs_core_working_dir.h>
#include  <cpu/include/cpu.h>
#include  <fs/source/sys/fs_sys_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------------- ENTRY ACCESS MODES ----------------
#define  FS_ENTRY_ACCESS_MODE_NONE                     DEF_BIT_NONE
#define  FS_ENTRY_ACCESS_MODE_EXCL                     DEF_BIT_00
#define  FS_ENTRY_ACCESS_MODE_CREATE                   DEF_BIT_01
#define  FS_ENTRY_ACCESS_MODE_ONCE                     DEF_BIT_02

#define  FS_ENTRY_NODE_FIELDS \
  FS_OBJ_FIELDS               \
  FS_SYS_API      * SysPtr;   \
  FS_VOL_HANDLE VolHandle;    \
  FS_SYS_POS    Pos;

#define  FS_ENTRY_DESC_FIELDS(entry_node_ptr_field) \
  FS_OBJ_FIELDS                                     \
  entry_node_ptr_field;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_entry_node {
  FS_ENTRY_NODE_FIELDS
} FS_ENTRY_NODE;

typedef struct fs_entry_node_handle {
  struct fs_entry_node *EntryNodePtr;
  FS_OBJ_ID            EntryNodeId;
} FS_ENTRY_NODE_HANDLE;

typedef struct fs_entry_loc_data {
  FS_VOL_HANDLE VolHandle;
  FS_SYS_POS    StartPos;
  CPU_CHAR      *RelPathPtr;
  CPU_INT08U    StartDepth;
} FS_ENTRY_LOC_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_ENTRY_NODE_HANDLE FSEntry_NodeNullHandle;

extern SLIST_MEMBER *FSEntry_OpenListHeadPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_ENTRY_NODE_HANDLE_IS_NULL(h)             (((h).EntryNodePtr == DEF_NULL) \
                                                      && ((h).EntryNodeId == 0u))

#define  FS_IS_VALID_DATE_TIME_TYPE(time_type)       (((time_type >= DEF_BIT_00) \
                                                       && (time_type <= DEF_BIT_03)) ? DEF_YES : DEF_NO)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_CHAR *FSEntry_Lookup(FS_ENTRY_LOC_DATA *p_entry_loc_data,
                         FS_SYS_POS        *p_parent_pos_data,
                         FS_SYS_POS        *p_entry_pos_data,
                         CPU_INT16S        *p_rel_depth,
                         RTOS_ERR          *p_err);

void FSEntry_VirtualPathResolve(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                CPU_CHAR          *p_path,
                                FS_ENTRY_LOC_DATA *p_entry_loc_data,
                                RTOS_ERR          *p_err);

FS_ENTRY_NODE_HANDLE FSEntry_NodeOpen(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                      CPU_CHAR          *p_path,
                                      FS_FLAGS          mode,
                                      FS_FLAGS          entry_type,
                                      RTOS_ERR          *p_err);

void FSEntry_NodeClose(FS_ENTRY_NODE_HANDLE      entry_node_handle,
                       FS_OBJ_ON_NULL_REF_CNT_CB on_null_ref_cnt,
                       RTOS_ERR                  *p_err);

CPU_BOOLEAN FSEntry_IsOpen(FS_WRK_DIR_HANDLE wrk_dir_handle,
                           CPU_CHAR          *p_rel_path,
                           RTOS_ERR          *p_err);

FS_ENTRY_NODE_HANDLE FSEntry_OpenFind(FS_VOL     *p_vol,
                                      FS_SYS_POS entry_pos);

CPU_SIZE_T FSEntry_PathGetInternal(FS_ENTRY_NODE *p_entry_node,
                                   CPU_INT08U    entry_type,
                                   CPU_CHAR      *p_buf,
                                   CPU_SIZE_T    buf_size,
                                   RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       ACCESS MODE FLAGS CHECK
 *******************************************************************************************************/

#if (FS_ENTRY_ACCESS_MODE_NONE != FS_FILE_ACCESS_MODE_NONE)
#error "Access flags #define's mismatch."
#endif

#if (FS_ENTRY_ACCESS_MODE_EXCL != FS_FILE_ACCESS_MODE_EXCL)
#error "Access flags #define's mismatch."
#endif

#if (FS_ENTRY_ACCESS_MODE_CREATE != FS_FILE_ACCESS_MODE_CREATE)
#error "Access flags #define's mismatch."
#endif

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
#if (FS_ENTRY_ACCESS_MODE_NONE != FS_DIR_ACCESS_MODE_NONE)
#error "Access flags #define's mismatch."
#endif

#if (FS_ENTRY_ACCESS_MODE_EXCL != FS_DIR_ACCESS_MODE_EXCL)
#error "Access flags #define's mismatch."
#endif

#if (FS_ENTRY_ACCESS_MODE_CREATE != FS_DIR_ACCESS_MODE_CREATE)
#error "Access flags #define's mismatch."
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
