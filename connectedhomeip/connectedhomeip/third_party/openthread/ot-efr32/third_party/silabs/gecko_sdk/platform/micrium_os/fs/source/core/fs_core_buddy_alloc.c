/***************************************************************************//**
 * @file
 * @brief File System - FS Core Buddy Allocator
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
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CFG ------------------------
#include  <common/include/rtos_path.h>
#include  <fs_core_cfg.h>

//                                                                 ----------------------- EXT ------------------------

#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/core/fs_core_buddy_alloc_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/source/core/fs_core_list_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                (FS, UTIL)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void BuddyAlloc_BlkSplit(BUDDY_ALLOC *p_buddy_alloc,
                                void        *p_blk,
                                CPU_INT08U  start_order,
                                CPU_INT08U  target_order,
                                RTOS_ERR    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               BuddyAlloc_Init()
 *
 * @brief    Initialize buddy allocator.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance. The instance is created by this
 *                           function if a null pointer is passed.
 *
 * @param    p_cfg           Pointer to a buddy allocator configuration structure.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to the initialized buddy allocator instance.
 *
 * @note     (1) The cache buffers allocation are managed by the buddy allocator. The buddy allocator
 *               allocates cache buffers from a memory segment provided by the user upon cache
 *               instance creation (that is FSCache_Create()). The cache buffers may be managed by a
 *               DMA engine and thus may need a DMA dedicated memory region. The memory segment can
 *               be specified in the DMA dedicated memory region if needed.
 *               Buddy allocator controls are allocated from a memory segment called the metadata
 *               region that can be the general heap.
 *******************************************************************************************************/
BUDDY_ALLOC *BuddyAlloc_Init(BUDDY_ALLOC     *p_buddy_alloc,
                             BUDDY_ALLOC_CFG *p_cfg,
                             RTOS_ERR        *p_err)
{
  CPU_SIZE_T octets_reqd;
  CPU_INT08U blk_size_log2;
  CPU_INT32U blk_ix;
  CPU_SIZE_T cur_tbl_pos;
  CPU_SIZE_T blk_tbl_size;
  CPU_SIZE_T list_end_tbl_size;
  CPU_SIZE_T max_order_blk_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    list_end_tbl_size = (p_cfg->MaxOrder + 1u) * sizeof(IB_SLIST_END);
    blk_tbl_size = FS_UTIL_MULT_PWR2(p_cfg->ZerothOrderBlkCnt, p_cfg->ZerothOrderBlkSizeLog2);

    if (p_buddy_alloc == DEF_NULL) {
      BREAK_ON_ERR(p_buddy_alloc = (BUDDY_ALLOC *)Mem_SegAlloc("FS - Buddy allocator",
                                                               p_cfg->MetaDataMemSegPtr,
                                                               sizeof(BUDDY_ALLOC),
                                                               p_err));
    }

    if (p_cfg->BlkTbl == DEF_NULL) {
      //                                                           See Note #1.
      BREAK_ON_ERR(p_buddy_alloc->BlkTbl = (CPU_INT08U *)Mem_SegAllocExt("FS - Buddy allocator blk table",
                                                                         p_cfg->DataMemSegPtr,
                                                                         blk_tbl_size,
                                                                         p_cfg->Align,
                                                                         &octets_reqd,
                                                                         p_err));
    } else {
      p_buddy_alloc->BlkTbl = p_cfg->BlkTbl;
    }

    BREAK_ON_ERR(p_buddy_alloc->FreeBlkListEndTbl = (IB_SLIST_END *)Mem_SegAlloc("FS - Buddy allocator free blk list",
                                                                                 p_cfg->MetaDataMemSegPtr,
                                                                                 list_end_tbl_size,
                                                                                 p_err));

    BREAK_ON_ERR(p_buddy_alloc->UsedBlkListEndTbl = (IB_SLIST_END *)Mem_SegAlloc("FS - Buddy allocator used blk list",
                                                                                 p_cfg->MetaDataMemSegPtr,
                                                                                 list_end_tbl_size,
                                                                                 p_err));

    p_buddy_alloc->ZerothOrderBlkSizeLog2 = p_cfg->ZerothOrderBlkSizeLog2;
    p_buddy_alloc->MaxOrder = p_cfg->MaxOrder;
    p_buddy_alloc->BlkTblSize = blk_tbl_size;

    BREAK_ON_ERR(IB_SList_Init(&p_buddy_alloc->BlkList,
                               p_cfg->MetaDataMemSegPtr,
                               p_cfg->ZerothOrderBlkCnt,
                               p_err));

    for (blk_size_log2 = 0u; blk_size_log2 <= p_cfg->MaxOrder; blk_size_log2++) {
      IB_SList_EndInit(&p_buddy_alloc->FreeBlkListEndTbl[blk_size_log2]);
      IB_SList_EndInit(&p_buddy_alloc->UsedBlkListEndTbl[blk_size_log2]);
    }

    cur_tbl_pos = 0u;
    max_order_blk_cnt = FS_UTIL_DIV_PWR2(p_cfg->ZerothOrderBlkCnt, p_cfg->MaxOrder);
    BREAK_ON_ERR(for) (blk_ix = 0u; blk_ix < max_order_blk_cnt; blk_ix++) {
      IB_SList_Push(&p_buddy_alloc->BlkList,
                    &p_buddy_alloc->FreeBlkListEndTbl[p_cfg->MaxOrder],
                    cur_tbl_pos);
      cur_tbl_pos += FS_UTIL_PWR2(p_cfg->MaxOrder);
    }
  } WITH_SCOPE_END

  return (p_buddy_alloc);
}

/****************************************************************************************************//**
 *                                           BuddyAlloc_BlkFind()
 *
 * @brief    Find the start of the memory block that contains a given location.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance.
 *
 * @param    p_loc           Pointer to a location in memory to look for.
 *
 * @param    p_order         Pointer to a variable that will receive the found block's order.
 *
 * @return   Pointer to the start of the found memory block.
 *******************************************************************************************************/
void *BuddyAlloc_BlkFind(BUDDY_ALLOC *p_buddy_alloc,
                         void        *p_loc,
                         CPU_INT08U  *p_order)
{
  void       *p_cur_blk;
  CPU_INT32U cur_order;
  CPU_INT32U cur_blk_ix;

  p_cur_blk = DEF_NULL;
  for (cur_order = 0u; cur_order <= p_buddy_alloc->MaxOrder; cur_order++) {
    for (cur_blk_ix = IB_SList_FirstGet(&p_buddy_alloc->UsedBlkListEndTbl[cur_order]);
         cur_blk_ix != IB_SLIST_VOID_IX;
         cur_blk_ix = IB_SList_NextGet(&p_buddy_alloc->BlkList, cur_blk_ix)) {
      p_cur_blk = (CPU_INT08U *)p_buddy_alloc->BlkTbl
                  + FS_UTIL_MULT_PWR2(cur_blk_ix, p_buddy_alloc->ZerothOrderBlkSizeLog2);

      if ((p_loc >= p_cur_blk)
          && (p_loc < (void *)((CPU_INT08U *)p_cur_blk + FS_UTIL_MULT_PWR2(p_buddy_alloc->ZerothOrderBlkSizeLog2, cur_order)))) {
        *p_order = cur_order;
        return (p_cur_blk);
      }
    }
  }
  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                           BuddyAlloc_BlkAlloc()
 *
 * @brief    Allocate a new block of the given order.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance.
 *
 * @param    size            Minimum size of the block to be allocated.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to the newly allocated block.
 *******************************************************************************************************/
void *BuddyAlloc_BlkAlloc(BUDDY_ALLOC *p_buddy_alloc,
                          CPU_SIZE_T  size,
                          RTOS_ERR    *p_err)
{
  void           *p_blk;
  BUDDY_ALLOC_IX blk_ix;
  CPU_INT08U     cur_order;
  CPU_INT08U     order;

  WITH_SCOPE_BEGIN(p_err) {
    order = FSUtil_Log2(size) - p_buddy_alloc->ZerothOrderBlkSizeLog2;
    //                                                             ------------------ FIND FREE GRP -------------------
    //                                                             Find smallest group available
    cur_order = order;                                          // with size greater or equal to given group size.
    p_blk = DEF_NULL;
    blk_ix = IB_SLIST_VOID_IX;
    for (cur_order = order; cur_order <= p_buddy_alloc->MaxOrder; cur_order++) {
      blk_ix = IB_SList_Pop(&p_buddy_alloc->BlkList, &p_buddy_alloc->FreeBlkListEndTbl[cur_order]);
      if (blk_ix != IB_SLIST_VOID_IX) {
        break;
      }
    }

    //                                                             ---------------------- SPLIT -----------------------
    if (blk_ix != IB_SLIST_VOID_IX) {                           // If suitable group is available, rem from free list.
      p_blk = (CPU_INT08U *)p_buddy_alloc->BlkTbl
              + FS_UTIL_MULT_PWR2(blk_ix, p_buddy_alloc->ZerothOrderBlkSizeLog2);

      if (cur_order - order > 0) {                              // If group is bigger than required, split it.
        BREAK_ON_ERR(BuddyAlloc_BlkSplit(p_buddy_alloc,
                                         p_blk,
                                         cur_order,
                                         order,
                                         p_err));
      }

      IB_SList_PushBack(&p_buddy_alloc->BlkList,
                        &p_buddy_alloc->UsedBlkListEndTbl[order],
                        blk_ix);
    }
  } WITH_SCOPE_END

  return (p_blk);
}

/****************************************************************************************************//**
 *                                           BuddyAlloc_BlkFree()
 *
 * @brief    Free a block and merge adjacent free blocks.
 *
 * @param    p_buddy_alloc   Pointer to buddy allocator instance.
 *
 * @param    p_blk           Pointer to the block to be freed.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) A block of maximum order may not be merged. Thus 'cur_blk_order' runs from 0 to
 *               max order exclusively.
 *******************************************************************************************************/
void BuddyAlloc_BlkFree(BUDDY_ALLOC *p_buddy_alloc,
                        void        *p_blk)
{
  CPU_INT08U  *p_target_blk;
  CPU_INT08U  *p_merge_blk;
  CPU_INT08U  *p_left_buddy;
  CPU_INT08U  *p_right_buddy;
  CPU_INT32U  merge_blk_ix;
  CPU_INT32U  cur_blk_ix;
  CPU_INT32U  left_buddy_blk_ix;
  CPU_INT32U  right_buddy_blk_ix;
  CPU_INT08U  target_blk_order = 0u;
  CPU_INT08U  cur_blk_order;
  CPU_INT32U  target_blk_ix;
  CPU_BOOLEAN target_blk_is_right_buddy;
  CPU_BOOLEAN blk_found;

  p_target_blk = (CPU_INT08U *)BuddyAlloc_BlkFind(p_buddy_alloc, p_blk, &target_blk_order);
  RTOS_ASSERT_CRITICAL((p_target_blk == (CPU_INT08U *)p_blk), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  target_blk_ix = FS_UTIL_DIV_PWR2((p_target_blk - (CPU_INT08U *)p_buddy_alloc->BlkTbl),
                                   p_buddy_alloc->ZerothOrderBlkSizeLog2);

  IB_SList_Rem(&p_buddy_alloc->BlkList,
               &p_buddy_alloc->UsedBlkListEndTbl[target_blk_order],
               target_blk_ix);

  IB_SList_Push(&p_buddy_alloc->BlkList,
                &p_buddy_alloc->FreeBlkListEndTbl[target_blk_order],
                target_blk_ix);

  for (cur_blk_order = target_blk_order;                        // See Note #1.
       cur_blk_order < p_buddy_alloc->MaxOrder; cur_blk_order++) {
    //                                                             Locate merge blk.
    target_blk_ix = FS_UTIL_DIV_PWR2((p_target_blk - (CPU_INT08U *)p_buddy_alloc->BlkTbl), p_buddy_alloc->ZerothOrderBlkSizeLog2);
    target_blk_is_right_buddy = (target_blk_ix & (FS_UTIL_PWR2(cur_blk_order + 1u) - 1u)) > 0u;
    p_merge_blk = target_blk_is_right_buddy ? p_target_blk - FS_UTIL_PWR2(p_buddy_alloc->ZerothOrderBlkSizeLog2 + cur_blk_order)
                  : p_target_blk + FS_UTIL_PWR2(p_buddy_alloc->ZerothOrderBlkSizeLog2 + cur_blk_order);

    //                                                             If the merge blk is beyond the blk tbl end,
    //                                                             can't merge anymore.
    if (p_merge_blk > (CPU_INT08U *)p_buddy_alloc->BlkTbl + p_buddy_alloc->BlkTblSize) {
      break;
    }

    //                                                             Chk if the merge block is free.
    merge_blk_ix = FS_UTIL_DIV_PWR2((p_merge_blk - (CPU_INT08U *)p_buddy_alloc->BlkTbl), p_buddy_alloc->ZerothOrderBlkSizeLog2);
    blk_found = DEF_NO;
    for (cur_blk_ix = IB_SList_FirstGet(&p_buddy_alloc->FreeBlkListEndTbl[cur_blk_order]);
         cur_blk_ix != IB_SLIST_VOID_IX;
         cur_blk_ix = IB_SList_NextGet(&p_buddy_alloc->BlkList, cur_blk_ix)) {
      if (cur_blk_ix == merge_blk_ix) {
        blk_found = DEF_YES;
        break;
      }
    }

    //                                                             If merge blk is not free, can't merge anymore.
    if (!blk_found) {
      break;
    }

    p_left_buddy = target_blk_is_right_buddy ? p_merge_blk  : p_target_blk;
    p_right_buddy = target_blk_is_right_buddy ? p_target_blk : p_merge_blk;

    left_buddy_blk_ix = FS_UTIL_DIV_PWR2((p_left_buddy - (CPU_INT08U *)p_buddy_alloc->BlkTbl), p_buddy_alloc->ZerothOrderBlkSizeLog2);
    right_buddy_blk_ix = FS_UTIL_DIV_PWR2((p_right_buddy - (CPU_INT08U *)p_buddy_alloc->BlkTbl), p_buddy_alloc->ZerothOrderBlkSizeLog2);

    IB_SList_Rem(&p_buddy_alloc->BlkList,
                 &p_buddy_alloc->FreeBlkListEndTbl[cur_blk_order],
                 left_buddy_blk_ix);

    IB_SList_Push(&p_buddy_alloc->BlkList,
                  &p_buddy_alloc->FreeBlkListEndTbl[cur_blk_order + 1u],
                  left_buddy_blk_ix);

    IB_SList_Rem(&p_buddy_alloc->BlkList,
                 &p_buddy_alloc->FreeBlkListEndTbl[cur_blk_order],
                 right_buddy_blk_ix);

    p_target_blk = p_left_buddy;
  }
}

/****************************************************************************************************//**
 *                                   BuddyAlloc_ZerothOrderBlkSizeLog2Get()
 *
 * @brief    Get base-2 logarithm zeroth order block size.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance.
 *
 * @return   Base-2 logarithm zeroth order block size.
 *******************************************************************************************************/
CPU_INT08U BuddyAlloc_ZerothOrderBlkSizeLog2Get(BUDDY_ALLOC *p_buddy_alloc)
{
  return (p_buddy_alloc->ZerothOrderBlkSizeLog2);
}

/****************************************************************************************************//**
 *                                           BuddyAlloc_MaxOrderGet()
 *
 * @brief    Get buddy allocator maximum order.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance.
 *
 * @return   Maximum order.
 *******************************************************************************************************/
CPU_INT08U BuddyAlloc_MaxOrderGet(BUDDY_ALLOC *p_buddy_alloc)
{
  return (p_buddy_alloc->MaxOrder);
}

/****************************************************************************************************//**
 *                                       BuddyAlloc_BlkTblBaseAddr()
 *
 * @brief    Get base address of blocks table associated to buddy allocator.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance.
 *
 * @return   Maximum order.
 *******************************************************************************************************/
CPU_ADDR BuddyAlloc_BlkTblBaseAddr(BUDDY_ALLOC *p_buddy_alloc)
{
  return ((CPU_ADDR)p_buddy_alloc->BlkTbl);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           BuddyAlloc_BlkSplit()
 *
 * @brief    Recursively split a block.
 *
 * @param    p_buddy_alloc   Pointer to a buddy allocator instance.
 *
 * @param    p_blk           Pointer to the block to be split.
 *
 * @param    start_order     Block order before splitting.
 *
 * @param    target_order    Block order to be obtained after splitting.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void BuddyAlloc_BlkSplit(BUDDY_ALLOC *p_buddy_alloc,
                                void        *p_blk,
                                CPU_INT08U  start_order,
                                CPU_INT08U  target_order,
                                RTOS_ERR    *p_err)
{
  CPU_INT08U *p_buddy_blk;
  CPU_INT08U cur_order;
  CPU_INT08U cur_split_ix;
  CPU_INT08U split_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    if (start_order == target_order) {
      return;
    }

    RTOS_ASSERT_CRITICAL((start_order > target_order), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    split_cnt = start_order - target_order;
    cur_order = start_order;
    BREAK_ON_ERR(for) (cur_split_ix = split_cnt; cur_split_ix > 0u; cur_split_ix--) {
      cur_order--;
      p_buddy_blk = (CPU_INT08U *)p_blk + FS_UTIL_PWR2(p_buddy_alloc->ZerothOrderBlkSizeLog2 + cur_order);
      if (p_buddy_blk < (CPU_INT08U *)p_buddy_alloc->BlkTbl + p_buddy_alloc->BlkTblSize) {
        CPU_INT08U blk_ix;
        blk_ix = FS_UTIL_DIV_PWR2((p_buddy_blk - (CPU_INT08U *)p_buddy_alloc->BlkTbl), p_buddy_alloc->ZerothOrderBlkSizeLog2);
        IB_SList_Push(&p_buddy_alloc->BlkList,
                      &p_buddy_alloc->FreeBlkListEndTbl[cur_order],
                      blk_ix);
      }
    }
  } WITH_SCOPE_END
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
