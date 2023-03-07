/******************************************************************************

 @file  rtos_heaptrack.h

 @brief Heap memory template based on teh TI-RTOS Heap MEm module.

    This file should be included in .c file to generate a heap memory
    management module with its own function names, heap size and, platform
    specific mutex implementation, etc. Note that although the idea of this
    file is the analogy of C++ template or ADA generic, this file can be
    included only once in a C file due to use of fixed type names and macro
    names.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#include <string.h>
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/heaps/HeapTrack.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
// This include file will bring HEAPMGR_SIZE and stackHeap is properly setup in the ti-rtos
// config file.
#include <xdc/cfg/global.h>

/* macros to override the function names for efficient linking and multiple instantiations */
#ifndef HEAPMGR_INIT
#define HEAPMGR_INIT   heapmgrInit
#endif

#ifndef HEAPMGR_MALLOC
#define HEAPMGR_MALLOC heapmgrMalloc
#endif

#ifndef HEAPMGR_FREE
#define HEAPMGR_FREE heapmgrFree
#endif

/* macro for debug assert */
#ifndef HEAPMGR_ASSERT
#define HEAPMGR_ASSERT(_exp)
#endif

#define FORCED_ALIGNEMENT 4

/* This limit is used when the API HEAPMGR_MALLOC_LIMITED is used.
 * It is the minimum number of free bytes that must be available in
 * the heap after the allocation is done (not taking the heap header into account).
 */
#ifndef HEAPMGR_FREE_SAFE_LIMIT
#ifdef HCI_TL_FULL
// Bigger to account for the maximum size of the TL Rx buffer which does not fully
// manage heap exhaustion yet.
#define HEAPMGR_FREE_SAFE_LIMIT  550
#else
#define HEAPMGR_FREE_SAFE_LIMIT  300
#endif
#endif

#ifndef HEAPMGR_PREFIXED
#define HEAPMGR_PREFIXED(_name) heapmgr ## _name
#endif

#ifdef HEAPMGR_METRICS
#define HEAPMGR_MEMALO HEAPMGR_PREFIXED(MemAlo)
#define HEAPMGR_MEMMAX HEAPMGR_PREFIXED(MemMax)
#define HEAPMGR_MEMFAIL HEAPMGR_PREFIXED(MemFail)
#endif
#define HEAPMGR_TOTALFREESIZE HEAPMGR_PREFIXED(MemFreeTotal)

/*********************************************************************
 * TYPEDEFS
 */
typedef struct Header_Custom {
   uint32_t size;
} Header_Custom;

/*********************************************************************
 * GLOBAL VARIABLES
 */
#ifdef HEAPMGR_METRICS
uint32_t HEAPMGR_MEMALO  = 0; // Current total memory allocated.
uint32_t HEAPMGR_MEMMAX  = 0; // Max total memory ever allocated at once.
uint16_t HEAPMGR_MEMFAIL = 0; // Memory allocation failure count
uint8_t  *HEAPMGR_HEAP;
#endif // HEAPMGR_METRICS
uint32_t HEAPMGR_TOTALFREESIZE = 0;

/*********************************************************************
 * EXTERNAL VARIABLES
 */
/* This variable is define in TI-RTOS */
extern const xdc_runtime_IHeap_Handle Memory_defaultHeapInstance;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8_t heapInitialize = 0;

#define HEAPMGR_OVERHEAD (24 + /*sizeof(HeapTrack_Header)*/ + FORCED_ALIGNEMENT + sizeof(Header_Custom))
/**
 * @brief   Initialize the heap memory management system.
 */
void HEAPMGR_INIT(void)
{
  uint8_t * temp;
  Memory_Stats memStats;

  if (heapInitialize == 1)
  {
    // Initialization already done. Only once initialization allowed.
    return;
  }
  heapInitialize = 1;
  // Since dynamic heap may be used, the RAM may not be initialize and contains
  // random data. for precausion, initialize the full buffer to 0xDD.
  HeapTrack_getStats((HeapTrack_Handle)stackHeap, &memStats);
  if (memStats.largestFreeSize == 0)
  {
    // This mean the Heap has not been properly created , is something else
    // than HeapTrack has been used?
    HEAPMGR_ASSERT();
  }
  // sizeof(HeapTrack_Header) need to be taken into account since it is not
  // taken into account by heapTrack (issue has been reported)
  temp = HEAPMGR_MALLOC(memStats.largestFreeSize - HEAPMGR_OVERHEAD);
  memset(temp,0x00, memStats.largestFreeSize - HEAPMGR_OVERHEAD);
  HEAPMGR_FREE(temp);

#ifdef HEAPMGR_METRICS
  {
    HEAPMGR_HEAP = NULL;
    HEAPMGR_MEMFAIL = 0;
    HEAPMGR_MEMMAX = 0;
  }
#endif // HEAPMGR_METRICS
}

/**
 * @brief   Implementation of the allocator functionality.
 * @param   size - number of bytes to allocate from the heap.
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *HEAPMGR_MALLOC(uint32_t size)
{
  Header_Custom *tmp;
  Error_Block eb;

  // Return NULL if size is 0
  if (size == 0)
  {
    return(NULL);
  }

  Error_init(&eb);
  /* Add room for the "malloc" like header */
  size += sizeof(Header_Custom);

  /* Using the default system heap API */
  tmp = Memory_alloc(HeapTrack_Handle_upCast(stackHeap), size, FORCED_ALIGNEMENT, &eb);

  if (tmp == NULL)
  {
#ifdef HEAPMGR_METRICS
    HEAPMGR_MEMFAIL++;
#endif // HEAPMGR_METRICS
#ifdef MEM_ALLOC_ASSERT
  // If allocation failed, assert.
  {
    // When EXT_HAL_ASSERT is defined, set the assert cause.  Otherwise, trap
    // the assert with argument FALSE.
#ifdef EXT_HAL_ASSERT
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
#else /* !EXT_HAL_ASSERT */
    HAL_ASSERT( FALSE );
#endif /* EXT_HAL_ASSERT */
  }
#endif /* MEM_ALLOC_ASSERT */
    return(NULL);
  }
#ifdef HEAPMGR_METRICS
  else
  {
    HEAPMGR_MEMALO += size;
    if (HEAPMGR_MEMMAX < HEAPMGR_MEMALO)
    {
      HEAPMGR_MEMMAX = HEAPMGR_MEMALO;
    }
  }
#endif // HEAPMGR_METRICS
  HEAPMGR_TOTALFREESIZE -= size;

  /* Store the size to be used in the custom_free */
  tmp->size = size;

  /* Return the buffer, but skipping over the internal header */
  return((uint8_t *)tmp + sizeof(Header_Custom));

}

/**
 * @brief   Implementation of the de-allocator functionality.
 * @param   ptr - pointer to the memory to free.
 */
void HEAPMGR_FREE(void *ptr)
{
  void  *tmp;
  uint_least16_t  hwikey;

  if (ptr != NULL)
  {
    /* Get the internal header */
    tmp = (Header_Custom *)((uint8_t *)ptr - sizeof(Header_Custom));

    /* Protect since HeapMem_freeNoGate does not */
    hwikey = (uint_least16_t) Hwi_disable();

#ifdef HEAPMGR_METRICS
    HEAPMGR_MEMALO -= ((Header_Custom*)tmp)->size;
#endif // HEAPMGR_METRICS
    HEAPMGR_TOTALFREESIZE += ((Header_Custom*)tmp)->size;

    /* Using the default system heap API */
    Memory_free(HeapTrack_Handle_upCast(stackHeap), (Ptr)tmp, ((Header_Custom*)tmp)->size);

    /* Restore the Key */
    Hwi_restore(hwikey);
  }
}

/**
 * @brief   return statistic on the Heap:
 *           - heap size
 *           - total free size
 *           - biggest free buffer allocatable
 * @param   stats - pointer to memory to write the information into.
 */
void HEAPMGR_GETSTATS(ICall_heapStats_t *stats)
{
  if (stats == NULL)
  {
    return;
  }

  HEAPMGR_LOCK();
  HeapTrack_getStats(stackHeap, (Memory_Stats *) stats);
  if (stats->largestFreeSize > HEAPMGR_OVERHEAD)
  {
    stats->largestFreeSize -= HEAPMGR_OVERHEAD;
  }
  else
  {
    stats->largestFreeSize = 0;
  }
  HEAPMGR_UNLOCK();
}

/**
 * @brief   Implementation of the limited allocator functionality.
 * The allocation is done only if at least HEAPMGR_FREE_SAFE_LIMIT bytes
 * remain available after the allocation.
 * @param   size - number of bytes to allocate from the heap.
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *HEAPMGR_MALLOC_LIMITED(uint32_t size)
{
    if((HEAPMGR_TOTALFREESIZE - size) > HEAPMGR_FREE_SAFE_LIMIT)
    {
      return(HEAPMGR_MALLOC(size));
    }
    else
    {
      return(NULL);
    }
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
UArg myHeapTrackInitFxn(UArg arg)
{
    HEAPMGR_INIT();
    return(0xBABE);
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
Ptr myHeapTrackAllocFxn(UArg arg,SizeT size ,SizeT alignement)
{
   return(HEAPMGR_MALLOC(size));
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
void myHeapTrackFreeFxn(UArg arg, Ptr myPtr, SizeT size)
{
  HEAPMGR_FREE(myPtr);
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
void myHeapTrackGetStatsFxn(UArg arg, Memory_Stats * stats)
{
  HEAPMGR_GETSTATS((ICall_heapStats_t *)stats);
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
Bool myHeapTrackIsBlockingFxn(UArg arg)
{
    return(TRUE);
}

/*********************************************************************
*********************************************************************/
