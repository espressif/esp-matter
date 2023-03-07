/***************************************************************************//**
 * @file
 * @brief File System - Core List Operations
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

#ifndef  FS_CORE_LIST_PRIV_H_
#define  FS_CORE_LIST_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  IB_SLIST_VOID_IX     ((IB_SLIST_IX)-1)
#define  IB_DLIST_VOID_IX     ((IB_DLIST_IX)-1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT08U IB_SLIST_IX;

typedef struct ib_slist_end {
  IB_SLIST_IX FirstIx;
} IB_SLIST_END;

typedef struct ib_slist_cfg {
  MEM_SEG     *MemSegPtr;
  IB_SLIST_IX ItemCnt;
} IB_SLIST_CFG;

typedef struct ib_slist {
  IB_SLIST_IX *IxTbl;
} IB_SLIST;

typedef CPU_INT08U IB_DLIST_IX;

typedef struct ib_dlist_end {
  IB_DLIST_IX FirstIx;
  IB_DLIST_IX LastIx;
} IB_DLIST_END;

typedef struct ib_dlist_ixes {
  IB_DLIST_IX NextIx;
  IB_DLIST_IX PrevIx;
} IB_DLIST_IXES;

typedef struct ib_dlist_cfg {
  MEM_SEG     *MemSegPtr;
  IB_DLIST_IX ItemCnt;
} IB_DLIST_CFG;

typedef struct ib_dlist {
  IB_DLIST_IXES *IxesTbl;
} IB_DLIST;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               SINGLY-LINKED LISTS FUNCTION PROTOTYPES
 *******************************************************************************************************/

void IB_SList_Init(IB_SLIST    *p_list,
                   MEM_SEG     *p_seg,
                   IB_SLIST_IX item_cnt,
                   RTOS_ERR    *p_err);

void IB_SList_EndInit(IB_SLIST_END *p_list_end);

IB_SLIST_IX IB_SList_FirstGet(IB_SLIST_END *p_list_end);

IB_SLIST_IX IB_SList_NextGet(IB_SLIST    *p_list,
                             IB_SLIST_IX cur_item_ix);

void IB_SList_Push(IB_SLIST     *p_list,
                   IB_SLIST_END *p_list_end,
                   IB_SLIST_IX  new_item_ix);

void IB_SList_PushBack(IB_SLIST     *p_list,
                       IB_SLIST_END *p_list_end,
                       IB_SLIST_IX  item_ix);

void IB_SList_Rem(IB_SLIST     *p_list,
                  IB_SLIST_END *p_list_end,
                  IB_SLIST_IX  rem_item_ix);

IB_SLIST_IX IB_SList_Pop(IB_SLIST     *p_list,
                         IB_SLIST_END *p_list_end);

/********************************************************************************************************
 *                               DOUBLY-LINKED LISTS FUNCTION PROTOTYPES
 *******************************************************************************************************/

void IB_DList_Init(IB_DLIST    *p_list,
                   MEM_SEG     *p_seg,
                   IB_DLIST_IX item_cnt,
                   RTOS_ERR    *p_err);

void IB_DList_EndInit(IB_DLIST_END *p_list_end);

IB_DLIST_IX IB_DList_FirstGet(IB_DLIST_END *p_list_end);

IB_DLIST_IX IB_DList_NextGet(IB_DLIST    *p_list,
                             IB_DLIST_IX cur_item_ix);

IB_DLIST_IX IB_DList_LastGet(IB_DLIST_END *p_list_end);

IB_DLIST_IX IB_DList_PrevGet(IB_DLIST    *p_list,
                             IB_DLIST_IX cur_item_ix);

void IB_DList_InsertAfter(IB_DLIST     *p_list,
                          IB_DLIST_END *p_list_end,
                          IB_DLIST_IX  insert_item_ix,
                          IB_DLIST_IX  new_item_ix);

void IB_DList_Push(IB_DLIST     *p_list,
                   IB_DLIST_END *p_list_end,
                   IB_DLIST_IX  new_item_ix);

void IB_DList_PushBack(IB_DLIST     *p_list,
                       IB_DLIST_END *p_list_end,
                       IB_DLIST_IX  new_item_ix);

void IB_DList_Rem(IB_DLIST     *p_list,
                  IB_DLIST_END *p_list_end,
                  IB_DLIST_IX  rem_item_ix);

IB_DLIST_IX IB_DList_Pop(IB_DLIST     *p_list,
                         IB_DLIST_END *p_list_end);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
