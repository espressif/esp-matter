/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpUtils.h
 *
 * This file contains the API of the Utils component.  This component implements the
 * following features:
 *
 *        - Link Lists (LL)
 *        - Array Lists (AL)
 *        - Circular buffer (CircB)
 *        - Stack control (stack)
 *        - CRC calculation (crc)
 *        - CRC32 calculation (crc32)
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPUTILS_H_
#define _GPUTILS_H_

#ifdef GP_DIVERSITY_ROM_CODE
#include "gpUtils_RomCode.h"
#else //def GP_DIVERSITY_ROM_CODE

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "hal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* NOTE: When using this macro, make sure a and be are no functions
 *      since this will result in double evaluations! */
#define GP_UTILS_MAX(a,b)                (((a)>(b))?(a):(b))

#define GP_UTILS_LL_MEMORY_DECLARATION(elemType, nrOfElements)      \
    typedef struct {                                                \
        gpUtils_Link_t link;                                        \
        elemType elem;                                              \
    }gpUtils_ll_sub_##elemType##_t;                                 \
                                                                    \
    typedef struct {                                                \
        gpUtils_ll_sub_##elemType##_t sub_##elemType##_array[nrOfElements]; \
    }gpUtils_ll_##elemType##_t

#define GP_UTILS_LL_MEMORY_ALOCATION(elemType, memoryName)  \
    gpUtils_ll_##elemType##_t memoryName[1]


#define GP_UTILS_AL_MEMORY_DECLARATION(elemType, nrOfElements)      \
    typedef struct {                                                \
        gpUtils_ArrayListHdr_t arrayListHdr;                                        \
        elemType elem;                                              \
    }gpUtils_al_sub_##elemType##_t;                                 \
                                                                    \
    typedef struct {                                                \
        gpUtils_ArrayInfo_t bufInfo;                                             \
        gpUtils_al_sub_##elemType##_t sub_##elemType##_array[nrOfElements];  \
    }gpUtils_al_##elemType##_t

#define GP_UTILS_AL_MEMORY_ALOCATION(elemType, memoryName)  \
    gpUtils_al_##elemType##_t memoryName[1]

#define GP_UTILS_LL_SIZE_OF(elem_type)        ((UInt32)sizeof(gpUtils_ll_sub_##elem_type##_t))
#define GP_UTILS_AL_SIZE_OF(elem_type)        ((UInt32)sizeof(gpUtils_al_sub_##elem_type##_t))
#define GP_UTILS_AL_SIZE_BUF_INFO             ((UInt32)sizeof(gpUtils_ArrayInfo_t))

#define GP_UTILS_LL_GET_ELEM(plnk)            (void*)((UIntPtr)plnk + sizeof(gpUtils_Link_t))
#define GP_UTILS_LL_GET_LINK(pelem)           (gpUtils_Link_t*)((UIntPtr)pelem - sizeof(gpUtils_Link_t))

#define GP_UTILS_AL_GET_ELEM(phdr)            (void*)((UIntPtr)phdr + sizeof(gpUtils_ArrayListHdr_t))
#define GP_UTILS_AL_GET_HDR(pelem)            (gpUtils_ArrayListHdr_t*)((UIntPtr)pelem - sizeof(gpUtils_ArrayListHdr_t))
#define GP_UTILS_AL_GET_NEXT_HDR(phdr, pfre)  (gpUtils_ArrayListHdr_t *)((UIntPtr)phdr + (UInt16)pfre->pbuf_info->elem_size)
#define GP_UTILS_AL_GET_PREV_HDR(phdr, pfre)  (gpUtils_ArrayListHdr_t *)((UIntPtr)phdr - (UInt16)pfre->pbuf_info->elem_size)

#define GP_UTILS_LL_IS_LIST_EMPTY(plist)         (plist->plnk_first == NULL)

#define GP_UTILS_LL_ACQUIRE_LOCK(plist)     HAL_ACQUIRE_MUTEX(plist->lock)
#define GP_UTILS_LL_RELEASE_LOCK(plist)     HAL_RELEASE_MUTEX(plist->lock)
#define GP_UTILS_LL_FREE_ACQUIRE_LOCK(pfre) HAL_ACQUIRE_MUTEX(pfre->lock)
#define GP_UTILS_LL_FREE_RELEASE_LOCK(pfre) HAL_RELEASE_MUTEX(pfre->lock)


#define GP_UTILS_END_OF_STACK
#define GP_UTILS_STACK_PATTERN
#define GP_UTILS_END_OF_ISRAM
#define GP_UTILS_INIT_STACK()

#define GP_UTILS_CHECK_STACK_PATTERN()
#define GP_UTILS_CHECK_STACK_POINTER()
#define GP_UTILS_CHECK_STACK_USAGE()
#define GP_UTILS_DUMP_STACK_POINTER()
#define GP_UTILS_DUMP_STACK_POINTER2(x)

#define GP_UTILS_DUMP_STACK_TRACK()
#define GP_UTILS_RESET_STACK_TRACK()
#define GP_UTILS_STACK_TRACK()


#define GP_UTILS_AL_ELEM_FREE     0
#define GP_UTILS_AL_ELEM_RESERVED 1

#define GP_UTILS_CRC32_INITIAL_REMAINDER (UInt32)(0xFFFFFFFF)
#define GP_UTILS_CRC32_FINAL_XOR_VALUE   (UInt32)(0xFFFFFFFF)

#define GP_UTILS_LOCK_CLAIM()           true
#define GP_UTILS_LOCK_RELEASE()         true
#define GP_UTILS_LOCK_CHECK_CLAIMED()   false

#define RADIO_INT
#define GPCOMTXRX
#define LWIP
#define SCHEDEVENT
#define POSTPROCESSING
#define HOSTPROCESSING

#define GP_UTILS_CPUMON_INIT()
#define GP_UTILS_CPUMON_NEW_SCHEDULER_LOOP()
#define GP_UTILS_CPUMON_PROCDONE(x)

#define GP_UTILS_CPUMON_IRQPROLOGUE()
#define GP_UTILS_CPUMON_IRQEPILOGUE()

#define GP_UTILS_CPUMON_HOSTPROCESSING_START()
#define GP_UTILS_CPUMON_HOSTPROCESSING_DONE()


#define GP_UTILS_REGIONS_OVERLAP(address, size, region_start, region_size) (            \
    size && region_size && (                                                            \
        (                                                                               \
            (address >= region_start) &&                                                \
            (address < (region_start + region_size))                                    \
        )                                                                               \
        ||                                                                              \
        (                                                                               \
            ((address + size) > region_start) &&                                        \
            ((address + size) < (region_start + region_size))                           \
        )                                                                               \
        ||                                                                              \
        (                                                                               \
            (address < region_start) &&                                                 \
            ((address + size) >= (region_start + region_size))                          \
        )                                                                               \
    )                                                                                   \
)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#ifdef GP_UTILS_DIVERSITY_LINKED_LIST
typedef struct gpUtils_Link {
    struct gpUtils_Link*   plnk_nxt;
    struct gpUtils_Link*   plnk_prv;
} gpUtils_Link_t;

typedef struct {
    gpUtils_Link_t* plnk_free;      // List of free cells
    gpUtils_Link_t* plnk_free_last; // Pointer to the last free cell for circular cell usage
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    HAL_CRITICAL_SECTION_DEF(lock)
#endif //GP_DIVERSITY_JUMPTABLES
} gpUtils_LinkFree_t;

typedef struct {
    gpUtils_Link_t * plnk_first;     // The first element in the list
    gpUtils_Link_t * plnk_last;      // The last element in the list
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    HAL_CRITICAL_SECTION_DEF(lock)
#endif //GP_DIVERSITY_JUMPTABLES
} gpUtils_LinkList_t;

typedef struct {
    void * pfirst;          // The first pointer
    void * psecond;         // The second pointer
#if !defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2)
    HAL_CRITICAL_SECTION_DEF(lock)  // common lock
#endif //GP_UTILS_DIVERSITY_USE_LIST_LOCK
} gpUtils_LinkCommon_t;

typedef union {
    gpUtils_LinkFree_t      free;
    gpUtils_LinkList_t      list;
    gpUtils_LinkCommon_t    common;
} gpUtils_Links_t;
#endif //GP_UTILS_DIVERSITY_LINKED_LIST

//Padding issues for arraylists
typedef UInt8 gpUtils_ArrayListHdr_t;  //The header of an arrayelement indicating if the element is in use

typedef struct {
    UInt16 elem_size;    //Array list element size including header
    UInt16 elem_cnt;     // Array list total number of elements
} gpUtils_ArrayInfo_t;

typedef struct {
    gpUtils_ArrayInfo_t *    pbuf_info;      // pointer to Array buffer info at start of buffer block
    gpUtils_ArrayListHdr_t * pfree;          // first known free element in array list
    gpUtils_ArrayListHdr_t * plast;          // Last element pointer of array list (kept for calculation ease)
} gpUtils_ArrayFree_t;

//Simple array lists (not linked)
typedef struct {
    gpUtils_ArrayListHdr_t   type;       // value indentifying a unique ID for the list
    gpUtils_ArrayInfo_t *    pbuf_info;  // pointer to Array buffer info at start of free list buffer block
    gpUtils_ArrayListHdr_t * pfirst;     // Start ptr of Arraylist
    gpUtils_ArrayListHdr_t * plast;      // End ptr of Arraylist
} gpUtils_ArrayList_t;

//Circular buffer
typedef struct {
    UInt8* pBuffer;     // Pointer to the memory block
    UInt16 size;        // Size of the memory block
    UInt16 readIndex;   // Read index
    UInt16 writeIndex;  // Write index
    Bool   full;        // Flag indicating a full buffer
} gpUtils_CircularBuffer_t;

typedef Bool (*gpUtils_ElemCompare_t)( void* , void* );
typedef Bool (*gpUtils_ElemCompareDelete_t)( void* , void* , Bool* );

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void gpUtils_WmrkSubscribe(void);

UInt32 gpUtil_encodeBase64(const UInt8 *in_buf, const UInt32 in_len, char *out_buf, const UInt32 out_len);

#ifdef GP_DIVERSITY_UTILS_MATH
#include "gpUtils_Math.h"
#endif // GP_DIVERSITY_UTILS_MATH

/****************************************************************************
 ****************************************************************************
 **                       Linked lists                                     **
 ****************************************************************************
 ****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpUtils_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

#ifdef GP_UTILS_DIVERSITY_LINKED_LIST
GP_API void  gpUtils_LLInit     (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLClear    (gpUtils_LinkList_t * plst);
GP_API void* gpUtils_LLNew      (gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLAdd      (void * pelem, gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLInsert   (void * pelem, void * pelem_cur, gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLInsertBefore (void * pelem, void * pelem_cur, gpUtils_LinkList_t * plst);
GP_API void   gpUtils_LLInsertSorted (void * pelem, gpUtils_LinkList_t * plst, gpUtils_ElemCompare_t callback);
GP_API void*  gpUtils_LLNewSorted    (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre, gpUtils_ElemCompareDelete_t callback);
GP_API void  gpUtils_LLUnlink   (void * pelem, gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLFree     (void * pelem, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLRelease  (void * pelem, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLDump     (gpUtils_LinkList_t * plst);
GP_API void  gpUtils_LLDumpFree (gpUtils_LinkFree_t * pfree);
GP_API void  gpUtils_LLDumpMemory (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre);
GP_API Bool  gpUtils_LLCheckConsistency(void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_LinkList_t * plst, gpUtils_LinkFree_t * pfre);
GP_API void  gpUtils_LLDeInit(gpUtils_LinkList_t* plst);
GP_API void  gpUtils_LLDeInitFree(gpUtils_LinkFree_t* pfre);

GP_API void* gpUtils_LLGetNext(void* pelem);
GP_API void* gpUtils_LLGetPrev(void* pelem);
GP_API gpUtils_Link_t* gpUtils_LLGetLink(void* pelem);
GP_API void* gpUtils_LLGetElem(gpUtils_Link_t* plnk);
GP_API void* gpUtils_LLGetFirstElem(gpUtils_LinkList_t* plst);
GP_API void* gpUtils_LLGetLastElem(gpUtils_LinkList_t* plst);
#endif //GP_UTILS_DIVERSITY_LINKED_LIST

//Array list basic operations
GP_API void  gpUtils_ALInit       (void * buf, UInt32 n_size_cell, UInt32 n_nr_of_elements, gpUtils_ArrayFree_t * pfre);
GP_API void* gpUtils_ALNew        (gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALFree       (void * pelem, gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALDumpFree   (gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALClear      (gpUtils_ArrayList_t* plst, gpUtils_ArrayListHdr_t type, gpUtils_ArrayFree_t* pfre);
GP_API void  gpUtils_ALAdd        (void * pelem, gpUtils_ArrayList_t* plst);
GP_API void  gpUtils_ALUnlink     (void * pelem, gpUtils_ArrayList_t* plst);
GP_API void  gpUtils_ALRelease    (void * pelem, gpUtils_ArrayList_t * plst, gpUtils_ArrayFree_t * pfre);
GP_API void  gpUtils_ALDump       (gpUtils_ArrayList_t* plst);
//GP_API void  gpUtils_ALDumpMemory (gpUtils_ArrayList_t* plst, gpUtils_ArrayFree_t* pfre);
//Array list functions
GP_API void*  gpUtils_ALGetNext  (gpUtils_ArrayList_t * plst, void* pelem);
GP_API void*  gpUtils_ALGetPrev  (gpUtils_ArrayList_t * plst, void* pelem);
GP_API void*  gpUtils_ALGetElem  (gpUtils_ArrayListHdr_t* phdr);
GP_API gpUtils_ArrayListHdr_t*   gpUtils_ALGetHdr  (void* pelem);
GP_API void*  gpUtils_ALGetFirstElem (gpUtils_ArrayList_t * plst);
GP_API void*  gpUtils_ALGetLastElem  (gpUtils_ArrayList_t * plst);
//Circular buffer basic operations
GP_API void   gpUtils_CircBInit           (gpUtils_CircularBuffer_t* pCircularBuffer, void * pBuffer, UInt16 size);
GP_API void   gpUtils_CircBClear          (gpUtils_CircularBuffer_t* pCircularBuffer);
GP_API UInt16 gpUtils_CircBAvailableData  (gpUtils_CircularBuffer_t* pCircularBuffer);
GP_API UInt16 gpUtils_CircBAvailableSpace (gpUtils_CircularBuffer_t* pCircularBuffer);
GP_API Bool   gpUtils_CircBWriteData      (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData, UInt16 length);
GP_API Bool   gpUtils_CircBWriteByte      (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8 Data);
GP_API Bool   gpUtils_CircBReadData       (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData, UInt16 length);
GP_API Bool   gpUtils_CircBReadByte       (gpUtils_CircularBuffer_t* pCircularBuffer, UInt8* pData);
GP_API Bool   gpUtils_CircBGetData        (gpUtils_CircularBuffer_t* pCircularBuffer, UInt16 index, UInt8* pData, UInt16 length);
GP_API Bool   gpUtils_CircBGetByte        (gpUtils_CircularBuffer_t* pCircularBuffer, UInt16 index, UInt8* pData);


//Crc checking
GP_API UInt16 gpUtils_CalculateCrc(UInt8* pData , UInt16 length);
GP_API void   gpUtils_UpdateCrc(UInt16* pCRCValue, UInt8 Data);
GP_API void gpUtils_CalculatePartialCrc(UInt16* pCrcValue, UInt8* pData, UInt16 length);

//Crc CCITT-16
GP_API UInt16 gpUtils_CalculateKermitCrc(UInt8 *pData, UInt16 length);

//Crc32 checking
GP_API UInt32 gpUtils_CalculateCrc32(UInt8* pData , UInt32 length);
GP_API void   gpUtils_UpdateCrc32(UInt32* pCRCValue, UInt8 Data);
GP_API void   gpUtils_CalculatePartialCrc32(UInt32* pCrcValue, UInt8* pData, UInt32 length);

//CRC-8
GP_API UInt8 gpUtils_CalculateCrc8(UInt8 data);

//Global lock
GP_API Bool gpUtils_LockClaim(void);
GP_API Bool gpUtils_LockRelease(void);
GP_API Bool gpUtils_LockCheckClaimed(void);


/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
#ifdef GP_UTILS_DIVERSITY_LINKED_LIST
void gpUtils_LLLockCreate(gpUtils_Links_t *plst);
void gpUtils_LLLockDestroy(gpUtils_Links_t * plst);
void gpUtils_LLLockAcquire(gpUtils_Links_t * plst);
void gpUtils_LLLockRelease(gpUtils_Links_t * plst);
Bool gpUtils_LLLockIsValid(gpUtils_Links_t * plst);
Bool gpUtils_LLLockIsAcquired(gpUtils_Links_t * plst);
#endif //GP_UTILS_DIVERSITY_LINKED_LIST

#ifdef __cplusplus
}
#endif

#endif //def GP_DIVERSITY_ROM_CODE

#endif    // _GPUTILS_H_
