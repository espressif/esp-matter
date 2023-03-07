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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_BUDDY_ALLOC_PRIV_H_
#define  FS_CORE_BUDDY_ALLOC_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_mem.h>
#include  <fs/source/core/fs_core_list_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT08U BUDDY_ALLOC_IX;

typedef struct buddy_alloc_cfg {
  CPU_SIZE_T ZerothOrderBlkSizeLog2;
  CPU_INT32U ZerothOrderBlkCnt;
  CPU_INT08U MaxOrder;
  CPU_SIZE_T Align;
  MEM_SEG    *DataMemSegPtr;
  MEM_SEG    *MetaDataMemSegPtr;
  void       *BlkTbl;
} BUDDY_ALLOC_CFG;

typedef struct buddy_alloc {
  void         *BlkTbl;
  CPU_SIZE_T   BlkTblSize;
  CPU_SIZE_T   ZerothOrderBlkSizeLog2;
  CPU_INT08U   MaxOrder;
  IB_SLIST     BlkList;
  IB_SLIST_END *FreeBlkListEndTbl;
  IB_SLIST_END *UsedBlkListEndTbl;
} BUDDY_ALLOC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

BUDDY_ALLOC *BuddyAlloc_Init(BUDDY_ALLOC     *p_alloc,
                             BUDDY_ALLOC_CFG *p_alloc_cfg,
                             RTOS_ERR        *p_err);

void *BuddyAlloc_BlkAlloc(BUDDY_ALLOC *p_alloc,
                          CPU_SIZE_T  size,
                          RTOS_ERR    *p_err);

void BuddyAlloc_BlkFree(BUDDY_ALLOC *p_alloc,
                        void        *p_blk);

void *BuddyAlloc_BlkFind(BUDDY_ALLOC *p_alloc,
                         void        *p_loc,
                         CPU_INT08U  *p_zeroth_order_blk_cnt_log2);

CPU_INT08U BuddyAlloc_ZerothOrderBlkSizeLog2Get(BUDDY_ALLOC *p_alloc);

CPU_INT08U BuddyAlloc_MaxOrderGet(BUDDY_ALLOC *p_alloc);

CPU_ADDR BuddyAlloc_BlkTblBaseAddr(BUDDY_ALLOC *p_alloc);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
