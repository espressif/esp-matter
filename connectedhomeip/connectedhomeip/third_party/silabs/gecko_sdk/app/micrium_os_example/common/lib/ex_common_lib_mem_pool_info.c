/***************************************************************************//**
 * @file
 * @brief Common Lib Mem Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_MEM_SEG_DATA_SIZE                            512u
#define  EX_LIB_MEM_POOL_INFO_NUM_ELEMENT                  2u
#define  EX_LIB_MEM_POOL_INFO_MAX_ELEMENT                  6u

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef  EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                  printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_SEG      ExMemSeg;
static CPU_INT08U   ExMemSegData[EX_MEM_SEG_DATA_SIZE];         // Could specify/use particular location for this data.
static MEM_DYN_POOL ExMemPool;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_CommonLibMemDynPoolInfo()
 *
 * @brief  Provide examples of MemSegCreate(), Mem_SegAllocExt() and Mem_DynPoolCreate()  functions.
 *******************************************************************************************************/
void Ex_CommonLibMemDynPoolInfo(void)
{
  RTOS_ERR          err;
  MEM_DYN_POOL_INFO info;
  CPU_SIZE_T        rem_blk_nbr;

  //                                                               Create regular, general-purpose, memory segment.
  Mem_SegCreate("Example memory segment",                       // Name of the memory segment, for debug purposes.
                &ExMemSeg,                                      // Pointer to MEM_SEG structure.
                (CPU_ADDR)&(ExMemSegData[0u]),                  // Pointer to data start location.
                EX_MEM_SEG_DATA_SIZE,                           // Segment size.
                LIB_MEM_PADDING_ALIGN_NONE,                     // No padding alignment required.
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Create a dynamic memory pool.
  //                                                               Name of the block, for debugging purposes.
  Mem_DynPoolCreate("Example dyn pool from example memory segment",
                    &ExMemPool,                                 // Pointer to the pool data.
                    &ExMemSeg,                                  // Pointer to MEM_SEG from which to allocate.
                    sizeof(CPU_INT64U),                         // Size of memory block to allocate from pool.
                    sizeof(CPU_ALIGN),                          // Alignment of memory blocks.
                    EX_LIB_MEM_POOL_INFO_NUM_ELEMENT,           // Number of elements to be allocated from the pool.
                    EX_LIB_MEM_POOL_INFO_MAX_ELEMENT,           // Maximum elements to be allocated from the pool.
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  rem_blk_nbr = Mem_DynPoolBlkNbrAvailGet(&ExMemPool,           // Pointer to the pool data.
                                          &info,                // Pointer to a MEM_DYN_POOL_INFO structure.
                                          &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Resize: %d \nBlank quantity max: %d\nNumber of block available: %d\n",
           rem_blk_nbr,
           info.BlkQtyMax,
           info.BlkNbrAvailCnt);

  //                                                               Name of the block, for debugging purposes.
  Mem_DynPoolCreate("Example block from example memory segment",
                    &ExMemPool,                                 // Pointer to the pool data.
                    &ExMemSeg,                                  // Pointer to MEM_SEG from which to allocate.
                    sizeof(CPU_INT64U),                         // Size of memory block to allocate from pool.
                    sizeof(CPU_ALIGN),                          // Alignment of memory blocks.
                    EX_LIB_MEM_POOL_INFO_NUM_ELEMENT,           // Number of elements to be allocated from the pool.
                    LIB_MEM_BLK_QTY_UNLIMITED,                  // Maximum elements to be allocated from the pool.
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  rem_blk_nbr = Mem_DynPoolBlkNbrAvailGet(&ExMemPool,           // Pointer to the pool data.
                                          &info,                // Pointer to a MEM_DYN_POOL_INFO structure.
                                          &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Resize: %d \nBlank quantity max: %d\nNumber of block available: %d\n",
           rem_blk_nbr,
           info.BlkQtyMax,
           info.BlkNbrAvailCnt);
}
