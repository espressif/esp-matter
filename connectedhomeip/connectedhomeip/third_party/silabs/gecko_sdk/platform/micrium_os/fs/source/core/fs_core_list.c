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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

#include  <fs/source/core/fs_core_list_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               IB_SList_Init()
 *
 * @brief    Initialize an index-based singly-linked list instance.
 *
 * @param    p_list      Pointer to a list instance.
 *
 * @param    p_seg       Pointer to memory segment.
 *
 * @param    item_cnt    Number of items in table.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void IB_SList_Init(IB_SLIST    *p_list,
                   MEM_SEG     *p_seg,
                   IB_SLIST_IX item_cnt,
                   RTOS_ERR    *p_err)
{
  IB_SLIST_IX ix;

  p_list->IxTbl = (CPU_INT08U *)Mem_SegAlloc("FS - index-based single list tbl",
                                             p_seg,
                                             item_cnt * sizeof(IB_SLIST_IX),
                                             p_err);

  for (ix = 0u; ix < item_cnt; ix++) {
    p_list->IxTbl[ix] = IB_SLIST_VOID_IX;
  }
}

/****************************************************************************************************//**
 *                                           IB_SList_EndInit()
 *
 * @brief    Initalize an index-based singly-link list end.
 *
 * @param    p_list_end  Pointer to a list end.
 *******************************************************************************************************/
void IB_SList_EndInit(IB_SLIST_END *p_list_end)
{
  p_list_end->FirstIx = IB_SLIST_VOID_IX;
}

/****************************************************************************************************//**
 *                                           IB_SList_FirstGet()
 *
 * @brief    Get the first item of an index-based singly-linked list.
 *
 * @param    p_list_end  Pointer to a list end.
 *
 * @return   Index of the first item.
 *******************************************************************************************************/
IB_SLIST_IX IB_SList_FirstGet(IB_SLIST_END *p_list_end)
{
  return (p_list_end->FirstIx);
}

/****************************************************************************************************//**
 *                                           IB_SList_NextGet()
 *
 * @brief    Get the item after a given item in a index-based singly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    cur_item_ix     Index of the current item.
 *
 * @return   Index of the next item in the list.
 *******************************************************************************************************/
IB_SLIST_IX IB_SList_NextGet(IB_SLIST    *p_list,
                             IB_SLIST_IX cur_item_ix)
{
  return (p_list->IxTbl[cur_item_ix]);
}

/****************************************************************************************************//**
 *                                               IB_SList_Push()
 *
 * @brief    Add a new item at the beginning of a index-based singly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    new_item_ix     Index of the new item.
 *******************************************************************************************************/
void IB_SList_Push(IB_SLIST     *p_list,
                   IB_SLIST_END *p_list_end,
                   IB_SLIST_IX  new_item_ix)
{
  //                                                               -------------- CHK IF ALREADY IN LIST --------------
#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  for (IB_DLIST_IX ix = IB_SList_FirstGet(p_list_end); ix != IB_SLIST_VOID_IX; ix = IB_SList_NextGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  p_list->IxTbl[new_item_ix] = p_list_end->FirstIx;
  p_list_end->FirstIx = new_item_ix;
}

/****************************************************************************************************//**
 *                                           IB_SList_PushBack()
 *
 * @brief    Add a new item at the end of an index-based singly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    new_item_ix     Index of the new item.
 *******************************************************************************************************/
void IB_SList_PushBack(IB_SLIST     *p_list,
                       IB_SLIST_END *p_list_end,
                       IB_SLIST_IX  new_item_ix)
{
  IB_SLIST_IX last_item_ix;

  //                                                               -------------- CHK IF ALREADY IN LIST --------------
#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  for (IB_DLIST_IX ix = IB_SList_FirstGet(p_list_end); ix != IB_SLIST_VOID_IX; ix = IB_SList_NextGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  last_item_ix = IB_SLIST_VOID_IX;
  for (IB_SLIST_IX ix = p_list_end->FirstIx; ix != IB_SLIST_VOID_IX; ix = p_list->IxTbl[ix]) {
    if (ix == IB_SLIST_VOID_IX) {
      break;
    }
    last_item_ix = ix;
  }

  p_list->IxTbl[new_item_ix] = IB_SLIST_VOID_IX;

  if (last_item_ix == IB_SLIST_VOID_IX) {
    p_list_end->FirstIx = new_item_ix;
  } else {
    p_list->IxTbl[last_item_ix] = new_item_ix;
  }
}

/****************************************************************************************************//**
 *                                               IB_SList_Rem()
 *
 * @brief    Remove an item from an index-based singly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    rem_item_ix     Index of the item to be removed.
 *******************************************************************************************************/
void IB_SList_Rem(IB_SLIST     *p_list,
                  IB_SLIST_END *p_list_end,
                  IB_SLIST_IX  rem_item_ix)
{
  IB_SLIST_IX prev_item_ix;
  IB_SLIST_IX next_item_ix;

  //                                                               ---------------- CHK IF IS IN LIST -----------------
#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  {
    CPU_BOOLEAN found = DEF_NO;
    for (IB_DLIST_IX ix = IB_SList_FirstGet(p_list_end); ix != IB_SLIST_VOID_IX; ix = IB_SList_NextGet(p_list, ix)) {
      if (ix == rem_item_ix) {
        found = DEF_YES;
      }
    }
    RTOS_ASSERT_CRITICAL(found, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  prev_item_ix = IB_SLIST_VOID_IX;
  for (IB_SLIST_IX ix = p_list_end->FirstIx; ix != IB_SLIST_VOID_IX; ix = p_list->IxTbl[ix]) {
    if (ix == rem_item_ix) {
      break;
    }
    prev_item_ix = ix;
  }

  next_item_ix = p_list->IxTbl[rem_item_ix];
  if (prev_item_ix == IB_SLIST_VOID_IX) {
    p_list_end->FirstIx = next_item_ix;
  } else {
    p_list->IxTbl[prev_item_ix] = next_item_ix;
  }
}

/****************************************************************************************************//**
 *                                               IB_SList_Pop()
 *
 * @brief    Remove the first item from a index-based singly-linked list.
 *
 * @param    p_list      Pointer to a list instance.
 *
 * @param    p_list_end  Pointer to a list end.
 *
 * @return   Index of the removed item.
 *******************************************************************************************************/
IB_SLIST_IX IB_SList_Pop(IB_SLIST     *p_list,
                         IB_SLIST_END *p_list_end)
{
  IB_SLIST_IX pop_item_ix;

  if (p_list_end->FirstIx == IB_SLIST_VOID_IX) {
    return (IB_SLIST_VOID_IX);
  }

  pop_item_ix = p_list_end->FirstIx;
  p_list_end->FirstIx = p_list->IxTbl[pop_item_ix];

  return (pop_item_ix);
}

/****************************************************************************************************//**
 *                                               IB_DList_Init()
 *
 * @brief    Initialize an index-based doubly-linked list instance.
 *
 * @param    p_list      Pointer to a list instance.
 *
 * @param    p_seg       Pointer to memory segment.
 *
 * @param    item_cnt    Number of items in table.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void IB_DList_Init(IB_DLIST    *p_list,
                   MEM_SEG     *p_seg,
                   IB_DLIST_IX item_cnt,
                   RTOS_ERR    *p_err)
{
  IB_DLIST_IX ix;

  p_list->IxesTbl = (IB_DLIST_IXES *)Mem_SegAlloc("FS - index-based double list tbl",
                                                  p_seg,
                                                  item_cnt * sizeof(IB_DLIST_IXES),
                                                  p_err);

  for (ix = 0u; ix < item_cnt; ix++) {
    p_list->IxesTbl[ix].PrevIx = IB_DLIST_VOID_IX;
    p_list->IxesTbl[ix].NextIx = IB_DLIST_VOID_IX;
  }
}

/****************************************************************************************************//**
 *                                           IB_DList_EndInit()
 *
 * @brief    Initialize an index-based doubly-linked list end.
 *
 * @param    p_list_end  Pointer to a list end.
 *******************************************************************************************************/
void IB_DList_EndInit(IB_DLIST_END *p_list_end)
{
  p_list_end->FirstIx = IB_DLIST_VOID_IX;
  p_list_end->LastIx = IB_DLIST_VOID_IX;
}

/****************************************************************************************************//**
 *                                           IB_DList_FirstGet()
 *
 * @brief    Get the first item of a index-based doubly-linked list.
 *
 * @param    p_list_end  Pointer to a list end.
 *
 * @return   Index of the first item.
 *******************************************************************************************************/
IB_DLIST_IX IB_DList_FirstGet(IB_DLIST_END *p_list_end)
{
  return (p_list_end->FirstIx);
}

/****************************************************************************************************//**
 *                                           IB_DList_NextGet()
 *
 * @brief    Get the item after a given item in a index-based doubly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    cur_item_ix     Index of the current item.
 *
 * @return   Index of the next item.
 *******************************************************************************************************/
IB_DLIST_IX IB_DList_NextGet(IB_DLIST    *p_list,
                             IB_DLIST_IX cur_item_ix)
{
  return (p_list->IxesTbl[cur_item_ix].NextIx);
}

/****************************************************************************************************//**
 *                                           IB_DList_LastGet()
 *
 * @brief    Get the last item of a index-based doubly-linked list.
 *
 * @param    p_list_end  Pointer to a list end.
 *
 * @return   Index of the last item.
 *******************************************************************************************************/
IB_DLIST_IX IB_DList_LastGet(IB_DLIST_END *p_list_end)
{
  return (p_list_end->LastIx);
}

/****************************************************************************************************//**
 *                                           IB_DList_PrevGet()
 *
 * @brief    Get the item before a given item in a index-based doubly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    cur_item_ix     Index of the current item.
 *******************************************************************************************************/
IB_DLIST_IX IB_DList_PrevGet(IB_DLIST    *p_list,
                             IB_DLIST_IX cur_item_ix)
{
  return (p_list->IxesTbl[cur_item_ix].PrevIx);
}

/****************************************************************************************************//**
 *                                           IB_DList_InsertAfter()
 *
 * @brief    Insert a new item after a given item in a index-based doubly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    insert_item_ix  Index of the item after which the new item will be inserted.
 *
 * @param    new_item_ix     Index of the item to be inserted.
 *******************************************************************************************************/
void IB_DList_InsertAfter(IB_DLIST     *p_list,
                          IB_DLIST_END *p_list_end,
                          IB_DLIST_IX  insert_item_ix,
                          IB_DLIST_IX  new_item_ix)
{
  IB_DLIST_IX next_item_ix;

  //                                                               -------------- CHK IF ALREADY IN LIST --------------
#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  for (IB_DLIST_IX ix = IB_DList_FirstGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_NextGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  for (IB_DLIST_IX ix = IB_DList_LastGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_PrevGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  next_item_ix = p_list->IxesTbl[insert_item_ix].NextIx;

  //                                                               --------------------- SET PREV ---------------------
  p_list->IxesTbl[new_item_ix].PrevIx = insert_item_ix;
  if (next_item_ix != IB_DLIST_VOID_IX) {
    p_list->IxesTbl[next_item_ix].PrevIx = new_item_ix;
  } else {
    p_list_end->LastIx = new_item_ix;
  }

  //                                                               --------------------- SET_NEXT ---------------------
  p_list->IxesTbl[insert_item_ix].NextIx = new_item_ix;
  p_list->IxesTbl[new_item_ix].NextIx = next_item_ix;
}

/****************************************************************************************************//**
 *                                               IB_DList_Push()
 *
 * @brief    Add a new item at the beginning of a index-based doubly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    new_item_ix     Index of the item to be added.
 *******************************************************************************************************/
void IB_DList_Push(IB_DLIST     *p_list,
                   IB_DLIST_END *p_list_end,
                   IB_DLIST_IX  new_item_ix)
{
  //                                                               -------------- CHK IF ALREADY IN LIST --------------
#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  for (IB_DLIST_IX ix = IB_DList_FirstGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_NextGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  for (IB_DLIST_IX ix = IB_DList_LastGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_PrevGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  p_list->IxesTbl[new_item_ix].PrevIx = IB_DLIST_VOID_IX;
  p_list->IxesTbl[new_item_ix].NextIx = p_list_end->FirstIx;

  if (p_list_end->LastIx == IB_DLIST_VOID_IX) {
    p_list_end->LastIx = new_item_ix;
  } else {
    p_list->IxesTbl[p_list_end->FirstIx].PrevIx = new_item_ix;
  }

  p_list_end->FirstIx = new_item_ix;
}

/****************************************************************************************************//**
 *                                           IB_DList_PushBack()
 *
 * @brief    Add a new item at the end of a index-based doubly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    new_item_ix     Index of the item to be added.
 *******************************************************************************************************/
void IB_DList_PushBack(IB_DLIST     *p_list,
                       IB_DLIST_END *p_list_end,
                       IB_DLIST_IX  new_item_ix)
{
  //                                                               -------------- CHK IF ALREADY IN LIST --------------
#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  for (IB_DLIST_IX ix = IB_DList_FirstGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_NextGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  for (IB_DLIST_IX ix = IB_DList_LastGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_PrevGet(p_list, ix)) {
    RTOS_ASSERT_CRITICAL(ix != new_item_ix, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  p_list->IxesTbl[new_item_ix].NextIx = IB_DLIST_VOID_IX;
  p_list->IxesTbl[new_item_ix].PrevIx = p_list_end->LastIx;

  if (p_list_end->FirstIx == IB_DLIST_VOID_IX) {
    p_list_end->FirstIx = new_item_ix;
  } else {
    p_list->IxesTbl[p_list_end->LastIx].NextIx = new_item_ix;
  }

  p_list_end->LastIx = new_item_ix;
}

/****************************************************************************************************//**
 *                                               IB_DList_Rem()
 *
 * @brief    Remove an item from a index-based doubly-linked list.
 *
 * @param    p_list          Pointer to a list instance.
 *
 * @param    p_list_end      Pointer to a list end.
 *
 * @param    rem_item_ix     Inded of the item to be removed.
 *******************************************************************************************************/
void IB_DList_Rem(IB_DLIST     *p_list,
                  IB_DLIST_END *p_list_end,
                  IB_DLIST_IX  rem_item_ix)
{
  IB_DLIST_IX prev_item_ix;
  IB_DLIST_IX next_item_ix;

#if (FS_CORE_LIST_DBG_CHK_EN == DEF_ENABLED)
  {
    CPU_BOOLEAN ix_found = DEF_NO;
    for (IB_DLIST_IX ix = IB_DList_FirstGet(p_list_end); ix != IB_DLIST_VOID_IX; ix = IB_DList_NextGet(p_list, ix)) {
      if (ix == rem_item_ix) {
        ix_found = DEF_YES;
        break;
      }
    }
    RTOS_ASSERT_CRITICAL(ix_found, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
#endif

  prev_item_ix = p_list->IxesTbl[rem_item_ix].PrevIx;
  next_item_ix = p_list->IxesTbl[rem_item_ix].NextIx;

  //                                                               ------------- SET PREV'S NEXT ADAPTER --------------
  if (prev_item_ix == IB_DLIST_VOID_IX) {
    p_list_end->FirstIx = next_item_ix;
  } else {
    p_list->IxesTbl[prev_item_ix].NextIx = next_item_ix;
  }

  //                                                               ------------- SET NEXT'S PREV ADAPTER --------------
  if (next_item_ix == IB_DLIST_VOID_IX) {
    p_list_end->LastIx = prev_item_ix;
  } else {
    p_list->IxesTbl[next_item_ix].PrevIx = prev_item_ix;
  }
}

/****************************************************************************************************//**
 *                                               IB_DList_Pop()
 *
 * @brief    Remove the first item of a index-based doubly-linked list.
 *
 * @param    p_list      Pointer to a list instance.
 *
 * @param    p_list_end  Pointer to a list end.
 *
 * @return   Index of the removed item.
 *******************************************************************************************************/
IB_DLIST_IX IB_DList_Pop(IB_DLIST     *p_list,
                         IB_DLIST_END *p_list_end)
{
  IB_DLIST_IX pop_item_ix;
  IB_DLIST_IX next_item_ix;

  pop_item_ix = p_list_end->FirstIx;

  if (pop_item_ix == IB_DLIST_VOID_IX) {
    return (IB_DLIST_VOID_IX);
  }

  next_item_ix = p_list->IxesTbl[p_list_end->FirstIx].NextIx;
  if (next_item_ix == IB_DLIST_VOID_IX) {
    p_list_end->LastIx = IB_DLIST_VOID_IX;
  } else {
    p_list->IxesTbl[next_item_ix].PrevIx = IB_DLIST_VOID_IX;
  }

  p_list_end->FirstIx = next_item_ix;

  return (pop_item_ix);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
