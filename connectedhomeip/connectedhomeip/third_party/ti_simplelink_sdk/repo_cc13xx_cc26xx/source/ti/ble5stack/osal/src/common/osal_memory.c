/******************************************************************************

 @file  osal_memory.c

 @brief OSAL Heap Memory management functions. There is an Application Note that
        should be read before studying and/or modifying this module:
        SWRA204 "Heap Memory Management"

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
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

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include <stdlib.h>

#include "comdef.h"
#include "osal.h"
#include "osal_memory.h"
#include "onboard.h"
#include "hal_mcu.h"
#include "hal_assert.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

#define OSALMEM_IN_USE             0x8000
#if (MAXMEMHEAP & OSALMEM_IN_USE)
#error MAXMEMHEAP is too big to manage!
#endif

#define OSALMEM_HDRSZ              sizeof(osalMemHdr_t)

// Round a value up to the ceiling of OSALMEM_HDRSZ for critical dependencies on even multiples.
#define OSALMEM_ROUND(X)       ((((X) + OSALMEM_HDRSZ - 1) / OSALMEM_HDRSZ) * OSALMEM_HDRSZ)

/* Minimum wasted bytes to justify splitting a block before allocation.
 * Adjust accordingly to attempt to balance the tradeoff of wasted space and runtime throughput
 * spent splitting blocks into sizes that may not be practically usable when sandwiched between
 * two blocks in use (and thereby not able to be coalesced.)
 * Ensure that this size is an even multiple of OSALMEM_HDRSZ.
 */
#if !defined OSALMEM_MIN_BLKSZ
#define OSALMEM_MIN_BLKSZ         (OSALMEM_ROUND((OSALMEM_HDRSZ * 2)))
#endif

#if !defined OSALMEM_LL_BLKSZ
#if defined NONWK
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(6) + (1 * OSALMEM_HDRSZ))
#else
/*
 * Profiling the sample apps with default settings shows the following long-lived allocations
 * which should live at the bottom of the small-block bucket so that they are never iterated over
 * by osal_mem_alloc/free(), nor ever considered for coalescing, etc. This saves significant
 * run-time throughput (on 8051 SOC if not also MSP). This is dynamic "dead space" and is not
 * available to the small-block bucket heap.
 *
 * Adjust this size accordingly to accommodate application-specific changes including changing the
 * size of long-lived objects profiled by sample apps and long-lived objects added by application.
 */
#if defined ZCL_KEY_ESTABLISH_OLD // CBKE no longer uses long lived memory allocations.
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(526) + (32 * OSALMEM_HDRSZ))
#elif defined TC_LINKKEY_JOIN
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(454) + (21 * OSALMEM_HDRSZ))
#elif ((defined SECURE) && (SECURE != 0))
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(418) + (19 * OSALMEM_HDRSZ))
#else
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(417) + (19 * OSALMEM_HDRSZ))
#endif
#endif
#endif

/* Adjust accordingly to attempt to accommodate the block sizes of the vast majority of
 * very high frequency allocations/frees by profiling the system runtime.
 * This default of 16 accommodates the OSAL timers block, osalTimerRec_t, and many others.
 * Ensure that this size is an even multiple of OSALMEM_MIN_BLKSZ for run-time efficiency.
 */
#if !defined OSALMEM_SMALL_BLKSZ
#define OSALMEM_SMALL_BLKSZ       (OSALMEM_ROUND(16))
#endif
#if !defined OSALMEM_SMALL_BLKCNT
#define OSALMEM_SMALL_BLKCNT       8
#endif

/*
 * These numbers setup the size of the small-block bucket which is reserved at the front of the
 * heap for allocations of OSALMEM_SMALL_BLKSZ or smaller.
 */

// Size of the heap bucket reserved for small block-sized allocations.
// Adjust accordingly to attempt to accommodate the vast majority of very high frequency operations.
#define OSALMEM_SMALLBLK_BUCKET  ((OSALMEM_SMALL_BLKSZ * OSALMEM_SMALL_BLKCNT) + OSALMEM_LL_BLKSZ)
// Index of the first available osalMemHdr_t after the small-block heap which will be set in-use in
// order to prevent the small-block bucket from being coalesced with the wilderness.
#define OSALMEM_SMALLBLK_HDRCNT   (OSALMEM_SMALLBLK_BUCKET / OSALMEM_HDRSZ)
// Index of the first available osalMemHdr_t after the small-block heap which will be set in-use in
#define OSALMEM_BIGBLK_IDX        (OSALMEM_SMALLBLK_HDRCNT + 1)
// The size of the wilderness after losing the small-block heap, the wasted header to block the
// small-block heap from being coalesced, and the wasted header to mark the end of the heap.
#define OSALMEM_BIGBLK_SZ         (MAXMEMHEAP - OSALMEM_SMALLBLK_BUCKET - OSALMEM_HDRSZ*2)
// Index of the last available osalMemHdr_t at the end of the heap which will be set to zero for
// fast comparisons with zero to determine the end of the heap.
#define OSALMEM_LASTBLK_IDX      ((MAXMEMHEAP / OSALMEM_HDRSZ) - 1)

// For information about memory profiling, refer to SWRA204 "Heap Memory Management", section 1.5.
#if !defined OSALMEM_PROFILER
#define OSALMEM_PROFILER           FALSE  // Enable/disable the memory usage profiling buckets.
#endif
#if !defined OSALMEM_PROFILER_LL
#define OSALMEM_PROFILER_LL        FALSE  // Special profiling of the Long-Lived bucket.
#endif

#if OSALMEM_PROFILER
#define OSALMEM_INIT              'X'
#define OSALMEM_ALOC              'A'
#define OSALMEM_REIN              'F'
#endif

/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */

typedef struct {
  // The 15 LSB's of 'val' indicate the total item size, including the header, in 8-bit bytes.
  unsigned len : 15;
  // The 1 MSB of 'val' is used as a boolean to indicate in-use or freed.
  unsigned inUse : 1;
} osalMemHdrHdr_t;

typedef union {
  /* Dummy variable so compiler forces structure to alignment of largest element while not wasting
   * space on targets when the halDataAlign_t is smaller than a UINT16.
   */
  halDataAlign_t alignDummy;
  uint16 val;
  osalMemHdrHdr_t hdr;
} osalMemHdr_t;

/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

#if !defined ( ZBIT ) && defined ewarm
static __no_init osalMemHdr_t theHeap[MAXMEMHEAP / OSALMEM_HDRSZ];
static __no_init osalMemHdr_t *ff1;  // First free block in the small-block bucket.
#else
static osalMemHdr_t theHeap[MAXMEMHEAP / OSALMEM_HDRSZ];
static osalMemHdr_t *ff1;  // First free block in the small-block bucket.
#endif

static uint8 osalMemStat;            // Discrete status flags: 0x01 = kicked.

#if OSALMEM_METRICS
static uint16 blkMax;  // Max cnt of all blocks ever seen at once.
static uint16 blkCnt;  // Current cnt of all blocks.
static uint16 blkFree; // Current cnt of free blocks.
static uint16 memAlo;  // Current total memory allocated.
static uint16 memMax;  // Max total memory ever allocated at once.
#endif

#if OSALMEM_PROFILER
#define OSALMEM_PROMAX  8
/* The profiling buckets must differ by at least OSALMEM_MIN_BLKSZ; the
 * last bucket must equal the max alloc size. Set the bucket sizes to
 * whatever sizes necessary to show how your application is using memory.
 */
static uint16 proCnt[OSALMEM_PROMAX] = {
OSALMEM_SMALL_BLKSZ, 48, 112, 176, 192, 224, 256, 65535 };
static uint16 proCur[OSALMEM_PROMAX+1] = { 0 };
static uint16 proMax[OSALMEM_PROMAX+1] = { 0 };
static uint16 proTot[OSALMEM_PROMAX+1] = { 0 };
static uint16 proSmallBlkMiss;
#endif

/* ------------------------------------------------------------------------------------------------
 *                                           Global Variables
 * ------------------------------------------------------------------------------------------------
 */

#ifdef DPRINTF_HEAPTRACE
extern int dprintf(const char *fmt, ...);
#endif /* DPRINTF_HEAPTRACE */

/**************************************************************************************************
 * @fn          osal_mem_init
 *
 * @brief       This function is the OSAL heap memory management initialization callback.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void osal_mem_init(void)
{
  HAL_ASSERT(((OSALMEM_MIN_BLKSZ % OSALMEM_HDRSZ) == 0));
  HAL_ASSERT(((OSALMEM_LL_BLKSZ % OSALMEM_HDRSZ) == 0));
  HAL_ASSERT(((OSALMEM_SMALL_BLKSZ % OSALMEM_HDRSZ) == 0));

#if OSALMEM_PROFILER
  (void)osal_memset(theHeap, OSALMEM_INIT, MAXMEMHEAP);
#endif

  // Setup a NULL block at the end of the heap for fast comparisons with zero.
  theHeap[OSALMEM_LASTBLK_IDX].val = 0;

  // Setup the small-block bucket.
  ff1 = theHeap;
  ff1->val = OSALMEM_SMALLBLK_BUCKET;                   // Set 'len' & clear 'inUse' field.
  // Set 'len' & 'inUse' fields - this is a 'zero data bytes' lifetime allocation to block the
  // small-block bucket from ever being coalesced with the wilderness.
  theHeap[OSALMEM_SMALLBLK_HDRCNT].val = (OSALMEM_HDRSZ | OSALMEM_IN_USE);

  // Setup the wilderness.
  theHeap[OSALMEM_BIGBLK_IDX].val = OSALMEM_BIGBLK_SZ;  // Set 'len' & clear 'inUse' field.

#if ( OSALMEM_METRICS )
  /* Start with the small-block bucket and the wilderness - don't count the
   * end-of-heap NULL block nor the end-of-small-block NULL block.
   */
  blkCnt = blkFree = 2;
#endif
}

/**************************************************************************************************
 * @fn          osal_mem_kick
 *
 * @brief       This function is the OSAL task initialization callback.
 * @brief       Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 *              Invoke this once after all long-lived blocks have been allocated -
 *              presently at the end of osal_init_system().
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void osal_mem_kick(void)
{
  halIntState_t intState;
  osalMemHdr_t *tmp = osal_mem_alloc(1);

  HAL_ASSERT((tmp != NULL));
  HAL_ENTER_CRITICAL_SECTION(intState);  // Hold off interrupts.

  /* All long-lived allocations have filled the LL block reserved in the small-block bucket.
   * Set 'osalMemStat' so searching for memory in this bucket from here onward will only be done
   * for sizes meeting the OSALMEM_SMALL_BLKSZ criteria.
   */
  ff1 = tmp - 1;       // Set 'ff1' to point to the first available memory after the LL block.
  osal_mem_free(tmp);
  osalMemStat = 0x01;  // Set 'osalMemStat' after the free because it enables memory profiling.

  HAL_EXIT_CRITICAL_SECTION(intState);  // Re-enable interrupts.
}

/**************************************************************************************************
 * @fn          osal_mem_alloc
 *
 * @brief       This function implements the OSAL dynamic memory allocation functionality.
 *
 * input parameters
 *
 * @param size - the number of bytes to allocate from the HEAP.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
#ifdef DPRINTF_OSALHEAPTRACE
void *osal_mem_alloc_dbg( uint16 size, const char *fname, unsigned lnum )
#else /* DPRINTF_OSALHEAPTRACE */
void *osal_mem_alloc( uint16 size )
#endif /* DPRINTF_OSALHEAPTRACE */
{
  osalMemHdr_t *prev = NULL;
  osalMemHdr_t *hdr;
  halIntState_t intState;
  uint8 coal = 0;

  size += OSALMEM_HDRSZ;

  // Calculate required bytes to add to 'size' to align to halDataAlign_t.
  if ( sizeof( halDataAlign_t ) == 2 )
  {
    size += (size & 0x01);
  }
  else if ( sizeof( halDataAlign_t ) != 1 )
  {
    const uint8 mod = size % sizeof( halDataAlign_t );

    if ( mod != 0 )
    {
      size += (sizeof( halDataAlign_t ) - mod);
    }
  }

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Smaller allocations are first attempted in the small-block bucket, and all long-lived
  // allocations are channelled into the LL block reserved within this bucket.
  if ((osalMemStat == 0) || (size <= OSALMEM_SMALL_BLKSZ))
  {
    hdr = ff1;
  }
  else
  {
    hdr = (theHeap + OSALMEM_BIGBLK_IDX);
  }

  do
  {
    if ( hdr->hdr.inUse )
    {
      coal = 0;
    }
    else
    {
      if ( coal != 0 )
      {
#if ( OSALMEM_METRICS )
        blkCnt--;
        blkFree--;
#endif

        prev->hdr.len += hdr->hdr.len;

        if ( prev->hdr.len >= size )
        {
          hdr = prev;
          break;
        }
      }
      else
      {
        if ( hdr->hdr.len >= size )
        {
          break;
        }

        coal = 1;
        prev = hdr;
      }
    }

    hdr = (osalMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);

    if ( hdr->val == 0 )
    {
      hdr = NULL;
      break;
    }
  } while (1);

  if ( hdr != NULL )
  {
    uint16 tmp = hdr->hdr.len - size;

    // Determine whether the threshold for splitting is met.
    if ( tmp >= OSALMEM_MIN_BLKSZ )
    {
      // Split the block before allocating it.
      osalMemHdr_t *next = (osalMemHdr_t *)((uint8 *)hdr + size);
      next->val = tmp;                     // Set 'len' & clear 'inUse' field.
      hdr->val = (size | OSALMEM_IN_USE);  // Set 'len' & 'inUse' field.

#if ( OSALMEM_METRICS )
      blkCnt++;
      if ( blkMax < blkCnt )
      {
        blkMax = blkCnt;
      }
      memAlo += size;
#endif
    }
    else
    {
#if ( OSALMEM_METRICS )
      memAlo += hdr->hdr.len;
      blkFree--;
#endif

      hdr->hdr.inUse = TRUE;
    }

#if ( OSALMEM_METRICS )
    if ( memMax < memAlo )
    {
      memMax = memAlo;
    }
#endif

#if ( OSALMEM_PROFILER )
#if !OSALMEM_PROFILER_LL
    if (osalMemStat != 0)  // Don't profile until after the LL block is filled.
#endif
    {
      uint8 idx;

      for ( idx = 0; idx < OSALMEM_PROMAX; idx++ )
      {
        if ( hdr->hdr.len <= proCnt[idx] )
        {
          break;
        }
      }
      proCur[idx]++;
      if ( proMax[idx] < proCur[idx] )
      {
        proMax[idx] = proCur[idx];
      }
      proTot[idx]++;

      /* A small-block could not be allocated in the small-block bucket.
       * When this occurs significantly frequently, increase the size of the
       * bucket in order to restore better worst case run times. Set the first
       * profiling bucket size in proCnt[] to the small-block bucket size and
       * divide proSmallBlkMiss by the corresponding proTot[] size to get % miss.
       * Best worst case time on TrasmitApp was achieved at a 0-15% miss rate
       * during steady state Tx load, 0% during idle and steady state Rx load.
       */
      if ((hdr->hdr.len <= OSALMEM_SMALL_BLKSZ) && (hdr >= (theHeap + OSALMEM_BIGBLK_IDX)))
      {
        proSmallBlkMiss++;
      }
    }

    (void)osal_memset((uint8 *)(hdr+1), OSALMEM_ALOC, (hdr->hdr.len - OSALMEM_HDRSZ));
#endif

    if ((osalMemStat != 0) && (ff1 == hdr))
    {
      ff1 = (osalMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);
    }

    hdr++;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.

  HAL_ASSERT(((size_t)hdr % sizeof(halDataAlign_t)) == 0);

#ifdef DPRINTF_OSALHEAPTRACE
  dprintf("osal_mem_alloc(%u)->%lx:%s:%u\n", size, (unsigned) hdr, fname, lnum);
#endif /* DPRINTF_OSALHEAPTRACE */
  return (void *)hdr;
}

/**************************************************************************************************
 * @fn          osal_mem_free
 *
 * @brief       This function implements the OSAL dynamic memory de-allocation functionality.
 *
 * input parameters
 *
 * @param ptr - A valid pointer (i.e. a pointer returned by osal_mem_alloc()) to the memory to free.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
#ifdef DPRINTF_OSALHEAPTRACE
void osal_mem_free_dbg(void *ptr, const char *fname, unsigned lnum)
#else /* DPRINTF_OSALHEAPTRACE */
void osal_mem_free(void *ptr)
#endif /* DPRINTF_OSALHEAPTRACE */
{
  osalMemHdr_t *hdr = (osalMemHdr_t *)ptr - 1;
  halIntState_t intState;

#ifdef DPRINTF_OSALHEAPTRACE
  dprintf("osal_mem_free(%lx):%s:%u\n", (unsigned) ptr, fname, lnum);
#endif /* DPRINTF_OSALHEAPTRACE */

  HAL_ASSERT(((uint8 *)ptr >= (uint8 *)theHeap) && ((uint8 *)ptr < (uint8 *)theHeap+MAXMEMHEAP));
  HAL_ASSERT(hdr->hdr.inUse);

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.
  hdr->hdr.inUse = FALSE;

  if (ff1 > hdr)
  {
    ff1 = hdr;
  }

#if OSALMEM_PROFILER
#if !OSALMEM_PROFILER_LL
  if (osalMemStat != 0)  // Don't profile until after the LL block is filled.
#endif
  {
    uint8 idx;

    for (idx = 0; idx < OSALMEM_PROMAX; idx++)
    {
      if (hdr->hdr.len <= proCnt[idx])
      {
        break;
      }
    }

    proCur[idx]--;
  }

  (void)osal_memset((uint8 *)(hdr+1), OSALMEM_REIN, (hdr->hdr.len - OSALMEM_HDRSZ) );
#endif
#if OSALMEM_METRICS
  memAlo -= hdr->hdr.len;
  blkFree++;
#endif

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
}

#if OSALMEM_METRICS
/*********************************************************************
 * @fn      osal_heap_block_max
 *
 * @brief   Return the maximum number of blocks ever allocated at once.
 *
 * @param   none
 *
 * @return  Maximum number of blocks ever allocated at once.
 */
uint16 osal_heap_block_max( void )
{
  return blkMax;
}

/*********************************************************************
 * @fn      osal_heap_block_cnt
 *
 * @brief   Return the current number of blocks now allocated.
 *
 * @param   none
 *
 * @return  Current number of blocks now allocated.
 */
uint16 osal_heap_block_cnt( void )
{
  return blkCnt;
}

/*********************************************************************
 * @fn      osal_heap_block_free
 *
 * @brief   Return the current number of free blocks.
 *
 * @param   none
 *
 * @return  Current number of free blocks.
 */
uint16 osal_heap_block_free( void )
{
  return blkFree;
}

/*********************************************************************
 * @fn      osal_heap_mem_used
 *
 * @brief   Return the current number of bytes allocated.
 *
 * @param   none
 *
 * @return  Current number of bytes allocated.
 */
uint16 osal_heap_mem_used( void )
{
  return memAlo;
}
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
/*********************************************************************
 * @fn      osal_heap_high_water
 *
 * @brief   Return the highest byte ever allocated in the heap.
 *
 * @param   none
 *
 * @return  Highest number of bytes ever used by the stack.
 */
uint16 osal_heap_high_water( void )
{
#if ( OSALMEM_METRICS )
  return memMax;
#else
  return MAXMEMHEAP;
#endif
}
#endif

/**************************************************************************************************
*/
