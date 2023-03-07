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

#define  EX_LIB_MEM_SEG_HW_DATA_SIZE                     512u
#define  EX_LIB_MEM_SEG_HW_ALLOC_BLK_INIT                  1u
#define  EX_LIB_MEM_SEG_HW_ALLOC_BLK_MAX                   3u

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

static MEM_SEG ExMemSegHW;
//                                                                 Could specify/use particular location for this data.
static CPU_INT08U   ExMemSegHW_Data[EX_LIB_MEM_SEG_HW_DATA_SIZE];
static MEM_DYN_POOL ExMemPool;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Ex_CommonLibMemDynPoolHW()
 *
 * @brief  Provide examples of MemSegCreate(), Mem_SegAllocHW() and Mem_DynPoolCreateHW() functions.
 *******************************************************************************************************/
void Ex_CommonLibMemDynPoolHW(void)
{
  RTOS_ERR   err;
  CPU_SIZE_T free_bytes;
  CPU_INT08U *p_buf;
  CPU_INT08U i;
  CPU_INT64U *p_data_get_1;
  CPU_INT64U *p_data_get_2;

  //                                                               Create memory segment with padding. Could be to  ...
  //                                                               align on cache lines or to respect constraints   ...
  //                                                               from dedicated memory, etc.
  Mem_SegCreate("Hardware example memory segment",              // Name of the memory segment, for debug purposes,
                &ExMemSegHW,                                    // Pointer to MEM_SEG structure.
                (CPU_ADDR)&(ExMemSegHW_Data[0u]),               // Pointer to data start location.
                EX_LIB_MEM_SEG_HW_DATA_SIZE,                    // Segment size.
                32u,                                            // 32-bytes padding for each allocated blocks.
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Alloc block for CPU_INT32U, aligned on 16-bytes ...
  //                                                               and the padding specified when creating the segment.
  //                                                               So if allocated block does not end of a 32-bytes ...
  //                                                               boundary, the rest will be padded.
  //                                                               Name of the block, for debugging purpose.
  p_buf = (CPU_INT08U *)Mem_SegAllocHW("Aligned and padded example block from example memory segment",
                                       &ExMemSegHW,             // Pointer to MEM_SEG from which to allocate.
                                                                // Size of the block to allocate.
                                       sizeof(CPU_INT08U) * 9u,
                                       16u,                     // Alignment for the block to allocate.
                                                                // Pointer to optional variable indicating how many ...
                                       &free_bytes,             // bytes are missing, if allocation failed.
                                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("p_buf address: %p",
           p_buf);

  for (i = 0u; i < 9u; ++i) {                                   // Fill with dummy data.
    p_buf[i] = i;
  }

  Mem_DynPoolCreateHW("Example persistent pool",                // Name of the persistent pool.
                      &ExMemPool,                               // Pointer to the pool data.
                      &ExMemSegHW,                              // Pointer to MEM_SEG structure.
                      sizeof(CPU_INT64U),                       // Size of memory block from the memory pool.
                      sizeof(CPU_ALIGN),                        // Alignment of memory blocks.
                      EX_LIB_MEM_SEG_HW_ALLOC_BLK_INIT,         // Initial number of blocks to allocate.
                      EX_LIB_MEM_SEG_HW_ALLOC_BLK_MAX,          // Maximum number of blocks that can be allocated.
                      &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Pointer to the pool data.
  p_data_get_1 = (CPU_INT64U *)Mem_DynPoolBlkGet(&ExMemPool, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Address p_data_get_1: %p\n",
           p_data_get_1);

  //                                                               Pointer to the pool data.
  p_data_get_2 = (CPU_INT64U *)Mem_DynPoolBlkGet(&ExMemPool, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Address p_data_get_2: %p\n", p_data_get_2);
}
