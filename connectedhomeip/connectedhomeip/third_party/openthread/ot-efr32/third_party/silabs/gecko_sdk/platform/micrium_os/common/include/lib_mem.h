/***************************************************************************//**
 * @file
 * @brief Common - Memory Operations
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

/****************************************************************************************************//**
 * @defgroup LIB_MEM LIB Memory API
 * @ingroup  LIB
 * @brief      LIB Memory API
 *
 * @addtogroup LIB_MEM
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _LIB_MEM_H_
#define  _LIB_MEM_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <common/include/rtos_path.h>
#include  <common_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LIB_MEM_PADDING_ALIGN_NONE                       1u

#define  LIB_MEM_BLK_QTY_UNLIMITED                       ((CPU_SIZE_T)-1)

#define  LIB_MEM_BUF_ALIGN_AUTO                           0u

/********************************************************************************************************
 *                                           DEFAULT CONFIGURATION
 *
 * Note(s) : (1) LIB_MEM_COPY_FNCT_PREFIX allows to specify a prefix to the Mem_Copy function used, in
 *               order to override the implementation provided by the compiler. This is done automatically
 *               for IAR as a workaround for a known issue with IAR's memcpy not working properly and
 *               doing unaligned accesses to memory. It is possible to use force the use of IAR's memcpy
 *               by #define'ing LIB_MEM_COPY_FNCT_PREFIX as nothing or by setting LIB_MEM_CFG_STD_C_LIB_EN
 *               to DEF_ENABLED. In those cases, the unaligned access issue would still be present.
 *******************************************************************************************************/

#ifndef  LIB_MEM_CFG_STD_C_LIB_EN
#define  LIB_MEM_CFG_STD_C_LIB_EN               DEF_DISABLED
#endif

#ifndef  LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN
#define  LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN   DEF_DISABLED
#endif

#ifndef  LIB_MEM_CFG_DBG_INFO_EN
#define  LIB_MEM_CFG_DBG_INFO_EN                DEF_DISABLED
#endif

#ifndef  LIB_MEM_CFG_HEAP_PADDING_ALIGN
#define  LIB_MEM_CFG_HEAP_PADDING_ALIGN         LIB_MEM_PADDING_ALIGN_NONE
#endif

#if  (LIB_MEM_CFG_STD_C_LIB_EN == DEF_DISABLED)
#ifndef LIB_MEM_COPY_FNCT_PREFIX                                // See Note #1.
#if (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)
#define  LIB_MEM_COPY_FNCT_PREFIX               __aapcs_core
#define  LIB_MEM_COPY_IAR_OVERRIDE_EN
#else
#define  LIB_MEM_COPY_FNCT_PREFIX
#endif

#endif
#endif

/********************************************************************************************************
 *                                       COMPILE-TIME SEGMENT INIT MACRO
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               MEM_SEG_INIT()
 *
 * @brief    Creates a new memory segment to be used for runtime memory allocation. Created segment
 *           will not be added to list checked for overlap or to debug, it must be added afterwards
 *           by calling the Mem_SegReg() function.
 *
 * @param    p_name          Pointer to segment name.
 *
 * @param    seg_base_addr   Address of segment's first byte.
 *
 * @param    size            Total size of segment (in bytes).
 *
 * @param    padding_align   Padding alignment (in bytes) that will be added to any allocated buffer
 *                           from this memory segment. MUST be a power of 2.
 *                           LIB_MEM_PADDING_ALIGN_NONE means no padding.
 *
 * @note     (1) Contrary to the Mem_SegCreate() function, this segment is not checked for overlap at
 *               run-time. Therefore, extra care should be taken when using this macro.
 *
 * @note     (2) Usage is as follows:
 *               @verbatim
 *               MEM_SEG  My_MemSeg = MEM_SEG_INIT("My Segment",
 *                                               (CPU_ADDR)&My_SegmentData[0u],
 *                                               MY_SEGMENT_SIZE,
 *                                               LIB_MEM_CFG_HEAP_PADDING_ALIGN);
 *               @endverbatim
 *******************************************************************************************************/

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
#define  MEM_SEG_INIT(p_name, seg_base_addr, size, padding_align)        \
  {                                                                      \
    /* .AddrBase         = */ (void *) (seg_base_addr),                  \
    /* .AddrEnd          = */ (void *)((seg_base_addr) + ((size) - 1u)), \
    /* .AddrNext         = */ (void *) (seg_base_addr),                  \
    /* .NextPtr          = */ DEF_NULL,                                  \
    /* .PaddingAlign     = */ (padding_align),                           \
    /* .NamePtr          = */ (p_name),                                  \
    /* .AllocInfoHeadPtr = */ DEF_NULL                                   \
  }
#else
#define  MEM_SEG_INIT(p_name, seg_base_addr, size, padding_align)    \
  {                                                                  \
    /* .AddrBase     = */ (void *) (seg_base_addr),                  \
    /* .AddrEnd      = */ (void *)((seg_base_addr) + ((size) - 1u)), \
    /* .AddrNext     = */ (void *) (seg_base_addr),                  \
    /* .NextPtr      = */ DEF_NULL,                                  \
    /* .PaddingAlign = */ (padding_align)                            \
  }
#endif

/********************************************************************************************************
 *                                       STD C LIB USE FOR MEM FUNCTIONS
 *******************************************************************************************************/

#if (LIB_MEM_CFG_STD_C_LIB_EN == DEF_ENABLED)
#include  <string.h>

#define  Mem_Set(p_mem, data_val, size)         memset((p_mem), (data_val), (size))

#define  Mem_Copy(p_dest, p_src, size)          memcpy((p_dest), (p_src), (size))

#define  Mem_Move(p_dest, p_src, size)          memmove((p_dest), (p_src), (size))

#define  Mem_Clr(p_mem, size)                   memset((p_mem), 0u, (size))

#define  Mem_Cmp(p_dest, p_src, size)         ((memcmp((p_dest), (p_src), (size)) == 0) ? (DEF_TRUE) : (DEF_FALSE))
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct mem_dyn_pool MEM_DYN_POOL;
typedef struct mem_seg MEM_SEG;

/****************************************************************************************************//**
 *                                   PERSISTENT DYN POOL ALLOC CALLBACK
 *
 * @note     (1) Callback that is called when a new block is allocated from the memory segment when a
 *               persistent dynamic memory pool is used.
 *******************************************************************************************************/

typedef CPU_BOOLEAN (*MEM_DYN_POOL_ALLOC_FNCT)  (MEM_DYN_POOL *p_pool,
                                                 MEM_SEG      *p_seg,
                                                 void         *p_blk,
                                                 void         *p_arg);

/*
 ********************************************************************************************************
 *                               MEMORY ALLOCATION TRACKING INFO DATA TYPE
 *******************************************************************************************************/

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
typedef struct mem_alloc_info MEM_ALLOC_INFO;

struct mem_alloc_info {                                         // ------------------ MEM ALLOC INFO ------------------
  const CPU_CHAR *NamePtr;                                      // Ptr to name.
  CPU_SIZE_T     Size;                                          // Total alloc'd size, in bytes.
  MEM_ALLOC_INFO *NextPtr;                                      // Ptr to next alloc info in list.
};
#endif

/*
 ********************************************************************************************************
 *                                       MEMORY SEGMENTS DATA TYPES
 *******************************************************************************************************/

struct mem_seg {                                                // --------------------- SEG DATA ---------------------
  void           *AddrBase;                                     // Seg start addr.
  void           *AddrEnd;                                      // Seg end addr (last addr).
  void           *AddrNext;                                     // Next free addr.

  MEM_SEG        *NextPtr;                                      // Ptr to next seg.

  CPU_SIZE_T     PaddingAlign;                                  // Padding alignment in byte.

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
  const CPU_CHAR *NamePtr;                                      // Ptr to seg name.
  MEM_ALLOC_INFO *AllocInfoHeadPtr;                             // Ptr to head of alloc info struct list.
#endif
};

typedef struct mem_seg_info {                                   // --------------------- SEG INFO ---------------------
  CPU_SIZE_T UsedSize;                                          // Used size, independently of alignment.
  CPU_SIZE_T TotalSize;                                         // Total seg capacity, in octets.

  CPU_ADDR   AddrBase;
  CPU_ADDR   AddrNextAlloc;                                     // Next aligned address, 0 if none available.
} MEM_SEG_INFO;

/****************************************************************************************************//**
 *                                       DYNAMIC MEMORY POOL DATA TYPE
 * @brief       Dynamic memory pool
 *
 * @note     (1) Dynamic memory pool blocks are not indexed in a table. Only freed blocks are linked using
 *               a singly linked list, in a LIFO fashion; newly freed blocks are inserted at the head of the
 *               list and blocks are also retrieved from the head of the list.
 *
 * @note     (2) There are 2 different types of dynamic memory pool:
 *               - (a) Standard dynamic memory pool. Pointers to the next block are only present when a block
 *                   is free, using the first location in the allocated memory block. The user of dynamic
 *                   memory pool must not assume his data will not be overwritten when a block is freed.
 *                   @verbatim
 *                                          +----------------+
 *                           +----------+   |  +----------+  |    +----------+   +----------+
 *              BlkFreePtr-->|(NextPtr) |---+  |          |  +--->|(NextPtr) |-->|(NextPtr) |--> DEF_NULL
 *                           |----------|      |  Blk in  |       |----------|   |----------|
 *                           |          |      |   use    |       |          |   |          |
 *                           |          |      |          |       |          |   |          |
 *                           +----------+      +----------+       +----------+   +----------+
 *                   @endverbatim
 *               - (b) Persistent dynamic memory pool. Pointers to the next block are always present but
 *                   only used when a block is free. The next pointer is allocated at the end of the block.
 *                   Hence, allocated block will be larger than the specified block size. Data integrity
 *                   of a block is guaranteed between a Free and a Get.
 *                   @verbatim
 *                           +----------+      +----------+       +----------+     +----------+
 *           BlkFreePtr -->  |          |      |  Blk in  |   +-->|          |  +->|          |
 *                           |          |      |   use    |   |   |          |  |  |          |
 *    Extra space allocated  |----------|      |----------|   |   |----------|  |  |----------|
 *    at end of each blk     |(NextPtr) |---+  |          |   |   |(NextPtr) |--+  |(NextPtr) |--> DEF_NULL
 *                           +----------+   |  +----------+   |   +----------+     +----------+
 *                                          +-----------------+
 *                   @endverbatim
 *******************************************************************************************************/

struct mem_dyn_pool {                                           // ---------------- DYN MEM POOL DATA -----------------
  CPU_INT08U              Opt;                                  // Dyn mem pool options.

  MEM_SEG                 *PoolSegPtr;                          // Mem pool from which blks are alloc'd.
  CPU_SIZE_T              BlkSize;                              // Size of pool blks, in octets.
  CPU_SIZE_T              BlkAlign;                             // Align req'd for blks, in octets.
  void                    *BlkFreePtr;                          // Ptr to first free blk.

  CPU_SIZE_T              BlkQtyMax;                            // Max qty of blk in dyn mem pool. 0 = unlimited.
  CPU_SIZE_T              BlkAllocCnt;                          // Cnt of alloc blk.

  MEM_DYN_POOL_ALLOC_FNCT AllocFnct;                            // Callback fnct to call when new blk is alloc from seg.
  void                    *AllocFnctArg;                        // Arg for alloc callback fnct.

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
  const CPU_CHAR          *NamePtr;                             // Ptr to mem pool name.
#endif
};

/*
 ********************************************************************************************************
 *                                   DYNAMIC MEMORY POOL INFORMATION
 *******************************************************************************************************/

typedef struct mem_dyn_pool_info {                              // ------------------ DYN POOL INFO -------------------
  CPU_SIZE_T BlkQtyMax;                                         // Max nbr of blk or LIB_MEM_BLK_QTY_UNLIMITED.
  CPU_SIZE_T BlkNbrAllocCnt;                                    // Cnt of alloc'd blks.
  CPU_SIZE_T BlkNbrRemCnt;                                      // Cnt of remaining blks that can be alloc'd.
  CPU_SIZE_T BlkNbrAvailCnt;                                    // Cnt of blks already alloc'd that have been freed.
} MEM_DYN_POOL_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       MEMORY DATA VALUE MACRO'S
 *
 * @note     (1) Some variables & variable buffers to pass & receive data values MUST start on appropriate
 *               CPU word-aligned addresses.  This is required because most word-aligned processors are more
 *               efficient & may even REQUIRE that multi-octet words start on CPU word-aligned addresses.
 *               For 16-bit word-aligned processors, this means that
 *               - all 16- & 32-bit words MUST start on addresses that are multiples of 2 octets
 *               For 32-bit word-aligned processors, this means that
 *               - all 16-bit       words MUST start on addresses that are multiples of 2 octets
 *               - all 32-bit       words MUST start on addresses that are multiples of 4 octets
 *               However, some data values macro's appropriately access data values from any CPU addresses,
 *               word-aligned or not.  Thus for processors that require data word alignment, data words can
 *               be accessed to/from any CPU address, word-aligned or not, without generating data-word-
 *               alignment exceptions/faults.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       ENDIAN WORD ORDER MACRO'S
 *
 * @brief          Convert data values to & from big-, little, or host-endian CPU word order.
 *
 * @param          val         Data value to convert (see Notes #1 & #2).
 *
 * @return        Converted data value (see Notes #1 & #2).
 *
 * @note     (1) Convert data values to the desired data-word order :
 *                   @verbatim
 *                       MEM_VAL_BIG_TO_LITTLE_xx()      Convert big-        endian data values
 *                                                               to little-     endian data values
 *                       MEM_VAL_LITTLE_TO_BIG_xx()      Convert little-     endian data values
 *                                                               to big-        endian data values
 *                       MEM_VAL_xxx_TO_HOST_xx()        Convert big-/little-endian data values
 *                                                               to host-       endian data values
 *                       MEM_VAL_HOST_TO_xxx_xx()        Convert host-       endian data values
 *                                                               to big-/little-endian data values
 *                   @endverbatim
 * @note     (2) 'val' data value to convert & any variable to receive the returned conversion MUST
 *                   start on appropriate CPU word-aligned addresses.
 *                   See also 'MEMORY DATA VALUE MACRO'S  Note #1a'.
 *
 * @note     (3) MEM_VAL_COPY_xxx() macro's are more efficient than generic endian word order macro's &
 *                   are also independent of CPU data-word-alignment & SHOULD be used whenever possible.
 *                   See also 'MEM_VAL_COPY_GET_xxx()  Note #4'
 *                           & 'MEM_VAL_COPY_SET_xxx()  Note #4'.
 *
 * @note     (4) Generic endian word order macro's are NOT atomic operations & MUST NOT be used on any
 *                   non-static (i.e. volatile) variables, registers, hardware, etc.; without the caller of
 *                   the macro's providing some form of additional protection (e.g. mutual exclusion).
 *
 * @note     (5) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
 *                   linked since each '[arch]_cpu_port.h' SHOULD ensure that the CPU data-word-memory order
 *                   configuration constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate
 *                   data-word-memory order value (see '[arch]_cpu_port.h  CPU WORD CONFIGURATION  Note #2').
 *                   The 'else'-conditional code is included as an extra precaution in case '[arch]_cpu_port.h' is
 *                   incorrectly configured.
 * @{
 *******************************************************************************************************/

#if    ((CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_64) \
  || (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32))

#define  MEM_VAL_BIG_TO_LITTLE_16(val)        ((CPU_INT16U)(((CPU_INT16U)((((CPU_INT16U)(val)) & (CPU_INT16U)    0xFF00u) >> (1u * DEF_OCTET_NBR_BITS))) \
                                                            | ((CPU_INT16U)((((CPU_INT16U)(val)) & (CPU_INT16U)    0x00FFu) << (1u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_BIG_TO_LITTLE_32(val)        ((CPU_INT32U)(((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0xFF000000u) >> (3u * DEF_OCTET_NBR_BITS)))   \
                                                            | ((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0x00FF0000u) >> (1u * DEF_OCTET_NBR_BITS))) \
                                                            | ((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0x0000FF00u) << (1u * DEF_OCTET_NBR_BITS))) \
                                                            | ((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0x000000FFu) << (3u * DEF_OCTET_NBR_BITS)))))

#elif   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)

#define  MEM_VAL_BIG_TO_LITTLE_16(val)        ((CPU_INT16U)(((CPU_INT16U)((((CPU_INT16U)(val)) & (CPU_INT16U)    0xFF00u) >> (1u * DEF_OCTET_NBR_BITS))) \
                                                            | ((CPU_INT16U)((((CPU_INT16U)(val)) & (CPU_INT16U)    0x00FFu) << (1u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_BIG_TO_LITTLE_32(val)        ((CPU_INT32U)(((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0xFF000000u) >> (1u * DEF_OCTET_NBR_BITS)))   \
                                                            | ((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0x00FF0000u) << (1u * DEF_OCTET_NBR_BITS))) \
                                                            | ((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0x0000FF00u) >> (1u * DEF_OCTET_NBR_BITS))) \
                                                            | ((CPU_INT32U)((((CPU_INT32U)(val)) & (CPU_INT32U)0x000000FFu) << (1u * DEF_OCTET_NBR_BITS)))))

#else

#define  MEM_VAL_BIG_TO_LITTLE_16(val)                                                  (val)
#define  MEM_VAL_BIG_TO_LITTLE_32(val)                                                  (val)

#endif

#define  MEM_VAL_LITTLE_TO_BIG_16(val)                          MEM_VAL_BIG_TO_LITTLE_16(val)
#define  MEM_VAL_LITTLE_TO_BIG_32(val)                          MEM_VAL_BIG_TO_LITTLE_32(val)

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_BIG_TO_HOST_16(val)                                                    (val)
#define  MEM_VAL_BIG_TO_HOST_32(val)                                                    (val)
#define  MEM_VAL_LITTLE_TO_HOST_16(val)                         MEM_VAL_LITTLE_TO_BIG_16(val)
#define  MEM_VAL_LITTLE_TO_HOST_32(val)                         MEM_VAL_LITTLE_TO_BIG_32(val)

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_BIG_TO_HOST_16(val)                            MEM_VAL_BIG_TO_LITTLE_16(val)
#define  MEM_VAL_BIG_TO_HOST_32(val)                            MEM_VAL_BIG_TO_LITTLE_32(val)
#define  MEM_VAL_LITTLE_TO_HOST_16(val)                                                 (val)
#define  MEM_VAL_LITTLE_TO_HOST_32(val)                                                 (val)

#else //                                                           See Note #5.

#error  CPU_CFG_ENDIAN_TYPE illegally #defined in '[arch]_cpu_port.h'. \
  [See '[arch]_cpu_port.h CONFIGURATION ERRORS']

#endif

#define  MEM_VAL_HOST_TO_BIG_16(val)                            MEM_VAL_BIG_TO_HOST_16(val)
#define  MEM_VAL_HOST_TO_BIG_32(val)                            MEM_VAL_BIG_TO_HOST_32(val)
#define  MEM_VAL_HOST_TO_LITTLE_16(val)                         MEM_VAL_LITTLE_TO_HOST_16(val)
#define  MEM_VAL_HOST_TO_LITTLE_32(val)                         MEM_VAL_LITTLE_TO_HOST_32(val)

///< @}

/****************************************************************************************************//**
 *                                           MEM_VAL_GET_INTxxU()
 *
 *
 * @brief    Decode data values from any CPU memory address.
 *
 * @param    addr    Lowest CPU memory address of data value to decode (see Notes #2 & #3).
 *
 * @return   Decoded data value from CPU memory address (see Notes #1 & #3).
 *
 * @note     (1) Decode data values based on the values' data-word order in CPU memory :
 *                   @verbatim
 *                   MEM_VAL_GET_xxx_BIG()           Decode big-   endian data values -- data words' most
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_GET_xxx_LITTLE()        Decode little-endian data values -- data words' least
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_GET_xxx()               Decode data values using CPU's native or configured
 *                                                       data-word order
 *                   @endverbatim
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *
 * @note     (3) MEM_VAL_GET_xxx() macro's decode data values without regard to CPU word-aligned addresses.
 *               Thus for processors that require data word alignment, data words can be decoded from any
 *               CPU address, word-aligned or not, without generating data-word-alignment exceptions/faults.
 *               @n
 *               However, any variable to receive the returned data value MUST start on an appropriate CPU
 *               word-aligned address.
 *               @n
 *               See also 'MEMORY DATA VALUE MACRO'S  Note #1'.
 *
 * @note     (4) MEM_VAL_COPY_GET_xxx() macro's are more efficient than MEM_VAL_GET_xxx() macro's & are
 *               also independent of CPU data-word-alignment & SHOULD be used whenever possible.
 *               @n
 *               See also 'MEM_VAL_COPY_GET_xxx()  Note #4'.
 *
 * @note     (5) MEM_VAL_GET_xxx() macro's are NOT atomic operations & MUST NOT be used on any non-static
 *               (i.e. volatile) variables, registers, hardware, etc.; without the caller of the macro's
 *               providing some form of additional protection (e.g. mutual exclusion).
 *
 * @note     (6) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
 *               linked since each '[arch]_cpu_port.h.h' SHOULD ensure that the CPU data-word-memory order
 *               configuration constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate
 *               data-word-memory order value (see '[arch]_cpu_port.h  CPU WORD CONFIGURATION  Note #2').  The
 *               'else'-conditional code is included as an extra precaution in case '[arch]_cpu_port.h' is
 *               incorrectly configured.
 * @{
 *******************************************************************************************************/

#define  MEM_VAL_GET_INT08U_BIG(addr)           ((CPU_INT08U) ((CPU_INT08U)(((CPU_INT08U)(*(((CPU_INT08U *)(addr)) + 0))) << (0u * DEF_OCTET_NBR_BITS))))

#define  MEM_VAL_GET_INT16U_BIG(addr)           ((CPU_INT16U)(((CPU_INT16U)(((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 0))) << (1u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT16U)(((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 1))) << (0u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_GET_INT32U_BIG(addr)           ((CPU_INT32U)(((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 0))) << (3u * DEF_OCTET_NBR_BITS)))   \
                                                              + ((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 1))) << (2u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 2))) << (1u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 3))) << (0u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_GET_INT64U_BIG(addr)           ((CPU_INT64U)(((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 0))) << (7u * DEF_OCTET_NBR_BITS)))   \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 1))) << (6u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 2))) << (5u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 3))) << (4u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 4))) << (3u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 5))) << (2u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 6))) << (1u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 7))) << (0u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_GET_INT08U_LITTLE(addr)        ((CPU_INT08U) ((CPU_INT08U)(((CPU_INT08U)(*(((CPU_INT08U *)(addr)) + 0))) << (0u * DEF_OCTET_NBR_BITS))))

#define  MEM_VAL_GET_INT16U_LITTLE(addr)        ((CPU_INT16U)(((CPU_INT16U)(((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 0))) << (0u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT16U)(((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 1))) << (1u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_GET_INT32U_LITTLE(addr)        ((CPU_INT32U)(((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 0))) << (0u * DEF_OCTET_NBR_BITS)))   \
                                                              + ((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 1))) << (1u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 2))) << (2u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT32U)(((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 3))) << (3u * DEF_OCTET_NBR_BITS)))))

#define  MEM_VAL_GET_INT64U_LITTLE(addr)        ((CPU_INT64U)(((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 0))) << (0u * DEF_OCTET_NBR_BITS)))   \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 1))) << (1u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 2))) << (2u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 3))) << (3u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 4))) << (4u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 5))) << (5u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 6))) << (6u * DEF_OCTET_NBR_BITS))) \
                                                              + ((CPU_INT64U)(((CPU_INT64U)(*(((CPU_INT08U *)(addr)) + 7))) << (7u * DEF_OCTET_NBR_BITS)))))

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_GET_INT08U(addr)                               MEM_VAL_GET_INT08U_BIG(addr)
#define  MEM_VAL_GET_INT16U(addr)                               MEM_VAL_GET_INT16U_BIG(addr)
#define  MEM_VAL_GET_INT32U(addr)                               MEM_VAL_GET_INT32U_BIG(addr)
#define  MEM_VAL_GET_INT64U(addr)                               MEM_VAL_GET_INT64U_BIG(addr)

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_GET_INT08U(addr)                               MEM_VAL_GET_INT08U_LITTLE(addr)
#define  MEM_VAL_GET_INT16U(addr)                               MEM_VAL_GET_INT16U_LITTLE(addr)
#define  MEM_VAL_GET_INT32U(addr)                               MEM_VAL_GET_INT32U_LITTLE(addr)
#define  MEM_VAL_GET_INT64U(addr)                               MEM_VAL_GET_INT64U_LITTLE(addr)

#else //                                                           See Note #6.

#error  CPU_CFG_ENDIAN_TYPE illegally #defined in '[arch]_cpu_port.h'. \
  [See '[arch]_cpu_port.h CONFIGURATION ERRORS']

#endif
///< @}

/****************************************************************************************************//**
 *                                           MEM_VAL_SET_INTxxU()
 *
 *
 * @brief    Encode data values to any CPU memory address.
 *
 * @param    addr    Lowest CPU memory address to encode data value (see Notes #2 & #3).
 *
 * @param    val     Data value to encode (see Notes #1 & #3).
 *
 * @note     (1) Encode data values into CPU memory based on the values' data-word order :
 *                   @verbatim
 *                   MEM_VAL_SET_xxx_BIG()           Encode big-   endian data values -- data words' most
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_SET_xxx_LITTLE()        Encode little-endian data values -- data words' least
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_SET_xxx()               Encode data values using CPU's native or configured
 *                                                       data-word order
 *                   @endverbatim
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *
 * @note     (3) MEM_VAL_SET_xxx() macro's encode data values without regard to CPU word-aligned addresses.
 *               Thus for processors that require data word alignment, data words can be encoded to any
 *               CPU address, word-aligned or not, without generating data-word-alignment exceptions/faults.
 *               @n
 *               However, 'val' data value to encode MUST start on an appropriate CPU word-aligned address.
 *               @n
 *               See also 'MEMORY DATA VALUE MACRO'S  Note #1'.
 *
 * @note     (4) MEM_VAL_COPY_SET_xxx() macro's are more efficient than MEM_VAL_SET_xxx() macro's & are
 *               also independent of CPU data-word-alignment & SHOULD be used whenever possible.
 *               @n
 *               See also 'MEM_VAL_COPY_SET_xxx()  Note #4'.
 *
 * @note     (5) MEM_VAL_SET_xxx() macro's are NOT atomic operations & MUST NOT be used on any non-static
 *               (i.e. volatile) variables, registers, hardware, etc.; without the caller of the macro's
 *               providing some form of additional protection (e.g. mutual exclusion).
 *
 * @note     (6) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
 *               linked since each '[arch]_cpu_port.h' SHOULD ensure that the CPU data-word-memory order
 *               configuration constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate
 *               data-word-memory order value (see '[arch]_cpu_port.h  CPU WORD CONFIGURATION  Note #2').
 *               The 'else'-conditional code is included as an extra precaution in case '[arch]_cpu_port.h'
 *               is incorrectly configured.
 * @{
 *******************************************************************************************************/

#define  MEM_VAL_SET_INT08U_BIG(addr, val)                     do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT08U)(val)) & (CPU_INT08U)              0xFFu) >> (0u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT16U_BIG(addr, val)                     do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT16U)(val)) & (CPU_INT16U)            0xFF00u) >> (1u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT16U)(val)) & (CPU_INT16U)            0x00FFu) >> (0u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT32U_BIG(addr, val)                     do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0xFF000000u) >> (3u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0x00FF0000u) >> (2u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 2)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0x0000FF00u) >> (1u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 3)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0x000000FFu) >> (0u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT64U_BIG(addr, val)                     do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0xFF00000000000000u) >> (7u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x00FF000000000000u) >> (6u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 2)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x0000FF0000000000u) >> (5u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 3)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x000000FF00000000u) >> (4u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 4)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x00000000FF000000u) >> (3u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 5)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x0000000000FF0000u) >> (2u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 6)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x000000000000FF00u) >> (1u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 7)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x00000000000000FFu) >> (0u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT08U_LITTLE(addr, val)                  do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT08U)(val)) & (CPU_INT08U)              0xFFu) >> (0u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT16U_LITTLE(addr, val)                  do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT16U)(val)) & (CPU_INT16U)            0x00FFu) >> (0u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT16U)(val)) & (CPU_INT16U)            0xFF00u) >> (1u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT32U_LITTLE(addr, val)                  do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0x000000FFu) >> (0u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0x0000FF00u) >> (1u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 2)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0x00FF0000u) >> (2u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 3)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & (CPU_INT32U)        0xFF000000u) >> (3u * DEF_OCTET_NBR_BITS))); } while (0)

#define  MEM_VAL_SET_INT64U_LITTLE(addr, val)                  do { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x00000000000000FFu) >> (0u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x000000000000FF00u) >> (1u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 2)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x0000000000FF0000u) >> (2u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 3)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x00000000FF000000u) >> (3u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 4)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x000000FF00000000u) >> (4u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 5)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x0000FF0000000000u) >> (5u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 6)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0x00FF000000000000u) >> (6u * DEF_OCTET_NBR_BITS))); \
                                                                    (*(((CPU_INT08U *)(addr)) + 7)) = ((CPU_INT08U)((((CPU_INT64U)(val)) & (CPU_INT64U)0xFF00000000000000u) >> (7u * DEF_OCTET_NBR_BITS))); } while (0)

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_SET_INT08U(addr, val)                          MEM_VAL_SET_INT08U_BIG((addr), (val))
#define  MEM_VAL_SET_INT16U(addr, val)                          MEM_VAL_SET_INT16U_BIG((addr), (val))
#define  MEM_VAL_SET_INT32U(addr, val)                          MEM_VAL_SET_INT32U_BIG((addr), (val))
#define  MEM_VAL_SET_INT64U(addr, val)                          MEM_VAL_SET_INT64U_BIG((addr), (val))

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_SET_INT08U(addr, val)                          MEM_VAL_SET_INT08U_LITTLE((addr), (val))
#define  MEM_VAL_SET_INT16U(addr, val)                          MEM_VAL_SET_INT16U_LITTLE((addr), (val))
#define  MEM_VAL_SET_INT32U(addr, val)                          MEM_VAL_SET_INT32U_LITTLE((addr), (val))
#define  MEM_VAL_SET_INT64U(addr, val)                          MEM_VAL_SET_INT64U_LITTLE((addr), (val))

#else //                                                           See Note #6.

#error  CPU_CFG_ENDIAN_TYPE illegally #defined in '[arch]_cpu_port.h'. \
  [See '[arch]_cpu_port.h CONFIGURATION ERRORS']

#endif
///< @}

/****************************************************************************************************//**
 *                                       MEM_VAL_COPY_GET_INTxxU_xxx()
 *
 *
 * @brief    Copy & decode data values from any CPU memory address to any CPU memory address.
 *
 * @param    addr_dest   Lowest CPU memory address to copy/decode source address's data value
 *                       (see Notes #2 & #3).
 *
 * @param    addr_src    Lowest CPU memory address of data value to copy/decode
 *                       (see Notes #2 & #3).
 *
 * @note     (1) Copy/decode data values based on the values' data-word order :
 *                   @verbatim
 *                   MEM_VAL_COPY_GET_xxx_BIG()      Decode big-   endian data values -- data words' most
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_GET_xxx_LITTLE()   Decode little-endian data values -- data words' least
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_GET_xxx()          Decode data values using CPU's native or configured
 *                                                       data-word order
 *                   @endverbatim
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *               CPU memory addresses/buffers  NOT checked for overlapping.
 *               - (a) IEEE Std 1003.1, 2004 Edition, Section 'memcpy() : DESCRIPTION' states that
 *                       "copying ... between objects that overlap ... is undefined".
 *
 * @note     (3) MEM_VAL_COPY_GET_xxx() macro's copy/decode data values without regard to CPU word-aligned
 *               addresses.  Thus for processors that require data word alignment, data words can be copied/
 *               decoded to/from any CPU address, word-aligned or not, without generating data-word-alignment
 *               exceptions/faults.
 *
 * @note     (4) MEM_VAL_COPY_GET_xxx() macro's are more efficient than MEM_VAL_GET_xxx() macro's & are
 *               also independent of CPU data-word-alignment & SHOULD be used whenever possible.
 *               See also 'MEM_VAL_GET_xxx()  Note #4'.
 *
 * @note     (5) Since octet-order copy/conversion are inverse operations, MEM_VAL_COPY_GET_xxx() &
 *               MEM_VAL_COPY_SET_xxx() macros are inverse, but identical, operations & are provided
 *               in both forms for semantics & consistency.
 *               See also 'MEM_VAL_COPY_SET_xxx()  Note #5'.
 *
 * @note     (6) MEM_VAL_COPY_GET_xxx() macro's are NOT atomic operations & MUST NOT be used on any non-
 *               static (i.e. volatile) variables, registers, hardware, etc.; without the caller of the
 *               macro's providing some form of additional protection (e.g. mutual exclusion).
 *
 * @note     (7) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
 *               linked since each '[arch]_cpu_port.h' SHOULD ensure that the CPU data-word-memory order
 *               configuration constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate
 *               data-word-memory order value (see '[arch]_cpu_port.h  CPU WORD CONFIGURATION  Note #2').
 *               The 'else'-conditional code is included as an extra precaution in case '[arch]_cpu_port.h'
 *               is incorrectly configured.
 * @{
 *******************************************************************************************************/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)      do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)      do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); } while (0)

#define  MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)      do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 3)); } while (0)

#define  MEM_VAL_COPY_GET_INT08U_LITTLE(addr_dest, addr_src)   do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT16U_LITTLE(addr_dest, addr_src)   do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT32U_LITTLE(addr_dest, addr_src)   do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 3)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT08U(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)
#define  MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)
#define  MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)      do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)      do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)      do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 3)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT08U_LITTLE(addr_dest, addr_src)   do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_GET_INT16U_LITTLE(addr_dest, addr_src)   do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); } while (0)

#define  MEM_VAL_COPY_GET_INT32U_LITTLE(addr_dest, addr_src)   do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 3)); } while (0)

#define  MEM_VAL_COPY_GET_INT08U(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT08U_LITTLE((addr_dest), (addr_src))
#define  MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT16U_LITTLE((addr_dest), (addr_src))
#define  MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT32U_LITTLE((addr_dest), (addr_src))

#else //                                                           See Note #7.

#error  CPU_CFG_ENDIAN_TYPE illegally #defined in '[arch]_cpu_port.h'. \
  [See '[arch]_cpu_port.h CONFIGURATION ERRORS']

#endif
///< @}

/****************************************************************************************************//**
 *                                       MEM_VAL_COPY_GET_INTU_xxx()
 *
 *
 * @brief    Copy & decode data values from any CPU memory address to any CPU memory address for
 *               any sized data values.
 *
 * @param    addr_dest   Lowest CPU memory address to copy/decode source address's data value
 *                       (see Notes #2 & #3).
 *
 * @param    addr_src    Lowest CPU memory address of data value to copy/decode
 *                       (see Notes #2 & #3).
 *
 * @param    val_size    Number of data value octets to copy/decode.
 *
 * @note     (1) Copy/decode data values based on the values' data-word order :
 *                   @verbatim
 *                   MEM_VAL_COPY_GET_INTU_BIG()     Decode big-   endian data values -- data words' most
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_GET_INTU_LITTLE()  Decode little-endian data values -- data words' least
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_GET_INTU()         Decode data values using CPU's native or configured
 *                                                       data-word order
 *                   @endverbatim
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *               CPU memory addresses/buffers  NOT checked for overlapping.
 *           - (a) IEEE Std 1003.1, 2004 Edition, Section 'memcpy() : DESCRIPTION' states that
 *                   "copying ... between objects that overlap ... is undefined".
 *
 * @note     (3) MEM_VAL_COPY_GET_INTU_xxx() macro's copy/decode data values without regard to CPU word-
 *               aligned addresses.  Thus for processors that require data word alignment, data words
 *               can be copied/decoded to/from any CPU address, word-aligned or not, without generating
 *               data-word-alignment exceptions/faults.
 *
 * @note     (4) MEM_VAL_COPY_GET_xxx() macro's are more efficient than MEM_VAL_COPY_GET_INTU_xxx()
 *               macro's & SHOULD be used whenever possible.
 *               See also 'MEM_VAL_COPY_GET_xxx()  Note #4'.
 *
 * @note     (5) Since octet-order copy/conversion are inverse operations, MEM_VAL_COPY_GET_INTU_xxx() &
 *               MEM_VAL_COPY_SET_INTU_xxx() macros are inverse, but identical, operations & are provided
 *               in both forms for semantics & consistency.
 *               See also 'MEM_VAL_COPY_SET_INTU_xxx()  Note #5'.
 *
 * @note     (6) MEM_VAL_COPY_GET_INTU_xxx() macro's are NOT atomic operations & MUST NOT be used on any
 *               non-static (i.e. volatile) variables, registers, hardware, etc.; without the caller of
 *               the macro's providing some form of additional protection (e.g. mutual exclusion).
 *
 * @note     (7) MISRA-C 2004 Rule 5.2 states that "identifiers in an inner scope shall not use the same
 *               name as an indentifier in an outer scope, and therefore hide that identifier".
 *               @n
 *               Therefore, to avoid possible redeclaration of commonly-used loop counter identifier names,
 *               'i' & 'j', MEM_VAL_COPY_GET_INTU_xxx() loop counter identifier names are prefixed with a
 *               single underscore.
 *
 * @note     (8) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
 *               linked since each '[arch]_cpu_port.h' SHOULD ensure that the CPU data-word-memory order
 *               configuration constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate
 *               data-word-memory order value (see '[arch]_cpu_port.h  CPU WORD CONFIGURATION  Note #2').
 *               The 'else'-conditional code is included as an extra precaution in case '[arch]_cpu_port.h'
 *               is incorrectly configured.
 * @{
 *******************************************************************************************************/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_COPY_GET_INTU_BIG(addr_dest, addr_src, val_size)       do {        \
    CPU_SIZE_T _i;                                                                  \
                                                                                    \
    for (_i = 0; _i < (val_size); _i++) {                                           \
      (*(((CPU_INT08U *)(addr_dest)) + _i)) = (*(((CPU_INT08U *)(addr_src)) + _i)); \
    }                                                                               \
} while (0)

#define  MEM_VAL_COPY_GET_INTU_LITTLE(addr_dest, addr_src, val_size)    do {        \
    CPU_SIZE_T _i;                                                                  \
    CPU_SIZE_T _j;                                                                  \
                                                                                    \
                                                                                    \
    _j = (val_size) - 1;                                                            \
                                                                                    \
    for (_i = 0; _i < (val_size); _i++) {                                           \
      (*(((CPU_INT08U *)(addr_dest)) + _i)) = (*(((CPU_INT08U *)(addr_src)) + _j)); \
      _j--;                                                                         \
    }                                                                               \
} while (0)

#define  MEM_VAL_COPY_GET_INTU(addr_dest, addr_src, val_size)           MEM_VAL_COPY_GET_INTU_BIG((addr_dest), (addr_src), (val_size))

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_COPY_GET_INTU_BIG(addr_dest, addr_src, val_size)       do {        \
    CPU_SIZE_T _i;                                                                  \
    CPU_SIZE_T _j;                                                                  \
                                                                                    \
                                                                                    \
    _j = (val_size) - 1;                                                            \
                                                                                    \
    for (_i = 0; _i < (val_size); _i++) {                                           \
      (*(((CPU_INT08U *)(addr_dest)) + _i)) = (*(((CPU_INT08U *)(addr_src)) + _j)); \
      _j--;                                                                         \
    }                                                                               \
} while (0)

#define  MEM_VAL_COPY_GET_INTU_LITTLE(addr_dest, addr_src, val_size)    do {        \
    CPU_SIZE_T _i;                                                                  \
                                                                                    \
    (*((CPU_INT08U *)(addr_dest))) = (*((CPU_INT08U *)(addr_src)));                 \
    for (_i = 1u; _i < (val_size); _i++) {                                          \
      (*(((CPU_INT08U *)(addr_dest)) + _i)) = (*(((CPU_INT08U *)(addr_src)) + _i)); \
    }                                                                               \
} while (0)

#define  MEM_VAL_COPY_GET_INTU(addr_dest, addr_src, val_size)           MEM_VAL_COPY_GET_INTU_LITTLE((addr_dest), (addr_src), (val_size))

#else //                                                           See Note #8.

#error  CPU_CFG_ENDIAN_TYPE illegally #defined in '[arch]_cpu_port.h'. \
  [See '[arch]_cpu_port.h CONFIGURATION ERRORS']

#endif

///< @}

/****************************************************************************************************//**
 *                                       MEM_VAL_COPY_SET_INTxxU_xxx()
 *
 *
 * @brief    Copy & encode data values from any CPU memory address to any CPU memory address.
 *
 * @param    addr_dest   Lowest CPU memory address to copy/encode source address's data value
 *                       (see Notes #2 & #3).
 *
 * @param    addr_src    Lowest CPU memory address of data value to copy/encode
 *                       (see Notes #2 & #3).
 *
 * @note     (1) Copy/encode data values based on the values' data-word order :
 *                   @verbatim
 *                   MEM_VAL_COPY_SET_xxx_BIG()      Encode big-   endian data values -- data words' most
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_SET_xxx_LITTLE()   Encode little-endian data values -- data words' least
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_SET_xxx()          Encode data values using CPU's native or configured
 *                                                       data-word order
 *                   @endverbatim
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *               CPU memory addresses/buffers  NOT checked for overlapping.
 *           - (a) IEEE Std 1003.1, 2004 Edition, Section 'memcpy() : DESCRIPTION' states that
 *                   "copying ... between objects that overlap ... is undefined".
 *
 * @note     (3) MEM_VAL_COPY_SET_xxx() macro's copy/encode data values without regard to CPU word-aligned
 *               addresses.  Thus for processors that require data word alignment, data words can be copied/
 *               encoded to/from any CPU address, word-aligned or not, without generating data-word-alignment
 *               exceptions/faults.
 *
 * @note     (4) MEM_VAL_COPY_SET_xxx() macro's are more efficient than MEM_VAL_SET_xxx() macro's & are
 *               also independent of CPU data-word-alignment & SHOULD be used whenever possible.
 *               See also 'MEM_VAL_SET_xxx()  Note #4'.
 *
 * @note     (5) Since octet-order copy/conversion are inverse operations, MEM_VAL_COPY_GET_xxx() &
 *               MEM_VAL_COPY_SET_xxx() macros are inverse, but identical, operations & are provided
 *               in both forms for semantics & consistency.
 *               See also 'MEM_VAL_COPY_GET_xxx()  Note #5'.
 *
 * @note     (6) MEM_VAL_COPY_SET_xxx() macro's are NOT atomic operations & MUST NOT be used on any
 *               non-static (i.e. volatile) variables, registers, hardware, etc.; without the caller
 *               of the  macro's providing some form of additional protection (e.g. mutual exclusion).
 * @{
 *******************************************************************************************************/
//                                                                 See Note #5.
#define  MEM_VAL_COPY_SET_INT08U_BIG(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT08U_BIG((addr_dest), (addr_src))
#define  MEM_VAL_COPY_SET_INT16U_BIG(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT16U_BIG((addr_dest), (addr_src))
#define  MEM_VAL_COPY_SET_INT32U_BIG(addr_dest, addr_src)               MEM_VAL_COPY_GET_INT32U_BIG((addr_dest), (addr_src))

#define  MEM_VAL_COPY_SET_INT08U_LITTLE(addr_dest, addr_src)            MEM_VAL_COPY_GET_INT08U_LITTLE((addr_dest), (addr_src))
#define  MEM_VAL_COPY_SET_INT16U_LITTLE(addr_dest, addr_src)            MEM_VAL_COPY_GET_INT16U_LITTLE((addr_dest), (addr_src))
#define  MEM_VAL_COPY_SET_INT32U_LITTLE(addr_dest, addr_src)            MEM_VAL_COPY_GET_INT32U_LITTLE((addr_dest), (addr_src))

#define  MEM_VAL_COPY_SET_INT08U(addr_dest, addr_src)                   MEM_VAL_COPY_GET_INT08U((addr_dest), (addr_src))
#define  MEM_VAL_COPY_SET_INT16U(addr_dest, addr_src)                   MEM_VAL_COPY_GET_INT16U((addr_dest), (addr_src))
#define  MEM_VAL_COPY_SET_INT32U(addr_dest, addr_src)                   MEM_VAL_COPY_GET_INT32U((addr_dest), (addr_src))
///< @}

/****************************************************************************************************//**
 *                                       MEM_VAL_COPY_SET_INTU_xxx()
 *
 *
 * @brief    Copy & encode data values from any CPU memory address to any CPU memory address for
 *               any sized data values.
 *
 * @param    addr_dest   Lowest CPU memory address to copy/encode source address's data value
 *                       (see Notes #2 & #3).
 *
 * @param    addr_src    Lowest CPU memory address of data value to copy/encode
 *                       (see Notes #2 & #3).
 *
 * @param    val_size    Number of data value octets to copy/encode.
 *
 * @note     (1) Copy/encode data values based on the values' data-word order :
 *                   @verbatim
 *                   MEM_VAL_COPY_SET_INTU_BIG()     Encode big-   endian data values -- data words' most
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_SET_INTU_LITTLE()  Encode little-endian data values -- data words' least
 *                                                       significant octet @ lowest memory address
 *                   MEM_VAL_COPY_SET_INTU()         Encode data values using CPU's native or configured
 *                                                       data-word order
 *                   @endverbatim
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *               CPU memory addresses/buffers  NOT checked for overlapping.
 *           - (a) IEEE Std 1003.1, 2004 Edition, Section 'memcpy() : DESCRIPTION' states that
 *                   "copying ... between objects that overlap ... is undefined".
 *
 * @note     (3) MEM_VAL_COPY_SET_INTU_xxx() macro's copy/encode data values without regard to CPU word-
 *               aligned addresses.  Thus for processors that require data word alignment, data words
 *               can be copied/encoded to/from any CPU address, word-aligned or not, without generating
 *               data-word-alignment exceptions/faults.
 *
 * @note     (4) MEM_VAL_COPY_SET_xxx() macro's are more efficient than MEM_VAL_COPY_SET_INTU_xxx()
 *               macro's & SHOULD be used whenever possible.
 *               See also 'MEM_VAL_COPY_SET_xxx()  Note #4'.
 *
 * @note     (5) Since octet-order copy/conversion are inverse operations, MEM_VAL_COPY_GET_INTU_xxx() &
 *               MEM_VAL_COPY_SET_INTU_xxx() macros are inverse, but identical, operations & are provided
 *               in both forms for semantics & consistency.
 *               See also 'MEM_VAL_COPY_GET_INTU_xxx()  Note #5'.
 *
 * @note     (6) MEM_VAL_COPY_SET_INTU_xxx() macro's are NOT atomic operations & MUST NOT be used on any
 *               non-static (i.e. volatile) variables, registers, hardware, etc.; without the caller of
 *               the macro's providing some form of additional protection (e.g. mutual exclusion).
 * @{
 *******************************************************************************************************/
//                                                                 See Note #5.
#define  MEM_VAL_COPY_SET_INTU_BIG(addr_dest, addr_src, val_size)       MEM_VAL_COPY_GET_INTU_BIG((addr_dest), (addr_src), (val_size))
#define  MEM_VAL_COPY_SET_INTU_LITTLE(addr_dest, addr_src, val_size)    MEM_VAL_COPY_GET_INTU_LITTLE((addr_dest), (addr_src), (val_size))
#define  MEM_VAL_COPY_SET_INTU(addr_dest, addr_src, val_size)           MEM_VAL_COPY_GET_INTU((addr_dest), (addr_src), (val_size))

///< @}

/****************************************************************************************************//**
 *                                           MEM_VAL_COPY_xxx()
 *
 * @brief    Copy data values from any CPU memory address to any CPU memory address.
 *
 * @param    addr_dest   Lowest CPU memory address to copy source address's data value
 *                       (see Notes #2 & #3).
 *
 * @param    addr_src    Lowest CPU memory address of data value to copy
 *                       (see Notes #2 & #3).
 *
 * @param    val_size    Number of data value octets to copy.
 *
 * @note     (1) MEM_VAL_COPY_xxx() macro's copy data values based on CPU's native data-word order.
 *
 * @note     (2) CPU memory addresses/pointers NOT checked for NULL.
 *               CPU memory addresses/buffers  NOT checked for overlapping.
 *           - (a) IEEE Std 1003.1, 2004 Edition, Section 'memcpy() : DESCRIPTION' states that
 *                   "copying ... between objects that overlap ... is undefined".
 *
 * @note     (3) MEM_VAL_COPY_xxx() macro's copy data values without regard to CPU word-aligned addresses.
 *               Thus for processors that require data word alignment, data words can be copied to/from any
 *               CPU address, word-aligned or not, without generating data-word-alignment exceptions/faults.
 *
 * @note     (4) MEM_VAL_COPY_xxx() macro's are more efficient than MEM_VAL_COPY() macro & SHOULD be
 *               used whenever possible.
 *
 * @note     (5) MEM_VAL_COPY_xxx() macro's are NOT atomic operations & MUST NOT be used on any non-static
 *               (i.e. volatile) variables, registers, hardware, etc.; without the caller of the macro's
 *               providing some form of additional protection (e.g. mutual exclusion).
 *
 * @note     (6) MISRA-C 2004 Rule 5.2 states that "identifiers in an inner scope shall not use the same
 *               name as an indentifier in an outer scope, and therefore hide that identifier".
 *               @n
 *               Therefore, to avoid possible redeclaration of commonly-used loop counter identifier name,
 *               'i', MEM_VAL_COPY() loop counter identifier name is prefixed with a single underscore.
 * @{
 *******************************************************************************************************/

#define  MEM_VAL_COPY_08(addr_dest, addr_src)                  do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); } while (0)

#define  MEM_VAL_COPY_16(addr_dest, addr_src)                  do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); } while (0)

#define  MEM_VAL_COPY_32(addr_dest, addr_src)                  do { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 3)); } while (0)

#define  MEM_VAL_COPY(addr_dest, addr_src, val_size)        do {                    \
    CPU_SIZE_T _i;                                                                  \
                                                                                    \
    for (_i = 0; _i < (val_size); _i++) {                                           \
      (*(((CPU_INT08U *)(addr_dest)) + _i)) = (*(((CPU_INT08U *)(addr_src)) + _i)); \
    }                                                                               \
} while (0)

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void Mem_Init(void);

/********************************************************************************************************
 *                                       MEMORY OPERATION API FNCTS
 *******************************************************************************************************/

#if (LIB_MEM_CFG_STD_C_LIB_EN == DEF_DISABLED)
void Mem_Clr(void       *p_mem,
             CPU_SIZE_T size);

void Mem_Set(void       *p_mem,
             CPU_INT08U data_val,
             CPU_SIZE_T size);

LIB_MEM_COPY_FNCT_PREFIX void Mem_Copy(void       *p_dest,
                                       const void *p_src,
                                       CPU_SIZE_T size);

void Mem_Move(void       *p_dest,
              const void *p_src,
              CPU_SIZE_T size);

CPU_BOOLEAN Mem_Cmp(const void *p1_mem,
                    const void *p2_mem,
                    CPU_SIZE_T size);

#endif

/********************************************************************************************************
 *                                   MEMORY SEGMENT ALLOCATION API FNCTS
 *******************************************************************************************************/

void Mem_SegCreate(const CPU_CHAR *p_name,
                   MEM_SEG        *p_seg,
                   CPU_ADDR       seg_base_addr,
                   CPU_SIZE_T     size,
                   CPU_SIZE_T     padding_align,
                   RTOS_ERR       *p_err);

void Mem_SegReg(MEM_SEG  *p_seg,
                RTOS_ERR *p_err);

void Mem_SegClr(MEM_SEG  *p_seg,
                RTOS_ERR *p_err);

void *Mem_SegAlloc(const CPU_CHAR *p_name,
                   MEM_SEG        *p_seg,
                   CPU_SIZE_T     size,
                   RTOS_ERR       *p_err);

void *Mem_SegAllocExt(const CPU_CHAR *p_name,
                      MEM_SEG        *p_seg,
                      CPU_SIZE_T     size,
                      CPU_SIZE_T     align,
                      CPU_SIZE_T     *p_bytes_reqd,
                      RTOS_ERR       *p_err);

void *Mem_SegAllocHW(const CPU_CHAR *p_name,
                     MEM_SEG        *p_seg,
                     CPU_SIZE_T     size,
                     CPU_SIZE_T     align,
                     CPU_SIZE_T     *p_bytes_reqd,
                     RTOS_ERR       *p_err);

CPU_SIZE_T Mem_SegRemSizeGet(MEM_SEG      *p_seg,
                             CPU_SIZE_T   align,
                             MEM_SEG_INFO *p_seg_info,
                             RTOS_ERR     *p_err);

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
void Mem_SegTotalUsageGet(CPU_SIZE_T *p_used,
                          CPU_SIZE_T *p_free,
                          RTOS_ERR   *p_err);

void Mem_OutputUsage(void (*out_fnct)(CPU_CHAR *p_str),
                     RTOS_ERR *p_err);
#endif

/********************************************************************************************************
 *                                   MEMORY DYNAMIC POOL ALLOCATION API FNCTS
 *******************************************************************************************************/
//                                                                 -------------- DYNAMIC MEM POOL FNCTS --------------
void Mem_DynPoolCreate(const CPU_CHAR *p_name,
                       MEM_DYN_POOL   *p_pool,
                       MEM_SEG        *p_seg,
                       CPU_SIZE_T     blk_size,
                       CPU_SIZE_T     blk_align,
                       CPU_SIZE_T     blk_qty_init,
                       CPU_SIZE_T     blk_qty_max,
                       RTOS_ERR       *p_err);

void Mem_DynPoolCreatePersistent(const CPU_CHAR          *p_name,
                                 MEM_DYN_POOL            *p_pool,
                                 MEM_SEG                 *p_seg,
                                 CPU_SIZE_T              blk_size,
                                 CPU_SIZE_T              blk_align,
                                 CPU_SIZE_T              blk_qty_init,
                                 CPU_SIZE_T              blk_qty_max,
                                 MEM_DYN_POOL_ALLOC_FNCT alloc_callback,
                                 void                    *p_callback_arg,
                                 RTOS_ERR                *p_err);

void Mem_DynPoolCreateHW(const CPU_CHAR *p_name,
                         MEM_DYN_POOL   *p_pool,
                         MEM_SEG        *p_seg,
                         CPU_SIZE_T     blk_size,
                         CPU_SIZE_T     blk_align,
                         CPU_SIZE_T     blk_qty_init,
                         CPU_SIZE_T     blk_qty_max,
                         RTOS_ERR       *p_err);

void *Mem_DynPoolBlkGet(MEM_DYN_POOL *p_pool,
                        RTOS_ERR     *p_err);

void Mem_DynPoolBlkFree(MEM_DYN_POOL *p_pool,
                        void         *p_blk,
                        RTOS_ERR     *p_err);

CPU_SIZE_T Mem_DynPoolBlkNbrAvailGet(MEM_DYN_POOL      *p_pool,
                                     MEM_DYN_POOL_INFO *p_pool_info,
                                     RTOS_ERR          *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 *
 *  Note(s) : (1) LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN cannot be enabled when the standard C library is
 *                   used because we cannot know which function is called by the standard function. Example,
 *                   memmove() might call memcpy() instead of Mem_Copy().
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  LIB_MEM_CFG_HEAP_SIZE
#error  LIB_MEM_CFG_HEAP_SIZE not #defined in 'common_cfg.h'. MUST be >= 0.
#endif

#if (defined(LIB_MEM_CFG_HEAP_BASE_ADDR_EN)) \
  && (LIB_MEM_CFG_HEAP_BASE_ADDR_EN == 1)    \
  && (!defined(LIB_MEM_CFG_HEAP_BASE_ADDR))
#error  LIB_MEM_CFG_HEAP_BASE_ADDR not #defined. MUST be defined when LIB_MEM_CFG_HEAP_BASE_ADDR_EN is enabled.
#endif

#ifdef   LIB_MEM_CFG_HEAP_BASE_ADDR
#if     (LIB_MEM_CFG_HEAP_BASE_ADDR == 0x0)
#error  LIB_MEM_CFG_HEAP_BASE_ADDR illegally #defined in 'common_cfg.h'. MUST be > 0x0.
#endif
#endif

#if    ((LIB_MEM_CFG_DBG_INFO_EN != DEF_DISABLED) \
  && (LIB_MEM_CFG_DBG_INFO_EN != DEF_ENABLED))
#error  LIB_MEM_CFG_DBG_INFO_EN illegally defined in 'common_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED.
#elif  ((LIB_MEM_CFG_HEAP_SIZE == 0u) \
  && (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED))
#error  LIB_MEM_CFG_HEAP_SIZE illegally defined in 'common_cfg.h'. MUST be > 0 when LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED.
#endif

#if ((LIB_MEM_CFG_STD_C_LIB_EN == DEF_ENABLED) \
  && (LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN == DEF_ENABLED))
//                                                                 See Note 1.
#error  Invalid configuration LIB_MEM_CFG_STD_C_LIB_EN and LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN cannot be enabled at the same time.
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of lib mem module include.
