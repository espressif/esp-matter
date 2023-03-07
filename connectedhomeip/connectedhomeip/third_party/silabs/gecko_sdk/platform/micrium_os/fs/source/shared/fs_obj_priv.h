/***************************************************************************//**
 * @file
 * @brief File System - Core Generic Object Operations
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

#ifndef  FS_OBJ_PRIV_H_
#define  FS_OBJ_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CFG ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/include/fs_obj.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_OBJ_FIELDS    \
  FS_OBJ_ID Id;           \
  CPU_INT16U   RefCnt;    \
  CPU_INT16U   IOCnt;     \
  CPU_BOOLEAN  IsClosing; \
  SLIST_MEMBER OpenListMember;

#define  FS_OBJ_WITH_NO_IO(obj_handle, p_err, prefix, closed_err_code)                \
  if (1) {                                                                            \
    if (FSObj_RefAcquire(prefix##_ToObjHandle(obj_handle))) {                         \
      goto PP_UNIQUE_LABEL(fs_obj_ref_with_enter);                                    \
    } else {                                                                          \
      RTOS_ERR_SET(*(p_err), closed_err_code);                                        \
    }                                                                                 \
  } else PP_UNIQUE_LABEL(fs_obj_ref_with_enter) :                                     \
    ON_EXIT {                                                                         \
    FSObj_RefRelease(prefix##_ToObjHandle(obj_handle).ObjPtr, prefix##_OnNullRefCnt); \
  } WITH                                                                              \


#define  FS_OBJ_WITH(obj_handle, p_err, prefix, closed_err_code)                    \
  if (1) {                                                                          \
    if (FSObj_IOAcquire(prefix##_ToObjHandle(obj_handle), prefix##_OnNullRefCnt)) { \
      goto PP_UNIQUE_LABEL(fs_obj_with_enter);                                      \
    } else {                                                                        \
      RTOS_ERR_SET(*(p_err), closed_err_code);                                      \
    }                                                                               \
  } else PP_UNIQUE_LABEL(fs_obj_with_enter) :                                       \
    ON_EXIT {                                                                       \
    FSObj_IORelease(prefix##_ToObjHandle(obj_handle).ObjPtr);                       \
  } WITH                                                                            \


/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_obj {
  FS_OBJ_FIELDS
} FS_OBJ;

typedef struct fs_obj_handle {
  FS_OBJ    *ObjPtr;
  FS_OBJ_ID ObjId;
} FS_OBJ_HANDLE;

typedef void (*FS_OBJ_ON_NULL_REF_CNT_CB) (FS_OBJ *p_obj);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern FS_OBJ_ID FSObj_CurId;

extern const FS_OBJ_HANDLE FSObj_NullHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FSObj_Init(RTOS_ERR *p_err);

CPU_BOOLEAN FSObj_RefAcquire(FS_OBJ_HANDLE obj_handle);

void FSObj_RefRelease(FS_OBJ                    *p_obj,
                      FS_OBJ_ON_NULL_REF_CNT_CB on_null_ref_cnt);

CPU_BOOLEAN FSObj_IOAcquire(FS_OBJ_HANDLE             obj_handle,
                            FS_OBJ_ON_NULL_REF_CNT_CB on_null_ref_cnt);

void FSObj_IORelease(FS_OBJ *p_obj);

void FSObj_IOEndWait(FS_OBJ *p_obj);

FS_OBJ_HANDLE FSObj_FirstRefAcquireLocked(SLIST_MEMBER *p_list_head);

FS_OBJ_HANDLE FSObj_NextRefAcquireLocked(FS_OBJ_HANDLE cur_obj_handle);

CPU_BOOLEAN FSObj_IsClosingTestSet(FS_OBJ *p_obj);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
