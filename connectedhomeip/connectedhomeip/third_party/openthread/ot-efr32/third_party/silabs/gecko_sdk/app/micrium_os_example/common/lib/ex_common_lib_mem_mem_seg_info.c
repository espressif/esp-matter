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
#define  EX_LIB_MEM_SEG_INFO_ALIGNMENT                     8u

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

static MEM_SEG    ExMemSeg;
static CPU_INT08U ExMemSegData[EX_MEM_SEG_DATA_SIZE];           // Could specify/use particular location for this data.

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_CommonLibMemSegInfo()
 *
 * @brief  Provide example of MemSegCreate() and Mem_SegRemSizeGet() functions.
 *******************************************************************************************************/
void Ex_CommonLibMemSegInfo(void)
{
  RTOS_ERR     err;
  MEM_SEG_INFO info;
  CPU_SIZE_T   rem_size;
  CPU_INT32U   *p_alloc_data;

  //                                                               Create regular, general-purpose, memory segment.
  Mem_SegCreate("Example memory segment",                       // Name of the memory segment, for debug purposes.
                &ExMemSeg,                                      // Pointer to MEM_SEG structure.
                (CPU_ADDR)&(ExMemSegData[0u]),                  // Pointer to data start location.
                EX_MEM_SEG_DATA_SIZE,                           // Segment size.
                LIB_MEM_PADDING_ALIGN_NONE,                     // No padding alignment required.
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Allocate block from general-purpose mem seg.
  //                                                               name of the block, for debugging purposes,
  p_alloc_data = (CPU_INT32U *)Mem_SegAlloc("Example block from example memory segment",
                                            &ExMemSeg,          // pointer to MEM_SEG from which to allocate,
                                                                // size of the block to allocate.
                                            sizeof(CPU_INT32U),
                                            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  *p_alloc_data = 0xDEADBEEF;                                   // Fill with dummy data.

  rem_size = Mem_SegRemSizeGet(&ExMemSeg,
                               EX_LIB_MEM_SEG_INFO_ALIGNMENT,   // Alignment in bytes.
                               &info,
                               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Base address: %d\nTotal size: %d\nUsed size: %d\n", info.AddrBase, info.TotalSize, info.UsedSize);

  EX_TRACE("Resize: %d\n", rem_size);
}
