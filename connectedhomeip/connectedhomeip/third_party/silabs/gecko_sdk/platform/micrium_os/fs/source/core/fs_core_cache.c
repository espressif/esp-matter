/***************************************************************************//**
 * @file
 * @brief File System - Core Cache Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/core/fs_core_priv.h>

#include  <fs/source/core/fs_core_list_priv.h>

#include  <fs/source/core/fs_core_buddy_alloc_priv.h>
#include  <fs/source/core/fs_core_job_sched_priv.h>

#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/shared/crc/edc_crc.h>

#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/source/collections/bitmap_priv.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR     RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH         (FS, CORE, CACHE)

#define  FS_CACHE_BLK_STATUS_NONE          DEF_BIT_NONE
#define  FS_CACHE_BLK_STATUS_DIRTY         DEF_BIT_00
#define  FS_CACHE_BLK_STATUS_ORDERED       DEF_BIT_01
#define  FS_CACHE_BLK_STATUS_VALID         DEF_BIT_02
#define  FS_CACHE_BLK_STATUS_STUB          DEF_BIT_03

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_CACHE_BLK_DESC_GET(p_cache, ix)               (&(p_cache)->BlkDescTbl[ix])

#define  FS_CACHE_BLK_DESC_IX_GET(p_cache, p_blk_desc)    ((p_blk_desc) - &(p_cache)->BlkDescTbl[0])

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

struct fs_cache {
  FS_CACHE_BLK_DESC *BlkDescTbl;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  JOB_SCHED         JobSched;
#endif
  BUDDY_ALLOC       BlkAlloc;
  FS_CACHE_WR_CB    WrFunc;
  IB_SLIST          BlkList;
  IB_SLIST_END      UsedListEnd;
  KAL_LOCK_HANDLE   BlkAcquireMutexHandle;
#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_CACHE_STAT     Stat;
#endif
};

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
typedef struct fs_cache_wr_job_exec_func_data {
  FS_CACHE    *CachePtr;
  CPU_BOOLEAN Invalidate;
} FS_CACHE_WR_JOB_EXEC_FUNC_DATA;
#endif

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
static MEM_DYN_POOL FSCache_GraphNodeDbgDataPool;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                               EXTERNALLY DEFINED DEBUG FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
void GraphDbgOutput_NodeAdd(CPU_INT64U node_id,
                            CPU_INT32U lb_nbr,
                            CPU_INT08U lb_type,
                            CPU_INT08U lb_state,
                            CPU_SIZE_T node_size);

void GraphDbgOutput_NodeEdit(CPU_INT64U node_id,
                             CPU_INT32U lb_nbr,
                             CPU_INT08U lb_type,
                             CPU_INT08U lb_state,
                             CPU_SIZE_T node_size);

void GraphDbgOutput_NodeRem(CPU_INT64U node_id);

void GraphDbgOutput_EdgeAdd(CPU_INT64U prev_node_id,
                            CPU_INT64U next_node_id);

void GraphDbgOutput_EdgeRem(CPU_INT64U prev_node_id,
                            CPU_INT64U next_node_id);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_CACHE_WR_JOB_HANDLE FSCache_VoidWrJobHandle = FS_CACHE_WR_JOB_HANDLE_INIT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
static CPU_INT32U FSCache_CurGraphNodeId = 0u;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FSCache_BlkEvict(FS_CACHE *p_cache,
                             RTOS_ERR *p_err);

static void FSCache_BlkFlushInternal(FS_CACHE          *p_cache,
                                     FS_CACHE_BLK_DESC *p_blk_desc,
                                     CPU_BOOLEAN       invalidate,
                                     RTOS_ERR          *p_err);

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
static void FSCache_WrJobExecCb(void     *p_job_data,
                                void     *p_caller_data,
                                void     *p_dbg_data,
                                RTOS_ERR *p_err);
#endif

static CPU_BOOLEAN FSCache_FlushPredCb(FS_CACHE          *p_cache,
                                       FS_CACHE_BLK_DESC *p_blk_desc,
                                       void              *p_flush_pred_data);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSCache_WrCb(FS_BLK_DEV_HANDLE blk_dev_handle,
                         CPU_INT08U        *p_blk,
                         FS_LB_NBR         lb_nbr,
                         RTOS_ERR          *p_err);
#endif

static FS_CACHE_BLK_NBR FSCache_BlkPtrToIx(FS_CACHE   *p_cache,
                                           CPU_INT08U *p_blk);

static CPU_INT08U *FSCache_BlkIxToPtr(FS_CACHE         *p_cache,
                                      FS_CACHE_BLK_NBR blk_ix);

static CPU_BOOLEAN FSCache_BlkGetInternal(FS_CACHE          *p_cache,
                                          FS_BLK_DEV_HANDLE blk_dev_handle,
                                          FS_LB_NBR         lb_nbr,
                                          CPU_INT08U        lb_type,
                                          CPU_BOOLEAN       is_dirty,
                                          CPU_INT08U        **pp_blk,
                                          RTOS_ERR          *p_err);

static FS_CACHE_BLK_DESC *FSCache_BlkFind(FS_CACHE          *p_cache,
                                          FS_BLK_DEV_HANDLE blk_dev_handle,
                                          FS_LB_NBR         lb_nbr);

static void FSCache_OnBlkDevClose(FS_BLK_DEV_HANDLE blk_dev_handle,
                                  RTOS_ERR          *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL DEBUG FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
static void FSCache_GraphNodeAdd(FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data);

static void FSCache_GraphNodeEdit(FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data);

static void FSCache_GraphNodeRem(FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data);

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
static void FSCache_GraphEdgeAdd(void *p_prev_dbg_data_v,
                                 void *p_next_dbg_data_v);

static void FSCache_GraphEdgeRem(void *p_prev_dbg_data_v,
                                 void *p_next_dbg_data_v);
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSCache_Create()
 *
 * @brief    Create a cache instance.
 *
 * @param    p_cache_cfg     Pointer to a cache configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Pointer to a cache instance.
 *******************************************************************************************************/
FS_CACHE *FSCache_Create(const FS_CACHE_CFG *p_cache_cfg,
                         RTOS_ERR           *p_err)
{
  FS_CACHE              *p_cache = DEF_NULL;
  FS_CACHE_CFG_INTERNAL cache_cfg;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- ARG CHK ----------------------
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_cache_cfg != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

    //                                                             Check that Core layer is initialized.
    CORE_ENTER_ATOMIC();
    if (!FSCore_Data.IsInit) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      CORE_EXIT_ATOMIC();
      break;
    }
    CORE_EXIT_ATOMIC();

    //                                                             --------------------- CFG CHK ----------------------
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
    RTOS_ASSERT_DBG_ERR_SET(p_cache_cfg->MinBlkCnt > 0u, *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    RTOS_ASSERT_DBG_ERR_SET(FS_UTIL_IS_PWR2(p_cache_cfg->MinLbSize), *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    RTOS_ASSERT_DBG_ERR_SET(FS_UTIL_IS_PWR2(p_cache_cfg->MaxLbSize), *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

    //                                                             ---------------- INTERNAL CFG INIT -----------------
    cache_cfg.Align = p_cache_cfg->Align;
    cache_cfg.EdgeToNodeRatio = 3u;
    cache_cfg.MaxLbSize = p_cache_cfg->MaxLbSize;
    cache_cfg.MetaDataMemSegPtr = FSCore_InitCfg.MemSegPtr;
    cache_cfg.BlkMemSegPtr = p_cache_cfg->BlkMemSegPtr;
    cache_cfg.MinBlkCnt = p_cache_cfg->MinBlkCnt;
    cache_cfg.MinLbSize = p_cache_cfg->MinLbSize;
    cache_cfg.ExtraStubWrJobCnt = 10u;
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
    cache_cfg.WrFunc = FSCache_WrCb;
#else
    cache_cfg.WrFunc = DEF_NULL;
#endif

    //                                                             ------------------- CACHE CREATE -------------------
    BREAK_ON_ERR(p_cache = FSCache_CreateInternal(&cache_cfg, p_err));
  } WITH_SCOPE_END

  return (p_cache);
}

/****************************************************************************************************//**
 *                                               FSCache_Assign()
 *
 * @brief    Bind a block device to a cache instance.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *******************************************************************************************************/
void FSCache_Assign(FS_BLK_DEV_HANDLE blk_dev_handle,
                    FS_CACHE          *p_cache,
                    RTOS_ERR          *p_err)
{
  FS_CACHE_BLK_DEV_DATA *p_blk_dev_data;
  CPU_BOOLEAN           core_media_data_found;
  FS_LB_SIZE            min_lb_size;
  FS_LB_SIZE            max_lb_size;
  FS_LB_SIZE            lb_size;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  min_lb_size = FS_UTIL_PWR2(BuddyAlloc_ZerothOrderBlkSizeLog2Get(&p_cache->BlkAlloc));
  max_lb_size = FS_UTIL_PWR2(BuddyAlloc_ZerothOrderBlkSizeLog2Get(&p_cache->BlkAlloc) + BuddyAlloc_MaxOrderGet(&p_cache->BlkAlloc));

  if ((lb_size < min_lb_size)
      || (lb_size > max_lb_size)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
    return;
  }

  core_media_data_found = DEF_NO;
  SLIST_FOR_EACH_ENTRY(FSCore_Data.CacheBlkDevDataListHeadPtr, p_blk_dev_data, FS_CACHE_BLK_DEV_DATA, ListMember) {
    if ((p_blk_dev_data->BlkDevHandle.BlkDevId == blk_dev_handle.BlkDevId)
        && (p_blk_dev_data->BlkDevHandle.BlkDevPtr == blk_dev_handle.BlkDevPtr)) {
      core_media_data_found = DEF_YES;
      break;
    }
  }

  if (!core_media_data_found) {
    p_blk_dev_data = (FS_CACHE_BLK_DEV_DATA *)Mem_DynPoolBlkGet(&FSCore_Data.CacheBlkDevDataPool, p_err);
    p_blk_dev_data->CachePtr = DEF_NULL;
    p_blk_dev_data->BlkDevHandle = blk_dev_handle;
    SList_Push(&FSCore_Data.CacheBlkDevDataListHeadPtr, &p_blk_dev_data->ListMember);
  }

  if (p_blk_dev_data->CachePtr != DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  p_blk_dev_data->CachePtr = p_cache;
  p_blk_dev_data->IsUnassigning = DEF_NO;

  FSBlkDev_OnCloseCbSet(blk_dev_handle, FSCache_OnBlkDevClose, p_err);
}

/****************************************************************************************************//**
 *                                           FSCache_DfltAssign()
 *
 * @brief    Create a cache instance with default parameters and bind it to a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    cache_blk_cnt   Number of cache blocks to allocate.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *
 * @return   Pointer to the created default cache.
 *
 * @note     (1) This function is a sort of wrapper that uses FSCache_Create() and FSCache_Assign().
 *
 * @note     (2) The default cache parameters are:
 *                   - FS_CACHE_CFG.BlkMemSegPtr = default core memory segment (that is heap region)
 *                   - FS_CACHE_CFG.Align        = default alignment required by block device
 *                   - FS_CACHE_CFG.MaxLbSize    = default block device sector size
 *                   - FS_CACHE_CFG.MinLbSize    = default block device sector size
 *******************************************************************************************************/
FS_CACHE *FSCache_DfltAssign(FS_BLK_DEV_HANDLE blk_dev_handle,
                             CPU_SIZE_T        cache_blk_cnt,
                             RTOS_ERR          *p_err)
{
  FS_CACHE     *p_cache;
  FS_CACHE_CFG cache_cfg;
  FS_LB_SIZE   lb_size;
  CPU_SIZE_T   align_req;
  CORE_DECLARE_IRQ_STATE;

  //                                                               --------------------- ARG CHK ----------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Check that Core layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSCore_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return (DEF_NULL);
  }
  CORE_EXIT_ATOMIC();

  lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  align_req = FSBlkDev_AlignReqGet(blk_dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  cache_cfg.BlkMemSegPtr = FSCore_InitCfg.MemSegPtr;
  cache_cfg.Align = align_req;
  cache_cfg.MaxLbSize = lb_size;
  cache_cfg.MinLbSize = lb_size;
  cache_cfg.MinBlkCnt = cache_blk_cnt;

  p_cache = FSCache_Create(&cache_cfg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  FSCache_Assign(blk_dev_handle, p_cache, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_cache);
}

/****************************************************************************************************//**
 *                                               FSCache_Get()
 *
 * @brief    Get a cache instance from a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @return   Pointer to the associated cache instance or DEF_NULL if cache has not been assigned.
 *
 * @internal
 * @note     (1) [INTERNAL] Any block device function call without any side effect could have been used
 *               here instead of FSBlkDev_LbCntGet().
 * @endinternal
 *******************************************************************************************************/
FS_CACHE *FSCache_Get(FS_BLK_DEV_HANDLE blk_dev_handle)
{
  FS_CACHE_BLK_DEV_DATA *p_core_blk_dev_data;

  p_core_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);

  return (p_core_blk_dev_data->CachePtr);
}

/****************************************************************************************************//**
 *                                           FSCache_MinBlkSizeGet()
 *
 * @brief    Get the minimum block size supported by the given cache instance.
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @return   Minimum supported block size.
 *******************************************************************************************************/
FS_LB_SIZE FSCache_MinBlkSizeGet(FS_CACHE *p_cache)
{
  CPU_INT08U min_lb_size_log2;
  FS_LB_SIZE min_lb_size;

  min_lb_size_log2 = BuddyAlloc_ZerothOrderBlkSizeLog2Get(&p_cache->BlkAlloc);
  min_lb_size = FS_UTIL_PWR2(min_lb_size_log2);

  return (min_lb_size);
}

/****************************************************************************************************//**
 *                                           FSCache_MaxBlkSizeGet()
 *
 * @brief    Get the maximum block size supported by the given cache instance.
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @return   Maximum supported block size.
 *******************************************************************************************************/
FS_LB_SIZE FSCache_MaxBlkSizeGet(FS_CACHE *p_cache)
{
  CPU_INT08U min_lb_size_log2;
  CPU_INT08U max_order;
  FS_LB_SIZE max_lb_size;

  min_lb_size_log2 = BuddyAlloc_ZerothOrderBlkSizeLog2Get(&p_cache->BlkAlloc);
  max_order = BuddyAlloc_MaxOrderGet(&p_cache->BlkAlloc);
  max_lb_size = FS_UTIL_PWR2(min_lb_size_log2 + max_order);

  return (max_lb_size);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSCache_BlkDevDataGet()
 *
 * @brief    Get block device cache data.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @return   Pointer to block device cache data.
 *******************************************************************************************************/
FS_CACHE_BLK_DEV_DATA *FSCache_BlkDevDataGet(FS_BLK_DEV_HANDLE blk_dev_handle)
{
  FS_CACHE_BLK_DEV_DATA *p_blk_dev_data;
  CPU_BOOLEAN           core_media_data_found;

  core_media_data_found = DEF_NO;
  SLIST_FOR_EACH_ENTRY(FSCore_Data.CacheBlkDevDataListHeadPtr, p_blk_dev_data, FS_CACHE_BLK_DEV_DATA, ListMember) {
    if ((p_blk_dev_data->BlkDevHandle.BlkDevId == blk_dev_handle.BlkDevId)
        && (p_blk_dev_data->BlkDevHandle.BlkDevPtr == blk_dev_handle.BlkDevPtr)) {
      core_media_data_found = DEF_YES;
      break;
    }
  }

  if (!core_media_data_found) {
    return (DEF_NULL);
  }

  return (p_blk_dev_data);
}

/****************************************************************************************************//**
 *                                           FSCache_CreateInternal()
 *
 * @brief    Create cache instance.
 *
 * @param    p_cache_cfg     Pointer to a cache configuration structure.
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to the created cache instance.
 *******************************************************************************************************/
FS_CACHE *FSCache_CreateInternal(FS_CACHE_CFG_INTERNAL *p_cache_cfg,
                                 RTOS_ERR              *p_err)
{
  FS_CACHE        *p_cache;
  CPU_SIZE_T      max_size_blk_cnt;
  CPU_SIZE_T      min_size_blk_cnt;
  CPU_SIZE_T      blk_desc_cnt;
  BUDDY_ALLOC_CFG buddy_alloc_cfg;
  CPU_SIZE_T      octets_reqd;
  CPU_SIZE_T      blk_desc_tbl_size;
  CPU_INT08U      max_lb_size_log2;
  CPU_INT08U      min_lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ----------- COMPUTE DATA STRUCTURE SIZES -----------
    max_lb_size_log2 = FSUtil_Log2(p_cache_cfg->MaxLbSize);
    min_lb_size_log2 = FSUtil_Log2(p_cache_cfg->MinLbSize);

    max_size_blk_cnt = p_cache_cfg->MinBlkCnt;
    min_size_blk_cnt = FS_UTIL_MULT_PWR2(max_size_blk_cnt, max_lb_size_log2 - min_lb_size_log2);
    blk_desc_cnt = min_size_blk_cnt;

    RTOS_ASSERT_DBG_ERR_SET(blk_desc_cnt < (FS_CACHE_BLK_NBR)-1,
                            *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

    blk_desc_tbl_size = sizeof(FS_CACHE_BLK_DESC) * blk_desc_cnt;

    //                                                             -------------------- ALLOC MEM ---------------------
    //                                                             Alloc cache metadata.
    BREAK_ON_ERR(p_cache = (FS_CACHE *)Mem_SegAllocExt("FS - Cache metadata",
                                                       p_cache_cfg->MetaDataMemSegPtr,
                                                       sizeof(FS_CACHE),
                                                       sizeof(CPU_ALIGN),
                                                       &octets_reqd,
                                                       p_err));

    //                                                             Alloc block descriptor table.
    BREAK_ON_ERR(p_cache->BlkDescTbl = (FS_CACHE_BLK_DESC *)Mem_SegAllocExt("FS - Cache blk desc table",
                                                                            p_cache_cfg->MetaDataMemSegPtr,
                                                                            blk_desc_tbl_size,
                                                                            sizeof(CPU_ALIGN),
                                                                            &octets_reqd,
                                                                            p_err));

    //                                                             --------------- INIT DATA STRUCTURES ---------------
    p_cache->WrFunc = p_cache_cfg->WrFunc;
#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
    p_cache->Stat.RdRwCnt = 0u;
    p_cache->Stat.MissCnt = 0u;
    p_cache->Stat.HitCnt = 0u;
    p_cache->Stat.SecFlushCnt = 0u;
#endif

    //                                                             ------------- GLOBAL CACHE LOCK CREATE -------------
    BREAK_ON_ERR(p_cache->BlkAcquireMutexHandle = KAL_LockCreate("FS cache block lock",
                                                                 DEF_NULL,
                                                                 p_err));

    //                                                             ------------------ BLK LIST INIT -------------------
    //                                                             Create used/free list.
    BREAK_ON_ERR(IB_SList_Init(&p_cache->BlkList,
                               p_cache_cfg->MetaDataMemSegPtr,
                               blk_desc_cnt,
                               p_err));

    //                                                             Init list heads (free & used)
    IB_SList_EndInit(&p_cache->UsedListEnd);

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    {
      JOB_SCHED_CFG job_sched_cfg;
      CPU_SIZE_T    max_ordering_link_cnt = blk_desc_cnt * p_cache_cfg->EdgeToNodeRatio * 2u;

      job_sched_cfg.ExecFunc = FSCache_WrJobExecCb;
      job_sched_cfg.ExecFuncDataPtr = (void *)p_cache;
      job_sched_cfg.MaxJobCnt = blk_desc_cnt + p_cache_cfg->ExtraStubWrJobCnt;
      job_sched_cfg.MaxOrderingLinkCnt = max_ordering_link_cnt;
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      job_sched_cfg.OnOrderAdd = FSCache_GraphEdgeAdd;
      job_sched_cfg.OnOrderRem = FSCache_GraphEdgeRem;
#endif

      BREAK_ON_ERR(JobSched_Init(&p_cache->JobSched,
                                 &job_sched_cfg,
                                 p_cache_cfg->MetaDataMemSegPtr,
                                 p_err));
    }
#endif

    //                                                             --------------- BLK BUDDY ALLOC INIT ---------------
    buddy_alloc_cfg.DataMemSegPtr = p_cache_cfg->BlkMemSegPtr;
    buddy_alloc_cfg.MetaDataMemSegPtr = p_cache_cfg->MetaDataMemSegPtr;
    buddy_alloc_cfg.Align = p_cache_cfg->Align;
    buddy_alloc_cfg.BlkTbl = DEF_NULL;
    buddy_alloc_cfg.MaxOrder = max_lb_size_log2 - min_lb_size_log2;
    buddy_alloc_cfg.ZerothOrderBlkCnt = min_size_blk_cnt;
    buddy_alloc_cfg.ZerothOrderBlkSizeLog2 = min_lb_size_log2;

    BREAK_ON_ERR(BuddyAlloc_Init(&p_cache->BlkAlloc,
                                 &buddy_alloc_cfg,
                                 p_err));

    //                                                             --------------- DBG DATA POOL CREATE ---------------
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    BREAK_ON_ERR(Mem_DynPoolCreate("FS - Graph node dbg data pool",
                                   &FSCache_GraphNodeDbgDataPool,
                                   DEF_NULL,
                                   sizeof(FS_CACHE_GRAPH_NODE_DBG_DATA),
                                   sizeof(CPU_ALIGN),
                                   0u,
                                   LIB_MEM_BLK_QTY_UNLIMITED,
                                   p_err));
#endif
  } WITH_SCOPE_END

  return (p_cache);
}

/****************************************************************************************************//**
 *                                           FSCache_WrJobSet()
 *
 * @brief    Create a new job and add it to the job scheduler.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    prev_wr_job_handle  Handle to the preceding write job.
 *                               FSCache_VoiWrJobHandle' may be used if no write job must precede.
 *
 * @param    wr_job_handle       Handle to an existing write job.
 *                               If 'FSCache_VoidWrJobHandle' is used a new write job is created.
 *
 * @param    p_blk_desc          Pointer to a cache block descritor to tie to the write job.
 *                               'DEF_NULL' is allowed to create write job stubs for ordering purposes.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Write job handle.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
FS_CACHE_WR_JOB_HANDLE FSCache_WrJobSet(FS_CACHE               *p_cache,
                                        FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                        FS_CACHE_WR_JOB_HANDLE wr_job_handle,
                                        FS_CACHE_BLK_DESC      *p_blk_desc,
                                        RTOS_ERR               *p_err)
{
  void                           *p_dbg_data;
  FS_CACHE_WR_JOB_EXEC_FUNC_DATA cb_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(p_cache != DEF_NULL, *p_err,
                            RTOS_ERR_INVALID_STATE, JobSched_VoidJobHandle);

    if (!JOB_SCHED_JOB_HANDLE_IS_VALID(&p_cache->JobSched, wr_job_handle)) {
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      if (p_blk_desc == DEF_NULL) {
        FS_CACHE_GRAPH_NODE_DBG_DATA *p_node_dbg_data;
        p_dbg_data = Mem_DynPoolBlkGet(&FSCache_GraphNodeDbgDataPool, p_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        p_node_dbg_data = (FS_CACHE_GRAPH_NODE_DBG_DATA *)p_dbg_data;

        p_node_dbg_data->HitCnt = 0u;
        p_node_dbg_data->Id = FSCache_CurGraphNodeId++;
        p_node_dbg_data->LbNbr = (FS_LB_NBR)-1;
        p_node_dbg_data->LbType = (CPU_INT08U)-1;
        p_node_dbg_data->IsWr = DEF_YES;

        FSCache_GraphNodeAdd(p_node_dbg_data);
      } else {
        p_dbg_data = (void *)&p_blk_desc->DbgData;
        ((FS_CACHE_GRAPH_NODE_DBG_DATA *)p_dbg_data)->IsWr = DEF_YES;
        FSCache_GraphNodeEdit((FS_CACHE_GRAPH_NODE_DBG_DATA *)p_dbg_data);
      }
#else
      p_dbg_data = DEF_NULL;
#endif

      cb_data.CachePtr = p_cache;
      cb_data.Invalidate = DEF_NO;
      BREAK_ON_ERR(wr_job_handle = JobSched_Add(&p_cache->JobSched,
                                                JobSched_VoidJobHandle,
                                                (void *)p_blk_desc,
                                                p_dbg_data,
                                                FSCache_WrJobExecCb,
                                                (void *)&cb_data,
                                                p_err));
      if (p_blk_desc != DEF_NULL) {
        p_blk_desc->WrJobHandle = wr_job_handle;
      }
    }
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    else {
      if (p_blk_desc != DEF_NULL) {
        FS_CACHE_GRAPH_NODE_DBG_DATA *p_node_dbg_data;
        BREAK_ON_ERR(p_node_dbg_data = JobSched_DbgDataGet(&p_cache->JobSched,
                                                           wr_job_handle,
                                                           p_err));
        FSCache_GraphNodeRem(&p_blk_desc->DbgData);
        p_blk_desc->DbgData.Id = p_node_dbg_data->Id;
        p_blk_desc->DbgData.IsWr = DEF_YES;
        FSCache_GraphNodeEdit(&p_blk_desc->DbgData);
      }
    }
#endif

    cb_data.CachePtr = p_cache;
    cb_data.Invalidate = DEF_NO;
    BREAK_ON_ERR(JobSched_Order(&p_cache->JobSched,
                                prev_wr_job_handle,
                                wr_job_handle,
                                FSCache_WrJobExecCb,
                                (void *)&cb_data,
                                p_err));
  } WITH_SCOPE_END

  return (wr_job_handle);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_WrJobJoin()
 *
 * @brief    Add a write job stub.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    prev_wr_job_handle  Handle to the preceding write job.
 *
 * @param    wr_job_handle       Handle to an existing write job.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Write job handle.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
FS_CACHE_WR_JOB_HANDLE FSCache_WrJobJoin(FS_CACHE               *p_cache,
                                         FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                         FS_CACHE_WR_JOB_HANDLE wr_job_handle,
                                         RTOS_ERR               *p_err)
{
  FS_CACHE_WR_JOB_HANDLE new_wr_job_handle;

  new_wr_job_handle = FSCache_WrJobSet(p_cache,
                                       prev_wr_job_handle,
                                       wr_job_handle,
                                       DEF_NULL,
                                       p_err);
  return (new_wr_job_handle);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_WrJobAppend()
 *
 * @brief    Append a job.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    prev_wr_job_handle  Handle to the preceding write job.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Write job handle.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
FS_CACHE_WR_JOB_HANDLE FSCache_WrJobAppend(FS_CACHE               *p_cache,
                                           FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                           RTOS_ERR               *p_err)
{
  FS_CACHE_WR_JOB_HANDLE wr_job_handle;

  wr_job_handle = FSCache_WrJobSet(p_cache,
                                   prev_wr_job_handle,
                                   JobSched_VoidJobHandle,
                                   DEF_NULL,
                                   p_err);
  return (wr_job_handle);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_StubWrJobRem()
 *
 * @brief    Remove a stub job.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    stub_job_handle     Stub job handle.
 *
 * @return   DEF_YES, if the stub was removed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
CPU_BOOLEAN FSCache_StubWrJobRem(FS_CACHE               *p_cache,
                                 FS_CACHE_WR_JOB_HANDLE stub_job_handle)
{
  CPU_BOOLEAN done;

  done = JobSched_StubJobTryRem(&p_cache->JobSched, stub_job_handle);
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
  if (done) {
    FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data;
    RTOS_ERR                     err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
    p_dbg_data = JobSched_DbgDataGet(&p_cache->JobSched, stub_job_handle, &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);
    FSCache_GraphNodeRem(p_dbg_data);
  }
#endif

  return (done);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_WrJobExec()
 *
 * @brief    Execute a given job, taking into account dependencies.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    wr_job_handle   Handle to the job to be executed.
 *
 * @param    invalidate      Flag to indicate whether to invalidate the flushed block(s).
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
void FSCache_WrJobExec(FS_CACHE               *p_cache,
                       FS_CACHE_WR_JOB_HANDLE wr_job_handle,
                       CPU_BOOLEAN            invalidate,
                       RTOS_ERR               *p_err)
{
  FS_CACHE_BLK_DESC              *p_blk_desc;
  FS_CACHE_WR_JOB_EXEC_FUNC_DATA cb_data;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(p_cache != DEF_NULL, *p_err, RTOS_ERR_INVALID_STATE,; );

    if (JOB_SCHED_JOB_HANDLE_IS_VALID(&p_cache->JobSched, wr_job_handle)) {
      BREAK_ON_ERR(p_blk_desc = (FS_CACHE_BLK_DESC *)JobSched_DataGet(&p_cache->JobSched,
                                                                      wr_job_handle,
                                                                      p_err));
      if (p_blk_desc != DEF_NULL) {
        BREAK_ON_ERR(FSCache_BlkFlushInternal(p_cache,
                                              p_blk_desc,
                                              invalidate,
                                              p_err));
      } else {
        cb_data.CachePtr = p_cache;
        cb_data.Invalidate = invalidate;
        BREAK_ON_ERR(JobSched_Exec(&p_cache->JobSched,
                                   wr_job_handle,
                                   FSCache_WrJobExecCb,
                                   (void *)&cb_data,
                                   p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSCache_Flush()
 *
 * @brief    Flush the cache instance.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    flush_pred          Predicate function that asserts if cache flush needs to happen.
 *
 * @param    p_flush_pred_data   Data for predicate function.
 *
 * @param    invalidate          Flag to indicate whether to invalidate the flushed block(s).
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
void FSCache_Flush(FS_CACHE                 *p_cache,
                   FS_CACHE_FLUSH_PREDICATE flush_pred,
                   void                     *p_flush_pred_data,
                   CPU_BOOLEAN              invalidate,
                   RTOS_ERR                 *p_err)
{
  FS_CACHE_BLK_DESC *p_cur_blk_desc;
  FS_CACHE_BLK_NBR  cur_blk_desc_ix;
  RTOS_ERR          flush_err;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(p_cache != DEF_NULL, *p_err, RTOS_ERR_INVALID_STATE,; );

    FSCache_Lock(p_cache);
    ON_EXIT {
      FSCache_Unlock(p_cache);
    } WITH {
      RTOS_ERR_SET(flush_err, RTOS_ERR_NONE);

      cur_blk_desc_ix = IB_SList_FirstGet(&p_cache->UsedListEnd);
      BREAK_ON_ERR(while) (cur_blk_desc_ix != IB_SLIST_VOID_IX) {
        p_cur_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, cur_blk_desc_ix);
        if (flush_pred(p_cache, p_cur_blk_desc, p_flush_pred_data)) {
          FSCache_BlkFlushInternal(p_cache,
                                   p_cur_blk_desc,
                                   invalidate,
                                   p_err);

          ASSERT_BREAK((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)
                       || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO),
                       RTOS_ERR_CODE_GET(*p_err));

          if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
            RTOS_ERR_SET(flush_err, RTOS_ERR_IO);
          }

          if (invalidate) {
            cur_blk_desc_ix = IB_SList_FirstGet(&p_cache->UsedListEnd);
          } else {
            cur_blk_desc_ix = IB_SList_NextGet(&p_cache->BlkList, cur_blk_desc_ix);
          }
        } else {
          cur_blk_desc_ix = IB_SList_NextGet(&p_cache->BlkList, cur_blk_desc_ix);
        }
      }
    }

    *p_err = flush_err;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSCache_Sync()
 *
 * @brief    Synchronize the cache on-disk by flushing it.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void FSCache_Sync(FS_CACHE          *p_cache,
                  FS_BLK_DEV_HANDLE blk_dev_handle,
                  RTOS_ERR          *p_err)
{
  FSCache_Flush(p_cache,
                FSCache_FlushPredCb,
                (void *)&blk_dev_handle,
                DEF_NO,
                p_err);
}

/****************************************************************************************************//**
 *                                           FSCache_Invalidate()
 *
 * @brief    Invalidate the cache.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void FSCache_Invalidate(FS_CACHE          *p_cache,
                        FS_BLK_DEV_HANDLE blk_dev_handle,
                        RTOS_ERR          *p_err)
{
  FSCache_Flush(p_cache,
                FSCache_FlushPredCb,
                (void *)&blk_dev_handle,
                DEF_YES,
                p_err);
}

/****************************************************************************************************//**
 *                                           FSCache_BlkGetOrdered()
 *
 * @brief    Get a cache block bound to the given location (the block device / logical block number
 *           pair). Same as FSCache_BlkGet() with added ordering requirements.
 *
 * @param    p_blk_dev_data      Data associated to considered block device.
 *
 * @param    lb_nbr              Logical block number.
 *
 * @param    lb_type             Logical block type.
 *
 * @param    mode                Cache block access (read or write).
 *
 * @param    prev_wr_job_handle  Handle to the preceding write job.
 *
 * @param    p_wr_job_handle     Pointer to the added write job handle.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Pointer to cache block, if NO error(s).
 *           NULL pointer,           otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
CPU_INT08U *FSCache_BlkAcquireOrdered(FS_CACHE_BLK_DEV_DATA  *p_blk_dev_data,
                                      FS_LB_NBR              lb_nbr,
                                      CPU_INT08U             lb_type,
                                      CPU_INT08U             mode,
                                      FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                      FS_CACHE_WR_JOB_HANDLE *p_wr_job_handle,
                                      RTOS_ERR               *p_err)
{
  FS_CACHE                       *p_cache;
  FS_CACHE_BLK_DESC              *p_blk_desc = DEF_NULL;
  CPU_INT08U                     *p_buf;
  FS_BLK_DEV_HANDLE              blk_dev_handle;
  FS_CACHE_WR_JOB_EXEC_FUNC_DATA cb_data;
  FS_CACHE_BLK_NBR               blk_ix;
  CPU_BOOLEAN                    blk_found;
  CPU_BOOLEAN                    wr_job_is_valid;
  CPU_BOOLEAN                    mark_dirty;
  CPU_BOOLEAN                    load;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG(p_blk_dev_data != DEF_NULL, RTOS_ERR_INVALID_STATE, DEF_NULL);

    mark_dirty = DEF_BIT_IS_SET(mode, FS_CACHE_BLK_GET_MODE_WR);
    load = DEF_BIT_IS_SET(mode, FS_CACHE_BLK_GET_MODE_RD);
    p_buf = DEF_NULL;

    blk_dev_handle = p_blk_dev_data->BlkDevHandle;
    p_cache = p_blk_dev_data->CachePtr;

    FSCache_Lock(p_cache);
    ON_BREAK {
      FSCache_Unlock(p_cache);
      break;
    } WITH {
      //                                                           *INDENT-OFF*
      BREAK_ON_ERR(do) {                                        // See Note #1.
        BREAK_ON_ERR(blk_found = FSCache_BlkGetInternal(p_cache,
                                                        blk_dev_handle,
                                                        lb_nbr,
                                                        lb_type,
                                                        mark_dirty,
                                                        &p_buf,
                                                        p_err));
        if (mark_dirty) {
          blk_ix = FSCache_BlkPtrToIx(p_cache, p_buf);
          p_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, blk_ix);

          wr_job_is_valid = JOB_SCHED_JOB_HANDLE_IS_VALID(&p_cache->JobSched, p_blk_desc->WrJobHandle);
          if (!wr_job_is_valid) {
            BREAK_ON_ERR((void)FSCache_WrJobSet(p_cache,
                                                JobSched_VoidJobHandle,
                                                JobSched_VoidJobHandle,
                                                p_blk_desc,
                                                p_err));
          }

          if (p_blk_desc->WrJobHandle.JobId != prev_wr_job_handle.JobId) {
            cb_data.CachePtr = p_cache;
            cb_data.Invalidate = DEF_NO;
            BREAK_ON_ERR(JobSched_Order(&p_cache->JobSched,
                                        prev_wr_job_handle,
                                        p_blk_desc->WrJobHandle,
                                        FSCache_WrJobExecCb,
                                        &cb_data,
                                        p_err));
          }

          if (p_wr_job_handle != DEF_NULL) {
            *p_wr_job_handle = p_blk_desc->WrJobHandle;
          }
        }
      } while (mark_dirty && (p_blk_desc != DEF_NULL) && !p_blk_desc->IsDirty);
      //                                                           *INDENT-ON*
      if (!blk_found && load) {
        RTOS_ASSERT_CRITICAL(lb_type != FS_LB_TYPE_INVALID,
                             RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

        BREAK_ON_ERR(FSBlkDev_Rd(blk_dev_handle,
                                 p_buf,
                                 lb_nbr,
                                 1u,
                                 p_err));
      }

      ASSERT_BREAK(!p_blk_dev_data->IsUnassigning, RTOS_ERR_BLK_DEV_CLOSED);
    }
  } WITH_SCOPE_END

  return (p_buf);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_BlkAcquire()
 *
 * @brief    Get a cache block bound to the given location (the block device / logical block number
 *           pair).
 *
 * @param    p_blk_dev_data  Data associated to considered block device.
 *
 * @param    lb_nbr          Logical block number.
 *
 * @param    lb_type         Logical block type.
 *
 * @param    mode            Cache block access (read or write).
 *
 * @param    p_err           Error pointer.
 *
 * @return   Pointer to cache block, if NO error(s).
 *           NULL pointer,           otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_DISABLED)
CPU_INT08U *FSCache_BlkAcquire(FS_CACHE_BLK_DEV_DATA *p_blk_dev_data,
                               FS_LB_NBR             lb_nbr,
                               CPU_INT08U            lb_type,
                               CPU_BOOLEAN           mode,
                               RTOS_ERR              *p_err)
{
  CPU_INT08U        *p_buf = DEF_NULL;
  FS_CACHE          *p_cache;
  FS_BLK_DEV_HANDLE blk_dev_handle;
  CPU_BOOLEAN       mark_dirty;
  CPU_BOOLEAN       load;
  CPU_BOOLEAN       blk_found;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(p_blk_dev_data != DEF_NULL, *p_err,
                            RTOS_ERR_INVALID_STATE, DEF_NULL);

    p_cache = p_blk_dev_data->CachePtr;
    blk_dev_handle = p_blk_dev_data->BlkDevHandle;

    mark_dirty = DEF_BIT_IS_SET(mode, FS_CACHE_BLK_GET_MODE_WR);
    load = DEF_BIT_IS_SET(mode, FS_CACHE_BLK_GET_MODE_RD);

    FSCache_Lock(p_cache);
    ON_BREAK {
      FSCache_Unlock(p_cache);
      break;
    } WITH {
      BREAK_ON_ERR(blk_found = FSCache_BlkGetInternal(p_cache,
                                                      blk_dev_handle,
                                                      lb_nbr,
                                                      lb_type,
                                                      mark_dirty,
                                                      &p_buf,
                                                      p_err));

      if (!blk_found && load) {
        RTOS_ASSERT_CRITICAL(lb_type != FS_LB_TYPE_INVALID,
                             RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

        BREAK_ON_ERR(FSBlkDev_Rd(blk_dev_handle,
                                 p_buf,
                                 lb_nbr,
                                 1u,
                                 p_err));
      }
    }
  } WITH_SCOPE_END

  return (p_buf);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_BlkRelease()
 *
 * @brief    Release a cache block.
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @param    p_blk       Pointer to block to release.
 *******************************************************************************************************/
void FSCache_BlkRelease(FS_CACHE_BLK_DEV_DATA *p_cache_blk_dev_data,
                        CPU_INT08U            *p_blk)
{
  PP_UNUSED_PARAM(p_blk);

  FSCache_Unlock(p_cache_blk_dev_data->CachePtr);
}

/****************************************************************************************************//**
 *                                           FSCache_BlkAlloc()
 *
 * @brief    Get a new cache block. Evict some cache block if needed.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    blk_size_log2   Base 2 logarithm of the desired block size.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this
 *                           function.
 *
 * @return   Pointer to a free cache block, if NO error(s).
 *           NULL pointer,                  otherwise.
 *
 * @note     (1) If no block descriptor is available, cache blocks are evicted to free descriptors.
 *               At least one block is evicted, but more blocks could be evicted as a result of
 *               specific ordering requirements.
 *******************************************************************************************************/
CPU_INT08U *FSCache_BlkAlloc(FS_CACHE   *p_cache,
                             CPU_INT08U blk_size_log2,
                             RTOS_ERR   *p_err)
{
  CPU_INT08U        *p_blk;
  FS_CACHE_BLK_DESC *p_blk_desc;
  FS_CACHE_BLK_NBR  blk_ix;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_SET(p_cache != DEF_NULL, *p_err, RTOS_ERR_INVALID_STATE, DEF_NULL);

    p_blk = DEF_NULL;
    BREAK_ON_ERR(while) (p_blk == DEF_NULL) {
      BREAK_ON_ERR(p_blk = (CPU_INT08U *)BuddyAlloc_BlkAlloc(&p_cache->BlkAlloc,
                                                             FS_UTIL_PWR2(blk_size_log2),
                                                             p_err));
      if (p_blk == DEF_NULL) {
        BREAK_ON_ERR(FSCache_BlkEvict(p_cache, p_err));
      }
    }

    blk_ix = FSCache_BlkPtrToIx(p_cache, p_blk);
    IB_SList_PushBack(&p_cache->BlkList,
                      &p_cache->UsedListEnd,
                      blk_ix);

    p_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, blk_ix);
    p_blk_desc->LbNbr = (FS_LB_NBR)-1;
    p_blk_desc->IsDirty = DEF_NO;
    p_blk_desc->BlkDevHandle = FSBlkDev_NullHandle;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    p_blk_desc->WrJobHandle = FSCache_VoidWrJobHandle;
#endif
  } WITH_SCOPE_END

  return (p_blk);
}

/****************************************************************************************************//**
 *                                               FSCache_BlkFree()
 *
 * @brief    Free a cache block.
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @param    p_buf       Pointer to the cache block to be freed.
 *******************************************************************************************************/
void FSCache_BlkFree(FS_CACHE   *p_cache,
                     CPU_INT08U *p_blk)
{
  FS_CACHE_BLK_NBR blk_ix;

  blk_ix = FSCache_BlkPtrToIx(p_cache, p_blk);

  BuddyAlloc_BlkFree(&p_cache->BlkAlloc, p_blk);

  IB_SList_Rem(&p_cache->BlkList, &p_cache->UsedListEnd, blk_ix);
}

/****************************************************************************************************//**
 *                                           FSCache_LbInvalidate()
 *
 * @brief    Invalidate specified logical block from the cache.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    blk_dev_handle  Handle to a block device to which the block belongs.
 *
 * @param    lb_nbr          Logical block number.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void FSCache_LbInvalidate(FS_CACHE          *p_cache,
                          FS_BLK_DEV_HANDLE blk_dev_handle,
                          FS_LB_NBR         lb_nbr,
                          RTOS_ERR          *p_err)
{
  FS_CACHE_BLK_DESC *p_blk_desc;

  WITH_SCOPE_BEGIN(p_err) {
    p_blk_desc = FSCache_BlkFind(p_cache,
                                 blk_dev_handle,
                                 lb_nbr);

    if (p_blk_desc != DEF_NULL) {
      BREAK_ON_ERR(FSCache_BlkFlushInternal(p_cache,
                                            p_blk_desc,
                                            DEF_YES,
                                            p_err));
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSCache_Lock()
 *
 * @brief    Lock the cache.
 *
 * @param    p_cache     Pointer to a cache instance.
 *******************************************************************************************************/
void FSCache_Lock(FS_CACHE *p_cache)
{
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  KAL_LockAcquire(p_cache->BlkAcquireMutexHandle,
                  KAL_OPT_NONE,
                  0u,
                  &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               FSCache_Unlock()
 *
 * @brief    Unlock the cache.
 *
 * @param    p_cache     Pointer to a cache instance.
 *******************************************************************************************************/
void FSCache_Unlock(FS_CACHE *p_cache)
{
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  KAL_LockRelease(p_cache->BlkAcquireMutexHandle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("FSCache_Unlock(): fatal error."));
    CPU_SW_EXCEPTION(; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSCache_BlkFind()
 *
 * @brief    Find a used cache block by associated block device and logical block number.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    lb_nbr          Logical block number to look for.
 *
 * @return   Pointer to the found cache block descriptor, if NO error(s).
 *           NULL pointer,                                otherwise.
 *******************************************************************************************************/
static FS_CACHE_BLK_DESC *FSCache_BlkFind(FS_CACHE          *p_cache,
                                          FS_BLK_DEV_HANDLE blk_dev_handle,
                                          FS_LB_NBR         lb_nbr)
{
  FS_CACHE_BLK_DESC *p_cur_blk_desc;
  FS_CACHE_BLK_NBR  buf_ix;

  if (lb_nbr == (FS_LB_NBR)-1) {
    return (DEF_NULL);
  }

  for (buf_ix = IB_SList_FirstGet(&p_cache->UsedListEnd);
       buf_ix != IB_SLIST_VOID_IX;
       buf_ix = IB_SList_NextGet(&p_cache->BlkList, buf_ix)) {
    p_cur_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, buf_ix);
    if ((p_cur_blk_desc->LbNbr == lb_nbr)
        && FS_BLK_DEV_HANDLE_ARE_EQUAL(p_cur_blk_desc->BlkDevHandle, blk_dev_handle)) {
      return (p_cur_blk_desc);
    }
  }
  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                           FSCache_JobExecCb()
 *
 * @brief    Callback called when a job is executed.
 *
 * @param    p_job_data      Pointer to data associated to the job being executed.
 *
 * @param    p_dbg_data      Pointer to data used for cache debug purposes.
 *
 * @param    p_caller_data   Pointer to argument passed to the job execution callback.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
static void FSCache_WrJobExecCb(void     *p_job_data,
                                void     *p_dbg_data,
                                void     *p_caller_data,
                                RTOS_ERR *p_err)
{
  FS_CACHE                       *p_cache;
  FS_CACHE_BLK_DESC              *p_blk_desc;
  CPU_INT08U                     *p_blk = DEF_NULL;
  FS_CACHE_WR_JOB_EXEC_FUNC_DATA *p_exec_func_data;
  FS_CACHE_BLK_NBR               blk_ix;
  CPU_BOOLEAN                    invalidate;

  p_blk_desc = (FS_CACHE_BLK_DESC *)p_job_data;
  p_exec_func_data = (FS_CACHE_WR_JOB_EXEC_FUNC_DATA *)p_caller_data;

  p_cache = p_exec_func_data->CachePtr;
  invalidate = p_exec_func_data->Invalidate;

  if (p_blk_desc != DEF_NULL) {
    blk_ix = FS_CACHE_BLK_DESC_IX_GET(p_cache, p_blk_desc);
    p_blk = FSCache_BlkIxToPtr(p_cache, blk_ix);
    if (p_cache->WrFunc != DEF_NULL) {
      p_cache->WrFunc(p_blk_desc->BlkDevHandle,
                      p_blk,
                      p_blk_desc->LbNbr,
                      p_err);
    }

    p_blk_desc->IsDirty = DEF_NO;

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    {
      p_blk_desc->DbgData.HitCnt = 0u;
      p_blk_desc->DbgData.IsWr = DEF_NO;
      FSCache_GraphNodeEdit(&p_blk_desc->DbgData);
    }
#endif
  }

  if (invalidate || (p_blk_desc == DEF_NULL)) {
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
    FSCache_GraphNodeRem((FS_CACHE_GRAPH_NODE_DBG_DATA *)p_dbg_data);
#else
    PP_UNUSED_PARAM(p_dbg_data);
#endif
    if (p_blk_desc != DEF_NULL) {
      FSCache_BlkFree(p_cache, p_blk);
    }
  }

  if (p_blk_desc != DEF_NULL) {                                 // Invalidate job handle.
    p_blk_desc->WrJobHandle = FSCache_VoidWrJobHandle;
  }
}
#endif

/****************************************************************************************************//**
 *                                               FSCache_BlkGet()
 *
 * @brief    Get a cache block bound to the given location (the block device / logical block number
 *           pair). If no cache block matching the given location is found in the cache, a new cache
 *           block is added to the cache, possibly evicting other cached blocks.
 *
 * @param    p_cache         Pointer to a cache instance.
 *
 * @param    blk_dev_handle  Block device handle.
 *
 * @param    lb_nbr          Logical block number.
 *
 * @param    lb_type         Logical block type.
 *
 * @param    mark_dirty      Mark cache block as dirty.
 *
 * @param    pp_blk          Pointer to a variable that will contain a pointer to the cache block.
 *
 * @param    p_err           Error pointer.
 *
 * @return   DEF_YES if a cache block was found matching the given location.
 *           DEF_NO otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FSCache_BlkGetInternal(FS_CACHE          *p_cache,
                                          FS_BLK_DEV_HANDLE blk_dev_handle,
                                          FS_LB_NBR         lb_nbr,
                                          CPU_INT08U        lb_type,
                                          CPU_BOOLEAN       mark_dirty,
                                          CPU_INT08U        **pp_blk,
                                          RTOS_ERR          *p_err)
{
  FS_CACHE_BLK_DESC *p_blk_desc;
  CPU_INT08U        *p_blk;
  FS_CACHE_BLK_NBR  blk_desc_ix;
  CPU_BOOLEAN       blk_found = DEF_NO;
  CPU_INT08U        lb_size_log2;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(lb_size_log2 = FSBlkDev_LbSizeLog2Get(blk_dev_handle, p_err));

    p_blk_desc = FSCache_BlkFind(p_cache,
                                 blk_dev_handle,
                                 lb_nbr);
    if (p_blk_desc != DEF_NULL) {
      blk_found = DEF_YES;                                      // Blk found in cache, disk does NOT need to be read.
                                                                // Get blk matching to blk desc pos in cache blk tbl.
      blk_desc_ix = FS_CACHE_BLK_DESC_IX_GET(p_cache, p_blk_desc);
      p_blk = FSCache_BlkIxToPtr(p_cache, blk_desc_ix);
      p_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, blk_desc_ix);

      p_blk_desc->IsDirty = p_blk_desc->IsDirty || mark_dirty;
      //                                                           Place blk desc ix at end of cache index list.
      IB_SList_Rem(&p_cache->BlkList, &p_cache->UsedListEnd, blk_desc_ix);
      IB_SList_PushBack(&p_cache->BlkList, &p_cache->UsedListEnd, blk_desc_ix);

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      p_blk_desc->DbgData.HitCnt += 1u;
      FSCache_GraphNodeEdit(&p_blk_desc->DbgData);
#endif
    } else {
      blk_found = DEF_NO;                                       // Blk NOT found in cache, disk sector must be read.
      BREAK_ON_ERR(p_blk = FSCache_BlkAlloc(p_cache,
                                            lb_size_log2,
                                            p_err));
      //                                                           Get blk desc matching to blk pos in cache blk desc tbl
      blk_desc_ix = FSCache_BlkPtrToIx(p_cache, p_blk);
      p_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, blk_desc_ix);
      //                                                           Init blk desc with log blk nbr & blk dev handle...
      //                                                           ...for retrieving a certain blk in cache later.
      p_blk_desc->LbNbr = lb_nbr;
      p_blk_desc->BlkDevHandle = blk_dev_handle;
      p_blk_desc->IsDirty = mark_dirty;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
      p_blk_desc->WrJobHandle = FSCache_VoidWrJobHandle;
#endif

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      p_blk_desc->DbgData.LbType = lb_type;
      p_blk_desc->DbgData.HitCnt = 0u;
      p_blk_desc->DbgData.Id = FSCache_CurGraphNodeId++;
      p_blk_desc->DbgData.LbNbr = lb_nbr;
      p_blk_desc->DbgData.IsWr = DEF_NO;

      FSCache_GraphNodeAdd(&p_blk_desc->DbgData);
#else
      PP_UNUSED_PARAM(lb_type);
#endif
    }

    *pp_blk = p_blk;
  } WITH_SCOPE_END

  return (blk_found);
}

/****************************************************************************************************//**
 *                                       FSCache_BlkFlushInternal()
 *
 * @brief    Flush a cache block taking into account ordering dependencies.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    p_start_blk_desc    Pointer to the descriptor associated with the block to be flushed.
 *
 * @param    invalidate          Flag to indicate whether to invalidate the flushed block(s).
 *
 * @param    p_err               Error pointer.
 *******************************************************************************************************/
static void FSCache_BlkFlushInternal(FS_CACHE          *p_cache,
                                     FS_CACHE_BLK_DESC *p_blk_desc,
                                     CPU_BOOLEAN       invalidate,
                                     RTOS_ERR          *p_err)
{
  CPU_INT08U       *p_blk;
  FS_CACHE_BLK_NBR blk_ix;

  WITH_SCOPE_BEGIN(p_err) {
    blk_ix = FS_CACHE_BLK_DESC_IX_GET(p_cache, p_blk_desc);
    p_blk = FSCache_BlkIxToPtr(p_cache, blk_ix);

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
    if (p_blk_desc->IsDirty) {
      FS_CACHE_WR_JOB_EXEC_FUNC_DATA cb_data;
      cb_data.CachePtr = p_cache;
      cb_data.Invalidate = invalidate;
      BREAK_ON_ERR(JobSched_Exec(&p_cache->JobSched,
                                 p_blk_desc->WrJobHandle,
                                 FSCache_WrJobExecCb,
                                 (void *)&cb_data,
                                 p_err));
    } else {
      if (invalidate) {
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
        FSCache_GraphNodeRem(&p_blk_desc->DbgData);
#endif
        FSCache_BlkFree(p_cache, p_blk);
      }
    }
#else
    if (p_blk_desc->IsDirty) {
      if (p_cache->WrFunc != DEF_NULL) {
        BREAK_ON_ERR(p_cache->WrFunc(p_blk_desc->BlkDevHandle,
                                     p_blk,
                                     p_blk_desc->LbNbr,
                                     p_err));
      }

      p_blk_desc->IsDirty = DEF_NO;
    }

    if (invalidate) {
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
      FSCache_GraphNodeRem(&p_blk_desc->DbgData);
#endif
      FSCache_BlkFree(p_cache, p_blk);
    }
#endif
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FSCache_BlkEvict()
 *
 * @brief    Evict a cache block.
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FSCache_BlkEvict(FS_CACHE *p_cache,
                             RTOS_ERR *p_err)
{
  FS_CACHE_BLK_DESC *p_blk_desc;
  FS_CACHE_BLK_NBR  blk_ix;

  WITH_SCOPE_BEGIN(p_err) {
    blk_ix = IB_SList_FirstGet(&p_cache->UsedListEnd);
    if (blk_ix == IB_SLIST_VOID_IX) {
      return;
    }

    p_blk_desc = FS_CACHE_BLK_DESC_GET(p_cache, blk_ix);
    BREAK_ON_ERR(FSCache_BlkFlushInternal(p_cache,
                                          p_blk_desc,
                                          DEF_YES,
                                          p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FSCache_FlushPredCb()
 *
 * @brief    Callback called when a flush predicate is evaluated.
 *
 * @param    p_cache             Pointer to a cache instance.
 *
 * @param    p_blk_desc          Pointer to cache block descriptor.
 *
 * @param    p_flush_pred_data   Pointer to argument passed to flush predicate function.
 *
 * @return   DEF_TRUE,  if block device handles from cache block and the one considered match.
 *           DEF_FALSE, otherwise
 *******************************************************************************************************/
static CPU_BOOLEAN FSCache_FlushPredCb(FS_CACHE          *p_cache,
                                       FS_CACHE_BLK_DESC *p_blk_desc,
                                       void              *p_flush_pred_data)
{
  FS_BLK_DEV_HANDLE blk_dev_handle;

  PP_UNUSED_PARAM(p_cache);

  blk_dev_handle = *((FS_BLK_DEV_HANDLE *)p_flush_pred_data);

  return (FS_BLK_DEV_HANDLE_ARE_EQUAL(p_blk_desc->BlkDevHandle, blk_dev_handle));
}

/****************************************************************************************************//**
 *                                               FSCache_WrCb()
 *
 * @brief    Callback called when a job is executed. Write a given logical block on a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_blk           Pointer to a cache block to write.
 *
 * @param    lb_nbr          Logical block number.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FSCache_WrCb(FS_BLK_DEV_HANDLE blk_dev_handle,
                         CPU_INT08U        *p_blk,
                         FS_LB_NBR         lb_nbr,
                         RTOS_ERR          *p_err)
{
  FSBlkDev_Wr(blk_dev_handle, p_blk, lb_nbr, 1u, p_err);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_BlkPtrToIx()
 *
 * @brief    Convert block pointer to a block index.
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @param    p_blk       Pointer to a cache block.
 *
 * @return   Block index.
 *******************************************************************************************************/
static FS_CACHE_BLK_NBR FSCache_BlkPtrToIx(FS_CACHE   *p_cache,
                                           CPU_INT08U *p_blk)
{
  FS_CACHE_BLK_NBR blk_ix;
  CPU_ADDR         blk_tbl_addr;
  CPU_INT08U       min_blk_size_log2;

  blk_tbl_addr = BuddyAlloc_BlkTblBaseAddr(&p_cache->BlkAlloc);
  min_blk_size_log2 = BuddyAlloc_ZerothOrderBlkSizeLog2Get(&p_cache->BlkAlloc);

  blk_ix = (FS_CACHE_BLK_NBR)(((CPU_ADDR)p_blk - blk_tbl_addr) >> min_blk_size_log2);

  return (blk_ix);
}

/****************************************************************************************************//**
 *                                           FSCache_BlkIxToPtr()
 *
 * @brief    Convert block index to a pointer to a cache block pointer..
 *
 * @param    p_cache     Pointer to a cache instance.
 *
 * @param    blk_ix      Block indec.
 *
 * @return   Pointer to a cache block.
 *******************************************************************************************************/
static CPU_INT08U *FSCache_BlkIxToPtr(FS_CACHE         *p_cache,
                                      FS_CACHE_BLK_NBR blk_ix)
{
  CPU_INT08U *p_blk;
  CPU_ADDR   blk_tbl_addr;
  CPU_INT08U min_blk_size_log2;

  blk_tbl_addr = BuddyAlloc_BlkTblBaseAddr(&p_cache->BlkAlloc);
  min_blk_size_log2 = BuddyAlloc_ZerothOrderBlkSizeLog2Get(&p_cache->BlkAlloc);

  p_blk = (CPU_INT08U *)(blk_tbl_addr + (blk_ix << min_blk_size_log2));

  return (p_blk);
}

/****************************************************************************************************//**
 *                                           FSCache_OnBlkDevClose()
 *
 * @brief    On block device close callback.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void FSCache_OnBlkDevClose(FS_BLK_DEV_HANDLE blk_dev_handle,
                                  RTOS_ERR          *p_err)
{
  FS_CACHE_BLK_DEV_DATA *p_blk_dev_data;

  p_blk_dev_data = FSCache_BlkDevDataGet(blk_dev_handle);

  FSCache_Lock(p_blk_dev_data->CachePtr);
  p_blk_dev_data->IsUnassigning = DEF_YES;
  FSCache_Unlock(p_blk_dev_data->CachePtr);

  FSCache_Invalidate(p_blk_dev_data->CachePtr, blk_dev_handle, p_err);

  SList_Rem(&FSCore_Data.CacheBlkDevDataListHeadPtr, &p_blk_dev_data->ListMember);

  Mem_DynPoolBlkFree(&FSCore_Data.CacheBlkDevDataPool, (void *)p_blk_dev_data, p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_CODE_GET(*p_err),; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL DEBUG FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSCache_GraphNodeAdd()
 *
 * @brief    Add a node in the graph
 *
 * @param    p_dbg_data  Pointer to data associated to graph node.
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
static void FSCache_GraphNodeAdd(FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data)
{
  CPU_SIZE_T node_size;

  node_size = p_dbg_data->LbType == (CPU_INT08U)-1 ? 15u : 25u + p_dbg_data->HitCnt;

  GraphDbgOutput_NodeAdd(p_dbg_data->Id,
                         p_dbg_data->LbNbr,
                         p_dbg_data->LbType,
                         p_dbg_data->IsWr,
                         node_size);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_GraphNodeEdit()
 *
 * @brief    Edit a node in the graph
 *
 * @param    p_dbg_data  Pointer to data associated to graph node.
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
static void FSCache_GraphNodeEdit(FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data)
{
  CPU_SIZE_T node_size;
  CPU_SIZE_T hit_cnt;

  hit_cnt = DEF_MIN(p_dbg_data->HitCnt, 80u);
  node_size = p_dbg_data->LbType == (CPU_INT08U)-1 ? 15u : 25u + hit_cnt;

  GraphDbgOutput_NodeEdit(p_dbg_data->Id,
                          p_dbg_data->LbNbr,
                          p_dbg_data->LbType,
                          p_dbg_data->IsWr,
                          node_size);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_GraphNodeRem()
 *
 * @brief    Remove a node in the graph
 *
 * @param    p_dbg_data  Pointer to data associated to graph node.
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
static void FSCache_GraphNodeRem(FS_CACHE_GRAPH_NODE_DBG_DATA *p_dbg_data)
{
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  GraphDbgOutput_NodeRem(p_dbg_data->Id);

  if (p_dbg_data->LbType == (CPU_INT08U)-1) {
    Mem_DynPoolBlkFree(&FSCache_GraphNodeDbgDataPool,
                       (void *)p_dbg_data,
                       &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_GraphEdgeAdd()
 *
 * @brief    Add an edge in the graph
 *
 * @param    p_prev_dbg_data     Pointer to previous edge data.
 *
 * @param    p_next_dbg_data     Pointer to next edge data.
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED) \
  && (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
static void FSCache_GraphEdgeAdd(void *p_prev_dbg_data_v,
                                 void *p_next_dbg_data_v)
{
  FS_CACHE_GRAPH_NODE_DBG_DATA *p_prev_dbg_data;
  FS_CACHE_GRAPH_NODE_DBG_DATA *p_next_dbg_data;

  p_prev_dbg_data = (FS_CACHE_GRAPH_NODE_DBG_DATA *)p_prev_dbg_data_v;
  p_next_dbg_data = (FS_CACHE_GRAPH_NODE_DBG_DATA *)p_next_dbg_data_v;

  GraphDbgOutput_EdgeAdd(p_prev_dbg_data->Id, p_next_dbg_data->Id);
}
#endif

/****************************************************************************************************//**
 *                                           FSCache_GraphEdgeRem()
 *
 * @brief    removed an edge from the graph
 *
 * @param    p_prev_dbg_data     Pointer to previous edge data.
 *
 * @param    p_next_dbg_data     Pointer to next edge data.
 *******************************************************************************************************/

#if (FS_CACHE_DBG_EN == DEF_ENABLED) \
  && (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
static void FSCache_GraphEdgeRem(void *p_prev_dbg_data_v,
                                 void *p_next_dbg_data_v)
{
  FS_CACHE_GRAPH_NODE_DBG_DATA *p_prev_dbg_data;
  FS_CACHE_GRAPH_NODE_DBG_DATA *p_next_dbg_data;

  p_prev_dbg_data = (FS_CACHE_GRAPH_NODE_DBG_DATA *)p_prev_dbg_data_v;
  p_next_dbg_data = (FS_CACHE_GRAPH_NODE_DBG_DATA *)p_next_dbg_data_v;

  GraphDbgOutput_EdgeRem(p_prev_dbg_data->Id, p_next_dbg_data->Id);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
