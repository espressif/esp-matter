/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#if defined(MEM_STATISTICS_INTERNAL) || defined(MEM_MANAGER_BENCH)
#include "fsl_component_timer_manager.h"
#include "fsl_component_mem_manager_internal.h"
#endif /* MEM_STATISTICS_INTERNAL MEM_MANAGER_BENCH*/
#include "fsl_component_mem_manager.h"
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
#include "fsl_debug_console.h"
#endif

#if defined(OSA_USED)
#include "fsl_os_abstraction.h"
#if (defined(USE_RTOS) && (USE_RTOS > 0U))
#define MEM_ENTER_CRITICAL() \
    OSA_SR_ALLOC();          \
    OSA_ENTER_CRITICAL()
#define MEM_EXIT_CRITICAL() OSA_EXIT_CRITICAL()
#else
#define MEM_ENTER_CRITICAL()
#define MEM_EXIT_CRITICAL()
#endif
#else
#define MEM_ENTER_CRITICAL() uint32_t regPrimask = DisableGlobalIRQ();
#define MEM_EXIT_CRITICAL()  EnableGlobalIRQ(regPrimask);
#endif
#if !defined(gMemManagerLight) || (gMemManagerLight == 0)
/*****************************************************************************
******************************************************************************
* Private macros
******************************************************************************
*****************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
#define __mem_get_LR() __get_LR()
#elif defined(__GNUC__)
#define __mem_get_LR() __builtin_return_address(0)
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define __mem_get_LR() __return_address()
#endif

#if (defined(MEM_MANAGER_PRE_CONFIGURE) && (MEM_MANAGER_PRE_CONFIGURE > 0U))
#undef _block_set_
#undef _eol_

#define _eol_       ;
#define _block_set_ MEM_BLOCK_BUFFER_NONAME_DEFINE

PoolsDetails_c

#undef _block_set_
#undef _number_of_blocks_
#undef _eol_
#undef _pool_id_

#define _eol_       ,
#define _block_set_ MEM_BLOCK_NONAME_BUFFER

    static uint8_t const *s_PoolList[] = {PoolsDetails_c};
#endif /*MEM_MANAGER_PRE_CONFIGURE*/

/*****************************************************************************
******************************************************************************
* Private type definitions
******************************************************************************
*****************************************************************************/
/*! @brief Buffer pools structure*/
typedef struct _mem_pool_structure
{
    struct _mem_pool_structure *nextPool;
    uint8_t *pHeap;
    uint32_t heapSize;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint16_t allocatedBlocksPeak;
    uint16_t poolFragmentWaste;
    uint16_t poolTotalFragmentWaste;
    uint16_t poolFragmentWastePeak;
    uint16_t poolFragmentMinWaste;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    uint16_t poolId;
    uint16_t blockSize;
    uint16_t numBlocks;
    uint16_t allocatedBlocks;
} mem_pool_structure_t;

/*! @brief Header description for buffers.*/
typedef struct _block_list_header
{
    uint16_t allocated;
    uint16_t blockSize;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint32_t caller;
    uint16_t allocatedBytes;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
} block_list_header_t;

/*! @brief State structure for memory manager. */
typedef struct _mem_manager_info
{
    mem_pool_structure_t *pHeadPool;
    uint16_t poolNum;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint16_t allocationFailures;
    uint16_t freeFailures;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
} mem_manager_info_t;

/*****************************************************************************
******************************************************************************
* Public memory declarations
******************************************************************************
*****************************************************************************/
/*****************************************************************************
 *****************************************************************************
 * Private prototypes
 *****************************************************************************
 *****************************************************************************/
/*****************************************************************************
 *****************************************************************************
 * Private memory definitions
 *****************************************************************************
 *****************************************************************************/
static mem_manager_info_t s_memmanager = {0};
#ifdef MEM_STATISTICS_INTERNAL
static mem_statis_t s_memStatis;
#endif /* MEM_STATISTICS_INTERNAL */

/*****************************************************************************
******************************************************************************
* Private API macro define
******************************************************************************
*****************************************************************************/
#define BLOCK_HDR_SIZE sizeof(block_list_header_t)
/*****************************************************************************
******************************************************************************
* Private functions
******************************************************************************
*****************************************************************************/
#ifdef MEM_STATISTICS_INTERNAL
static void MEM_BufferAllocates_memStatis(void *buffer, uint32_t time, uint32_t requestedSize)
{
    block_list_header_t *pBlock = (block_list_header_t *)buffer - 1;
    /* existing block must have a BlockHdr and a next BlockHdr */
    assert((pBlock != NULL));

    s_memStatis.nb_alloc++;
    /* Sort the buffers by size, based on defined thresholds */
    if (requestedSize <= SMALL_BUFFER_SIZE)
    {
        s_memStatis.nb_small_buffer++;
        UPDATE_PEAK(s_memStatis.nb_small_buffer, s_memStatis.peak_small_buffer);
    }
    else if (requestedSize <= LARGE_BUFFER_SIZE)
    {
        s_memStatis.nb_medium_buffer++;
        UPDATE_PEAK(s_memStatis.nb_medium_buffer, s_memStatis.peak_medium_buffer);
    }
    else
    {
        s_memStatis.nb_large_buffer++;
        UPDATE_PEAK(s_memStatis.nb_large_buffer, s_memStatis.peak_large_buffer);
    }
    /* the RAM allocated is the buffer size and the block header size*/
    s_memStatis.ram_allocated += (uint16_t)(requestedSize + BLOCK_HDR_SIZE);
    UPDATE_PEAK(s_memStatis.ram_allocated, s_memStatis.peak_ram_allocated);

    uint32_t block_size = 0U;

    block_size = pBlock->blockSize;

    assert(block_size >= requestedSize);
    /* ram lost is the difference between block size and buffer size */
    s_memStatis.ram_lost += (uint16_t)(block_size - requestedSize);
    UPDATE_PEAK(s_memStatis.ram_lost, s_memStatis.peak_ram_lost);

    // UPDATE_PEAK(((uint32_t)FreeBlockHdrList.tail + BLOCK_HDR_SIZE), s_memStatis.peak_upper_addr);

#ifdef MEM_MANAGER_BENCH
    if (time != 0U)
    {
        /* update mem stats used for benchmarking */
        s_memStatis.last_alloc_block_size = (uint16_t)block_size;
        s_memStatis.last_alloc_buff_size  = (uint16_t)requestedSize;
        s_memStatis.last_alloc_time       = (uint16_t)time;
        s_memStatis.total_alloc_time += time;
        s_memStatis.average_alloc_time = (uint16_t)(s_memStatis.total_alloc_time / s_memStatis.nb_alloc);
        UPDATE_PEAK((uint16_t)time, s_memStatis.peak_alloc_time);
    }
    else /* alloc time is not correct, we bypass this allocation's data */
    {
        s_memStatis.nb_alloc--;
    }
#else
    NOT_USED(time);
#endif /* MEM_MANAGER_BENCH */
}

static void MEM_BufferFrees_memStatis(void *buffer)
{
    block_list_header_t *pBlock = (block_list_header_t *)buffer - 1;
    s_memStatis.ram_allocated -= (uint16_t)(pBlock->allocatedBytes + BLOCK_HDR_SIZE);
    /* Sort the buffers by size, based on defined thresholds */
    if (pBlock->allocatedBytes <= SMALL_BUFFER_SIZE)
    {
        s_memStatis.nb_small_buffer--;
    }
    else if (pBlock->allocatedBytes <= LARGE_BUFFER_SIZE)
    {
        s_memStatis.nb_medium_buffer--;
    }
    else
    {
        s_memStatis.nb_large_buffer--;
    }

    uint16_t block_size = 0U;
    block_size          = pBlock->blockSize;

    assert(block_size >= pBlock->allocatedBytes);
    assert(s_memStatis.ram_lost >= (block_size - pBlock->allocatedBytes));

    /* as the buffer is free, the ram is not "lost" anymore */
    s_memStatis.ram_lost -= (block_size - pBlock->allocatedBytes);
}
#endif
#if defined(MEM_STATISTICS_INTERNAL)
static void MEM_Reports_memStatis(void)
{
    MEM_DBG_LOG("**************** MEM STATS REPORT **************");
    MEM_DBG_LOG("Nb Alloc:                  %d\r\n", s_memStatis.nb_alloc);
    MEM_DBG_LOG("Small buffers:             %d\r\n", s_memStatis.nb_small_buffer);
    MEM_DBG_LOG("Medium buffers:            %d\r\n", s_memStatis.nb_medium_buffer);
    MEM_DBG_LOG("Large buffers:             %d\r\n", s_memStatis.nb_large_buffer);
    MEM_DBG_LOG("Peak small:                %d\r\n ", s_memStatis.peak_small_buffer);
    MEM_DBG_LOG("Peak medium:               %d\r\n ", s_memStatis.peak_medium_buffer);
    MEM_DBG_LOG("Peak large:                %d\r\n ", s_memStatis.peak_large_buffer);
    MEM_DBG_LOG("Current RAM allocated:     %d bytes\r\n", s_memStatis.ram_allocated);
    MEM_DBG_LOG("Peak RAM allocated:        %d bytes\r\n", s_memStatis.peak_ram_allocated);
    MEM_DBG_LOG("Current RAM lost:          %d bytes\r\n", s_memStatis.ram_lost);
    MEM_DBG_LOG("Peak RAM lost:             %d bytes\r\n", s_memStatis.peak_ram_lost);
    MEM_DBG_LOG("Peak Upper Address:        %x\r\n", s_memStatis.peak_upper_addr);
#ifdef MEM_MANAGER_BENCH
    MEM_DBG_LOG("************************************************\r\n");
    MEM_DBG_LOG("********* MEM MANAGER BENCHMARK REPORT *********\r\n");
    MEM_DBG_LOG("Last Alloc Time:           %d us\r\n", s_memStatis.last_alloc_time);
    MEM_DBG_LOG("Last Alloc Block Size:     %d bytes\r\n", s_memStatis.last_alloc_block_size);
    MEM_DBG_LOG("Last Alloc Buffer Size:    %d bytes\r\n", s_memStatis.last_alloc_buff_size);
    MEM_DBG_LOG("Average Alloc Time:        %d us\r\n", s_memStatis.average_alloc_time);
    MEM_DBG_LOG("Peak Alloc Time:           %d us\r\n", s_memStatis.peak_alloc_time);
#endif /* MEM_MANAGER_BENCH */
    MEM_DBG_LOG("************************************************");
}
#endif /* MEM_STATISTICS_INTERNAL */

/*****************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/
/*!
 * @brief  Initialises the Memory Manager.
 *
 */
mem_status_t MEM_Init(void)
{
    static bool initialized = false;
    assert(sizeof(mem_pool_structure_t) == MEM_POOL_SIZE);
    assert(sizeof(block_list_header_t) == MEM_BLOCK_SIZE);
    if (!initialized)
    {
        s_memmanager.pHeadPool = NULL;
        s_memmanager.poolNum   = 0;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
        s_memmanager.allocationFailures = 0;
        s_memmanager.freeFailures       = 0;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
#if (defined(MEM_MANAGER_PRE_CONFIGURE) && (MEM_MANAGER_PRE_CONFIGURE > 0U))
        for (uint8_t i = 0; i < (sizeof(s_PoolList) / sizeof(s_PoolList[0])); i++)
        {
            (void)MEM_AddBuffer(s_PoolList[i]);
        }
#endif /*MEM_MANAGER_PRE_CONFIGURE*/
        initialized = true;
    }
    return kStatus_MemSuccess;
}

/*!
 * @brief Add memory buffer to memory manager buffer list.
 *
 * @note This API should be called when need add memory buffer to memory manager buffer list. First use
 * MEM_BLOCK_BUFFER_DEFINE to
 *        define memory buffer, then call MEM_AddBuffer function with MEM_BLOCK_BUFFER Macro.
 *  @code
 * MEM_BLOCK_BUFFER_DEFINE(app64, 5, 64,0);
 * MEM_BLOCK_BUFFER_DEFINE(app128, 6, 128,0);
 * MEM_BLOCK_BUFFER_DEFINE(app256, 7, 256,0);
 *
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app64));
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app128));
 * MEM_AddBuffer(MEM_BLOCK_BUFFER(app256));
 *  @endcode
 *
 * @param buffer                     Pointer the memory pool buffer, use MEM_BLOCK_BUFFER Macro as the input parameter.
 *
 * @retval kStatus_MemSuccess        Memory manager add Buffer succeed.
 * @retval kStatus_MemInitError      Memory manager add Buffer error occurred.
 */
mem_status_t MEM_AddBuffer(const uint8_t *buffer)
{
    mem_config_t *memConfig     = (mem_config_t *)(void *)buffer;
    mem_pool_structure_t *pPool = (mem_pool_structure_t *)(void *)memConfig->pbuffer;
    uint8_t *pHeap              = &memConfig->pbuffer[sizeof(mem_pool_structure_t)];
    mem_pool_structure_t *pPrevPool, *pTempPool;

    assert(buffer);
    assert(memConfig->numberOfBlocks);
    assert(memConfig->blockSize);

    MEM_ENTER_CRITICAL();
#if (defined(MEM_MANAGER_PRE_CONFIGURE) && (MEM_MANAGER_PRE_CONFIGURE == 0U))
    (void)MEM_Init();
#endif
    pPool->pHeap     = pHeap;
    pPool->numBlocks = memConfig->numberOfBlocks;
    pPool->blockSize = memConfig->blockSize;
    pPool->poolId    = *(uint16_t *)(void *)(&buffer[4]);
    pPool->heapSize =
        (MEM_POOL_SIZE + (uint32_t)memConfig->numberOfBlocks * (MEM_BLOCK_SIZE + (uint32_t)memConfig->blockSize));
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    pPool->allocatedBlocksPeak    = 0;
    pPool->poolTotalFragmentWaste = 0;
    pPool->poolFragmentWaste      = 0;
    pPool->poolFragmentWastePeak  = 0;
    pPool->poolFragmentMinWaste   = 0xffff;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    if (s_memmanager.pHeadPool == NULL)
    {
        s_memmanager.pHeadPool = pPool;
    }
    else
    {
        pTempPool = s_memmanager.pHeadPool;
        pPrevPool = pTempPool;
        while (NULL != pTempPool)
        {
            if (((pPool->blockSize >= pPrevPool->blockSize) && (pPool->blockSize <= pTempPool->blockSize)) ||
                (pPool->blockSize <= pPrevPool->blockSize))
            {
                if (pTempPool == s_memmanager.pHeadPool)
                {
                    s_memmanager.pHeadPool = pPool;
                }
                else
                {
                    pPrevPool->nextPool = pPool;
                }
                pPool->nextPool = pTempPool;
                break;
            }
            pPrevPool = pTempPool;
            pTempPool = pTempPool->nextPool;
        }
        if (pPool->blockSize > pPrevPool->blockSize)
        {
            pPrevPool->nextPool = pPool;
        }
    }

    s_memmanager.poolNum++;
    MEM_EXIT_CRITICAL();
    return kStatus_MemSuccess;
}

/*!
 * @brief Remove memory buffer from memory manager buffer list.
 *
 * @note This API should be called when need remove memory buffer to memory manager buffer list. Use with
 * MEM_BLOCK_BUFFER Macro as input parameter.
 *
 * @param buffer                     Pointer the memory pool buffer, use MEM_BLOCK_BUFFER Macro as the input parameter.
 *
 * @retval kStatus_MemSuccess        Memory manager remove buffer succeed.
 * @retval kStatus_MemUnknownError      Memory manager remove buffer error occurred.
 */
#if (defined(MEM_MANAGER_BUFFER_REMOVE) && (MEM_MANAGER_BUFFER_REMOVE > 0U))
mem_status_t MEM_RemoveBuffer(uint8_t *buffer)
{
    mem_config_t *memConfig     = (mem_config_t *)(void *)buffer;
    mem_pool_structure_t *pPool = (mem_pool_structure_t *)(void *)memConfig->pbuffer;
    uint8_t *pHeap              = &memConfig->pbuffer[sizeof(mem_pool_structure_t)];
    mem_pool_structure_t *pPrevPool, *pTempPool;

    assert(buffer);
    assert(memConfig->numberOfBlocks > 0U);
    assert(memConfig->blockSize > 0U);

    MEM_ENTER_CRITICAL();
    pTempPool = s_memmanager.pHeadPool;
    pPrevPool = pTempPool;
    while (NULL != pTempPool)
    {
        if (0U != pPool->allocatedBlocks)
        {
            break;
        }
        if (pTempPool->pHeap == pHeap)
        {
            if (pPool == s_memmanager.pHeadPool)
            {
                s_memmanager.pHeadPool = pPool->nextPool;
            }
            else
            {
                pPrevPool->nextPool = pPool->nextPool;
            }
            s_memmanager.poolNum--;
            MEM_EXIT_CRITICAL();
            return kStatus_MemSuccess;
        }
        pPrevPool = pTempPool;
        pTempPool = pTempPool->nextPool;
    }
    MEM_EXIT_CRITICAL();
    return kStatus_MemUnknownError;
}
#endif // MEM_MANAGER_BUFFER_REMOVE

/*!
 * @brief Allocate a block from the memory pools. The function uses the
 *        numBytes argument to look up a pool with adequate block sizes.
 *
 * @param numBytes           The number of bytes will be allocated.
 * @param poolId             The ID of the pool where to search for a free buffer.
 * @retval Memory buffer address when allocate success, NULL when allocate fail.
 */
void *MEM_BufferAllocWithId(uint32_t numBytes, uint8_t poolId)
{
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    uint32_t fragmentWaste = 0;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    mem_pool_structure_t *pPool = s_memmanager.pHeadPool;
    block_list_header_t *pBlock;
    void *buffer = NULL;

    MEM_ENTER_CRITICAL();
#ifdef MEM_MANAGER_BENCH
    uint32_t START_TIME = 0U, STOP_TIME = 0U, ALLOC_TIME = 0U;
    START_TIME = TM_GetTimestamp();
#endif /* MEM_MANAGER_BENCH */

    while (0U != numBytes)
    {
        if ((numBytes <= pPool->blockSize) && (pPool->poolId == poolId))
        {
            for (uint32_t i = 0; i < pPool->numBlocks; i++)
            {
                pBlock = (block_list_header_t *)(void *)(pPool->pHeap + i * ((uint32_t)pPool->blockSize +
                                                                             (uint32_t)sizeof(block_list_header_t)));
                if (0U == pBlock->allocated)
                {
                    pBlock->allocated = 1;
                    pBlock->blockSize = pPool->blockSize;
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
                    pBlock->allocatedBytes = (uint16_t)numBytes;
                    pBlock->caller         = (uint32_t)((uint32_t *)__mem_get_LR());
#endif /*MEM_MANAGER_ENABLE_TRACE*/
                    pBlock++;
                    pPool->allocatedBlocks++;
                    buffer = pBlock;
                    (void)memset(buffer, 0x0, pBlock->blockSize);
                    break;
                }
            }
        }
        if (NULL != buffer)
        {
            break;
        }
        /* Try next pool*/
        pPool = pPool->nextPool;
        if (NULL == pPool)
        {
            break;
        }
    }
#ifdef MEM_MANAGER_BENCH
    STOP_TIME  = TM_GetTimestamp();
    ALLOC_TIME = STOP_TIME - START_TIME;
#endif /* MEM_MANAGER_BENCH */
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    if (NULL == buffer)
    {
        s_memmanager.allocationFailures++;
    }
#endif /*MEM_MANAGER_ENABLE_TRACE*/

#ifdef MEM_STATISTICS_INTERNAL
#ifdef MEM_MANAGER_BENCH
    MEM_BufferAllocates_memStatis(buffer, ALLOC_TIME, numBytes);
#else
    MEM_BufferAllocates_memStatis(buffer, 0, numBytes);
#endif

    if ((s_memStatis.nb_alloc % NB_ALLOC_REPORT_THRESHOLD) == 0U)
    {
        MEM_Reports_memStatis();
    }
#endif /* MEM_STATISTICS_INTERNAL */
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
    if (pPool->allocatedBlocks > pPool->allocatedBlocksPeak)
    {
        pPool->allocatedBlocksPeak = pPool->allocatedBlocks;
    }
    fragmentWaste = pPool->blockSize - numBytes;
    if (fragmentWaste > pPool->poolFragmentWastePeak)
    {
        pPool->poolFragmentWastePeak = (uint16_t)fragmentWaste;
    }
    pPool->poolFragmentWaste = (uint16_t)fragmentWaste;
    pPool->poolTotalFragmentWaste += (uint16_t)fragmentWaste;
    if (fragmentWaste < pPool->poolFragmentMinWaste)
    {
        pPool->poolFragmentMinWaste = (uint16_t)fragmentWaste;
    }
#endif /*MEM_MANAGER_ENABLE_TRACE*/
    MEM_EXIT_CRITICAL();
    return buffer;
}

/*!
 * @brief Memory buffer free.
 *
 * @param buffer                     The memory buffer address will be free.
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
mem_status_t MEM_BufferFree(void *buffer /* IN: Block of memory to free*/
)
{
    block_list_header_t *pBlock;
    MEM_ENTER_CRITICAL();

    do
    {
        if (NULL == buffer)
        {
            break;
        }
#if defined(MEM_STATISTICS_INTERNAL)
        MEM_BufferFrees_memStatis(buffer);
#endif /* MEM_STATISTICS_INTERNAL */
        pBlock = (block_list_header_t *)buffer - 1;
        assert(pBlock);
        if (1U == pBlock->allocated)
        {
            (void)memset(pBlock, 0x0, (sizeof(block_list_header_t) + (uint32_t)pBlock->blockSize));
            MEM_EXIT_CRITICAL();
            return kStatus_MemSuccess;
        }

#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
        s_memmanager.freeFailures++;
#endif /*MEM_MANAGER_ENABLE_TRACE*/

    } while (false);

    MEM_EXIT_CRITICAL();
    return kStatus_MemFreeError;
}

/*!
 * @brief Returns the size of a given buffer.
 *
 * @param buffer                     The memory buffer address will be free.
 * @retval The size of a given buffer.
 */
uint16_t MEM_BufferGetSize(void *buffer) /* IN: Block of memory to get size*/
{
    block_list_header_t *pBlock;
    assert(buffer);

    pBlock = (block_list_header_t *)buffer - 1;
    assert(pBlock);

    return pBlock->blockSize;
}

/*!
 * @brief Frees all allocated blocks by selected source and in selected pool.
 *
 * @param poolId                     Selected pool Id (4 LSBs of poolId parameter) and selected
 *                                   source Id (4 MSBs of poolId parameter).
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
mem_status_t MEM_BufferFreeAllWithId(uint8_t poolId)
{
    mem_pool_structure_t *pPool = s_memmanager.pHeadPool;

    MEM_ENTER_CRITICAL();

    while (pPool != NULL)
    {
        if (pPool->poolId == poolId)
        {
            (void)memset(pPool->pHeap, 0x0,
                         ((sizeof(block_list_header_t) + (uint32_t)pPool->blockSize) * pPool->numBlocks));
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
            pPool->allocatedBlocksPeak    = 0;
            pPool->poolTotalFragmentWaste = 0;
            pPool->poolFragmentWaste      = 0;
            pPool->poolFragmentWastePeak  = 0;
            pPool->poolFragmentMinWaste   = 0xffff;
#endif /*MEM_MANAGER_ENABLE_TRACE*/
        }
        pPool = pPool->nextPool;
    }

    MEM_EXIT_CRITICAL();
    return kStatus_MemSuccess;
}

/*!
 * @brief Memory buffer realloc.
 *
 * @param buffer                     The memory buffer address will be reallocated.
 * @param new_size                   The number of bytes will be reallocated
 * @retval kStatus_MemSuccess        Memory free succeed.
 * @retval kStatus_MemFreeError      Memory free error occurred.
 */
void *MEM_BufferRealloc(void *buffer, uint32_t new_size)
{
    void *realloc_buffer = NULL;
    uint16_t block_size  = 0U;

    if (new_size == 0U)
    {
        /* new requested size is 0, free old buffer */
        (void)MEM_BufferFree(buffer);
        realloc_buffer = NULL;
    }
    else if (buffer == NULL)
    {
        /* input buffer is NULL simply allocate a new buffer and return it */
        realloc_buffer = MEM_BufferAllocWithId(new_size, 0U);
    }
    else
    {
        block_size = MEM_BufferGetSize(buffer);

        if ((uint16_t)new_size <= block_size)
        {
            /* current buffer is large enough for the new requested size
               we can still use it */
            realloc_buffer = buffer;
        }
        else
        {
            /* not enough space in the current block, creating a new one */
            realloc_buffer = MEM_BufferAllocWithId(new_size, 0U);

            if (realloc_buffer != NULL)
            {
                /* copy input buffer data to new buffer */
                (void)memcpy(realloc_buffer, buffer, (uint32_t)block_size);

                /* free old buffer */
                (void)MEM_BufferFree(buffer);
            }
        }
    }

    return realloc_buffer;
}

/*!
 * @brief Get the address after the last allocated block if MemManagerLight is used.
 *
 * @retval 0      Return 0 in case of the legacy MemManager.
 */
uint32_t MEM_GetHeapUpperLimit(void)
{
    return 0;
}

/*!
 * @brief Trace memory manager all information to use debug.
 *
 */
#if (defined(MEM_MANAGER_ENABLE_TRACE) && (MEM_MANAGER_ENABLE_TRACE > 0U))
void MEM_Trace(void)
{
    mem_pool_structure_t *pPool = s_memmanager.pHeadPool;
    block_list_header_t *pBlock;
    (void)PRINTF("MEM_Trace debug information, Pools Number:%d   allocationFailures: %d  freeFailures:%d \r\n",
                 s_memmanager.poolNum, s_memmanager.allocationFailures, s_memmanager.allocationFailures,
                 s_memmanager.freeFailures);
    while (NULL != pPool)
    {
        (void)PRINTF("POOL: ID %d  blockSize:%d   status:\r\n", pPool->poolId, pPool->blockSize);
        (void)PRINTF(
            "numBlocks allocatedBlocks  allocatedBlocksPeak  poolFragmentWaste poolFragmentWastePeak "
            "poolFragmentMinWaste poolTotalFragmentWaste\r\n");
        (void)PRINTF(
            "    %d          %d                %d                %d                  %d                       %d       "
            "              %d           \r\n",
            pPool->numBlocks, pPool->allocatedBlocks, pPool->allocatedBlocksPeak, pPool->poolFragmentWaste,
            pPool->poolFragmentWastePeak, pPool->poolFragmentMinWaste, pPool->poolTotalFragmentWaste);
        (void)PRINTF("Currently pool meory block allocate status: \r\n");
        for (uint32_t i = 0; i < pPool->numBlocks; i++)
        {
            pBlock = (block_list_header_t *)(void *)(pPool->pHeap +
                                                     i * ((uint32_t)pPool->blockSize + sizeof(block_list_header_t)));

            (void)PRINTF("Block %d caller : 0x%x Allocated %d bytes: %d  \r\n", i, pBlock->caller, pBlock->allocated,
                         pBlock->allocatedBytes);
        }
        /* Try next pool*/
        pPool = pPool->nextPool;
    }
}
#endif /*MEM_MANAGER_ENABLE_TRACE*/
#endif /*gMemManagerLight*/
