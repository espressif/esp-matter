/******************************************************************************

 @file  rtos_heaposal.h

 @brief osal heap management, modified to be used by TI-RTOS HeapCallback module..

        This file should be included in .c file to generate a heap memory
        management module with its own function names, heap size and, platform
        specific mutex implementation, etc. Note that although the idea of this
        file is the analogy of C++ template or ADA generic, this file can be
        included only once in a C file due to use of fixed type names and macro
        names.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include <string.h>
#include <stdint.h>
#ifndef FREERTOS
#include <ti/sysbios/heaps/HeapCallback.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/cfg/global.h>
#endif
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

#ifndef HEAPMGR_GETMETRICS
#define HEAPMGR_GETMETRICS heapmgrGetMetrics
#endif

#ifndef HEAPMGR_SANITY_CHECK
#define HEAPMGR_SANITY_CHECK heapmgrSanityCheck
#endif

#ifndef HEAPMGR_PREFIXED
#define HEAPMGR_PREFIXED(_name) heapmgr ## _name
#endif

/* macros to lock and unlock a mutex to synchronize tasks using this heap */
#ifndef HEAPMGR_LOCK
#define HEAPMGR_LOCK()
#endif

#ifndef HEAPMGR_UNLOCK
#define HEAPMGR_UNLOCK()
#endif

/* macro for implementation specific initialization routine */
#ifndef HEAPMGR_IMPL_INIT
#define HEAPMGR_IMPL_INIT()
#endif

/* macros for string operation */
#ifndef HEAPMGR_MEMSET
#define HEAPMGR_MEMSET(_d,_v,_c) memset(_d,_v,_c)
#endif

/* macro for debug assert */
#ifndef HEAPMGR_ASSERT
#define HEAPMGR_ASSERT(_exp)
#endif

/* constant value for heap size */
#undef AUTOHEAPSIZE
#ifndef HEAPMGR_SIZE
  #define HEAPMGR_SIZE 3072
#elif defined(HEAPMGR_SIZE) && (HEAPMGR_SIZE == 0)
  #undef HEAPMGR_SIZE
  #define AUTOHEAPSIZE
#endif
/* Minimum wasted bytes to justify splitting a block before allocation */
#ifndef HEAPMGR_MIN_BLKSZ
#define HEAPMGR_MIN_BLKSZ    4
#endif

/* Profiling memory allocations showed that a significant % of very high
 * frequency allocations/frees are for block sizes less than or equal to 16.
 */
#ifndef HEAPMGR_SMALL_BLKSZ
#define HEAPMGR_SMALL_BLKSZ  16
#endif

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

#ifdef HEAPMGR_PROFILER
#ifndef osal_memset
#define osal_memset memset
#endif
#define HEAPMGR_INIT_X       'X'
#define HEAPMGR_ALOC         'A'
#define HEAPMGR_REIN         'F'
#endif

/* Namespace */
#define HEAPMGR_FF1 HEAPMGR_PREFIXED(Ff1)
#define HEAPMGR_FF2 HEAPMGR_PREFIXED(Ff2)
#define HEAPMGR_HEAPSTORE HEAPMGR_PREFIXED(HeapStore)
#define HEAPMGR_HEAP HEAPMGR_PREFIXED(Heap)
#ifdef HEAPMGR_METRICS
#define HEAPMGR_BLKMAX HEAPMGR_PREFIXED(BlkMax)
#define HEAPMGR_BLKCNT HEAPMGR_PREFIXED(BlkCnt)
#define HEAPMGR_BLKFREE HEAPMGR_PREFIXED(BlkFree)
#define HEAPMGR_MEMALO HEAPMGR_PREFIXED(MemAlo)
#define HEAPMGR_MEMMAX HEAPMGR_PREFIXED(MemMax)
#define HEAPMGR_MEMUB  HEAPMGR_PREFIXED(MemUB)
#define HEAPMGR_MEMFAIL HEAPMGR_PREFIXED(MemFail)
#endif
#define HEAPMGR_TOTALFREESIZE HEAPMGR_PREFIXED(MemFreeTotal)

typedef uint8_t  hmU8_t;
typedef uint16_t hmU16_t;
typedef uint32_t hmU32_t;

/** @internal memory block header */
typedef hmU32_t  heapmgrHdr_t;

/** @internal flag bit in memory block header to indicate that the block is allocated */
#define HEAPMGR_IN_USE   0x80000000u

/* This number sets the size of the small-block bucket. Although profiling
 * shows max simultaneous alloc of 16x18, timing without profiling overhead
 * shows that the best worst case is achieved with the following.
 */
#define SMALLBLKHEAP        232

/* To maintain data alignment of the pointer returned, reserve the greater
 * space for the memory block header. */
#if defined _M_IX86 || defined _M_IA64
#define HEAPMGR_ALIGN_SIZE 1
#elif defined __ICC430__
#define HEAPMGR_ALIGN_SIZE 2
#elif defined __ICCARM__
#define HEAPMGR_ALIGN_SIZE 4
#elif defined __GNUC__ && defined i386
#define HEAPMGR_ALIGN_SIZE 4
#elif defined __GNUC__ && defined __arm__
#define HEAPMGR_ALIGN_SIZE 4
#elif defined (ccs)  || (rvmdk)
#define HEAPMGR_ALIGN_SIZE 4
#elif defined __TI_COMPILER_VERSION__ && defined __TI_ARM__
#define HEAPMGR_ALIGN_SIZE 4
#else
#error "Unsupported platform or compiler"
#endif

#define HDRSZ 4
typedef hmU32_t heapmgrAlign_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static heapmgrHdr_t *HEAPMGR_FF1;  // First free block in the small-block bucket.
static heapmgrHdr_t *HEAPMGR_FF2;  // First free block after the small-block bucket.

#ifdef HEAPMGR_METRICS
hmU32_t HEAPMGR_BLKMAX  = 0; // Max cnt of all blocks ever seen at once.
hmU32_t HEAPMGR_BLKCNT  = 0; // Current cnt of all blocks.
hmU32_t HEAPMGR_BLKFREE = 0; // Current cnt of free blocks.
hmU32_t HEAPMGR_MEMALO  = 0; // Current total memory allocated.
hmU32_t HEAPMGR_MEMMAX  = 0; // Max total memory ever allocated at once.
hmU32_t HEAPMGR_MEMUB   = 0; // Upper-bound of memory usage
hmU16_t HEAPMGR_MEMFAIL = 0; // Memory allocation failure count
#endif // HEAPMGR_METRICS
hmU32_t HEAPMGR_TOTALFREESIZE = 0;

#ifdef HEAPMGR_PROFILER
#define HEAPMGR_PROMAX  8
/* The profiling buckets must differ by at least HEAPMGR_MIN_BLKSZ; the
 * last bucket must equal the max alloc size. Set the bucket sizes to
 * whatever sizes necessary to show how your application is using memory.
 */
static hmU16_t proCnt[HEAPMGR_PROMAX] =
{
  HEAPMGR_SMALL_BLKSZ, 48, 112, 176, 192, 224, 256, 65535
};
static hmU16_t proCur[HEAPMGR_PROMAX] = { 0 };
static hmU16_t proMax[HEAPMGR_PROMAX] = { 0 };
static hmU16_t proTot[HEAPMGR_PROMAX] = { 0 };
static hmU16_t proSmallBlkMiss;
#endif // HEAPMGR_PROFILER

/** @intenral Memory Allocation Heap. */
#ifdef AUTOHEAPSIZE
static heapmgrAlign_t *HEAPMGR_HEAPSTORE = NULL;
static hmU8_t         *HEAPMGR_HEAP = NULL;

hmU32_t HEAPMGR_SIZE;

#else // !AUTOHEAPSIZE
static heapmgrAlign_t  HEAPMGR_HEAPSTORE[ HEAPMGR_SIZE/sizeof(heapmgrAlign_t) ];
static hmU8_t         *HEAPMGR_HEAP = (hmU8_t *)&HEAPMGR_HEAPSTORE;
#endif // AUTOHEAPSIZE
static hmU8_t heapInitialize = 0;

#define  HEAPMGR_OVERHEAD (sizeof(heapmgrHdr_t) + sizeof(heapmgrAlign_t))


/**
 * @brief   Initialize the heap memory management system.
 */
void HEAPMGR_INIT(void)
{
  heapmgrHdr_t *tmp;

  if (heapInitialize == 1)
  {
    // Initialization already done. Only once initialization allowed.
    return;
  }
  heapInitialize = 1;
#ifdef AUTOHEAPSIZE
  extern uint32_t heapStart;
  extern uint32_t heapEnd;

  // Need to align the Heap buffer on a 32-bits boundary.
  HEAPMGR_HEAPSTORE = (heapmgrAlign_t *) (((uint32_t) &heapStart + sizeof(uint32_t)-1) & ~(sizeof(uint32_t)-1));
  HEAPMGR_SIZE      = (((uint32_t)&heapEnd) - ((uint32_t)HEAPMGR_HEAPSTORE)); // both values are word aligned
  HEAPMGR_HEAP      = (hmU8_t *)HEAPMGR_HEAPSTORE;
#endif // AUTOHEAPSIZE

  for (tmp = (heapmgrHdr_t *)HEAPMGR_HEAPSTORE;
       tmp < (heapmgrHdr_t *)((uint32)HEAPMGR_HEAPSTORE+HEAPMGR_SIZE);
       tmp++)
  {
    *tmp = 0;
  }

  /* Implementation specific initialization */
  HEAPMGR_IMPL_INIT();

#ifdef HEAPMGR_PROFILER
  (void)HEAPMGR_MEMSET(HEAPMGR_HEAP, HEAPMGR_INIT_X, (HEAPMGR_SIZE/HDRSZ)*HDRSZ);
#endif // HEAPMGR_PROFILER

  // Setup a NULL block at the end of the heap for fast comparisons with zero.
  tmp = (heapmgrHdr_t *)(HEAPMGR_HEAP + (HEAPMGR_SIZE / HDRSZ)*HDRSZ - HDRSZ);
  *tmp = 0;

  // Setup a small-block bucket.
  tmp = (heapmgrHdr_t *)HEAPMGR_HEAP;
  *tmp = (SMALLBLKHEAP / HDRSZ) * HDRSZ;

  // Setup the wilderness.
  tmp = (heapmgrHdr_t *)(HEAPMGR_HEAP + (SMALLBLKHEAP / HDRSZ)*HDRSZ);
  *tmp = ((HEAPMGR_SIZE/HDRSZ) * HDRSZ) - (SMALLBLKHEAP/HDRSZ)*HDRSZ - HDRSZ;

  // Setup a NULL block that is never freed so that the small-block bucket
  // is never coalesced with the wilderness.
  HEAPMGR_FF1 = tmp;
  HEAPMGR_FF2 = HEAPMGR_MALLOC(0);
  HEAPMGR_FF1 = (heapmgrHdr_t *)HEAPMGR_HEAP;

#ifdef HEAPMGR_METRICS
  /* Start with the small-block bucket and the wilderness - don't count the
   * end-of-heap NULL block nor the end-of-small-block NULL block.
   */
  HEAPMGR_BLKCNT = HEAPMGR_BLKFREE = 2;
  HEAPMGR_MEMFAIL = 0;
#endif // HEAPMGR_METRICS

  // free size is the total size minus the NULL block define for FF2.
  HEAPMGR_TOTALFREESIZE = HEAPMGR_SIZE - HDRSZ;
}

/**
 * @brief   Implementation of the allocator functionality.
 * @param   size - number of bytes to allocate from the heap.
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *HEAPMGR_MALLOC(hmU32_t size)
{
  heapmgrHdr_t *prev = NULL;
  heapmgrHdr_t *hdr;
  heapmgrHdr_t tmp;
  hmU8_t coal = 0;

  HEAPMGR_ASSERT(size);

  size += HDRSZ;

  // Calculate required bytes to add to 'size' to align to heapmgrAlign_t.
  if (sizeof(heapmgrAlign_t) == 2)
  {
    size += (size & 0x01);
  }
  else if (sizeof(heapmgrAlign_t) != 1)
  {
    const hmU8_t mod = size % sizeof(heapmgrAlign_t);

    if (mod != 0)
    {
      size += (sizeof(heapmgrAlign_t) - mod);
    }
  }

  HEAPMGR_LOCK();  /* Lock the mutex */

  // Smaller allocations are first attempted in the small-block bucket.
  if (size <= HEAPMGR_SMALL_BLKSZ)
  {
    hdr = HEAPMGR_FF1;
  }
  else
  {
    hdr = HEAPMGR_FF2;
  }
  tmp = *hdr;

  do
  {
    if (tmp & HEAPMGR_IN_USE)
    {
      tmp ^= HEAPMGR_IN_USE;
      coal = 0;
    }
    else
    {
      if (coal != 0)
      {
#ifdef HEAPMGR_METRICS
        HEAPMGR_BLKCNT--;
        HEAPMGR_BLKFREE--;
#endif // HEAPMGR_METRICS

        *prev += *hdr;

        if (*prev >= size)
        {
          hdr = prev;
          tmp = *hdr;
          break;
        }
      }
      else
      {
        if (tmp >= size)
        {
          break;
        }

        coal = 1;
        prev = hdr;
      }
    }

    hdr = (heapmgrHdr_t *)((hmU8_t *)hdr + tmp);

    tmp = *hdr;
    if (tmp == 0)
    {
      hdr = NULL;
      break;
    }


  }
  while (1);

  if (hdr == NULL)
  {
#ifdef HEAPMGR_METRICS
    HEAPMGR_MEMFAIL++;
#endif
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
  }
  else
  {
    tmp -= size;

    // Determine whether the threshold for splitting is met.
    if (tmp >= HEAPMGR_MIN_BLKSZ)
    {
      // Split the block before allocating it.
      heapmgrHdr_t *next = (heapmgrHdr_t *)((hmU8_t *)hdr + size);
      *next = tmp;
      *hdr = (size | HEAPMGR_IN_USE);

#ifdef HEAPMGR_METRICS
      HEAPMGR_BLKCNT++;
      if (HEAPMGR_BLKMAX < HEAPMGR_BLKCNT)
      {
        HEAPMGR_BLKMAX = HEAPMGR_BLKCNT;
      }
      HEAPMGR_MEMALO += size;
      {
        hmU32_t ub = (hmU32_t)(unsigned)(((hmU8_t *)hdr + size) - HEAPMGR_HEAP);
        if (HEAPMGR_MEMUB < ub)
        {
          HEAPMGR_MEMUB = ub;
        }
      }
#endif // HEAPMGR_METRICS
      HEAPMGR_TOTALFREESIZE -= size;
    }
    else
    {
#ifdef HEAPMGR_METRICS
      HEAPMGR_MEMALO += (hmU32_t) *hdr;
      HEAPMGR_BLKFREE--;
      {
        hmU32_t ub = (hmU32_t)(unsigned)(((hmU8_t *)hdr + *hdr) - HEAPMGR_HEAP);
        if (HEAPMGR_MEMUB < ub)
        {
          HEAPMGR_MEMUB = ub;
        }
      }
#endif // HEAPMGR_METRICS
      HEAPMGR_TOTALFREESIZE -= (hmU32_t) *hdr;

      *hdr |= HEAPMGR_IN_USE;
    }

#ifdef HEAPMGR_METRICS
    if (HEAPMGR_MEMMAX < HEAPMGR_MEMALO)
    {
      HEAPMGR_MEMMAX = HEAPMGR_MEMALO;
    }
#endif // HEAPMGR_METRICS

#ifdef HEAPMGR_PROFILER
    {
      hmU8_t idx;
      size = *hdr ^ HEAPMGR_IN_USE;

      for (idx = 0; idx < HEAPMGR_PROMAX; idx++)
      {
        if (size <= proCnt[idx])
        {
          break;
        }
      }
      proCur[idx]++;
      if (proMax[idx] < proCur[idx])
      {
        proMax[idx] = proCur[idx];
      }
      proTot[idx]++;
    }
#endif // HEAPMGR_PROFILER

    hdr = (heapmgrHdr_t *) ((hmU8_t *) hdr + HDRSZ);

#ifdef HEAPMGR_PROFILER
    (void)osal_memset((hmU8_t *)hdr, HEAPMGR_ALOC, (size - HDRSZ));

    /* A small-block could not be allocated in the small-block bucket.
     * When this occurs significantly frequently, increase the size of the
     * bucket in order to restore better worst case run times. Set the first
     * profiling bucket size in proCnt[] to the small-block bucket size and
     * divide proSmallBlkMiss by the corresponding proTot[] size to get % miss.
     * Best worst case time on TrasmitApp was achieved at a 0-15% miss rate
     * during steady state Tx load, 0% during idle and steady state Rx load.
     */
    if ((size <= HEAPMGR_SMALL_BLKSZ) && (hdr > HEAPMGR_FF2))
    {
      proSmallBlkMiss++;
    }
#endif // HEAPMGR_PROFILER
  }

  HEAPMGR_UNLOCK();  /* unlock the mutex */

  return((void *)hdr);
}

/**
 * @brief         Re-allocates a memory block of the requested
 *                size.
 *                Note that this function always re-allocates
 *                even if the requested size is lower than the
 *                original.
 * @param ptr     pointer to the existing memory block.
 * @param size    size in bytes of the memory block to
 *                re-allocate
 *
 * @return void*  pointer to the memory block of newly
 *                re-allocation or if reallocation fails
 *                returns the pointer to the original memory
 *                block.
 */
void *HEAPMGR_REALLOC(void* ptr, hmU32_t size)
{
  void *newPtr;
  heapmgrHdr_t *currHdr;
  hmU32_t origSize;

  HEAPMGR_ASSERT(size);

  currHdr = (heapmgrHdr_t *)((hmU8_t *)ptr - HDRSZ);

  origSize = (hmU32_t)((*currHdr) & (~HEAPMGR_IN_USE));

  if (origSize == size)
  {
    return(ptr);
  }

  newPtr = HEAPMGR_MALLOC(size);

  if (newPtr)
  {
    hmU32_t n = origSize < size ? origSize : size;
    memcpy(newPtr, ptr, n);
    HEAPMGR_FREE(ptr);
    return(newPtr);
  }

  return(NULL);
}


/**
 * @brief   Implementation of the de-allocator functionality.
 * @param   ptr - pointer to the memory to free.
 */
void HEAPMGR_FREE(void *ptr)
{
  heapmgrHdr_t *currHdr;

  HEAPMGR_LOCK();

  HEAPMGR_ASSERT(((hmU8_t *)ptr >= (hmU8_t *)HEAPMGR_HEAPSTORE) &&
                 ((hmU8_t *)ptr < (hmU8_t *)HEAPMGR_HEAPSTORE+(HEAPMGR_SIZE/HDRSZ)*HDRSZ));

  currHdr = (heapmgrHdr_t *)((hmU8_t *)ptr - HDRSZ);

  HEAPMGR_ASSERT(*currHdr & HEAPMGR_IN_USE);

  *currHdr &= ~HEAPMGR_IN_USE;

#ifdef HEAPMGR_PROFILER
  {
    hmU32_t size = *currHdr;
    hmU8_t idx;

    for (idx = 0; idx < HEAPMGR_PROMAX; idx++)
    {
      if (size <= proCnt[idx])
      {
        break;
      }
    }

    proCur[idx]--;
  }
#endif

#ifdef HEAPMGR_METRICS
  HEAPMGR_MEMALO -= (hmU32_t) *currHdr;
  HEAPMGR_BLKFREE++;
#endif // HEAPMGR_METRICS

  HEAPMGR_TOTALFREESIZE += (hmU32_t) *currHdr;

  if (HEAPMGR_FF1 > currHdr)
  {
    HEAPMGR_FF1 = currHdr;
  }

#ifdef HEAPMGR_PROFILER
  (void)HEAPMGR_MEMSET((hmU8_t *)currHdr+HDRSZ, HEAPMGR_REIN, (*currHdr - HDRSZ));
#endif // HEAPMGR_PROFILER

  HEAPMGR_UNLOCK();
}

hmU32_t HEAPMGR_GETLARGESTFREEBUF(void)
{
  heapmgrHdr_t *prev = NULL;
  heapmgrHdr_t *hdr;
  heapmgrHdr_t len;
  hmU8_t coal = 0;
  hmU32_t freeSize = 0;
  hmU32_t maxFreeSize = 0;

  HEAPMGR_LOCK();  /* Lock the mutex */

  // small-block bucket is excluded from the search.
  hdr = HEAPMGR_FF2;

  do
  {
    len = *hdr;
    if (len == 0)
    {
      break;
    }

    if (len & HEAPMGR_IN_USE)
    {
      len ^= HEAPMGR_IN_USE;
      coal = 0;
      if (maxFreeSize < freeSize)
      {
        maxFreeSize = freeSize;
      }
    }
    else
    {
      if (coal != 0)
      {
        *prev += len;
        freeSize += len;
      }
      else
      {
        freeSize = len;
        coal = 1;
        prev = hdr;
      }
    }

    hdr = (heapmgrHdr_t *)((hmU8_t *)hdr + len);
  }
  while (1);

  if (freeSize > maxFreeSize)
  {
    maxFreeSize = freeSize;
  }
  HEAPMGR_UNLOCK();  /* unlock the mutex */

  return(maxFreeSize);
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
  stats->totalSize =       HEAPMGR_SIZE;
  stats->totalFreeSize =   HEAPMGR_TOTALFREESIZE;
  stats->largestFreeSize = HEAPMGR_GETLARGESTFREEBUF();

  if (stats->largestFreeSize > HEAPMGR_OVERHEAD)
  {
    stats->largestFreeSize -= HEAPMGR_OVERHEAD;
  }
  else
  {
    stats->largestFreeSize = 0;
  }
}

/**
 * @brief   Implementation of the limited allocator functionality.
 * The allocation is done only if at least HEAPMGR_FREE_SAFE_LIMIT bytes
 * remain available after the allocation.
 * @param   size - number of bytes to allocate from the heap.
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *HEAPMGR_MALLOC_LIMITED(hmU32_t size)
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

#ifdef HEAPMGR_METRICS
/**
 * @brief   obtain heap usage metrics
 * @param   pBlkMax   pointer to a variable to store max cnt of all blocks ever seen at once
 * @param   pBlkCnt   pointer to a variable to store current cnt of all blocks
 * @param   pBlkFree  pointer to a variable to store current cnt of free blocks
 * @param   pMemAlo   pointer to a variable to store current total memory allocated
 * @param   pMemMax   pointer to a variable to store max total memory ever allocated at once
 * @param   pMemUB    pointer to a variable to store the upper bound of memory usage
 */
void HEAPMGR_GETMETRICS(hmU32_t *pBlkMax,
                        hmU32_t *pBlkCnt,
                        hmU32_t *pBlkFree,
                        hmU32_t *pMemAlo,
                        hmU32_t *pMemMax,
                        hmU32_t *pMemUB)
{
  HEAPMGR_LOCK();
  *pBlkMax = HEAPMGR_BLKMAX;
  *pBlkCnt = HEAPMGR_BLKCNT;
  *pBlkFree = HEAPMGR_BLKFREE;
  *pMemAlo = HEAPMGR_MEMALO;
  *pMemMax = HEAPMGR_MEMMAX;
  *pMemUB = HEAPMGR_MEMUB;
  HEAPMGR_UNLOCK();
}

/**
 * @brief   Sanity checks heap
 * @return  0 when heap is OK. Non-zero, otherwise.
 */
int HEAPMGR_SANITY_CHECK(void)
{
  heapmgrHdr_t *hdr;
  heapmgrHdr_t tmp;
  int result = 0;

  HEAPMGR_LOCK();

  hdr = HEAPMGR_FF1;
  for (;;)
  {
    tmp = *hdr;
    tmp &= ~HEAPMGR_IN_USE;
    if (tmp == 0)
    {
      if ((hmU8_t *) hdr != (HEAPMGR_HEAP + (HEAPMGR_SIZE / HDRSZ)*HDRSZ - HDRSZ))
      {
        result = 1;
      }
      break;
    }
    hdr = (heapmgrHdr_t *)((hmU8_t *)hdr + tmp);
    if ((hmU8_t *) hdr >= (HEAPMGR_HEAP + (HEAPMGR_SIZE / HDRSZ) *HDRSZ))
    {
      result = 2;
      break;
    }
  }

  HEAPMGR_UNLOCK();
  return(result);
}
#endif // HEAPMGR_METRICS

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
UArg osalHeapInitFxn(UArg arg)
{
    HEAPMGR_INIT();
    return(0xBABE);
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
Ptr osalHeapAllocFxn(UArg arg,SizeT size ,SizeT alignement)
{
   return(HEAPMGR_MALLOC(size));
}


/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
void osalHeapFreeFxn(UArg arg, Ptr myPtr, SizeT size)
{
  HEAPMGR_FREE(myPtr);
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
void osalHeapGetStatsFxn(UArg arg, Memory_Stats * stats)
{
  HEAPMGR_GETSTATS((ICall_heapStats_t*)&stats);
}

/**
 * @brief   HeapCallback function pointer:
 *          used in conjonction with the HeapCallback Module in TI-RTOS
 */
Bool osalHeapIsBlockingFxn(UArg arg)
{
    return(TRUE);
}


/*********************************************************************
*********************************************************************/
