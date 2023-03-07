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
#define  EX_MEM_DYN_POOL_PERSISTENT_BLK_NBR_INIT           1u
#define  EX_MEM_DYN_POOL_PERSISTENT_OFFSET              1234u

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
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN Mem_DynPoolCreatePersistent_Callback(MEM_DYN_POOL *p_pool,
                                                        MEM_SEG      *p_seg,
                                                        void         *p_blk,
                                                        void         *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  pool_object {
  CPU_INT32U Index;
  CPU_INT16U Data;
} POOL_OBJECT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_SEG      ExMemSeg;
static CPU_INT08U   ExMemSegData[EX_MEM_SEG_DATA_SIZE];         // Could specify/use particular location for this data.
static MEM_DYN_POOL ExMemPool;
static CPU_INT32U   ExMemDynPoolPersistentGlobalCtr = 0;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   Ex_CommonLibMemDynPoolPersistent()
 *
 * @brief  Provide example of Mem_DynPoolCreatePersistent() and its callback function.
 *******************************************************************************************************/
void Ex_CommonLibMemDynPoolPersistent(void)
{
  RTOS_ERR    err;
  POOL_OBJECT *p_data_get_1;
  POOL_OBJECT *p_data_get_2;

  //                                                               Create regular, general-purpose, memory segment.
  Mem_SegCreate("Example memory segment",                       // Name of the memory segment, for debug purposes.
                &ExMemSeg,                                      // Pointer to MEM_SEG structure.
                (CPU_ADDR)&(ExMemSegData[0u]),                  // Pointer to data start location.
                EX_MEM_SEG_DATA_SIZE,                           // Segment size.
                LIB_MEM_PADDING_ALIGN_NONE,                     // No padding alignment required.
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Create a persistent dynamic pool
  Mem_DynPoolCreatePersistent("Example persistent pool",        // Name of the persistent pool.
                              &ExMemPool,                       // Pointer to the pool data.
                              &ExMemSeg,                        // Pointer to MEM_SEG structure.
                              sizeof(POOL_OBJECT),              // Size of memory block from the memory pool.
                              sizeof(CPU_ALIGN),                // Alignment of memory blocks.
                                                                // Number of block to allocate at initialization.
                              EX_MEM_DYN_POOL_PERSISTENT_BLK_NBR_INIT,
                              LIB_MEM_BLK_QTY_UNLIMITED,        // Unlimited block taken from pool.
                                                                // Callback function.
                              Mem_DynPoolCreatePersistent_Callback,
                              //                                   Callback argument.
                              (void *)&ExMemDynPoolPersistentGlobalCtr,
                              &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  p_data_get_1 = (POOL_OBJECT *)Mem_DynPoolBlkGet(&ExMemPool, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Address: %p Index: %d\n", p_data_get_1, p_data_get_1->Index);

  //                                                               Pointer to the pool data.
  p_data_get_2 = (POOL_OBJECT *)Mem_DynPoolBlkGet(&ExMemPool, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Address: %p Index: %d\n", p_data_get_2, p_data_get_2->Index);

  Mem_DynPoolBlkFree(&ExMemPool,                                // Pointer to the pool data.
                     (void *)p_data_get_1,                      // Pointer to first byte of memory block.
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Mem_DynPoolBlkFree(&ExMemPool,                                // Pointer to the pool data.
                     (void *)p_data_get_2,                      // Pointer to first byte of memory block.
                     &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                 Mem_DynPoolCreatePersistent_Callback()
 *
 * @brief  Callback function executed when a block is obtained from pool for the first time.
 *         This callback give you the chance, to take an action after a block is successfully
 *         allocated from the pool.
 *
 * @param  p_pool  Pointer to pool from which block was allocated.
 *
 * @param  p_seg   Pointer to segment with which pool was created.
 *
 * @param  p_blk   Pointer to allocated block. In this case pointer to a POOL_OBJECT structure.
 *
 * @param  p_arg   Pointer to arg. In this case, pointer to variable
 *                 ExMemDynPoolPersistentGlobalCtr, as specified at pool creation.
 *
 * @return  DEF_YES, if NO error(s),
 *          DEF_NO , otherwise.
 *
 * @note   (1) In this case, the callback function is called each time a Mem_DynPoolBlkGet() is use.
 *             This callback is mainly used to initialize a block of memory after its creation.
 *
 * @note   (2) A persistent pool is useful for storing OS variable, like semaphore,
 *             mutex or other system call than cannot be freed.
 *******************************************************************************************************/
static CPU_BOOLEAN Mem_DynPoolCreatePersistent_Callback(MEM_DYN_POOL *p_pool,
                                                        MEM_SEG      *p_seg,
                                                        void         *p_blk,
                                                        void         *p_arg)
{
  POOL_OBJECT *p_block;
  CPU_INT32U  arg_value;

  PP_UNUSED_PARAM(p_seg);
  PP_UNUSED_PARAM(p_pool);

  p_block = (POOL_OBJECT *)p_blk;                               // Cast argument pointer into POOL_OBJECT pointer.
  arg_value = (*(CPU_INT32U *)p_arg);                           // Do some manipulations.

  p_block->Index = arg_value;
  p_block->Data = EX_MEM_DYN_POOL_PERSISTENT_OFFSET + arg_value;

  (*(CPU_INT32U *)p_arg)++;                                     // Increment the pointer value (+1).

  return (DEF_YES);                                             // Indicate callback operation(s) was/were successful.
}
